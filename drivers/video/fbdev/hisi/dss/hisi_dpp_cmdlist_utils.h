 /*
 * Copyright (c) 2019-2020 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */
#ifndef HISI_DPP_CMDLIST_UTILS_H
#define HISI_DPP_CMDLIST_UTILS_H

#include <linux/types.h>
#include "hisi_display_effect.h"

#define DPP_CMDLIST_REGION_NUM 2	// only support two region currently
#define DPP_CMDLIST_BUFFER_NUM 3
#define DPP_CMDLIST_TOTAL_BUFFER (DPP_CMDLIST_REGION_NUM * DPP_CMDLIST_BUFFER_NUM)
#define get_cmdlist_item_len(length) (((length + 2) / 3 + 1) * 4)
#define get_max_item(item_len) (item_len / sizeof(struct cmd_item))

// platform-dependant
#define DPP_CMDLIST_MCTL DSS_MCTL5
#define MAX_DPP_MODULE_NUM 4
#define CMDLIST_REG_OFFSET 0x40

#define DPP_GMP_LUT_BASE 0x10000
#define DPP_GMP_BASE 0x40000
#define DPP_GAMMA_BASE 0x40000
#define DPP_DEGAMMA_BASE 0x40000
#define DPP_XCC_LUT_BASE 0x40000

enum {
	EN_DPP_CMDLIST_NOT_INIT = 0x0,
	EN_DPP_CMDLIST_DISABLE = 0x1,
	EN_DPP_CMDLIST_ENABLE_TOP = 0x2,
	EN_DPP_CMDLIST_ENABLE_BOTTOM = 0x3,
};

enum {
	EN_DPP_CH_MODULE_GMP = 0x1,
	EN_DPP_CH_MODULE_GAMMA = 0x2,
	EN_DPP_CH_MODULE_DEGAMMA = 0x4,
	EN_DPP_CH_MODULE_XCC = 0x8,
	EN_DPP_CH_MODULE_ALL = 0xF,
};

enum {
	EN_DPP_CMDLIST_STATUS_FREE = 0,
	EN_DPP_CMDLIST_STATUS_PREPARING = 0x10,
	EN_DPP_CMDLIST_STATUS_READY = 0x20,
	EN_DPP_CMDLIST_STATUS_WORKING = 0x40,
	EN_DPP_CMDLIST_STATUS_FINISHED = 0x80,
};

enum config_region {
	CONFIG_REGION_TOP,
	CONFIG_REGION_BOTTOM,
	CONFIG_REGION_INVALID,
};

typedef struct dpp_cmdlist_data {
	uint8_t buffer_status;
	uint8_t dpp_module[MAX_DPP_MODULE_NUM];
	uint8_t dpp_module_num;
	struct dss_cmdlist_node *cmdlist_node;
} dpp_cmdlist_data_t;

bool hisi_dpp_cmdlist_get_enable(void);

void hisi_dpp_cmdlist_init(struct hisi_fb_data_type *hisifd);

void hisi_dpp_cmdlist_interrupt_on(struct hisi_fb_data_type *hisifd);

void hisi_dpp_cmdlist_interrupt_off(struct hisi_fb_data_type *hisifd);

void hisi_dpp_cmdlist_roi_config(struct hisi_fb_data_type *hisifd,
	struct dss_rect roi);

void hisi_dpp_cmdlist_region_start(struct hisi_fb_data_type *hisifd,
	uint8_t region);

void hisi_dpp_cmdlist_region_complete(void);

int hisi_effect_dpp_cmdlist_set_lut_common(struct hisi_fb_data_type *hisifd,
	struct dpp_buf_info *buf_info, uint32_t dpp_sel);

bool hisi_dpp_cmdlist_buffer_need_update(struct hisi_fb_data_type *hisifd,
	int8_t config_region);

int hisi_dpp_cmdlist_set_lut_roi(struct hisi_fb_data_type *hisifd,
	uint32_t modules, struct dpp_buf_info *buf_info,
	bool in_roi);

void hisi_effect_dpproi_verify(struct hisi_fb_data_type *hisifd,
	struct dss_effect_info *effect_info);

// platform-dependant
void hisi_dpe_vactive_line_interrupt(const struct hisi_fb_data_type *hisifd,
	bool enable);

void hisi_dpe_cmdlist_interrupt_unmask(const struct hisi_fb_data_type *hisifd);

size_t hisi_display_effect_get_dpp_lut_size(const struct hisi_fb_data_type *hisifd);

void hisi_dpp_cmdlist_reset(struct hisi_fb_data_type *hisifd);

void hisi_display_effect_set_dpp_config_type(struct hisi_fb_data_type *hisifd);

void dpp_cmdlist_ch_spin_lock(struct hisi_fb_data_type *hisifd, int8_t dst_channel);

bool dpp_cmdlist_ch_spin_trylock(struct hisi_fb_data_type *hisifd, int8_t dst_channel);

void dpp_cmdlist_ch_spin_unlock(struct hisi_fb_data_type *hisifd, int8_t dst_channel);

void dpp_cmdlist_ch_spin_lock_irqsave(struct hisi_fb_data_type *hisifd, int8_t dst_channel);

bool dpp_cmdlist_ch_spin_trylock_irqsave(struct hisi_fb_data_type *hisifd,
	int8_t dst_channel);

void dpp_cmdlist_ch_spin_unlock_irqrestore(struct hisi_fb_data_type *hisifd,
	int8_t dst_channel);

void hisi_effect_dpp_cmdlist_config(struct hisi_fb_data_type *hisifd,
	uint32_t buff_sel);

void hisi_dpp_cmdlist_action(struct hisi_fb_data_type *hisifd);

struct dss_rect effect_get_combined_roi(const struct roi_rect *roi,
	uint32_t roi_region_count);

#endif
