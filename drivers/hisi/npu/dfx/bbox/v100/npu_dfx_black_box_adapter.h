/*
 * npu_dfx_black_box_adapter.h
 *
 * about npu black box adapter
 *
 * Copyright (c) 2012-2020 Huawei Technologies Co., Ltd.
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
 * use in hi3690, kirin990_cs2, hi6280, hi6290
 */
#ifndef __NPU_BLACK_BOX_ADAPTER_H
#define __NPU_BLACK_BOX_ADAPTER_H

#ifdef CONFIG_NPU_NOC
static struct noc_err_para_s npu_noc_para[] = {
	{
		.masterid = (u32)SOC_NPU0_MID,
		.targetflow = TARGET_FLOW_DEFAULT,
		.bus = NOC_ERRBUS_NPU,
	}, {
		.masterid = (u32)SOC_NPU1_MID,
		.targetflow = TARGET_FLOW_DEFAULT,
		.bus = NOC_ERRBUS_NPU,
	},
};

u32 modid_array[] = {(u32)RDR_EXC_TYPE_NOC_NPU0, (u32)RDR_EXC_TYPE_NOC_NPU1};
#endif

#endif /* __NPU_BLACK_BOX_ADAPTER_H */
