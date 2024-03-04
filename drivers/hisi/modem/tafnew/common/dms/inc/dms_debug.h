/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may
 * *    be used to endorse or promote products derived from this software
 * *    without specific prior written permission.
 *
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __DMS_DEBUG_H__
#define __DMS_DEBUG_H__

#include "dms.h"
#include "ps_tag.h"
#include "ps_common_def.h"
#if (defined(CONFIG_HISI_BALONG_EXTRA_MODEM_MBB))
#include "AtMntn.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* Log Print Module Define */
#ifndef THIS_MODU
#define THIS_MODU ps_nas
#endif

typedef enum {
    DMS_SDM_USB_ENABLE = 0,
    DMS_SDM_USB_ENABLE_ERR,
    DMS_SDM_USB_DISABLE,
    DMS_SDM_USB_DISABLE_ERR,

    DMS_SDM_VCOM_OPEN_BEGIN,
    DMS_SDM_VCOM_OPEN_PCUI  = DMS_SDM_VCOM_OPEN_BEGIN + DMS_PHY_BEAR_USB_PCUI,
    DMS_SDM_VCOM_OPEN_CTRL  = DMS_SDM_VCOM_OPEN_BEGIN + DMS_PHY_BEAR_USB_CTRL,
    DMS_SDM_VCOM_OPEN_PCUI2 = DMS_SDM_VCOM_OPEN_BEGIN + DMS_PHY_BEAR_USB_PCUI2,

    DMS_SDM_VCOM_OPEN_ERR_BEGIN,
    DMS_SDM_VCOM_OPEN_ERR_PCUI  = DMS_SDM_VCOM_OPEN_ERR_BEGIN + DMS_PHY_BEAR_USB_PCUI,
    DMS_SDM_VCOM_OPEN_ERR_CTRL  = DMS_SDM_VCOM_OPEN_ERR_BEGIN + DMS_PHY_BEAR_USB_CTRL,
    DMS_SDM_VCOM_OPEN_ERR_PCUI2 = DMS_SDM_VCOM_OPEN_ERR_BEGIN + DMS_PHY_BEAR_USB_PCUI2,

    DMS_SDM_VCOM_CLOSE_BEGIN,
    DMS_SDM_VCOM_CLOSE_PCUI  = DMS_SDM_VCOM_CLOSE_BEGIN + DMS_PHY_BEAR_USB_PCUI,
    DMS_SDM_VCOM_CLOSE_CTRL  = DMS_SDM_VCOM_CLOSE_BEGIN + DMS_PHY_BEAR_USB_CTRL,
    DMS_SDM_VCOM_CLOSE_PCUI2 = DMS_SDM_VCOM_CLOSE_BEGIN + DMS_PHY_BEAR_USB_PCUI2,

    DMS_SDM_VCOM_CLOSE_ERR_BEGIN,
    DMS_SDM_VCOM_CLOSE_ERR_PCUI  = DMS_SDM_VCOM_CLOSE_ERR_BEGIN + DMS_PHY_BEAR_USB_PCUI,
    DMS_SDM_VCOM_CLOSE_ERR_CTRL  = DMS_SDM_VCOM_CLOSE_ERR_BEGIN + DMS_PHY_BEAR_USB_CTRL,
    DMS_SDM_VCOM_CLOSE_ERR_PCUI2 = DMS_SDM_VCOM_CLOSE_ERR_BEGIN + DMS_PHY_BEAR_USB_PCUI2,

    DMS_SDM_VCOM_EVT_BEGIN,
    DMS_SDM_VCOM_EVT_PCUI  = DMS_SDM_VCOM_EVT_BEGIN + DMS_PHY_BEAR_USB_PCUI,
    DMS_SDM_VCOM_EVT_CTRL  = DMS_SDM_VCOM_EVT_BEGIN + DMS_PHY_BEAR_USB_CTRL,
    DMS_SDM_VCOM_EVT_PCUI2 = DMS_SDM_VCOM_EVT_BEGIN + DMS_PHY_BEAR_USB_PCUI2,

    DMS_SDM_VCOM_RD_CB_BEGIN,
    DMS_SDM_VCOM_RD_CB_PCUI  = DMS_SDM_VCOM_RD_CB_BEGIN + DMS_PHY_BEAR_USB_PCUI,
    DMS_SDM_VCOM_RD_CB_CTRL  = DMS_SDM_VCOM_RD_CB_BEGIN + DMS_PHY_BEAR_USB_CTRL,
    DMS_SDM_VCOM_RD_CB_PCUI2 = DMS_SDM_VCOM_RD_CB_BEGIN + DMS_PHY_BEAR_USB_PCUI2,

    DMS_SDM_VCOM_RD_CB_SUCC_BEGIN,
    DMS_SDM_VCOM_RD_CB_SUCC_PCUI  = DMS_SDM_VCOM_RD_CB_SUCC_BEGIN + DMS_PHY_BEAR_USB_PCUI,
    DMS_SDM_VCOM_RD_CB_SUCC_CTRL  = DMS_SDM_VCOM_RD_CB_SUCC_BEGIN + DMS_PHY_BEAR_USB_CTRL,
    DMS_SDM_VCOM_RD_CB_SUCC_PCUI2 = DMS_SDM_VCOM_RD_CB_SUCC_BEGIN + DMS_PHY_BEAR_USB_PCUI2,

    DMS_SDM_VCOM_RD_CB_ERR_BEGIN,
    DMS_SDM_VCOM_RD_CB_ERR_PCUI  = DMS_SDM_VCOM_RD_CB_ERR_BEGIN + DMS_PHY_BEAR_USB_PCUI,
    DMS_SDM_VCOM_RD_CB_ERR_CTRL  = DMS_SDM_VCOM_RD_CB_ERR_BEGIN + DMS_PHY_BEAR_USB_CTRL,
    DMS_SDM_VCOM_RD_CB_ERR_PCUI2 = DMS_SDM_VCOM_RD_CB_ERR_BEGIN + DMS_PHY_BEAR_USB_PCUI2,

    DMS_SDM_VCOM_WRT_BEGIN,
    DMS_SDM_VCOM_WRT_PCUI  = DMS_SDM_VCOM_WRT_BEGIN + DMS_PHY_BEAR_USB_PCUI,
    DMS_SDM_VCOM_WRT_CTRL  = DMS_SDM_VCOM_WRT_BEGIN + DMS_PHY_BEAR_USB_CTRL,
    DMS_SDM_VCOM_WRT_PCUI2 = DMS_SDM_VCOM_WRT_BEGIN + DMS_PHY_BEAR_USB_PCUI2,
    DMS_SDM_VCOM_WRT_NCM   = DMS_SDM_VCOM_WRT_BEGIN + DMS_PHY_BEAR_USB_NCM,

    DMS_SDM_VCOM_WRT_SUSS_BEGIN,
    DMS_SDM_VCOM_WRT_SUSS_PCUI  = DMS_SDM_VCOM_WRT_SUSS_BEGIN + DMS_PHY_BEAR_USB_PCUI,
    DMS_SDM_VCOM_WRT_SUSS_CTRL  = DMS_SDM_VCOM_WRT_SUSS_BEGIN + DMS_PHY_BEAR_USB_CTRL,
    DMS_SDM_VCOM_WRT_SUSS_PCUI2 = DMS_SDM_VCOM_WRT_SUSS_BEGIN + DMS_PHY_BEAR_USB_PCUI2,
    DMS_SDM_VCOM_WRT_SUSS_NCM   = DMS_SDM_VCOM_WRT_SUSS_BEGIN + DMS_PHY_BEAR_USB_NCM,

    DMS_SDM_VCOM_WRT_CB_BEGIN,
    DMS_SDM_VCOM_WRT_CB_PCUI  = DMS_SDM_VCOM_WRT_CB_BEGIN + DMS_PHY_BEAR_USB_PCUI,
    DMS_SDM_VCOM_WRT_CB_CTRL  = DMS_SDM_VCOM_WRT_CB_BEGIN + DMS_PHY_BEAR_USB_CTRL,
    DMS_SDM_VCOM_WRT_CB_PCUI2 = DMS_SDM_VCOM_WRT_CB_BEGIN + DMS_PHY_BEAR_USB_PCUI2,

    DMS_SDM_VCOM_WRT_CB_ERR_BEGIN,
    DMS_SDM_VCOM_WRT_CB_ERR_PCUI  = DMS_SDM_VCOM_WRT_CB_ERR_BEGIN + DMS_PHY_BEAR_USB_PCUI,
    DMS_SDM_VCOM_WRT_CB_ERR_CTRL  = DMS_SDM_VCOM_WRT_CB_ERR_BEGIN + DMS_PHY_BEAR_USB_CTRL,
    DMS_SDM_VCOM_WRT_CB_ERR_PCUI2 = DMS_SDM_VCOM_WRT_CB_ERR_BEGIN + DMS_PHY_BEAR_USB_PCUI2,

    DMS_SDM_DBG_INFO_MAX
} DMS_SDM_MsgId;

#define DMS_LOG_LEVEL_NONE  0x0000
#define DMS_LOG_LEVEL_INFO  0x0001
#define DMS_LOG_LEVEL_WARN  0x0002
#define DMS_LOG_LEVEL_ERROR 0x0004
#define DMS_LOG_LEVEL_DEBUG (DMS_LOG_LEVEL_INFO | DMS_LOG_LEVEL_WARN | DMS_LOG_LEVEL_ERROR)

#pragma pack(push, 8)

typedef struct {
    VOS_UINT32 debugLevel;  /* DMS_PORT DEBUG级别:ERR, WARNING, INFO, DEBUG */
    VOS_UINT64 portIdMask;  /* DMS_PortId端口ID: DMS_PORT0-DMS_PORT63 */
} DMS_PortDebugCfg;
#pragma pack(pop)

extern DMS_PortDebugCfg g_dmsPortDebugCfg;

VOS_BOOL DMS_TestBit(VOS_UINT64 bitMaskValue, VOS_UINT32 bitNum);

#ifdef _lint
#define DMS_LOGI(fmt, ...)
#define DMS_LOGW(fmt, ...)
#define DMS_LOGE(fmt, ...)

#define DMS_PORT_LOGI(indexNum, fmt, ...)
#define DMS_PORT_LOGW(indexNum, fmt, ...)
#define DMS_PORT_LOGE(indexNum, fmt, ...)
#else
#define DMS_LOGI(fmt, ...) do { \
    if (g_dmsPortDebugCfg.debugLevel & DMS_LOG_LEVEL_INFO) { \
        PS_PRINTF_INFO("[%s][LINE:%d] " fmt "\n", \
                       __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } \
} while (0)

#define DMS_LOGW(fmt, ...) do { \
    if (g_dmsPortDebugCfg.debugLevel & DMS_LOG_LEVEL_WARN) { \
        PS_PRINTF_WARNING("[%s][LINE:%d] " fmt "\n", \
                          __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } \
} while (0)

#define DMS_LOGE(fmt, ...) do { \
    if (g_dmsPortDebugCfg.debugLevel & DMS_LOG_LEVEL_ERROR) { \
        PS_PRINTF_ERR("[%s][LINE:%d] " fmt "\n", \
                      __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } \
} while (0)

#define DMS_PORT_LOGI(indexNum, fmt, ...) do { \
    if ((g_dmsPortDebugCfg.debugLevel & DMS_LOG_LEVEL_INFO) && \
        DMS_TestBit(g_dmsPortDebugCfg.portIdMask, indexNum)) { \
        PS_PRINTF_INFO("[PORT:%2d][%s][LINE:%d] " fmt "\n", \
                       indexNum, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } \
} while (0)

#define DMS_PORT_LOGW(indexNum, fmt, ...) do { \
    if ((g_dmsPortDebugCfg.debugLevel & DMS_LOG_LEVEL_WARN) && \
        DMS_TestBit(g_dmsPortDebugCfg.portIdMask, indexNum)) { \
        PS_PRINTF_WARNING("[PORT:%2d][%s][LINE:%d] " fmt "\n", \
                          indexNum, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } \
} while (0)

#define DMS_PORT_LOGE(indexNum, fmt, ...) do { \
    if ((g_dmsPortDebugCfg.debugLevel & DMS_LOG_LEVEL_ERROR) && \
        DMS_TestBit(g_dmsPortDebugCfg.portIdMask, indexNum)) { \
        PS_PRINTF_ERR("[PORT:%2d][%s][LINE:%d] " fmt "\n", \
                       indexNum, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
    } \
} while (0)
#endif

#if (defined(CONFIG_HISI_BALONG_EXTRA_MODEM_MBB))
#define DMS_INFO_LOG(String) TAF_LOG(WUEPS_PID_AT, SUBMOD_NULL, PS_LOG_LEVEL_INFO, (String))
#define DMS_INFO_LOG1(String, Para1) \
    TAF_LOG1(WUEPS_PID_AT, SUBMOD_NULL, PS_LOG_LEVEL_INFO, (String), (VOS_INT32)(Para1))
#define DMS_INFO_LOG2(String, Para1, Para2) \
    TAF_LOG2(WUEPS_PID_AT, SUBMOD_NULL, PS_LOG_LEVEL_INFO, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2))
#define DMS_INFO_LOG3(String, Para1, Para2, Para3)                                                           \
    TAF_LOG3(WUEPS_PID_AT, SUBMOD_NULL, PS_LOG_LEVEL_INFO, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), \
             (VOS_INT32)(Para3))
#define DMS_INFO_LOG4(String, Para1, Para2, Para3, Para4)                                                    \
    TAF_LOG4(WUEPS_PID_AT, SUBMOD_NULL, PS_LOG_LEVEL_INFO, (String), (VOS_INT32)(Para1), (VOS_INT32)(Para2), \
             (VOS_INT32)(Para3), (VOS_INT32)(Para4))
#endif

/* MODEM上行统计信息 */
#define DMS_MODEM_DBG_UL_DATA_READ_CB_NUM(n) (g_dmsStatsInfo.modemStats.ulDataReadCBNum += (n))
#define DMS_MODEM_DBG_UL_GET_RD_FAIL_NUM(n) (g_dmsStatsInfo.modemStats.ulGetRDFailNum += (n))
#define DMS_MODEM_DBG_UL_GET_RD_SUCC_NUM(n) (g_dmsStatsInfo.modemStats.ulGetRDSuccNum += (n))
#define DMS_MODEM_DBG_UL_INVALID_RD_NUM(n) (g_dmsStatsInfo.modemStats.ulInvalidRDNum += (n))
#define DMS_MODEM_DBG_UL_RETURN_BUFF_SUCC_NUM(n) (g_dmsStatsInfo.modemStats.ulReturnBuffSuccNum += (n))
#define DMS_MODEM_DBG_UL_RETURN_BUFF_FAIL_NUM(n) (g_dmsStatsInfo.modemStats.ulReturnBuffFailNum += (n))

/* MODEM下行数据统计信息 */
#define DMS_MODEM_DBG_DL_WRITE_ASYNC_SUCC_NUM(n) (g_dmsStatsInfo.modemStats.dlWriteAsyncSuccNum += (n))
#define DMS_MODEM_DBG_DL_WRITE_ASYNC_FAIL_NUM(n) (g_dmsStatsInfo.modemStats.dlWriteAsyncFailNum += (n))
#define DMS_MODEM_DBG_DL_FREE_BUFF_NUM(n) (g_dmsStatsInfo.modemStats.dlFreeBuffNum += (n))

/* hsuart相关统计信息 */
#define DMS_HSUART_DBG_IOCTL_MSC_READ_CB_NUM(n) (g_dmsStatsInfo.hsuartStats.mscReadCBNum += (n))
#define DMS_HSUART_DBG_IOCTL_MSC_WRITE_SUCC_NUM(n) (g_dmsStatsInfo.hsuartStats.mscWriteSuccNum += (n))
#define DMS_HSUART_DBG_IOCTL_MSC_WRITE_FAIL_NUM(n) (g_dmsStatsInfo.hsuartStats.mscWriteFailNum += (n))
#define DMS_HSUART_DBG_IOCTL_SWITCH_CB_NUM(n) (g_dmsStatsInfo.hsuartStats.switchCmdCBNum += (n))
#define DMS_HSUART_DBG_IOCTL_CFG_FC_SUCC_NUM(n) (g_dmsStatsInfo.hsuartStats.cfgFlowCtrlSuccNum += (n))
#define DMS_HSUART_DBG_IOCTL_CFG_FC_FAIL_NUM(n) (g_dmsStatsInfo.hsuartStats.cfgFlowCtrlFailNum += (n))
#define DMS_HSUART_DBG_IOCTL_CLEAR_BUFF_SUCC_NUM(n) (g_dmsStatsInfo.hsuartStats.clearBuffSuccNum += (n))
#define DMS_HSUART_DBG_IOCTL_CLEAR_BUFF_FAIL_NUM(n) (g_dmsStatsInfo.hsuartStats.clearBuffFailNum += (n))

/* HSUART上行数据统计信息 */
#define DMS_HSUART_DBG_UL_DATA_READ_CB_NUM(n) (g_dmsStatsInfo.hsuartStats.ulDataReadCBNum += (n))
#define DMS_HSUART_DBG_UL_GET_RD_SUCC_NUM(n) (g_dmsStatsInfo.hsuartStats.ulGetRDSuccNum += (n))
#define DMS_HSUART_DBG_UL_GET_RD_FAIL_NUM(n) (g_dmsStatsInfo.hsuartStats.ulGetRDFailNum += (n))
#define DMS_HSUART_DBG_UL_INVALID_RD_NUM(n) (g_dmsStatsInfo.hsuartStats.ulInvalidRDNum += (n))
#define DMS_HSUART_DBG_UL_INVALID_CMD_DATA_NUM(n) (g_dmsStatsInfo.hsuartStats.ulRcvInvalidCmdNum += (n))
#define DMS_HSUART_DBG_UL_VALID_CMD_DATA_NUM(n) (g_dmsStatsInfo.hsuartStats.ulValidCmdNum += (n))
#define DMS_HSUART_DBG_UL_PPP_DATA_NUM(n) (g_dmsStatsInfo.hsuartStats.ulPppDataNum += (n))
#define DMS_HSUART_DBG_UL_IP_DATA_NUM(n) (g_dmsStatsInfo.hsuartStats.ulIpDataNum += (n))
#define DMS_HSUART_DBG_UL_OM_DATA_NUM(n) (g_dmsStatsInfo.hsuartStats.ulOmDataNum += (n))
#define DMS_HSUART_DBG_UL_INVALID_DATA_NUM(n) (g_dmsStatsInfo.hsuartStats.ulInvalidModeDataNum += (n))
#define DMS_HSUART_DBG_UL_RETURN_BUFF_SUCC_NUM(n) (g_dmsStatsInfo.hsuartStats.ulReturnBuffSuccNum += (n))
#define DMS_HSUART_DBG_UL_RETURN_BUFF_FAIL_NUM(n) (g_dmsStatsInfo.hsuartStats.ulReturnBuffFailNum += (n))

/* HSUART下行数据统计信息 */
#define DMS_HSUART_DBG_DL_WRITE_ASYNC_SUCC_NUM(n) (g_dmsStatsInfo.hsuartStats.dlWriteAsyncSuccNum += (n))
#define DMS_HSUART_DBG_DL_WRITE_ASYNC_FAIL_NUM(n) (g_dmsStatsInfo.hsuartStats.dlWriteAsyncFailNum += (n))
#define DMS_HSUART_DBG_DL_WRITE_SYNC_SUCC_NUM(n) (g_dmsStatsInfo.hsuartStats.dlWriteSyncSuccNum += (n))
#define DMS_HSUART_DBG_DL_WRITE_SYNC_FAIL_NUM(n) (g_dmsStatsInfo.hsuartStats.dlWriteSyncFailNum += (n))
#define DMS_HSUART_DBG_DL_WRITE_SYNC_SUCC_LEN(n) (g_dmsStatsInfo.hsuartStats.dlWriteSyncSuccLen += (n))
#define DMS_HSUART_DBG_DL_WRITE_SYNC_FAIL_LEN(n) (g_dmsStatsInfo.hsuartStats.dlWriteSyncFailLen += (n))
#define DMS_HSUART_DBG_DL_FREE_BUFF_NUM(n) (g_dmsStatsInfo.hsuartStats.dlFreeBuffNum += (n))

typedef struct {
    VOS_UINT32 calledNum; /* 调用次数或者消息次数,或者表明该函数是否被调用 */

    VOS_UINT32 rserved1; /* 表明该位置或者其他,HAL_DIAG_SYS_DEBUG_ENUM决定 */

    VOS_UINT32 rserved2; /* 表明特殊含义或者其他,HAL_DIAG_SYS_DEBUG_ENUM决定 */

    VOS_UINT32 rserved3; /* 表明特殊含义或者其他,HAL_DIAG_SYS_DEBUG_ENUM决定 */

    VOS_UINT32 rtcTime; /* 表明最近执行过的时间 */
} DMS_DebugInfoTbl;


struct DMS_DebugModemStats {
    /* 上行统计信息 */
    VOS_UINT32 ulDataReadCBNum;
    VOS_UINT32 ulGetRDSuccNum;
    VOS_UINT32 ulGetRDFailNum;
    VOS_UINT32 ulInvalidRDNum;
    VOS_UINT32 ulReturnBuffSuccNum;
    VOS_UINT32 ulReturnBuffFailNum;

    /* 下行统计信息 */
    VOS_UINT32 dlWriteAsyncSuccNum;
    VOS_UINT32 dlWriteAsyncFailNum;
    VOS_UINT32 dlFreeBuffNum;
    VOS_UINT32 reserved;
};


struct DMS_DebugHsuartStats {
    /* IOCTRL统计信息 */
    VOS_UINT32 mscReadCBNum;
    VOS_UINT32 mscWriteSuccNum;
    VOS_UINT32 mscWriteFailNum;
    VOS_UINT32 switchCmdCBNum;
    VOS_UINT32 cfgFlowCtrlSuccNum;
    VOS_UINT32 cfgFlowCtrlFailNum;
    VOS_UINT32 clearBuffSuccNum;
    VOS_UINT32 clearBuffFailNum;

    /* 上行统计信息 */
    VOS_UINT32 ulDataReadCBNum;
    VOS_UINT32 ulGetRDSuccNum;
    VOS_UINT32 ulGetRDFailNum;
    VOS_UINT32 ulInvalidRDNum;
    VOS_UINT32 ulRcvInvalidCmdNum;
    VOS_UINT32 ulValidCmdNum;
    VOS_UINT32 ulIpDataNum;
    VOS_UINT32 ulPppDataNum;
    VOS_UINT32 ulOmDataNum;
    VOS_UINT32 ulInvalidModeDataNum;
    VOS_UINT32 ulReturnBuffSuccNum;
    VOS_UINT32 ulReturnBuffFailNum;

    /* 下行统计信息 */
    VOS_UINT32 dlWriteAsyncSuccNum;
    VOS_UINT32 dlWriteAsyncFailNum;
    VOS_UINT32 dlWriteSyncSuccNum;
    VOS_UINT32 dlWriteSyncFailNum;
    VOS_UINT32 dlWriteSyncSuccLen;
    VOS_UINT32 dlWriteSyncFailLen;
    VOS_UINT32 dlFreeBuffNum;
};

/*
 * 结构说明: 记录DMS模块的可维可测信息
 */
struct DMS_DebugStats {
    struct DMS_DebugModemStats  modemStats;
    struct DMS_DebugHsuartStats hsuartStats;
};

extern VOS_UINT32            g_dmsDebugLevel;
extern DMS_DebugInfoTbl      g_dmsSdmInfoTable[DMS_SDM_DBG_INFO_MAX];
extern struct DMS_DebugStats g_dmsStatsInfo;

extern VOS_VOID DMS_Debug(DMS_SDM_MsgId type, VOS_UINT32 rserved1, VOS_UINT32 rserved2, VOS_UINT32 rserved3);
#define DMS_DBG_SDM_FUN(enType, ulRserved1, ulRserved2, ulRserved3) \
    DMS_Debug(enType, ulRserved1, ulRserved2, ulRserved3)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* __DMS_DEBUG_H__ */
