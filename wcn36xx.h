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

#ifndef _WCN36XX_H_
#define _WCN36XX_H_

#include <linux/completion.h>
#include <linux/printk.h>
#include <linux/firmware.h>
#include <linux/workqueue.h>
#include <mach/msm_smd.h>
#include <net/mac80211.h>

#include "smd.h"
#include "dxe.h"

#define DRIVER_PREFIX "wcn36xx: "
#define WLAN_NV_FILE               "wlan/prima/WCNSS_qcom_wlan_nv.bin"

#define wcn36xx_error(fmt, arg...) \
	pr_err(DRIVER_PREFIX "ERROR " fmt "\n", ##arg); \
	__WARN()

#define wcn36xx_info(fmt, arg...) \
	pr_info(DRIVER_PREFIX fmt "\n", ##arg)

#define ENTER() pr_info(DRIVER_PREFIX "%s\n", __func__)

static inline void buff_to_be(u32 *buf, size_t len)
{
	int i;
	for (i = 0; i< len; i++)
	{
		buf[i] = cpu_to_be32(buf[i]);
	}
}
struct nv_data {
   int    	is_valid;
   void 	*table;
};
struct wcn_vif {
	u8 vif_id;
};
struct wcn_sta {
	u8 sta_id;
};
struct wcn36xx_dxe_ch;
struct wcn36xx {
	struct ieee80211_hw 	*hw;
	struct workqueue_struct 	*wq;
	struct workqueue_struct 	*ctl_wq;
	struct device 		*dev;
	const struct firmware 	*nv;
	struct mac_address addresses[2];

	// IRQs
	int 			tx_irq; 	// TX complete irq
	int 			rx_irq; 	// RX ready irq
	void __iomem    	*mmio;

	// SMD related
	smd_channel_t 		*smd_ch;
	u8			*smd_buf;
	struct work_struct 	smd_work;
	struct work_struct 	start_work;
	struct work_struct 	rx_ready_work;
	struct completion 	smd_compl;

	// DXE chanels
	struct wcn36xx_dxe_ch 	dxe_tx_l_ch;	// TX low channel
	struct wcn36xx_dxe_ch 	dxe_tx_h_ch;	// TX high channel
	struct wcn36xx_dxe_ch 	dxe_rx_l_ch;	// RX low channel
	struct wcn36xx_dxe_ch 	dxe_rx_h_ch;	// RX high channel

	// Memory pools
	struct wcn36xx_dxe_mem_pool	mgmt_mem_pool;
	struct wcn36xx_dxe_mem_pool	data_mem_pool;
};

#endif	/* _WCN36XX_H_ */