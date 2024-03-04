/*
 * devdrv_user_common_adapter.h
 *
 * about devdrv user common adapter
 *
 * Copyright (c) 2012-2019 Huawei Technologies Co., Ltd.
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
#ifndef __DEVDRV_USER_COMMON_ADAPTER_H
#define __DEVDRV_USER_COMMON_ADAPTER_H

#define DEVDRV_MAX_SINK_SHORT_STREAM_ID    64
#define DEVDRV_MAX_SINK_LONG_STREAM_ID    0
#define DEVDRV_MAX_NON_SINK_STREAM_ID    64
#define DEVDRV_MAX_STREAM_ID    (DEVDRV_MAX_NON_SINK_STREAM_ID + DEVDRV_MAX_SINK_LONG_STREAM_ID + DEVDRV_MAX_SINK_SHORT_STREAM_ID)
#define DEVDRV_MAX_EVENT_ID    256
#define DEVDRV_MAX_MODEL_ID    64
#define DEVDRV_MAX_SINK_TASK_ID    15000 // all streams are encoded together
#define DEVDRV_MAX_SINK_LONG_TASK_ID    0
#define DEVDRV_MAX_TASK_START_ID 15000
#define DEVDRV_MAX_NOTIFY_ID    256

#define DEVDRV_SQ_SLOT_SIZE 64
#define DEVDRV_CQ_SLOT_SIZE 16

#define DEVDRV_MAX_SQ_NUM 64
#define DEVDRV_MAX_CQ_NUM 1
#define DEVDRV_MAX_DFX_SQ_NUM 4
#define DEVDRV_MAX_DFX_CQ_NUM 10

#define DEVDRV_MAX_SQ_DEPTH 256
#define DEVDRV_MAX_CQ_DEPTH 1024
#define NPU_DEV_NUM 1

#define DEVDRV_DFX_SQ_OFFSET \
	((DEVDRV_MAX_SQ_DEPTH * DEVDRV_SQ_SLOT_SIZE * DEVDRV_MAX_SQ_NUM) + \
		(DEVDRV_MAX_CQ_NUM * DEVDRV_MAX_CQ_DEPTH * DEVDRV_CQ_SLOT_SIZE) + \
		(NPU_DEV_NUM * DEVDRV_MAX_CQ_DEPTH * DEVDRV_CQ_SLOT_SIZE))

#define DEVDRV_INFO_OCCUPY_SIZE \
	((DEVDRV_SQ_INFO_OCCUPY_SIZE) + (DEVDRV_CQ_INFO_OCCUPY_SIZE) + (DEVDRV_STREAM_INFO_OCCUPY_SIZE))

#endif
