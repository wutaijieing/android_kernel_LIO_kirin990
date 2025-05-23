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
#ifndef TAF_TYPE_DEF_H
#define TAF_TYPE_DEF_H

#include "v_typdef.h"

#if (VOS_WIN32 == VOS_OS_VER)
#ifdef _NAS_SECURE_
#include "UT_Stub.h"
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

/* LOCAL */
#ifdef LOCAL
#undef LOCAL
#endif

#ifdef _EXPORT_LOCAL
#define LOCAL
#else
#define LOCAL static
#endif

/* ---------------LogSaver可维可测功能相关定义----------------------- */
/* 1.1: OM_GreenChannel的第二个参数usPrimId的定义 */
#define TAF_OM_GREEN_CHANNEL_PS (0xC001) /* PS相关 */

/*
 * 1.2: OM_GreenChannel的第三个参数可带一些可维可测信息，为了提高定位效率，对一些典型
 *   错误做了如下枚举定义
 */
enum TAF_OM_GreenchannelErr {
    TAF_OM_GREENCHANNEL_PS_CID_NOT_DEFINE = 0,
    TAF_OM_GREENCHANNEL_PS_CREATE_PPP_REQ_ERR,
    TAF_OM_GREENCHANNEL_PS_CREATE_RAW_DATA_PPP_REQ_ERR,
    TAF_OM_GREENCHANNEL_PS_IP_TYPE_DIAL_FAIL,
    TAF_OM_GREENCHANNEL_PS_PPP_TYPE_DIAL_FAIL,
    TAF_OM_GREENCHANNEL_PS_DEACTIVE_PDP_ERR_EVT,
    TAF_OM_GREENCHANNEL_PS_ACTIVE_PDP_REJ,
    TAF_OM_GREENCHANNEL_PS_MODIFY_PDP_REJ,
    TAF_OM_GREENCHANNEL_PS_NET_ORIG_DEACTIVE_IND,

    TAF_OM_GREENCHANNEL_ERR_BUTT
};

/* 字节序定义 */
#define TAF_LITTLE_ENDIAN 1234
#define TAF_BIG_ENDIAN 4321

#define TAF_BYTE_ORDER TAF_LITTLE_ENDIAN

/* OS定义 */
#define TAF_WIN32 1
#define TAF_PSOS 2
#define TAF_VXWORKS 3
#define TAF_LINUX 4
#define TAF_UNIX 5
#define TAF_SOLARIS 6
#define TAF_TLINUX 7
#define TAF_HPUNIX 8
#define TAF_IBMUNIX 9
#define TAF_RTT 10
#define TAF_WINCE 11
#define TAF_NUCLEUS 12

#ifndef TAF_OS_VER
#define TAF_OS_VER TAF_VXWORKS
#endif

#ifndef CONST
#define CONST const
#endif

#ifndef CONST_T
#define CONST_T const
#endif

#ifndef STATIC
#if (defined(LLT_OS_VER))
#define STATIC
#else
#define STATIC static
#endif
#endif

#undef PUBLIC
#undef PRIVATE
#undef EXTERN

#define PUBLIC extern
#define EXTERN extern
#ifdef _EXPORT_PRIVATE
#define PRIVATE
#else
#define PRIVATE static
#endif

#undef OUT
#undef IN

#define OUT
#define IN

/* TAF API数据类型 */
typedef int           TAF_INT;
typedef signed char   TAF_INT8;
typedef unsigned char TAF_UINT8;

typedef signed short   TAF_INT16;
typedef unsigned short TAF_UINT16;

typedef signed int   TAF_INT32;
typedef unsigned int TAF_UINT32;

typedef char          TAF_CHAR;
typedef unsigned char TAF_UCHAR;
#if (VOS_WIN32 == VOS_OS_VER) && (LLT_OS_VER == LLT_LINUX)
#ifndef TAF_VOID
#define TAF_VOID void
#endif
#else
typedef void TAF_VOID;
#endif

typedef unsigned int TAF_BOOL;
enum {
    TAF_FALSE = 0,
    TAF_TRUE  = 1
};

typedef TAF_UINT8 MN_CLIENT_TYPE;
enum MN_CLIENT_Type {
    TAF_APP_CLIENT,
    TAF_AT_CLIENT,
    TAF_CLIENT_TYPE_BUTT
};

typedef TAF_UINT16 MN_CLIENT_ID_T;    /* APP/AT Client ID type */
typedef TAF_UINT8  MN_OPERATION_ID_T; /* Async operation ID type */

/* 表示所有的Client */
#define MN_CLIENT_ALL ((MN_CLIENT_ID_T)(-1))
#define MN_OP_ID_BUTT ((MN_OPERATION_ID_T)(-1))

#define TAF_NULL_PTR 0 /* null pointer */

#define TAF_MAX_STATUS_TI 1

#define TAF_ERR_CODE_BASE (0)         /* TAF层通用错误码定义 */
#define TAF_ERR_PHONE_BASE (150)      /* 电话管理错误码 */
#define TAF_ERR_PB_BASE (350)         /* 电话本错误码 */
#define TAF_ERR_SAT_BASE (400)        /* SAT错误码 */
#define TAF_ERR_SS_BASE (0x400)       /* SS错误码 */
#define TAF_ERR_LCS_BASE (0x500)      /* LCS错误码 */
#define TAF_ERR_SS_IMS_BASE (0x1000)  /* IMS回复的SS错误码 */
#define TAF_ERR_SS_IMSA_BASE (0x2000) /* IMSA回复的SS错误码 */
#define TAF_ERR_DECODE_BASE (0x2500)  /* 解码失败错误码 */

enum TAF_ERROR_Code {
    TAF_ERR_NO_ERROR   = (TAF_ERR_CODE_BASE),     /* 成功 */
    TAF_ERR_ERROR      = (TAF_ERR_CODE_BASE + 1), /* 失败 */
    TAF_ERR_NULL_PTR   = (TAF_ERR_CODE_BASE + 2), /* 空指针 */
    TAF_ERR_PARA_ERROR = (TAF_ERR_CODE_BASE + 3), /* 参数错误 */
    TAF_ERR_TIME_OUT   = (TAF_ERR_CODE_BASE + 4), /* 定时器超时 */
    /* CallId或者OpId或者Smi无效 */
    TAF_ERR_TAF_ID_INVALID = (TAF_ERR_CODE_BASE + 5),
    /* 号码有误(号码value无效) */
    TAF_ERR_NUM_VALUE_INVALID = (TAF_ERR_CODE_BASE + 6),
    /* 号码长度为0或者超出最大长度 */
    TAF_ERR_NUM_LEN_INVALID  = (TAF_ERR_CODE_BASE + 7),
    TAF_ERR_CAPABILITY_ERROR = (TAF_ERR_CODE_BASE + 8), /* 终端能力不支持 */
    /* 没有空闲ClientId，请求失败 */
    TAF_ERR_CLIENTID_NO_FREE    = (TAF_ERR_CODE_BASE + 9),
    TAF_ERR_CALLBACK_FUNC_ERROR = (TAF_ERR_CODE_BASE + 10), /* 回调函数错误 */
    TAF_ERR_MSG_DECODING_FAIL   = (TAF_ERR_CODE_BASE + 11), /* 消息解码失败 */
    TAF_ERR_TI_ALLOC_FAIL       = (TAF_ERR_CODE_BASE + 12), /* TI分配失败 */
    TAF_ERR_TI_GET_FAIL         = (TAF_ERR_CODE_BASE + 13), /* 获取Ti失败 */
    TAF_ERR_CMD_TYPE_ERROR      = (TAF_ERR_CODE_BASE + 14), /* 命令类型错误 */
    /* APP与TAF之间MUX链路建立失败 */
    TAF_ERR_MUX_LINK_EST_FAIL      = (TAF_ERR_CODE_BASE + 15),
    TAF_ERR_USIM_SIM_CARD_NOTEXIST = (TAF_ERR_CODE_BASE + 16), /* USIM卡不存在 */
    TAF_ERR_CLIENTID_NOT_EXIST     = (TAF_ERR_CODE_BASE + 17), /* ClientId不存在 */
    TAF_ERR_NEED_PIN1              = (TAF_ERR_CODE_BASE + 18), /* 需要输入PIN1码 */
    TAF_ERR_NEED_PUK1              = (TAF_ERR_CODE_BASE + 19), /* 需要输入PUK1 */
    TAF_ERR_USIM_SIM_INVALIDATION  = (TAF_ERR_CODE_BASE + 20), /* 无效的USIM/SIM卡 */
    TAF_ERR_SIM_BUSY               = (TAF_ERR_CODE_BASE + 21), /* SIM卡忙 */
    TAF_ERR_SIM_LOCK               = (TAF_ERR_CODE_BASE + 22), /* SIM卡锁卡 */
    TAF_ERR_SIM_INCORRECT_PASSWORD = (TAF_ERR_CODE_BASE + 23), /* 不正确的密码 */
    TAF_ERR_SIM_FAIL               = (TAF_ERR_CODE_BASE + 24), /* SIM卡操作失败 */
    /* 开机未本地初始化完成，接收工具的切换FTM模式请求 */
    TAF_ERR_NOT_READY          = (TAF_ERR_CODE_BASE + 25),
    TAF_ERR_FILE_NOT_EXIST     = (TAF_ERR_CODE_BASE + 26), /* OPL 文件不存在 */
    TAF_ERR_NO_NETWORK_SERVICE = (TAF_ERR_CODE_BASE + 27), /* 无网络 */
    TAF_ERR_GET_CSQLVL_FAIL    = (TAF_ERR_CODE_BASE + 28), /* 获取CSQLVL信息错误 */
    TAF_ERR_AT_ERROR           = (TAF_ERR_CODE_BASE + 29), /* 输出AT_ERROR */
    /* 输出AT_CME_OPERATION_NOT_SUPPORTED */
    TAF_ERR_CME_OPT_NOT_SUPPORTED = (TAF_ERR_CODE_BASE + 30),
    TAF_ERR_AT_CONNECT            = (TAF_ERR_CODE_BASE + 31), /* 输出AT_CONNECT */
    /* 获取(U)SIM卡USIM_SVR_OPLMN_LIST服务失败 */
    TAF_ERR_USIM_SVR_OPLMN_LIST_INAVAILABLE = (TAF_ERR_CODE_BASE + 32),
    TAF_ERR_FDN_CHECK_FAILURE               = (TAF_ERR_CODE_BASE + 33), /* FDN业务检查失败 */
    TAF_ERR_INTERNAL                        = (TAF_ERR_CODE_BASE + 34), /* 内部错误 */
    TAF_ERR_NET_SEL_MENU_DISABLE            = (TAF_ERR_CODE_BASE + 36), /* PLMN SEL菜单Disable */
    /* 设置syscfg时有CS/IMS业务存在 */
    TAF_ERR_SYSCFG_CS_IMS_SERV_EXIST = (TAF_ERR_CODE_BASE + 37),
    TAF_ERR_NEED_PUK2 = (TAF_ERR_CODE_BASE + 38), /* 需要输入PUK2 */
    /* USSD定时器超时 TAF_ERR_USSD_TIME_OUT => TAF_ERR_USSD_NET_TIMEOUT */
    TAF_ERR_USSD_NET_TIMEOUT = (TAF_ERR_CODE_BASE + 39),
    TAF_ERR_BUSY_ON_USSD     = (TAF_ERR_CODE_BASE + 40), /* 已经存在USSD业务 */
    TAF_ERR_BUSY_ON_SS       = (TAF_ERR_CODE_BASE + 41), /* 已经存在SS业务 */
    /* USSD业务等待APP回复超时 */
    TAF_ERR_USSD_USER_TIMEOUT = (TAF_ERR_CODE_BASE + 42),
    /* SS业务等待网络回复超时 */
    TAF_ERR_SS_NET_TIMEOUT          = (TAF_ERR_CODE_BASE + 43),
    TAF_ERR_USSD_TERMINATED_BY_USER = (TAF_ERR_CODE_BASE + 44), /* USSD业务被用户打断 */
    /* 逻辑通道上没有对应的数据单元 */
    TAF_ERR_NO_SUCH_ELEMENT             = (TAF_ERR_CODE_BASE + 45),
    TAF_ERR_MISSING_RESOURCE            = (TAF_ERR_CODE_BASE + 46), /* 无剩余通道资源 */
    TAF_ERR_SS_DOMAIN_SELECTION_FAILURE = (TAF_ERR_CODE_BASE + 47), /* 业务域选择失败 */
    /* 业务域选择缓存保护定时器超时 */
    TAF_ERR_SS_DOMAIN_SELECTION_TIMER_EXPIRED = (TAF_ERR_CODE_BASE + 48),
    /* 业务域选择收到关机指示清除缓存 */
    TAF_ERR_SS_POWER_OFF      = (TAF_ERR_CODE_BASE + 49),
    TAF_ERR_PHY_INIT_FAILURE  = (TAF_ERR_CODE_BASE + 50), /* 物理层初始化失败 */
    TAF_ERR_UNSPECIFIED_ERROR = (TAF_ERR_CODE_BASE + 51), /* 其他错误类型 */
    TAF_ERR_NO_RF             = (TAF_ERR_CODE_BASE + 52), /* NO RF */

    TAF_ERR_IMS_NOT_SUPPORT   = (TAF_ERR_CODE_BASE + 53), /* IMS不支持 */
    TAF_ERR_IMS_SERVICE_EXIST = (TAF_ERR_CODE_BASE + 54), /* IMS服务存在 */
    /* IMS语音优选域为PS_ONLY */
    TAF_ERR_IMS_VOICE_DOMAIN_PS_ONLY = (TAF_ERR_CODE_BASE + 55),
    TAF_ERR_IMS_STACK_TIMEOUT        = (TAF_ERR_CODE_BASE + 56), /* IMS协议栈超时 */

    TAF_ERR_1X_RAT_NOT_SUPPORTED = (TAF_ERR_CODE_BASE + 57), /* 当前1X RAT不支持 */

    /* USSI不支持SRVCC，CALL触发SRVCC场景主动退出会话 */
    TAF_ERR_NOT_SUPPORT_SRVCC = (TAF_ERR_CODE_BASE + 58),

    /* ^SILENTPIN PIN密文解密失败 */
    TAF_ERR_SILENT_AES_DEC_PIN_FAIL = (TAF_ERR_CODE_BASE + 59),

    /* ^SILENTPIN PIN码解锁失败 */
    TAF_ERR_SILENT_VERIFY_PIN_ERR = (TAF_ERR_CODE_BASE + 60),

    /* ^SILENTPIN PIN码加密失败 */
    TAF_ERR_SILENT_AES_ENC_PIN_FAIL = (TAF_ERR_CODE_BASE + 61),

    /* 打开ims开关时，不支持lte */
    TAF_ERR_IMS_OPEN_LTE_NOT_SUPPORT = (TAF_ERR_CODE_BASE + 62),

    TAF_ERR_NOT_FIND_FILE      = (TAF_ERR_CODE_BASE + 63), /* 文件不存在 */
    TAF_ERR_NOT_FIND_NV        = (TAF_ERR_CODE_BASE + 64), /* NV不存在 */
    TAF_ERR_MODEM_ID_ERROR     = (TAF_ERR_CODE_BASE + 65), /* MODEM ID 参数错误 */
    TAF_ERR_NV_NOT_SUPPORT_ERR = (TAF_ERR_CODE_BASE + 66), /* 不支持错误 */
    TAF_ERR_WRITE_NV_TIMEOUT   = (TAF_ERR_CODE_BASE + 67), /* 写nv超时 */

    TAF_ERR_NETWORK_FAILURE = (TAF_ERR_CODE_BASE + 68), /* 网络原因失败 */

    /* PAM 逻辑通道打开失败，AP需要识别，细化新错误类型 */
    TAF_ERR_SCI_ERROR = (TAF_ERR_CODE_BASE + 69),

    /* EMAT 打开逻辑通道错误 */
    TAF_ERR_EMAT_OPENCHANNEL_ERROR = (TAF_ERR_CODE_BASE + 70),
    /* EMAT 打开逻辑通道回复错误 */
    TAF_ERR_EMAT_OPENCHANNEL_CNF_ERROR = (TAF_ERR_CODE_BASE + 71),
    /* EMAT 关闭逻辑通道错误 */
    TAF_ERR_EMAT_CLOSECHANNEL_ERROR = (TAF_ERR_CODE_BASE + 72),
    /* EMAT 关闭逻辑通道回复错误 */
    TAF_ERR_EMAT_CLOSECHANNEL_CNF_ERROR = (TAF_ERR_CODE_BASE + 73),
    TAF_ERR_EMAT_GETEID_ERROR           = (TAF_ERR_CODE_BASE + 74), /* EMAT 获取EID错误 */
    /* EMAT 获取EID的数据错误 */
    TAF_ERR_EMAT_GETEID_DATA_ERROR = (TAF_ERR_CODE_BASE + 75),
    TAF_ERR_EMAT_GETPKID_ERROR     = (TAF_ERR_CODE_BASE + 76), /* EMAT 获取PKID错误 */
    /* EMAT 获取PKID的数据错误 */
    TAF_ERR_EMAT_GETPKID_DATA_ERROR = (TAF_ERR_CODE_BASE + 77),
    TAF_ERR_EMAT_CLEANPROFILE_ERROR = (TAF_ERR_CODE_BASE + 78), /* EMAT 清除Profile错误 */
    /* EMAT 清除Profile的回复数据错误 */
    TAF_ERR_EMAT_CLEANPROFILE_DATA_ERROR = (TAF_ERR_CODE_BASE + 79),
    TAF_ERR_EMAT_CHECKPROFILE_ERROR      = (TAF_ERR_CODE_BASE + 80), /* EMAT 检测Profile错误 */
    /* EMAT 检测Profile的回复数据错误 */
    TAF_ERR_EMAT_CHECKPROFILE_DATA_ERROR = (TAF_ERR_CODE_BASE + 81),
    /* EMAT 获取数据的TPDU回复错误 */
    TAF_ERR_EMAT_TPDU_CNF_ERROR = (TAF_ERR_CODE_BASE + 82),
    /* EMAT 保存TPDU回复的数据错误 */
    TAF_ERR_EMAT_TPDU_DATASTORE_ERROR = (TAF_ERR_CODE_BASE + 83),
    /* eSIM切换设置drv返回错误 */
    TAF_ERR_ESIMSWITCH_SET_ERROR = (TAF_ERR_CODE_BASE + 84),
    /* eSIM切换设置当前卡激活，不允许切换 */
    TAF_ERR_ESIMSWITCH_SET_NOT_ENABLE_ERROR = (TAF_ERR_CODE_BASE + 85),
    TAF_ERR_ESIMSWITCH_QRY_ERROR            = (TAF_ERR_CODE_BASE + 86), /* eSIM切换查询错误 */

    TAF_ERR_PHONE_MSG_UNMATCH      = (TAF_ERR_PHONE_BASE + 1), /* 消息关系不匹配 */
    TAF_ERR_PHONE_ATTACH_FORBIDDEN = (TAF_ERR_PHONE_BASE + 2), /* 禁止ATTACH过程 */
    TAF_ERR_PHONE_DETACH_FORBIDDEN = (TAF_ERR_PHONE_BASE + 3), /* 禁止DETACH过程 */

    TAF_ERR_PB_NOT_INIT             = (TAF_ERR_PB_BASE + 1), /* PB模块尚未初始化 */
    TAF_ERR_PB_MALLOC_FAIL          = (TAF_ERR_PB_BASE + 2), /* 分配内存失败 */
    TAF_ERR_PB_WRONG_INDEX          = (TAF_ERR_PB_BASE + 3), /* 错误的index */
    TAF_ERR_PB_WRONG_PARA           = (TAF_ERR_PB_BASE + 4), /* 错误的输入参数 */
    TAF_ERR_PB_STORAGE_FULL         = (TAF_ERR_PB_BASE + 5), /* 介质已满 */
    TAF_ERR_PB_STORAGE_OP_FAIL      = (TAF_ERR_PB_BASE + 6), /* 卡操作失败 */
    TAF_ERR_PB_NOT_FOUND            = (TAF_ERR_PB_BASE + 7), /* 无匹配记录 */
    TAF_ERR_PB_DIAL_STRING_TOO_LONG = (TAF_ERR_PB_BASE + 8), /* 输入的号码有错 */

    TAF_ERR_SAT_MALLOC_FAIL      = (TAF_ERR_SAT_BASE + 1), /* 分配内存失败 */
    TAF_ERR_SAT_WRONG_PARA       = (TAF_ERR_SAT_BASE + 2), /* 错误的输入参数 */
    TAF_ERR_SAT_STORAGE_OP_FAIL  = (TAF_ERR_SAT_BASE + 3), /* 卡操作失败 */
    TAF_ERR_SAT_STORAGE_OP_93_SW = (TAF_ERR_SAT_BASE + 4), /* 93回复 */
    TAF_ERR_SAT_NO_MAIN_MENU     = (TAF_ERR_SAT_BASE + 5), /* 没有主菜单 */

    TAF_ERR_LCS_METHOD_NOT_SUPPORTED          = (TAF_ERR_LCS_BASE + 0), /* 定位方法不支持 */
    TAF_ERR_LCS_ADDITIONAL_ASSIS_DATA_REQIRED = (TAF_ERR_LCS_BASE + 1), /* 需要额外的辅助数据 */
    TAF_ERR_LCS_NOT_ENOUGH_SATELLITES         = (TAF_ERR_LCS_BASE + 2), /* 没有足够的卫星 */
    TAF_ERR_LCS_UE_BUSY                       = (TAF_ERR_LCS_BASE + 3), /* 设备繁忙 */
    TAF_ERR_LCS_NETWORK_ERROR                 = (TAF_ERR_LCS_BASE + 4), /* 网络错误 */
    /* 打开网络连接失败，过多连接 */
    TAF_ERR_LCS_TOO_MANY_CONNECTIONS = (TAF_ERR_LCS_BASE + 5),
    /* 打开网络连接失败，用户过多 */
    TAF_ERR_LCS_TOO_MANY_USERS          = (TAF_ERR_LCS_BASE + 6),
    TAF_ERR_LCS_FAILURE_DUE_TO_HANDOVER = (TAF_ERR_LCS_BASE + 7),  /* 由于切换导致失败 */
    TAF_ERR_LCS_INTERNET_CONN_FAILURE   = (TAF_ERR_LCS_BASE + 8),  /* 网络连接失败 */
    TAF_ERR_LCS_MEMORY_ERROR            = (TAF_ERR_LCS_BASE + 9),  /* 内存错误 */
    TAF_ERR_LCS_UNKNOWN_ERROR           = (TAF_ERR_LCS_BASE + 10), /* 未知错误 */

    TAF_DECODE_ERR_NOT_SUPPORT_ENCODING_TYPE = (TAF_ERR_DECODE_BASE + 0), /* 不支持的编码类型 */
    /* UTF8数据单个字符超过目前支持最大字节数，目前最大支持3个字节 */
    TAF_DECODE_ERR_UTF8_BEYOND_MAX_BYTE_LIMIT = (TAF_ERR_DECODE_BASE + 1),
    /* UTF8数据字符帧头字节异常，指示字符包含的字节数非法，取值0,2,3，如果为1则非法 */
    TAF_DECODE_ERR_UTF8_ABNORMAL_BYTE_HEADER = (TAF_ERR_DECODE_BASE + 2),
    /* UTF8数据字符内容字节异常，非字符头字节的高位bit非"10" */
    TAF_DECODE_ERR_UTF8_ABNORMAL_BYTE_CONTENT = (TAF_ERR_DECODE_BASE + 3),
    /* 字节序转换时字节个数异常 */
    TAF_DECODE_ERR_BYTE_NUM_ABNORMAL = (TAF_ERR_DECODE_BASE + 4),

    TAF_ERR_CODE_BUTT = (0xFFFFFFFF)
};
typedef VOS_UINT32 TAF_ERROR_CodeUint32;

#ifndef TAF_SUCCESS
#define TAF_SUCCESS TAF_ERR_NO_ERROR /* 函数执行成功 */
#endif
#ifndef TAF_FAILURE
#define TAF_FAILURE TAF_ERR_ERROR /* 函数执行失败 */
#endif

typedef TAF_UINT8 TAF_PARA_TYPE;

#define TAF_PH_MS_CLASS_PARA 91

#define TAF_PH_IMSI_ID_PARA 98 /* +CIMI - 获取IMSI */

#define TAF_PH_NETWORKNAMEFROMUSIM_PARA 105 /* ^SPIN, 从USIM中获取当前运营商名字 */

#define TAF_PH_IDENTIFICATION_INFO_PARA 107
#define TAF_PH_CUR_FREQ 108
#define TAF_PH_ICC_ID 110
#define TAF_PH_CELL_RSCP_VALUE_PARA 111
#define TAF_PH_UE_RFPOWER_FREQ_PARA 112
#define TAF_PH_ICC_TYPE_PARA 114
#define TAF_PH_ICC_STATUS_PARA 115
#define TAF_PH_LOAD_DEFAULT 117
#define TAF_PH_PNN_PARA 120
#define TAF_PH_OPL_PARA 121

#define TAF_PH_CPNN_PARA 125
#define TAF_PH_PNN_RANGE_PARA 126
#define TAF_PH_OPL_RANGE_PARA 127

#define TAF_PH_SIMLOCK_VALUE_PARA (137) /* ^SIMLOCK=2获取数据卡的锁卡状态 */

#define TAF_TELE_PARA_BUTT (142)

/* 内部使用的参数查询宏定义 */
#define TAF_MMA_AT_QUERY_PARA_BEGIN (TAF_TELE_PARA_BUTT + 1) /* 137 */

/* 获取手机漫游状态 */
#define TAF_PH_ROAM_STATUS_PARA (TAF_MMA_AT_QUERY_PARA_BEGIN + 1) /* 138 */

/* 获取手机所处域信息 */
#define TAF_PH_DOMAIN_PARA (TAF_PH_ROAM_STATUS_PARA + 1) /* 139 */

/* GMR命令，获取mobile software revision, release date, release time */
#define TAF_PH_GMR_PARA (TAF_PH_DOMAIN_PARA + 1) /* 140 */

/* 产品名称，GMM，CGMM使用 */
#define TAF_PH_PRODUCT_NAME_PARA (TAF_PH_GMR_PARA + 1) /* 141 */

/* 参数设置结果 */
typedef TAF_UINT8 TAF_PARA_SET_RESULT;
#define TAF_PARA_OK 0                           /* 参数设置成功 */
#define TAF_PARA_SET_ERROR 1                    /* 设置参数错误 */
#define TAF_PARA_WRITE_NVIM_ERROR 2             /* 写NVIM失败 */
#define TAF_PARA_TYPE_NOT_SUPPORT 3             /* 不支持的参数类型 */
#define TAF_PARA_CID_NOT_SUPPORT 4              /* CID取值超出支持范围 */
#define TAF_PARA_NOT_INCLUDE_ALL_OP_IE 5        /* 参数设置非修改，未包含所有可选参数 */
#define TAF_PARA_IE_DECODE_ERROR 6              /* 消息解码失败 */
#define TAF_PARA_IE_ENCODE_ERROR 7              /* 消息编码失败 */
#define TAF_PARA_IE_VALUE_ERROR 8               /* 参数取值错误 */
#define TAF_PARA_CMD_NOT_MATCH_PARA 9           /* 无法对该参数执行操作 */
#define TAF_PARA_SIM_IS_BUSY 10                 /* SIM卡正忙 */
#define TAF_PARA_PDP_CONTEXT_NOT_DEFINED 11     /* PDP上下文未定义 */
#define TAF_PARA_SEC_PDP_CONTEXT_NOT_DEFINED 12 /* 从属PDP上下文未定义 */
#define TAF_PARA_TFT_NOT_DEFINED 13             /* TFT未定义 */
#define TAF_PARA_QOS_NOT_DEFINED 14             /* QOS未定义 */
#define TAF_PARA_MIN_QOS_NOT_DEFINED 15         /* MIN QOS未定义 */
#define TAF_PARA_SPN_NO_EXIST 16                /* spn文件不存在 */
#define TAF_PARA_ALLOC_MEM_FAIL 17              /* 查询参数时,申请内存失败 */
#define TAF_PARA_NO_USIM_ERROR 18               /* 查询imsi时，USIM不存 */
#define TAF_PARA_AUTH_NOT_DEFINED 19            /* AUTH未定义 */
#define TAF_PARA_MEMORY_FULL 20
#define TAF_PARA_INVALID_INDEX 21
#define TAF_PARA_NDIS_AUTHDATA_NOT_DEFINED 22 /* NDIS AUTHDATA未定义 */
#define TAF_PARA_EPS_QOS_NOT_DEFINED 23       /* EPS QOS未定义 */
#define TAF_PARA_INVALID_PLMNID 24
#define TAF_PARA_DUPLICATE_PLMNINFO 25
#define TAF_PARA_5G_QOS_NOT_DEFINED (26) /* 5G QOS未定义 */
#define TAF_PARA_UNSPECIFIED_ERROR 255   /* 其他错误 */

/*
 * 年，月，日，时，分，秒，均为BCD编码格式，
 *   高字节为10位数，低字节为个位数，即: 0x51 转为0x15,表示10进制数15
 *   时区为有符号整数，以15分钟为单位
 */
typedef struct {
    VOS_UINT8 year;
    VOS_UINT8 month;
    VOS_UINT8 day;
    VOS_UINT8 hour;
    VOS_UINT8 minute;
    VOS_UINT8 second;
    VOS_INT8  timeZone;
    VOS_UINT8 reserved;
} TIME_ZONE_Time;

/* 消息MMCMM_INFO_IND的结构体 */
typedef struct {
    VOS_UINT8      ieFlg;
    VOS_INT8       localTimeZone;
    VOS_UINT8      dst;
    VOS_UINT8      lsaid[3];
    VOS_UINT8      reserve[2];
    TIME_ZONE_Time universalTimeandLocalTimeZone;
} NAS_MM_InfoInd;


typedef struct {
    VOS_UINT32 modemId;
    VOS_UINT32 nvItemId;
    VOS_UINT32 nvLength;
    VOS_UINT32 isNeedCheck;
    VOS_UINT32 offset;
    VOS_VOID  *data;
} TAF_WRITE_AcoreNv;


typedef struct {
    TAF_UINT32 cellIdLowBit;
    TAF_UINT32 cellIdHighBit;
} TAF_CELL_Id;


enum TAFAGENT_MTA_ProcAcoreNvTpye {
    ID_TAFAGENT_MTA_PROC_ACORE_NV_TPYE_ENUM_NV_FLUSH,

    ID_TAFAGENT_MTA_PROC_ACORE_NV_TPYE_ENUM_NV_UPGRADE_BACKUP,

    ID_TAFAGENT_MTA_PROC_ACORE_NV_TPYE_ENUM_NV_BACKUP_FNV,

    ID_TAFAGENT_MTA_PROC_ACORE_NV_TPYE_ENUM_NV_FREVERT_FNV,

    ID_TAFAGENT_MTA_PROC_ACORE_NV_TPYE_ENUM_BUTT
};
typedef VOS_UINT32 TAFAGENT_MTA_ProcAcoreNvTpyeUint32;

#define TAF_REBOOT_MOD_ID_MEM 0x68000000
#define TAF_REBOOT_MOD_ID_BUTT 0X6FFFFFFF

#if (VOS_OS_VER == VOS_WIN32)
#define TAF_MEM_CMP(pucDestBuffer, pucSrcBuffer, ulBufferLen) memcmp(pucDestBuffer, pucSrcBuffer, ulBufferLen)
#else
#define TAF_MEM_CMP(pucDestBuffer, pucSrcBuffer, ulBufferLen) VOS_MemCmp(pucDestBuffer, pucSrcBuffer, ulBufferLen)

#endif

#define TAF_MEM_CHK_RTN_VAL(lReturnValue, ulDestLen, ulSrcLen)                         \
    {                                                                                  \
        TAF_STD_MemFunc_CheckReturnValue((lReturnValue), (ulDestLen), (ulSrcLen),      \
                                         (VOS_UINT)((THIS_FILE_ID << 16) | __LINE__)); \
    }

#define TAF_MEM_CHK_RTN_VAL_CONTINUE(lReturnValue, ulDestLen, ulSrcLen)                  \
    {                                                                                    \
        TAF_STD_MemFuncCheckResultContinue((lReturnValue), (ulDestLen), (ulSrcLen),      \
                                           (VOS_UINT)((THIS_FILE_ID << 16) | __LINE__)); \
    }
#define TAF_STRCPY_CHK_RTN_VAL_CONTINUE(lReturnValue, ulDestLen, ulSrcLen)                  \
    {                                                                                       \
        TAF_STD_StrcpyFuncCheckResultContinue((lReturnValue), (ulDestLen), (ulSrcLen),      \
                                              (VOS_UINT)((THIS_FILE_ID << 16) | __LINE__)); \
    }

#define TAF_SPRINTF_CHK_RTN_VAL_CONTINUE(lCharNum, ulDestLen, ulSrcLen)                      \
    {                                                                                        \
        TAF_STD_SprintfFuncCheckResultContinue((lCharNum), (ulDestLen), (ulSrcLen),          \
                                               (VOS_UINT)((THIS_FILE_ID << 16) | __LINE__)); \
    }
#define TAF_STRTOK_CHK_RTN_VAL_CONTINUE(strtoken)                                                       \
    {                                                                                                   \
        TAF_STD_StrtokFuncCheckResultContinue((strtoken), (VOS_UINT)((THIS_FILE_ID << 16) | __LINE__)); \
    }

#define TAF_MIN(x, y) (((x) < (y)) ? (x) : (y))

/* 三个值取最小 */
#define TAF_3_MIN(n1, n2, n3) TAF_MIN(TAF_MIN(n1, n2), n3)

#define TAF_BIT_TO_BYTE_LEN(a) (((a) + 7) / 8)

#if (OSA_CPU_ACPU == VOS_OSA_CPU) && (VOS_OS_VER != VOS_WIN32)
/* 仅供AT^NVSTUB使用 */
#define TAF_ACORE_NV_READ(modemid, id, item, len) mdrv_nv_readex(modemid, id, item, len)

#define TAF_ACORE_NV_READ_IN_CCORE(modemid, id, item, len) TAF_AGENT_ReadACoreEncryptionNv(modemid, id, item, len)
/* 对应mdrv_nv_writeex接口，在C需要对NV项做安全校验 */
#define TAF_ACORE_NV_WRITE(ulModemId, ulNvItemId, pData, ulNvLength) \
    TAF_AGENT_WriteACoreNv(AT_FillACoreNvWriteStru(ulModemId, ulNvItemId, pData, ulNvLength))

/* 对应mdrv_nv_write_partex接口，在C需要对NV项做安全校验 */
#define TAF_ACORE_NV_WRITE_PART(ulModemId, ulNvItemId, ulOffset, pData, ulNvLength) \
    TAF_AGENT_WriteACoreNv(AT_FillACoreNvWritePartStru(ulModemId, ulNvItemId, ulOffset, pData, ulNvLength))

#ifdef FEATURE_AT_NV_WRITE_SUPPORT
/* 对应mdrv_nv_writeex接口，仅供AT^NVWR使用，C核无需做安全校验，仅在ENG版本中开启 */
#define TAF_ACORE_NV_WRITE_NO_CHECK(ulModemId, ulNvItemId, pData, ulNvLength) \
    TAF_AGENT_WriteACoreNv(AT_FillACoreNvWriteNoCheckStru(ulModemId, ulNvItemId, pData, ulNvLength))

/* 对应mdrv_nv_write_partex接口，仅供AT^NVWRPART使用，C核无需做安全校验，仅在ENG版本中开启 */
#define TAF_ACORE_NV_WRITE_PART_NO_CHECK(ulModemId, ulNvItemId, ulOffset, pData, ulNvLength) \
    TAF_AGENT_WriteACoreNv(AT_FillACoreNvWritePartNoCheckStru(ulModemId, ulNvItemId, ulOffset, pData, ulNvLength))
#endif
/*  读取出厂区nv配置 */
#define TAF_ACORE_NV_READ_FACTORY(modemid, id, item, len) mdrv_nv_readex_factory(modemid, id, item, len)
/*  读取使用区nv配置 */
#define TAF_ACORE_NV_READ_IMG(modemid, id, item, len) mdrv_nv_readex_img(modemid, id, item, len)

#ifdef FEATURE_NV_SEC_ON
#define TAF_ACORE_NV_FLUSH() TAF_AGENT_ProcACoreNv(ID_TAFAGENT_MTA_PROC_ACORE_NV_TPYE_ENUM_NV_FLUSH)
#define TAF_ACORE_NV_UPGRADE_BACKUP(uloption) \
    TAF_AGENT_ProcACoreNv(ID_TAFAGENT_MTA_PROC_ACORE_NV_TPYE_ENUM_NV_UPGRADE_BACKUP)
#define TAF_ACORE_NV_BACKUP_FNV() TAF_AGENT_ProcACoreNv(ID_TAFAGENT_MTA_PROC_ACORE_NV_TPYE_ENUM_NV_BACKUP_FNV)
#define TAF_ACORE_NV_FREVERT_FNV() TAF_AGENT_ProcACoreNv(ID_TAFAGENT_MTA_PROC_ACORE_NV_TPYE_ENUM_NV_FREVERT_FNV)
#else
#define TAF_ACORE_NV_FLUSH() mdrv_nv_flush()
#define TAF_ACORE_NV_UPGRADE_BACKUP(uloption) mdrv_nv_backup()
#define TAF_ACORE_NV_BACKUP_FNV() mdrv_nv_backup_factorynv()
#define TAF_ACORE_NV_FREVERT_FNV() mdrv_nv_revert_factorynv()
#endif

#define TAF_ACORE_NV_READ_PART(modemid, id, off, item, len) mdrv_nv_read_partex(modemid, id, off, item, len)
#define TAF_ACORE_NV_GET_LENGTH(id, len) mdrv_nv_get_length(id, len)
#define TAF_ACORE_NV_RESTORE_RESULT() mdrv_nv_restore_result()
#define TAF_ACORE_NV_GET_NV_LIST_NUM() mdrv_nv_get_nvid_num()
#define TAF_ACORE_NV_GET_NV_ID_LIST(list) mdrv_nv_get_nvid_list(list)
#define TAF_ACORE_NV_UPGRADE_RESTORE() mdrv_nv_restore()
#define TAF_ACORE_NV_GET_REVERT_NUM(enNvItem) mdrv_nv_get_revert_num(enNvItem)
#define TAF_ACORE_NV_GET_REVERT_LIST(enNvItem, pusNvList, ulNvNum) mdrv_nv_get_revert_list(enNvItem, pusNvList, ulNvNum)
#endif

#if (VOS_WIN32 == VOS_OS_VER)
#ifdef DMT
#define TAF_ACORE_NV_READ(modemid, id, item, len) mdrv_nv_readex(modemid, id, item, len)

#define TAF_ACORE_NV_READ_IN_CCORE(modemid, id, item, len) TAF_AGENT_ReadACoreEncryptionNv(modemid, id, item, len)
VOS_UINT32 TAF_ACORE_NV_READ_FACTORY(VOS_UINT16 modemid, VOS_UINT16 id, VOS_VOID *item, VOS_UINT32 len);

#define TAF_ACORE_NV_WRITE_OLD(modemid, id, item, len) mdrv_nv_writeex(modemid, id, item, len)

#define TAF_ACORE_NV_WRITE(ulModemId, ulNvItemId, pData, ulNvLength) \
    TAF_AGENT_WriteACoreNv(AT_FillACoreNvWriteStru(ulModemId, ulNvItemId, pData, ulNvLength))

#define TAF_ACORE_NV_WRITE_NO_CHECK(ulModemId, ulNvItemId, pData, ulNvLength) \
    TAF_AGENT_WriteACoreNv(AT_FillACoreNvWriteNoCheckStru(ulModemId, ulNvItemId, pData, ulNvLength))

#define TAF_ACORE_NV_WRITE_PART(ulModemId, ulNvItemId, ulOffset, pData, ulNvLength) \
    TAF_AGENT_WriteACoreNv(AT_FillACoreNvWritePartStru(ulModemId, ulNvItemId, ulOffset, pData, ulNvLength))

#define TAF_ACORE_NV_WRITE_PART_NO_CHECK(ulModemId, ulNvItemId, ulOffset, pData, ulNvLength) \
    TAF_AGENT_WriteACoreNv(AT_FillACoreNvWritePartNoCheckStru(ulModemId, ulNvItemId, ulOffset, pData, ulNvLength))

#define TAF_ACORE_NV_GET_LENGTH(id, len) mdrv_nv_get_length(id, len)
#define TAF_ACORE_NV_FLUSH() TAF_AGENT_ProcACoreNv(ID_TAFAGENT_MTA_PROC_ACORE_NV_TPYE_ENUM_NV_FLUSH)
#define TAF_ACORE_NV_UPGRADE_BACKUP(uloption) \
    TAF_AGENT_ProcACoreNv(ID_TAFAGENT_MTA_PROC_ACORE_NV_TPYE_ENUM_NV_UPGRADE_BACKUP)
#define TAF_ACORE_NV_UPGRADE_RESTORE() mdrv_nv_restore()
#define TAF_ACORE_NV_BACKUP_FNV() TAF_AGENT_ProcACoreNv(ID_TAFAGENT_MTA_PROC_ACORE_NV_TPYE_ENUM_NV_BACKUP_FNV)
#define TAF_ACORE_NV_FREVERT_FNV() TAF_AGENT_ProcACoreNv(ID_TAFAGENT_MTA_PROC_ACORE_NV_TPYE_ENUM_NV_FREVERT_FNV)
#else
#define TAF_ACORE_NV_READ(modemid, id, item, len) mdrv_nv_readex(modemid, id, item, len)
#define TAF_ACORE_NV_READ_IN_CCORE(modemid, id, item, len) TAF_AGENT_ReadACoreEncryptionNv(modemid, id, item, len)

#define TAF_ACORE_NV_READ_FACTORY(modemid, id, item, len) mdrv_nv_readex_factory(modemid, id, item, len)

#define TAF_ACORE_NV_WRITE_OLD(modemid, id, item, len) mdrv_nv_writeex(modemid, id, item, len)

#define TAF_ACORE_NV_WRITE(ulModemId, ulNvItemId, pData, ulNvLength) \
    mdrv_nv_writeex(ulModemId, ulNvItemId, pData, ulNvLength)

#define TAF_ACORE_NV_WRITE_NO_CHECK(ulModemId, ulNvItemId, pData, ulNvLength) \
    mdrv_nv_writeex(ulModemId, ulNvItemId, pData, ulNvLength)

#define TAF_ACORE_NV_WRITE_PART(ulModemId, ulNvItemId, ulOffset, pData, ulNvLength) \
    mdrv_nv_write_partex(ulModemId, ulNvItemId, ulOffset, pData, ulNvLength)

#define TAF_ACORE_NV_WRITE_PART_NO_CHECK(ulModemId, ulNvItemId, ulOffset, pData, ulNvLength) \
    mdrv_nv_write_partex(ulModemId, ulNvItemId, ulOffset, pData, ulNvLength)

#define TAF_ACORE_NV_GET_LENGTH(id, len) mdrv_nv_get_length(id, len)
#define TAF_ACORE_NV_FLUSH() mdrv_nv_flush()
#define TAF_ACORE_NV_UPGRADE_BACKUP(uloption) mdrv_nv_backup()
#define TAF_ACORE_NV_UPGRADE_RESTORE() mdrv_nv_restore()
#define TAF_ACORE_NV_BACKUP_FNV() mdrv_nv_backup_factorynv()
#define TAF_ACORE_NV_FREVERT_FNV() mdrv_nv_revert_factorynv()

#endif
#endif

extern VOS_VOID TAF_STD_MemFuncCheckResultContinue(VOS_INT32 returnValue, VOS_UINT32 destLen, VOS_UINT32 srcLen,
                                                   VOS_UINT32 fileIdAndLine);
extern VOS_VOID TAF_STD_StrcpyFuncCheckResultContinue(VOS_INT32 returnValue, VOS_UINT32 destLen, VOS_UINT32 srcLen,
                                                      VOS_UINT32 fileIdAndLine);
extern VOS_VOID TAF_STD_SprintfFuncCheckResultContinue(VOS_INT32 charNum, VOS_UINT32 destLen, VOS_UINT32 srcLen,
                                                       VOS_UINT32 fileIdAndLine);
extern VOS_VOID TAF_STD_StrtokFuncCheckResultContinue(VOS_UINT8 *strtoken, VOS_UINT32 fileIdAndLine);
extern VOS_VOID TAF_STD_MemFunc_CheckReturnValue(VOS_INT32 returnValue, VOS_UINT32 destLen, VOS_UINT32 srcLen,
                                                 VOS_UINT32 fileIdAndLine);

extern VOS_UINT32 TAF_STD_MemFunc_Min(VOS_UINT32 destSize, VOS_UINT32 srcSize);

extern VOS_UINT32         TAF_AGENT_ReadACoreEncryptionNv(VOS_UINT32 modemId, VOS_UINT32 nvItemId, VOS_UINT8 *dataPtr,
                                                          VOS_UINT32 length);
extern VOS_UINT32         TAF_AGENT_WriteACoreNv(TAF_WRITE_AcoreNv *writeAcoreNv);
extern VOS_UINT32         TAF_AGENT_ProcACoreNv(TAFAGENT_MTA_ProcAcoreNvTpyeUint32 procACoreNvType);
extern TAF_WRITE_AcoreNv* AT_FillACoreNvWriteStru(VOS_UINT32 modemId, VOS_UINT32 nvItemId, VOS_UINT8 *data,
                                                  VOS_UINT32 nvLength);
extern TAF_WRITE_AcoreNv* AT_FillACoreNvWriteNoCheckStru(VOS_UINT32 modemId, VOS_UINT32 nvItemId, VOS_UINT8 *data,
                                                         VOS_UINT32 nvLength);
extern TAF_WRITE_AcoreNv* AT_FillACoreNvWritePartStru(VOS_UINT32 modemId, VOS_UINT32 nvItemId, VOS_UINT32 offset,
                                                      VOS_UINT8 *data, VOS_UINT32 nvLength);
extern TAF_WRITE_AcoreNv* AT_FillACoreNvWritePartNoCheckStru(VOS_UINT32 modemId, VOS_UINT32 nvItemId, VOS_UINT32 offset,
                                                             VOS_UINT8 *data, VOS_UINT32 nvLength);

extern MN_CLIENT_ID_T MN_GetRealClientId(MN_CLIENT_ID_T clientId, VOS_UINT32 pid);

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

#endif /* TAF_TYPE_DEF_H */
