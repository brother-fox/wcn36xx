/*
 * Contact: Eugene Krasnikov <k.eugene.e@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <linux/vmalloc.h>
#include "smd.h"

int wcn36xx_smd_send_and_wait(struct wcn36xx *wcn, size_t len)
{
	int avail;

	init_completion(&wcn->smd_compl);
	avail = smd_write_avail(wcn->smd_ch);
	if (avail >= len) {
		avail = smd_write(wcn->smd_ch, wcn->smd_buf, len);
		if (avail != len) {
			wcn36xx_error("Cannot write to SMD channel");
			return -EAGAIN;
		}
	} else {
		wcn36xx_error("SMD channel can accept only %d bytes", avail);
		return -ENOMEM;
	}

	if (wait_for_completion_timeout(&wcn->smd_compl,
		msecs_to_jiffies(SMD_MSG_TIMEOUT)) <= 0) {
		wcn36xx_error("Timeout while waiting SMD response");
		return -ETIME;
	}
	return 0;
}

int wcn36xx_smd_rsp_status_check(void *buf, size_t len)
{
	struct wcn36xx_fw_msg_status_rsp * rsp;
	if (len < sizeof(struct wcn36xx_fw_msg_header) +
		sizeof(struct wcn36xx_fw_msg_status_rsp))
		return -EIO;
	rsp = (struct wcn36xx_fw_msg_status_rsp *)
		(buf + sizeof(struct wcn36xx_fw_msg_header));

	if (WCN36XX_FW_MSG_RESULT_SUCCESS != rsp->status) {
		return -EIO;
	}
	return 0;
}

#define INIT_MSG(msg_header, msg_body, type) \
	msg_header.msg_type = type; \
	msg_header.msg_ver = WCN36XX_FW_MSG_VER0; \
	msg_header.msg_len = sizeof(msg_header)+sizeof(*msg_body); \
	memset(msg_body, 0, sizeof(*msg_body));

#define INIT_MSG_S(msg_header, type) \
	msg_header.msg_type = type; \
	msg_header.msg_ver = WCN36XX_FW_MSG_VER0; \
	msg_header.msg_len = sizeof(msg_header);

#define PREPARE_BUF(send_buf, msg_header, msg_body) \
	memset(send_buf, 0, msg_header.msg_len); \
	memcpy(send_buf, &msg_header, sizeof(msg_header)); \
	memcpy((void*)(send_buf + sizeof(msg_header)), msg_body, sizeof(*msg_body));		\

#define PREPARE_BUF_S(send_buf, msg_header) \
	memset(send_buf, 0, msg_header.msg_len); \
	memcpy(send_buf, &msg_header, sizeof(msg_header)); \

int wcn36xx_smd_load_nv(struct wcn36xx *wcn)
{
	struct nv_data *nv_d;
	struct wcn36xx_fw_msg_header msg_header;
	struct wcn36xx_fw_msg_nv_load_header msg_body;
	int fw_bytes_left;
	int ret = 0, fw_size, i = 0;

	u16 fm_offset = 0;
	u16 send_buf_offset = 0;

	i = 0;

	nv_d = (struct nv_data *)wcn->nv->data;
	fw_size = wcn->nv->size;

	INIT_MSG(msg_header, &msg_body, WCN36XX_FW_MSG_TYPE_LOAD_NV_REQ)
	msg_header.msg_len += WCN36XX_NV_FRAGMENT_SIZE;

	// First add message header
	memcpy(wcn->smd_buf, &msg_header, sizeof(msg_header));
	msg_body.frag_num = 0;
	do {
		// Do not forget that we already copied general msg header
		send_buf_offset = sizeof(msg_header);

		fw_bytes_left = wcn->nv->size - fm_offset - 4;
		if (fw_bytes_left > WCN36XX_NV_FRAGMENT_SIZE) {
			msg_body.is_last = 0;
			msg_body.msg_len = WCN36XX_NV_FRAGMENT_SIZE;
		} else {
			msg_body.is_last = 1;
			msg_body.msg_len = fw_bytes_left;

			// Do not forget update general message len
			msg_header.msg_len = sizeof(msg_header)
				+ sizeof(msg_body) + fw_bytes_left;
			memcpy(wcn->smd_buf, &msg_header, sizeof(msg_header));
		}

		// Add load NV request message header
		memcpy((void*)(wcn->smd_buf + send_buf_offset), &msg_body,
			sizeof(msg_body));

		send_buf_offset += sizeof(msg_body);

		// Add NV body itself
		// Rework me
		memcpy((void*)(wcn->smd_buf + send_buf_offset),
			(void*)((void*)(&nv_d->table) + fm_offset), msg_body.msg_len);

		ret = wcn36xx_smd_send_and_wait(wcn, msg_header.msg_len);
		if(ret)	return ret;

		msg_body.frag_num++;
		fm_offset += WCN36XX_NV_FRAGMENT_SIZE;

	} while(msg_body.is_last != 1);

	return ret;

}

int wcn36xx_smd_start(struct wcn36xx *wcn)
{
	struct wcn36xx_fw_msg_header msg_header;
	struct wcn36xx_fw_msg_start_req msg_body;

	INIT_MSG(msg_header, &msg_body, WCN36XX_FW_MSG_TYPE_START_REQ)

	msg_body.driver_type = WCN36XX_FW_MSG_DRIVER_TYPE_PROD;
	msg_body.conf_len = 0;

	PREPARE_BUF(wcn->smd_buf, msg_header, &msg_body)

	return wcn36xx_smd_send_and_wait(wcn, msg_header.msg_len);
}
static int wcn36xx_smd_start_rsp(void *buf, size_t len)
{
	struct wcn36xx_fw_msg_start_rsp * rsp;

	if(wcn36xx_smd_rsp_status_check(buf, len))
		return -EIO;

	rsp = (struct wcn36xx_fw_msg_start_rsp *)
		(buf + sizeof(struct wcn36xx_fw_msg_header));

	wcn36xx_info("WLAN ver=%s, CRM ver=%s",
		rsp->wlan_ver, rsp->crm_ver);
	return 0;
}

int wcn36xx_smd_init_scan(struct wcn36xx *wcn)
{
	struct wcn36xx_fw_msg_init_scan_req msg_body;
	struct wcn36xx_fw_msg_header msg_header;

	INIT_MSG(msg_header, &msg_body, WCN36XX_FW_MSG_TYPE_INIT_SCAN_REQ)

	msg_body.scan_mode = SMD_MSG_SCAN_MODE;

	PREPARE_BUF(wcn->smd_buf, msg_header, &msg_body)

	return wcn36xx_smd_send_and_wait(wcn, msg_header.msg_len);
}

int wcn36xx_smd_start_scan(struct wcn36xx *wcn, u8 ch)
{
	struct wcn36xx_fw_msg_start_scan_req msg_body;
	struct wcn36xx_fw_msg_header msg_header;

	INIT_MSG(msg_header, &msg_body, WCN36XX_FW_MSG_TYPE_START_SCAN_REQ)

	msg_body.ch = ch;

	PREPARE_BUF(wcn->smd_buf, msg_header, &msg_body)

	return wcn36xx_smd_send_and_wait(wcn, msg_header.msg_len);
}
int wcn36xx_smd_end_scan(struct wcn36xx *wcn, u8 ch)
{
	struct wcn36xx_fw_msg_end_scan_req msg_body;
	struct wcn36xx_fw_msg_header msg_header;

	INIT_MSG(msg_header, &msg_body, WCN36XX_FW_MSG_TYPE_END_SCAN_REQ)

	msg_body.ch = ch;

	PREPARE_BUF(wcn->smd_buf, msg_header, &msg_body)

	return wcn36xx_smd_send_and_wait(wcn, msg_header.msg_len);
}
int wcn36xx_smd_deinit_scan(struct wcn36xx *wcn)
{
	struct wcn36xx_fw_msg_deinit_scan_req msg_body;
	struct wcn36xx_fw_msg_header msg_header;

	INIT_MSG(msg_header, &msg_body, WCN36XX_FW_MSG_TYPE_DEINIT_SCAN_REQ)

	msg_body.scan_mode = SMD_MSG_SCAN_MODE;

	PREPARE_BUF(wcn->smd_buf, msg_header, &msg_body)

	return wcn36xx_smd_send_and_wait(wcn, msg_header.msg_len);
}
int wcn36xx_smd_update_scan_params(struct wcn36xx *wcn){
	struct wcn36xx_fw_msg_update_scan_params_req msg_body;
	struct wcn36xx_fw_msg_header msg_header;

	INIT_MSG(msg_header, &msg_body, WCN36XX_FW_MSG_TYPE_UPDATE_SCAN_PARAM_REQ)

	// TODO read this from config
	msg_body.enable_11d	= 0;
	msg_body.resolved_11d = 0;
	msg_body.ch_count = 26;
	msg_body.active_min_ch_time = 60;
	msg_body.active_max_ch_time = 120;
	msg_body.passive_min_ch_time = 60;
	msg_body.passive_max_ch_time = 110;
	msg_body.phy_ch_state = 0;

	PREPARE_BUF(wcn->smd_buf, msg_header, &msg_body)

	return wcn36xx_smd_send_and_wait(wcn, msg_header.msg_len);
}
int wcn36xx_smd_add_sta(struct wcn36xx *wcn, struct mac_address addr, u32 status)
{
	struct wcn36xx_fw_msg_add_sta_req msg_body;
	struct wcn36xx_fw_msg_header msg_header;

	INIT_MSG(msg_header, &msg_body, WCN36XX_FW_MSG_TYPE_ADD_STA_REQ)

	memcpy(&msg_body.mac, &addr, ETH_ALEN);
	msg_body.status = status;

	PREPARE_BUF(wcn->smd_buf, msg_header, &msg_body)

	return wcn36xx_smd_send_and_wait(wcn, msg_header.msg_len);
}

int wcn36xx_smd_enter_imps(struct wcn36xx *wcn)
{
	struct wcn36xx_fw_msg_header msg_header;

	INIT_MSG_S(msg_header, WCN36XX_FW_MSG_TYPE_ENTER_IMPS_REQ)
	PREPARE_BUF_S(wcn->smd_buf, msg_header)

	return wcn36xx_smd_send_and_wait(wcn, msg_header.msg_len);
}

int wcn36xx_smd_exit_imps(struct wcn36xx *wcn)
{
	struct wcn36xx_fw_msg_header msg_header;

	INIT_MSG_S(msg_header, WCN36XX_FW_MSG_TYPE_EXIT_IMPS_REQ)
	PREPARE_BUF_S(wcn->smd_buf, msg_header)

	return wcn36xx_smd_send_and_wait(wcn, msg_header.msg_len);
}

static void wcn36xx_smd_notify(void *data, unsigned event)
{
	struct wcn36xx *wcn = (struct wcn36xx *)data;

	switch (event) {
	case SMD_EVENT_OPEN:
		complete(&wcn->smd_compl);
		break;
	case SMD_EVENT_DATA:
		queue_work(wcn->wq, &wcn->smd_work);
		break;
	case SMD_EVENT_CLOSE:
		break;
	case SMD_EVENT_STATUS:
		break;
	case SMD_EVENT_REOPEN_READY:
		break;
	default:
		wcn36xx_error("SMD_EVENT not supported");
		break;
	}
}
static void wcn36xx_smd_rsp_process (void *buf, size_t len)
{
	struct wcn36xx_fw_msg_header * msg_header = buf;

	switch (msg_header->msg_type) {
	case WCN36XX_FW_MSG_TYPE_START_RSP:
		wcn36xx_smd_start_rsp(buf, len);
		break;
	case WCN36XX_FW_MSG_TYPE_ADD_STA_RSP:
	case WCN36XX_FW_MSG_TYPE_INIT_SCAN_RSP:
	case WCN36XX_FW_MSG_TYPE_START_SCAN_RSP:
	case WCN36XX_FW_MSG_TYPE_END_SCAN_RSP:
	case WCN36XX_FW_MSG_TYPE_DEINIT_SCAN_RSP:
	case WCN36XX_FW_MSG_TYPE_UPDATE_SCAN_PARAM_RSP:
	case WCN36XX_FW_MSG_TYPE_LOAD_NV_RSP:
	case WCN36XX_FW_MSG_TYPE_ENTER_IMPS_RSP:
	case WCN36XX_FW_MSG_TYPE_EXIT_IMPS_RSP:
		wcn36xx_smd_rsp_status_check(buf, len);
		break;
	default:
		wcn36xx_error("SMD_EVENT not supported");
	}
}

static void wcn36xx_smd_work(struct work_struct *work)
{
	int msg_len;
	int avail;
	void *msg;
	int ret;
	struct wcn36xx *wcn =
		container_of(work, struct wcn36xx, smd_work);

	if (!wcn)
		return;

	while (1) {
		msg_len = smd_cur_packet_size(wcn->smd_ch);
		if (0 == msg_len) {
			complete(&wcn->smd_compl);
			return;
		}

		avail = smd_read_avail(wcn->smd_ch);
		if (avail < msg_len) {
			complete(&wcn->smd_compl);
			return;
		}
		msg = vmalloc(msg_len);
		if (NULL == msg) {
			complete(&wcn->smd_compl);
			return;
		}
		ret = smd_read(wcn->smd_ch, msg, msg_len);
		if (ret != msg_len) {
			complete(&wcn->smd_compl);
			return;
		}
		wcn36xx_smd_rsp_process(msg, msg_len);
		vfree(msg);
	}
}
int wcn36xx_smd_open_chan(struct wcn36xx *wcn)
{
	int ret = 0;

	INIT_WORK(&wcn->smd_work, wcn36xx_smd_work);
	init_completion(&wcn->smd_compl);

	ret = smd_named_open_on_edge("WLAN_CTRL", SMD_APPS_WCNSS,
		&wcn->smd_ch, wcn, wcn36xx_smd_notify);
	if (0 != ret) {
		wcn36xx_error("smd_named_open_on_edge");
		goto out;
	}
	ret = wait_for_completion_timeout(&wcn->smd_compl,
		msecs_to_jiffies(SMD_MSG_TIMEOUT));
	if (ret <= 0) {
		wcn36xx_error("Cannot open SMD channel");
		goto out;
	}
out:
	return ret;
}