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


#ifndef __SI_EMAT_H__
#define __SI_EMAT_H__

#include "si_app_emat.h"
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#include "pam_om.h"
#include "usimm_api.h"
#include "ccore_nv_stru_pam.h"
#endif

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
#include "pam_app_om.h"
#include "si_acore_api.h"
#endif

#include "nv_stru_pam.h"
#include "at_oam_interface.h"
#include "mn_error_code.h"

#ifndef MODEM_FUSION_VERSION
#include "msp_diag_comm.h"
#else
#include "mdrv_diag.h"
#endif

#include "mdrv_nvim.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

#if ((OSA_CPU_ACPU == VOS_OSA_CPU) || (VOS_OS_VER == VOS_WIN32))
#define EMAT_GEN_LOG_MODULE(Level) (MDRV_DIAG_GEN_LOG_MODULE(MODEM_ID_0, DIAG_MODE_COMM, Level))

#define EMAT_KEY_INFO_LOG(string)            (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_INFO),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s", string)
#define EMAT_KEY_NORMAL_LOG(string)          (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s", string)
#define EMAT_KEY_WARNING_LOG(string)         (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "WARNING:%s", string)
#define EMAT_KEY_ERROR_LOG(string)           (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_ERROR),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "ERROR:%s", string)

#define EMAT_KEY_INFO_LOG1(string, para1)    (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_INFO),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s,%d", string, para1)
#define EMAT_KEY_NORMAL_LOG1(string, para1)  (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s,%d", string, para1)
#define EMAT_KEY_WARNING_LOG1(string, para1) (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "WARNING:%s,%d", string, para1)
#define EMAT_KEY_ERROR_LOG1(string, para1)   (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_ERROR),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "ERROR:%s,%d", string, para1)

#define EMAT_INFO_LOG(string)                (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_INFO),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s", string)
#define EMAT_NORMAL_LOG(string)              (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s", string)
#define EMAT_WARNING_LOG(string)             (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "WARNING:%s", string)
#define EMAT_ERROR_LOG(string)               (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_ERROR),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "ERROR:%s", string)

#define EMAT_INFO_LOG1(string, para1)        (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_INFO),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s,%d", string, para1)
#define EMAT_NORMAL_LOG1(string, para1)      (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_NORMAL),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "NORMAL:%s,%d", string, para1)
#define EMAT_WARNING_LOG1(string, para1)     (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "WARNING:%s,%d", string, para1)
#define EMAT_ERROR_LOG1(string, para1)       (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_ERROR),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "ERROR:%s,%d", string, para1)

#define EMAT_WARNING_NOSLOTID_LOG(string)    (VOS_VOID)mdrv_diag_log_report(EMAT_GEN_LOG_MODULE(PS_LOG_LEVEL_WARNING),I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "WARNING:%s", string)

#elif (OSA_CPU_CCPU == VOS_OSA_CPU)

#define EMAT_KEY_WARNING_LOG(string)         USIMM_LogPrint(slotId, PS_LOG_LEVEL_WARNING, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "WARNING:%s", string)
#define EMAT_KEY_ERROR_LOG(string)           USIMM_LogPrint(slotId, PS_LOG_LEVEL_ERROR, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "ERROR:%s", string)

#define EMAT_KEY_INFO_LOG1(string, para1)    USIMM_LogPrint1(slotId, PS_LOG_LEVEL_INFO, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "Info:%s,%d", string, para1)
#define EMAT_KEY_NORMAL_LOG1(string, para1)  USIMM_LogPrint1(slotId, PS_LOG_LEVEL_NORMAL, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "Normal:%s,%d", string, para1)
#define EMAT_KEY_WARNING_LOG1(string, para1) USIMM_LogPrint1(slotId, PS_LOG_LEVEL_WARNING, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "WARNING:%s,%d", string, para1)
#define EMAT_KEY_ERROR_LOG1(string, para1)   USIMM_LogPrint1(slotId, PS_LOG_LEVEL_ERROR, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "ERROR:%s,%d", string, para1)

#define EMAT_WARNING_NOSLOTID_LOG(string)    USIMM_LogPrint(USIMM_GetCurrInstanceSlotId(), PS_LOG_LEVEL_WARNING, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "Warning:%s", string)

#define EMAT_INFO_LOG(string)                USIMM_LogPrint(slotId, PS_LOG_LEVEL_INFO, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "%s", (VOS_CHAR*)__FUNCTION__)
#define EMAT_NORMAL_LOG(string)              USIMM_LogPrint(slotId, PS_LOG_LEVEL_NORMAL, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__,"%s", (VOS_CHAR*)__FUNCTION__)
#define EMAT_WARNING_LOG(string)             USIMM_LogPrint(slotId, PS_LOG_LEVEL_WARNING, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "%s", (VOS_CHAR*)__FUNCTION__)
#define EMAT_ERROR_LOG(string)               USIMM_LogPrint(slotId, PS_LOG_LEVEL_ERROR, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "%s", (VOS_CHAR*)__FUNCTION__)

#define EMAT_INFO_LOG1(string, para1)        USIMM_LogPrint1(slotId, PS_LOG_LEVEL_INFO, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "%s: %d", (VOS_CHAR*)__FUNCTION__, para1)
#define EMAT_NORMAL_LOG1(string, para1)      USIMM_LogPrint1(slotId, PS_LOG_LEVEL_NORMAL, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "%s: %d", (VOS_CHAR*)__FUNCTION__, para1)
#define EMAT_WARNING_LOG1(string, para1)     USIMM_LogPrint1(slotId, PS_LOG_LEVEL_WARNING, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "%s: %d", (VOS_CHAR*)__FUNCTION__, para1)
#define EMAT_ERROR_LOG1(string, para1)       USIMM_LogPrint1(slotId, PS_LOG_LEVEL_ERROR, I0_MAPS_EMAT_PID, VOS_NULL_PTR, __LINE__, "%s: %d", (VOS_CHAR*)__FUNCTION__, para1)
#endif

#define EMAT_TAGNOTFOUND 0xFFFFFFFF

#define EMAT_ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))

/* PKID ListForVerification的TAG */
#define EMAT_ESIM_PKID_LFV_TAG 0xA9
/* PKID ListForSigning的TAG */
#define EMAT_ESIM_PKID_LFS_TAG 0xAA

#define EMAT_ESIM_EID_DATA_LEN 21
#define EMAT_ESIM_PKID_DATA_MIN_LEN 17
#define EMAT_ESIM_PKID_DATA_MAX_LEN 256
#define EMAT_ESIM_CLEAN_DATA_LEN 6
#define EMAT_ESIM_CHECK_DATA_MIN_LEN 5

enum SI_EMAT_Req {
    SI_EMAT_ESIM_EID_QUERY_REQ  = 0,
    SI_EMAT_ESIM_PKID_QUERY_REQ = 1,
    SI_EMAT_ESIM_CLEAN_REQ      = 2,
    SI_EMAT_ESIM_CHECK_REQ      = 3,

    SI_EMAT_REQ_BUTT
};
typedef VOS_UINT32 SI_EMAT_ReqUint32;

enum SI_EMAT_Status {
    SI_EMAT_STATUS_IDLE          = 0,
    SI_EMAT_STATUS_OPEN_CHANNEL  = 1,
    SI_EMAT_STATUS_EID_WORKING   = 2,
    SI_EMAT_STATUS_PKID_WORKING  = 3,
    SI_EMAT_STATUS_CLEAN_WORKING = 4,
    SI_EMAT_STATUS_CHECK_WORKING = 5,
    SI_EMAT_STATUS_CLOSE_CHANNEL = 6,
    SI_EMAT_STATUS_EID_CNF       = 7,
    SI_EMAT_STATUS_PKID_CNF      = 8,
    SI_EMAT_STATUS_CLEAN_CNF     = 9,
    SI_EMAT_STATUS_CHECK_CNF     = 10,

    SI_EMAT_STATUS_BUTT
};
typedef VOS_UINT32 SI_EMAT_StatusUint32;


typedef VOS_UINT32 (*PFSIEMATPIDMSGPROC)(SI_PIH_CardSlotUint32 slotId, PS_SI_Msg *msg);


typedef VOS_UINT32 (*PFSIEMATSTATUSMSGPROC)(SI_PIH_CardSlotUint32 slotId);


typedef struct {
    VOS_UINT32         msgPid;
    PFSIEMATPIDMSGPROC procFunc; /* 处理函数 */
} SI_EMAT_PIDMSGPROC_FUNC;


typedef struct {
    SI_EMAT_StatusUint32  curStatus; /* 当前的处理状态 */
    PFSIEMATSTATUSMSGPROC      procFunc;   /* 处理函数 */
} SI_EMAT_STATUS_MSGPROC_FUNC;


typedef struct {
    VOS_MSG_HEADER
    VOS_UINT32                msgName; /* 消息名 */
    VOS_UINT16                client;  /* 客户端ID */
    VOS_UINT8                 opId;
    VOS_UINT8                 rsv;
    SI_EMAT_EventUint32       eventType;
} SI_EMAT_MsgHeader;


typedef struct {
    SI_EMAT_ReqUint32     curReq;    /* 当前正在处理的AT命令消息 */
    SI_EMAT_StatusUint32  curStatus; /* 当前AT命令的处理状态 */
    VOS_UINT32            appType;   /* 当前通道的应用类型 */
    SI_EMAT_EventUint32   eventType; /* 当前事件类型 */
    VOS_UINT16            client;    /* 客户端ID */
    VOS_UINT8             opId;      /* 操作ID */
    VOS_UINT8             channelId; /* 当前通道的channelId */
} SI_EMAT_CurrnetStatus;

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
VOS_VOID I0_SI_EMAT_PidMsgProc(PS_SI_Msg *msg);

VOS_UINT32 I0_WuepsEMATPidInit(enum VOS_InitPhaseDefine initPhrase);

VOS_VOID I1_SI_EMAT_PidMsgProc(PS_SI_Msg *msg);

VOS_UINT32 I1_WuepsEMATPidInit(enum VOS_InitPhaseDefine initPhrase);

VOS_VOID I2_SI_EMAT_PidMsgProc(PS_SI_Msg *msg);

VOS_UINT32 I2_WuepsEMATPidInit(enum VOS_InitPhaseDefine initPhrase);

#endif

VOS_UINT32 SI_COMM_BerFindTag(VOS_UINT8 tag, VOS_UINT8 *data, VOS_UINT32 spareBuffSize, VOS_UINT32 dataLen,
                              VOS_UINT32 findNum);

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
