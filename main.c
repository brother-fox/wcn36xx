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
#include <linux/module.h>
#include <linux/qcomwlan_pwrif.h>
#include <linux/platform_device.h>
#include <linux/wcnss_wlan.h>
#include <linux/vmalloc.h>
#include "wcn36xx.h"
#include "dxe.h"

static int wcn36xx_start(struct ieee80211_hw *hw)
{
	ENTER();
	return 0;
}
static void wcn36xx_stop(struct ieee80211_hw *hw)
{
	ENTER();
}

static void wcn36xx_remove_interface(struct ieee80211_hw *hw,
				   struct ieee80211_vif *vif)
{
	struct wcn36xx *wcn = hw->priv;
	ENTER();
	vfree(wcn->smd_buf);
}
static int wcn36xx_change_interface(struct ieee80211_hw *hw,
				      struct ieee80211_vif *vif,
				      enum nl80211_iftype new_type, bool p2p)
{
	ENTER();
	return 0;
}
static int wcn36xx_config(struct ieee80211_hw *hw, u32 changed)
{
	ENTER();
	return 0;
}
static u64 wcn36xx_prepare_multicast(struct ieee80211_hw *hw,
				       struct netdev_hw_addr_list *mc_list)
{
	ENTER();
	return 0;
}
#define WCN36XX_SUPPORTED_FILTERS (FIF_PROMISC_IN_BSS | \
				  FIF_ALLMULTI | \
				  FIF_FCSFAIL | \
				  FIF_BCN_PRBRESP_PROMISC | \
				  FIF_CONTROL | \
				  FIF_OTHER_BSS)
static void wcn36xx_configure_filter(struct ieee80211_hw *hw,
				       unsigned int changed,
				       unsigned int *total, u64 multicast)
{
	ENTER();
	changed &= WCN36XX_SUPPORTED_FILTERS;
	*total &= WCN36XX_SUPPORTED_FILTERS;
}

static void wcn36xx_tx(struct ieee80211_hw *hw, struct sk_buff *skb)
{
	ENTER();
}

static int wcn36xx_set_key(struct ieee80211_hw *hw, enum set_key_cmd cmd,
			     struct ieee80211_vif *vif,
			     struct ieee80211_sta *sta,
			     struct ieee80211_key_conf *key_conf)
{
	ENTER();
	return 0;
}
static int wcn36xx_hw_scan(struct ieee80211_hw *hw,
			     struct ieee80211_vif *vif,
			     struct cfg80211_scan_request *req)
{
	struct wcn36xx *wcn = hw->priv;
	int ch;
	int i;
	ENTER();
	wcn36xx_smd_enter_imps(wcn);
	wcn36xx_smd_update_scan_params(wcn);
	wcn36xx_smd_exit_imps(wcn);
	for(i = 0; i < req->n_channels; i++) {
		wcn36xx_smd_init_scan(wcn);
		ch = ieee80211_frequency_to_channel(req->channels[i]->center_freq);
		wcn36xx_info("Scanning on channel %d", ch);
		wcn36xx_smd_start_scan(wcn, ch);
		// do this as timer
		msleep(200);
		wcn36xx_smd_end_scan(wcn, ch);
		wcn36xx_smd_deinit_scan(wcn);
	}

	ieee80211_sched_scan_results(wcn->hw);
	ieee80211_scan_completed(wcn->hw, false);
	return 0;
}
static void wcn36xx_cancel_hw_scan(struct ieee80211_hw *hw,
				     struct ieee80211_vif *vif)
{
	ENTER();
}
static int wcn36xx_sched_scan_start(struct ieee80211_hw *hw,
				      struct ieee80211_vif *vif,
				      struct cfg80211_sched_scan_request *req,
				      struct ieee80211_sched_scan_ies *ies)
{
	ENTER();
	return 0;
}
static void wcn36xx_sched_scan_stop(struct ieee80211_hw *hw,
				      struct ieee80211_vif *vif)
{
	ENTER();
}
static void wcn36xx_bss_info_changed(struct ieee80211_hw *hw,
				       struct ieee80211_vif *vif,
				       struct ieee80211_bss_conf *bss_conf,
				       u32 changed)
{
	ENTER();
}
static int wcn36xx_set_frag_threshold(struct ieee80211_hw *hw, u32 value)
{
	ENTER();
	return 0;
}
static int wcn36xx_set_rts_threshold(struct ieee80211_hw *hw, u32 value)
{
	ENTER();
	return 0;
}
static int wcn36xx_conf_tx(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
			     u16 queue,
			     const struct ieee80211_tx_queue_params *params)
{
	ENTER();
	return 0;
}
static u64 wcn36xx_get_tsf(struct ieee80211_hw *hw,
			     struct ieee80211_vif *vif)
{
	ENTER();
	return 0;
}
static int wcn36xx_get_survey(struct ieee80211_hw *hw, int idx,
				struct survey_info *survey)
{
	ENTER();
	return 0;
}
static int wcn36xx_sta_state(struct ieee80211_hw *hw,
			       struct ieee80211_vif *vif,
			       struct ieee80211_sta *sta,
			       enum ieee80211_sta_state old_state,
			       enum ieee80211_sta_state new_state)
{
	ENTER();
	return 0;
}
static int wcn36xx_ampdu_action(struct ieee80211_hw *hw,
				  struct ieee80211_vif *vif,
				  enum ieee80211_ampdu_mlme_action action,
				  struct ieee80211_sta *sta, u16 tid, u16 *ssn,
				  u8 buf_size)
{
	ENTER();
	return 0;
}
static bool wcn36xx_tx_frames_pending(struct ieee80211_hw *hw)
{
	ENTER();
	return true;
}
static int wcn36xx_set_bitrate_mask(struct ieee80211_hw *hw,
				   struct ieee80211_vif *vif,
				   const struct cfg80211_bitrate_mask *mask)
{
	ENTER();
	return 0;
}
static void wcn36xx_channel_switch(struct ieee80211_hw *hw,
				     struct ieee80211_channel_switch *ch_switch)
{
	ENTER();
}
static int wcn36xx_suspend(struct ieee80211_hw *hw,
			    struct cfg80211_wowlan *wow)
{
	ENTER();
	return 0;
}
static int wcn36xx_resume(struct ieee80211_hw *hw)
{
	ENTER();
	return 0;
}
static int wcn36xx_add_interface(struct ieee80211_hw *hw,
				   struct ieee80211_vif *vif)
{

	struct wcn36xx *wcn = hw->priv;
	int ret = 0;
	ENTER();
	// SMD initialization
	wcn36xx_smd_open_chan(wcn);

	// Not to receive INT untill the whole buf from SMD is read
	smd_disable_read_intr(wcn->smd_ch);

	// Allocate memory pools for Mgmt BD headers and Data BD headers
	wcn36xx_dxe_allocate_mem_pools(wcn);
	wcn36xx_dxe_alloc_ctl_blks(wcn);

	INIT_WORK(&wcn->rx_ready_work, wcn36xx_rx_ready_work);

	ret = request_firmware(&wcn->nv, WLAN_NV_FILE, wcn->dev);
	if (ret) {
		//TODO error handling
		wcn36xx_error("request FM %d", ret);
	}
	// maximu SMD message size is 4k
	wcn->smd_buf = vmalloc(4096);

	//TODO pass configuration to FW
	wcn36xx_smd_load_nv(wcn);
	wcn36xx_smd_start(wcn);

	// DMA chanel initialization
	wcn36xx_dxe_init(wcn);
	wcn36xx_dxe_request_irqs(wcn);

	wcn36xx_smd_add_sta(wcn, wcn->addresses[0], 0);
	wcn36xx_smd_enter_imps(wcn);
	wcn36xx_smd_exit_imps(wcn);
	wcn36xx_smd_add_sta(wcn, wcn->addresses[0], 1);

	return 0;
}
static const struct ieee80211_ops wcn36xx_ops = {
	.start 			= wcn36xx_start,
	.stop	 		= wcn36xx_stop,
	.add_interface		= wcn36xx_add_interface,
	.remove_interface 	= wcn36xx_remove_interface,
	.change_interface 	= wcn36xx_change_interface,
#ifdef CONFIG_PM
	.suspend 		= wcn36xx_suspend,
	.resume			= wcn36xx_resume,
#endif
	.config 		= wcn36xx_config,
	.prepare_multicast 	= wcn36xx_prepare_multicast,
	.configure_filter 	= wcn36xx_configure_filter,
	.tx 			= wcn36xx_tx,
	.set_key 		= wcn36xx_set_key,
	.hw_scan 		= wcn36xx_hw_scan,
	.cancel_hw_scan 	= wcn36xx_cancel_hw_scan,
	.sched_scan_start 	= wcn36xx_sched_scan_start,
	.sched_scan_stop 	= wcn36xx_sched_scan_stop,
	.bss_info_changed 	= wcn36xx_bss_info_changed,
	.set_frag_threshold 	= wcn36xx_set_frag_threshold,
	.set_rts_threshold 	= wcn36xx_set_rts_threshold,
	.conf_tx 		= wcn36xx_conf_tx,
	.get_tsf 		= wcn36xx_get_tsf,
	.get_survey 		= wcn36xx_get_survey,
	.sta_state 		= wcn36xx_sta_state,
	.ampdu_action 		= wcn36xx_ampdu_action,
	.tx_frames_pending 	= wcn36xx_tx_frames_pending,
	.set_bitrate_mask 	= wcn36xx_set_bitrate_mask,
	.channel_switch 	= wcn36xx_channel_switch
};

static struct ieee80211_hw *wcn36xx_alloc_hw(void)
{
	struct ieee80211_hw *hw;
	ENTER();
	hw = ieee80211_alloc_hw(sizeof(struct wcn36xx), &wcn36xx_ops);
	return hw;
}
#define CHAN2G(_freq, _idx) { \
	.band = IEEE80211_BAND_2GHZ, \
	.center_freq = (_freq), \
	.hw_value = (_idx), \
	.max_power = 25, \
}
static struct ieee80211_channel wcn_2ghz_channels[] = {
	CHAN2G(2412, 0), /* Channel 1 */
	CHAN2G(2417, 1), /* Channel 2 */
	CHAN2G(2422, 2), /* Channel 3 */
	CHAN2G(2427, 3), /* Channel 4 */
	CHAN2G(2432, 4), /* Channel 5 */
	CHAN2G(2437, 5), /* Channel 6 */
	CHAN2G(2442, 6), /* Channel 7 */
	CHAN2G(2447, 7), /* Channel 8 */
	CHAN2G(2452, 8), /* Channel 9 */
	CHAN2G(2457, 9), /* Channel 10 */
	CHAN2G(2462, 10), /* Channel 11 */
	CHAN2G(2467, 11), /* Channel 12 */
	CHAN2G(2472, 12), /* Channel 13 */
	CHAN2G(2484, 13)  /* Channel 14 */

};
#define RATE(_bitrate, _hw_rate, _flags) { \
	.bitrate        = (_bitrate),                   \
	.flags          = (_flags),                     \
	.hw_value       = (_hw_rate),                   \
	.hw_value_short = (_hw_rate)  \
}
static struct ieee80211_rate wcn_legacy_rates[] = {
	RATE(10, BIT(0), 0),
	RATE(20, BIT(1), IEEE80211_RATE_SHORT_PREAMBLE),
	RATE(55, BIT(2), IEEE80211_RATE_SHORT_PREAMBLE),
	RATE(110, BIT(3), IEEE80211_RATE_SHORT_PREAMBLE),
	RATE(60, BIT(4), 0),
	RATE(90, BIT(5), 0),
	RATE(120, BIT(6), 0),
	RATE(180, BIT(7), 0),
	RATE(240, BIT(8), 0),
	RATE(360, BIT(9), 0),
	RATE(480, BIT(10), 0),
	RATE(540, BIT(11), 0)
};
static struct ieee80211_supported_band wcn_band_2ghz = {
	.channels 	= wcn_2ghz_channels,
	.n_channels 	= ARRAY_SIZE(wcn_2ghz_channels),
	.bitrates 	= wcn_legacy_rates,
	.n_bitrates 	= ARRAY_SIZE(wcn_legacy_rates),
	.ht_cap		= {
		.cap = IEEE80211_HT_CAP_GRN_FLD | IEEE80211_HT_CAP_SGI_20 |
			(1 << IEEE80211_HT_CAP_RX_STBC_SHIFT),
		.ht_supported = true,
		.ampdu_factor = IEEE80211_HT_MAX_AMPDU_8K,
		.ampdu_density = IEEE80211_HT_MPDU_DENSITY_8,
		.mcs = {
			.rx_mask = { 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, },
			.rx_highest = cpu_to_le16(72),
			.tx_params = IEEE80211_HT_MCS_TX_DEFINED,
		}
	}
};

static int wcn36xx_init_ieee80211(struct wcn36xx * wcn_priv)
{
	int ret = 0;

	wcn_priv->hw->flags = IEEE80211_HW_SIGNAL_DBM |
		IEEE80211_HW_SUPPORTS_PS |
		IEEE80211_HW_SUPPORTS_DYNAMIC_PS |
		IEEE80211_HW_AP_LINK_PS |
		/* Need ?*/
		IEEE80211_HW_REPORTS_TX_ACK_STATUS |
		/* Need ?*/
		IEEE80211_HW_CONNECTION_MONITOR |
		IEEE80211_HW_HAS_RATE_CONTROL |
		IEEE80211_HW_REPORTS_TX_ACK_STATUS;

	wcn_priv->hw->wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION);
	wcn_priv->hw->wiphy->bands[IEEE80211_BAND_2GHZ] = &wcn_band_2ghz;

	wcn_priv->hw->wiphy->max_scan_ssids = 1;

	// TODO make a conf file where to read this information from
	wcn_priv->hw->max_listen_interval = 200;

	wcn_priv->hw->queues = 4;

	SET_IEEE80211_DEV(wcn_priv->hw, wcn_priv->dev);

	wcn_priv->hw->sta_data_size = sizeof(struct wcn_sta);
	wcn_priv->hw->vif_data_size = sizeof(struct wcn_vif);


	return ret;
}

static int __init wcn36xx_init(void)
{
	struct ieee80211_hw *hw;
	struct wcn36xx *wcn;
	struct resource *wcnss_memory;
	int ret;

	hw = wcn36xx_alloc_hw();
	wcn = hw->priv;
	wcn->hw = hw;

	wcn->dev = wcnss_wlan_get_device();
	if (wcn->dev == NULL) {
		//TODO error handling
		wcn36xx_error("device");
	}
	wcn->wq = create_freezable_workqueue("wcn36xx_wq");
	wcn->ctl_wq = create_freezable_workqueue("wcn36xx_ctl_wq");

	wcn36xx_init_ieee80211(wcn);
	// TODO read me from config
	wcn->addresses[0].addr[0] = 0xD0;
	wcn->addresses[0].addr[1] = 0x51;
	wcn->addresses[0].addr[2] = 0x62;
	wcn->addresses[0].addr[3] = 0x27;
	wcn->addresses[0].addr[4] = 0x26;
	wcn->addresses[0].addr[5] = 0x4C;

	wcn->addresses[1].addr[0] = 0xD0;
	wcn->addresses[1].addr[1] = 0x51;
	wcn->addresses[1].addr[2] = 0x62;
	wcn->addresses[1].addr[3] = 0x27;
	wcn->addresses[1].addr[4] = 0x26;
	wcn->addresses[1].addr[5] = 0x4D;

	wcn->hw->wiphy->n_addresses = ARRAY_SIZE(wcn->addresses);
	wcn->hw->wiphy->addresses = wcn->addresses;
	ret = ieee80211_register_hw(wcn->hw);

	wcnss_memory = wcnss_wlan_get_memory_map(wcn->dev);
	if (wcnss_memory == NULL) {
		//TODO error handling
		wcn36xx_error("wcnss_wlan_get_memory_map");
	}

	wcn->tx_irq = wcnss_wlan_get_dxe_tx_irq(wcn->dev);
	wcn->rx_irq = wcnss_wlan_get_dxe_rx_irq(wcn->dev);

	wcn->mmio = ioremap(wcnss_memory->start, resource_size(wcnss_memory));
	if (NULL == wcn->mmio) {
		//TODO error handling
		wcn36xx_error("mmio");
	}
	return 0;
}

module_init(wcn36xx_init);

static void __exit wcn36xx_exit(void)
{
	ENTER();

}
module_exit(wcn36xx_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eugene Krasnikov k.eugene.e@gmail.com");