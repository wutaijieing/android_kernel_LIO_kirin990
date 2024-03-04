/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
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

#ifndef CSS_AT_INTERFACE_H
#define CSS_AT_INTERFACE_H

#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#pragma pack(4)

#define MCC_INFO_VERSION_LEN (9)
#define AT_CSS_MAX_MCC_ID_NUM (17)
/* Added for BlackCell Feature 2017-4-22 begin */
#define AT_CSS_BLACK_CELL_LIST_VERSION_LEN (9)
/* Added for BlackCell Feature 2017-4-22 end */
/* add for CloudLine Feature 2017-6-28 begin */
#define AT_CSS_CLOUD_LINE_VERSION_LEN (9) /* 云端预置高铁线路版本号字符串长度 */
#define AT_CSS_PLMN_MAX_LINE_NUM (64)     /* 云端预置高铁线路一个PLMN下包含的最大线路个数 */
#define AT_CSS_TACLAC_MAX_LINE_NUM (10)   /* 云端预置高铁线路一个TAC/LAC所对应的最大线路个数 */
/* add for CloudLine Feature 2017-6-28 end */

/* Added for EMC, 2018-1-9, begin */
#define AT_CSS_MRU_MAX_NUM (10) /* MRU可以存储的最大个数 */
/* Added for EMC, 2018-1-9, end */


enum CSS_AT_MsgType {
    /* AT->CSS */
    ID_AT_CSS_MCC_INFO_SET_REQ          = 0x0001, /* _H2ASN_MsgChoice AT_CSS_MccInfoSetReq */
    ID_AT_CSS_MCC_VERSION_INFO_REQ      = 0x0002, /* _H2ASN_MsgChoice AT_CSS_MccVersionInfoReq */
    ID_AT_CSS_BLACK_CELL_LIST_SET_REQ   = 0x0003, /* _H2ASN_MsgChoice AT_CSS_BlackCellListSetReq */
    ID_AT_CSS_BLACK_CELL_LIST_QUERY_REQ = 0x0004, /* _H2ASN_MsgChoice AT_CSS_BlackCellListQueryReq */
    ID_AT_CSS_LINE_INDEX_LIST_SET_REQ   = 0x0005, /* _H2ASN_MsgChoice AT_CSS_LineIndexListSetReq */
    ID_AT_CSS_LINE_DETAIL_SET_REQ       = 0x0006, /* _H2ASN_MsgChoice AT_CSS_LineDetailSetReq */
    ID_AT_CSS_LINE_INDEX_LIST_QUERY_REQ = 0x0007, /* _H2ASN_MsgChoice AT_CSS_LineIndexListQueryReq */

    /* CSS->AT */
    ID_CSS_AT_MCC_INFO_SET_CNF          = 0x1001, /* _H2ASN_MsgChoice CSS_AT_MccInfoSetCnf */
    ID_CSS_AT_MCC_VERSION_INFO_CNF      = 0x1002, /* _H2ASN_MsgChoice CSS_AT_MccVersionInfoCnf */
    ID_CSS_AT_QUERY_MCC_INFO_NOTIFY     = 0x1003, /* _H2ASN_MsgChoice CSS_AT_QueryMccInfoNotify */
    ID_CSS_AT_BLACK_CELL_LIST_SET_CNF   = 0x1004, /* _H2ASN_MsgChoice CSS_AT_BlackCellListSetCnf */
    ID_CSS_AT_BLACK_CELL_LIST_QUERY_CNF = 0x1005, /* _H2ASN_MsgChoice CSS_AT_BlackCellListQueryCnf */
    ID_CSS_AT_BLACK_CELL_MCC_NOTIFY     = 0x1006, /* _H2ASN_MsgChoice CSS_AT_BlackCellMccNotify */
    ID_CSS_AT_LINE_INDEX_LIST_SET_CNF   = 0x1007, /* _H2ASN_MsgChoice CSS_AT_LineIndexListSetCnf */
    ID_CSS_AT_LINE_DETAIL_SET_CNF       = 0x1008, /* _H2ASN_MsgChoice CSS_AT_LineDetailSetCnf */
    ID_CSS_AT_LINE_INDEX_LIST_QUERY_CNF = 0x1009, /* _H2ASN_MsgChoice CSS_AT_LineIndexListQueryCnf */
    ID_CSS_AT_LINE_PLMN_NOTIFY          = 0x100a, /* _H2ASN_MsgChoice CSS_AT_LinePlmnNotify */
    ID_CSS_AT_LINE_INDEX_NOTIFY         = 0x100b, /* _H2ASN_MsgChoice CSS_AT_LineIndexNotify */

    ID_CSS_AT_MSG_BUTT
};
typedef VOS_UINT32 CSS_AT_MsgTypeUint32;

enum AT_CSS_Rat {
    AT_CSS_RAT_TYPE_GSM = 0, /* GSM接入技术 */
    AT_CSS_RAT_TYPE_WCDMA,   /* WCDMA接入技术 */
    AT_CSS_RAT_TYPE_LTE,     /* LTE接入技术 */

    AT_CSS_RAT_TYPE_BUTT
};
typedef VOS_UINT8 AT_CSS_RatUint8;

enum AT_CSS_SetMccOperateType {
    AT_CSS_SET_MCC_TYPE_ADD_MCC = 0,    /* 新增MCC */
    AT_CSS_SET_MCC_TYPE_DELETE_ALL_MCC, /* 删除所有MCC信息 */
    AT_CSS_SET_MCC_TYPE_DELETE_ONE_MCC, /* 删除一个MCC信息 */

    AT_CSS_SET_MCC_TYPE_BUTT
};
typedef VOS_UINT8 AT_CSS_SetMccOperateTypeUint8;

/* Added for BlackCell Feature 2017-4-22 begin */

enum AT_CSS_BlackCellListOperateType {
    AT_CSS_BLACK_CELL_LIST_ADD_ONE = 0, /* 新增一条黑小区信息 */
    AT_CSS_BLACK_CELL_LIST_DELETE_ALL,  /* 删除所有黑小区信息 */

    AT_CSS_BLACK_CELL_LIST_TYPE_BUTT
};
typedef VOS_UINT8 AT_CSS_BlackCellListOperateTypeUint8;
/* Added for BlackCell Feature 2017-4-22 end */

/* add for CloudLine Feature 2017-6-29 begin */
/* 结构说明: 高铁索引表操作类型 */
enum AT_CSS_LineIndexListOperateType {
    AT_CSS_LINE_INDEX_LIST_ADD = 0,    /* 新增高铁索引信息 */
    AT_CSS_LINE_INDEX_LIST_DELETE_ALL, /* 删除高铁索引信息 */

    AT_CSS_LINE_INDEX_LIST_TYPE_BUTT
};
typedef VOS_UINT8 AT_CSS_LineIndexListOperateTypeUint8;

/* 结构说明: 高铁高铁详细信息表操作类型 */
enum AT_CSS_LineDetailOperateType {
    AT_CSS_LINE_DETAIL_ADD = 0,    /* 新增高铁线路详细信息 */
    AT_CSS_LINE_DETAIL_DELETE_ALL, /* 删除高铁线路详细信息 */

    AT_CSS_LINE_DETAIL_TYPE_BUTT
};
typedef VOS_UINT8 AT_CSS_LineDetailOperateTypeUint8;

/* 结构说明: 高铁高铁详细信息表类型 */
enum AT_CSS_LineDetailInfoType {
    AT_CSS_LINE_DETAIL_INFO_TACLAC_PAIR = 0, /* TACLAC对 */
    AT_CSS_LINE_DETAIL_INFO_HO_PATH,         /* HO预置路径 */
    AT_CSS_LINE_DETAIL_INFO_HO_BAR,          /* HO预置Bar路径 */
    /* add for Cloudline Lte Feature 2018-4-16 begin */
    AT_CSS_LINE_DETAIL_INFO_LTE_HO_PATH, /* LTE预置路径 */
    /* add for Cloudline Lte Feature 2018-4-16 end */
    AT_CSS_LINE_DETAIL_INFO_NO_INFO = 0xFF /* 表示没有消息信息，停止解析 */
};
typedef VOS_UINT8 AT_CSS_LineDetailInfoTypeUint8;
/* add for CloudLine Feature 2017-6-29 end */

typedef struct {
    /*
     * MCC在aucMcc[2]中的存放格式,mcc为460:
     *              ||(BIT8)|(BIT7)|(BIT6)|(BIT5)|(BIT4)|(BIT3)|(BIT2)|(BIT1)
     *     ---------------------------------------------------------------------------
     * aucMcc[0]    ||    MCC digit 2 = 6        |           MCC digit 1 = 4
     *     ---------------------------------------------------------------------------
     * aucMcc[1]    ||    无效                   |           MCC digit 3 = 0
     */
    VOS_UINT8 mcc[2]; /* MCC ID */
    VOS_UINT8 rsv[2];
} AT_CSS_MccId;


typedef struct {
    VOS_UINT32 mcc;
    VOS_UINT32 mnc;
} CSS_AT_PlmnId;

typedef struct {
    VOS_UINT32 freqHigh;
    VOS_UINT32 freqLow;
} AT_CSS_FreqRange;

typedef struct {
    VOS_MSG_HEADER        /* _H2ASN_Skip */
    VOS_UINT32     msgId; /* _H2ASN_Skip */
    VOS_UINT16     clientId;
    VOS_UINT8      rsv[2];
} AT_CSS_MccVersionInfoReq;

typedef struct {
    VOS_MSG_HEADER        /* _H2ASN_Skip */
    VOS_UINT32     msgId; /* _H2ASN_Skip */
    VOS_UINT16     clientId;
    VOS_UINT8      rsv[1];
    VOS_UINT8      versionId[MCC_INFO_VERSION_LEN]; /* 版本号，固定为xx.xx.xxx */
} CSS_AT_MccVersionInfoCnf;


typedef struct {
    VOS_MSG_HEADER        /* _H2ASN_Skip */
    VOS_UINT32     msgId; /* _H2ASN_Skip */
    /* add for fill right clientId 2016-02-23, begin  */
    MODEM_ID_ENUM_UINT16 modemId;
    /* add for fill right clientId 2016-02-23, end  */
    VOS_UINT16                    clientId;
    VOS_UINT8                     seq;                             /* 流水号 */
    AT_CSS_SetMccOperateTypeUint8 operateType;                     /* 操作类型 */
    VOS_UINT8                     versionId[MCC_INFO_VERSION_LEN]; /* 版本号，固定为xx.xx.xxx */
    /* modified for fill right clientId 2016-02-23, begin  */
    VOS_UINT8 rsv[1];
    /* modified for fill right clientId 2016-02-23, end  */

    /*
     * 1)aucMccINfoBuff里存储的是MCC的信息，存储区的真实大小是ulMccInfoBuffLen里记录的字节数；
     * 2)aucMccINfoBuff里的格式为AT_CSS_MCC_INFO_STRU结构的格式，其中MNC个数,
     *   BAND个数，预置频段个数，预置频点的个数都是可变的。
     * typedef struct
     * {
     *     VOS_UINT8                           supportFlag;    // 1:表示支持GSM 2:表示支持WCDMA 4:表示支持LTE，三者可以自由组合
     *     AT_CSS_FreqRange              freqRange;
     * } AT_CSS_FreqRangeWithRat;
     * typedef struct
     * {
     *     VOS_UINT8                                   bandInd;
     *     VOS_UINT8                                   breqRangNum;
     *     //后面必须紧跟ucFreqRangNum个AT_CSS_FreqRangeWithRat结构
     *       的FreqRange信息，如果没有FreqRange，则需要将ucFreqRangNum填为0
     *     AT_CSS_FreqRangeWithRat             freqRangeArray[ucFreqRangNum];
     *     VOS_UINT8                                   preConfigFreqNum;
     *     //后面必须紧跟ucPreConfigFreqNum个AT_CSS_FreqRangeWithRat结构
     *       的PreConfigFreq信息，如果没有PreConfigFreq，则需要将ucPreConfigFreqNum填为0
     *     AT_CSS_FreqRangeWithRat             preConfigFreqArray[ucPreConfigFreqNum];      //预置频点列表
     * }AT_CSS_BandInfo;
     * typedef struct
     * {
     *     MNC在aucMnc[2]中的存放格式，mnc为01:
     *         ---------------------------------------------------------------------------
     *                  ||(BIT8)|(BIT7)|(BIT6)|(BIT5)|(BIT4)|(BIT3)|(BIT2)|(BIT1)
     *         ---------------------------------------------------------------------------
     *     aucMnc[0]    ||    MNC digit 3 = f        |           无效
     *         ---------------------------------------------------------------------------
     *     aucMnc[1]    ||    MNC digit 2 = 1        |           MNC digit 1 = 0
     *         ---------------------------------------------------------------------------
     *     VOS_UINT8                           aucMnc[2];
     *     VOS_UINT8                           ucBandCount;// BAND的个数
     *     //后面必须紧跟ucBandCount个band的信息，如果没有BAND，则需要将ucBandCount填为0
     *     AT_CSS_BandInfo               bandArray[ucBandCount];
     * }AT_CSS_MncInfo;
     * typedef struct
     * {
     *     MCC在aucMcc[2]中的存放格式,mcc为460:
     *         ---------------------------------------------------------------------------
     *                  ||(BIT8)|(BIT7)|(BIT6)|(BIT5)|(BIT4)|(BIT3)|(BIT2)|(BIT1)
     *         ---------------------------------------------------------------------------
     *     aucMcc[0]    ||    MCC digit 2 = 6        |           MCC digit 1 = 4
     *         ---------------------------------------------------------------------------
     *     aucMcc[1]    ||    无效                   |           MCC digit 3 = 0
     *         ---------------------------------------------------------------------------
     *     VOS_UINT8                           aucMcc[2];
     *     VOS_UINT8                           ucMncCount;// MNC的个数
     *     //后面必须紧跟ucMncCount个AT_CSS_MncInfo结构的mnc信息，如果没有mnc，则需要将ucMncCount填为0
     *     AT_CSS_MncInfo                mncAarry[ucMncCount];
     * }AT_CSS_MCC_INFO_STRU;
     * 3)aucMccINfoBuff中存储信息的格式AP与CSS直接对接，AT不会进行修改，AP先形成上述的格式，
     *   然后转成字符串格式发给AT，AT将字符串格式还原二进制码流，然后发给CSS；
     *   例如AP形成某一个字节为0x22，然后转化为字符串'22'，AT收到后再转成0x22;
     * 4)aucMccINfoBuff中的格式为小端；
     * 5)ulMccInfoBuffLen不能超过1.6K；
     */
    VOS_UINT32 mccInfoBuffLen;
    VOS_UINT8  mccInfoBuff[4];
} AT_CSS_MccInfoSetReq;

/* Added for BlackCell Feature 2017-4-22 begin */

typedef struct {
    VOS_MSG_HEADER                              /* _H2ASN_Skip */
        VOS_UINT32                       msgId; /* _H2ASN_Skip */
    VOS_UINT16                           clientId;
    VOS_UINT8                            seq;                                           /* 流水号 */
    AT_CSS_BlackCellListOperateTypeUint8 operateType;                                   /* 操作类型 */
    VOS_UINT8                            versionId[AT_CSS_BLACK_CELL_LIST_VERSION_LEN]; /* 版本号，固定为xx.xx.xxx */
    AT_CSS_RatUint8                      ratType;                                       /* 系统制式 */
    VOS_UINT8                            rsv[2];

    /*
     * 1)aucBlackCellListBuff里存储的是黑小区的信息，存储区的真实大小是ulBlackCellListBuffLen里记录的字节数；
     * 2)aucBlackCellListBuff里的格式为AT_CSS_GsmCloudBlackcell结构的格式
     * 码流参数BS1,BS2和BS3的编码格式相同，都按AT_CSS_GsmCloudBlackcell结构的格式编码：
     * 其中ucPairCount，astBlackListArray是可以变的
     * typedef struct
     * {
     *     CSS_PlmnId    plmn;
     *     VOS_UNIT8           pairCount;
     *     //后面必须紧跟ucPairCount个AT_CSS_GsmCloudBlackcellPair结构的pair信息，
     *       如果没有如果该mcc没有黑小区信息，AP不需要下发该MCC的信息，Modem认为该MCC没有黑小区信息，
     *       后续不再要求上报^REPORTBLACKCELLMCC来获取黑小区
     *     AT_CSS_GsmCloudBlackcellPair blackListPairArray[ucPairCount];
     * }AT_CSS_GsmCloudBlackcell;
     * typedef struct
     * {
     *     VOS_UNIT32  ulMcc; //Modem内部plmn格式;比如460---0x00000604
     *     VOS_UNIT32  ulMnc; //Modem内部plmn格式;比如 01---0x000F0100
     * }CSS_PlmnId;
     * typedef struct
     * {
     *     VOS_UNIT16  usSourceCellLai;
     *     VOS_UNIT16  usSourceCellId;
     *     VOS_UNIT16  usBlackCellArfcn; //包含了GSM的频段信息，比如0x1029表示GSM900频段,41频点
     *     VOS_UNIT8   ucBlackCellBsic;  //包含ncc,bcc;比如0x14表示<2,4>
     *     VOS_UNIT8   ucBlackCellReduceVal;//针对本黑小区惩罚时降低能量db值
     * }AT_CSS_GsmCloudBlackcellPair;
     */

    VOS_UINT32 blackCellListBuffLen;
    VOS_UINT8  blackCellListBuff[4];
} AT_CSS_BlackCellListSetReq;


typedef struct {
    VOS_MSG_HEADER        /* _H2ASN_Skip */
    VOS_UINT32     msgId; /* _H2ASN_Skip */
    VOS_UINT16     clientId;
    VOS_UINT8      rsv[2];
} AT_CSS_BlackCellListQueryReq;
/* Added for BlackCell Feature 2017-4-22 end */

typedef struct {
    VOS_MSG_HEADER        /* _H2ASN_Skip */
    VOS_UINT32     msgId; /* _H2ASN_Skip */
    VOS_UINT16     clientId;
    VOS_UINT8      seq; /* 流水号 */
    VOS_UINT8      rsv[1];

    VOS_UINT32 result; /* 0表示成功，1表示失败 */
} CSS_AT_MccInfoSetCnf;

typedef struct {
    VOS_MSG_HEADER        /* _H2ASN_Skip */
    VOS_UINT32     msgId; /* _H2ASN_Skip */
    VOS_UINT16     clientId;
    VOS_UINT8      rsv[1];
    VOS_UINT8      versionId[MCC_INFO_VERSION_LEN]; /* 版本号，固定为xx.xx.xxx */
    VOS_UINT32     mccNum;                          /* MCC 个数 */
    AT_CSS_MccId   mccId[AT_CSS_MAX_MCC_ID_NUM];    /* MCC ID列表 */
} CSS_AT_QueryMccInfoNotify;


typedef struct {
    VOS_UINT32 mccNum;                     /* MCC 个数 */
    VOS_UINT32 mcc[AT_CSS_MAX_MCC_ID_NUM]; /* MCC 列表,Modem内部plmn格式;比如460---0x00000604 */
} CSS_AT_BlackCellMccInfo;

/* Added for BlackCell Feature 2017-4-22 begin */

typedef struct {
    VOS_MSG_HEADER        /* _H2ASN_Skip */
    VOS_UINT32     msgId; /* _H2ASN_Skip */
    VOS_UINT16     clientId;
    VOS_UINT8      seq;   /* 流水号 */
    VOS_UINT8      reserved;
    VOS_UINT32     result; /* VOS_OK表示成功，VOS_ERR表示失败 */
} CSS_AT_BlackCellListSetCnf;


typedef struct {
    VOS_MSG_HEADER                 /* _H2ASN_Skip */
    VOS_UINT32              msgId; /* _H2ASN_Skip */
    VOS_UINT16              clientId;
    VOS_UINT8               reserved;
    VOS_UINT8               versionId[AT_CSS_BLACK_CELL_LIST_VERSION_LEN]; /* 版本号，固定为xx.xx.xxx */
    CSS_AT_BlackCellMccInfo mccInfo;                                       /* 黑小区MCC信息 */
} CSS_AT_BlackCellListQueryCnf;


typedef struct {
    VOS_MSG_HEADER                 /* _H2ASN_Skip */
    VOS_UINT32              msgId; /* _H2ASN_Skip */
    VOS_UINT16              clientId;
    VOS_UINT8               reserved;
    VOS_UINT8               versionId[AT_CSS_BLACK_CELL_LIST_VERSION_LEN]; /* 版本号，固定为xx.xx.xxx */
    CSS_AT_BlackCellMccInfo mccInfo;                                       /* 黑小区MCC信息 */
} CSS_AT_BlackCellMccNotify;
/* Added for BlackCell Feature 2017-4-22 end */

/* add for CloudLine Feature 2017-6-29 begin */


typedef struct {
    VOS_MSG_HEADER                              /* _H2ASN_Skip */
        VOS_UINT32                       msgId; /* _H2ASN_Skip */
    VOS_UINT16                           clientId;
    MODEM_ID_ENUM_UINT16                 modemId;
    VOS_UINT8                            seq;                                      /* 流水号 */
    AT_CSS_LineIndexListOperateTypeUint8 operateType;                              /* 操作类型 */
    VOS_UINT8                            versionId[AT_CSS_CLOUD_LINE_VERSION_LEN]; /* 版本号，固定为xx.xx.xxx */
    VOS_UINT8                            rsv;

    /*
     * 1)aucLineIndexListBuff里存储的是高铁线路索引的信息，存储区的真实大小是ulLineIndexListBuffLen里记录的字节数；
     * 2)aucLineIndexListBuff里的格式为AT_CSS_CLOUD_LINE_INDEX_LIST_INFO_STRU结构的格式
     * 当LineIndex为0xFFFF时表示该PLMN下没有高铁线路信息。
     * typedef struct
     * {
     *     CSS_PlmnId stPlmn;
     *     VOS_UNIT16          usLineIndex;
     *     VOS_UINT8           ucTacNum;
     *     VOS_UINT8           ucLacNum;
     *     //后面必须紧跟ucTacNum个VOS_UINT16的TAC列表信息。
     *     VOS_UINT16          ausTacList[ucTacNum];
     *     //后面必须紧跟ucLacNum个VOS_UINT16的LAC列表信息。
     *     VOS_UINT16          ausLacList[ucLacNum];
     * }AT_CSS_CloudLineIndexInfo;
     */
    VOS_UINT32 lineIndexListBuffLen;
    VOS_UINT8  lineIndexListBuff[4];
} AT_CSS_LineIndexListSetReq;


typedef struct {
    VOS_MSG_HEADER         /* _H2ASN_Skip */
    VOS_UINT32     msgId;  /* _H2ASN_Skip */
    VOS_UINT16     clientId;
    VOS_UINT8      seq;    /* 流水号 */
    VOS_UINT8      rsv;
    VOS_UINT32     result; /* VOS_OK表示成功，VOS_ERR表示失败 */
} CSS_AT_LineIndexListSetCnf;


typedef struct {
    VOS_MSG_HEADER                           /* _H2ASN_Skip */
    VOS_UINT32                        msgId; /* _H2ASN_Skip */
    VOS_UINT16                        clientId;
    MODEM_ID_ENUM_UINT16              modemId;
    VOS_UINT8                         seq;                                      /* 流水号 */
    AT_CSS_LineDetailOperateTypeUint8 operateType;                              /* 操作类型 */
    VOS_UINT8                         versionId[AT_CSS_CLOUD_LINE_VERSION_LEN]; /* 版本号，固定为xx.xx.xxx */
    VOS_UINT8                         rsv;

    /*
     * 1)aucLineDetailBuff里存储的是高铁线路索引的信息，存储区的真实大小是ulLineDetailBuffLen里记录的字节数。
     * 2)aucLineDetailBuff里的格式为AT_CSS_LineDetailInfo结构的格式:
     * typedef struct
     * {
     *     CSS_PlmnId                             stPlmn;
     *     VOS_UNIT16                                      usLineIndex;
     *     AT_CSS_LineDetailInfoTypeUint8          enInfoType;
     *     VOS_UINT8                                       aucLeftBuff[ulLineDetailBuffLen-11];
     * }AT_CSS_LineDetailInfo;
     * 3)AT_CSS_LineDetailInfo结构中的aucLeftBuff根据enInfoType的指示进行编码：
     *   i.当enInfoType为AT_CSS_LINE_DETAIL_INFO_TACLAC_PAIR时aucLeftBuff里的格式为AT_CSS_CloudLineTaclacMapInfo结构的格式：
     *     typedef struct
     *     {
     *         VOS_UITN16                                       usTac;
     *         VOS_UINT16                                       usLac;
     *     }AT_CSS_CloudLineTaclacPairInfo;
     *     typedef struct
     *     {
     *         VOS_UINT8                                        ucPairNum;
     *         //后面必须紧跟ucPairNum个AT_CSS_CloudLineTaclacPairInfo的信息。
     *         AT_CSS_CloudLineTaclacPairInfo          astCloudTacLacPairInfoList[ucPairNum];
     *     }AT_CSS_CloudLineTaclacMapInfo;
     *   ii.当enInfoType为AT_CSS_LINE_DETAIL_INFO_HO_PATH时aucLeftBuff里的格式为AT_CSS_CLOUD_LINE_HO_PATH_INFO_STRU结构的格式：
     *     typedef struct
     *     {
     *         VOS_UINT32                                       usSrcCellId;
     *         VOS_UINT16                                       usSrcArfcn;
     *         VOS_UINT16                                       usSrcBsic;
     *         VOS_UINT16                                       usDstArfcn;
     *         VOS_UINT16                                       usDstBsic;
     *         VOS_UINT16                                       usRxlevA;
     *         VOS_UINT16                                       usRxlevB;
     *     }AT_CSS_CLOUD_LINE_HO_PATH_CELL_INFO_STRU;
     *     typedef struct
     *     {
     *         VOS_UITN16                                       usLac;
     *         VOS_UINT8                                        ucCellNum;
     *         //后面必须紧跟ucCellNum个AT_CSS_CLOUD_LINE_HO_PATH_CELL_INFO_STRU的信息。
     *         AT_CSS_CLOUD_LINE_HO_PATH_CELL_INFO_STRU         astCellInfoList[ucCellNum];
     *     }AT_CSS_CLOUD_LINE_HO_PATH_LAC_INFO_STRU;
     *     typedef struct
     *     {
     *         VOS_UINT8                                        ucRat;
     *         VOS_UINT8                                        ucDirection;
     *         VOS_UINT8                                        ucLacNum;
     *         //后面必须紧跟ucLacNum个AT_CSS_CLOUD_LINE_HO_PATH_LAC_INFO_STRU的信息。
     *         AT_CSS_CLOUD_LINE_HO_PATH_LAC_INFO_STRU          astLacInfoList[ucLacNum];
     *     }AT_CSS_CLOUD_LINE_HO_PATH_INFO_STRU;
     *   iii.当enInfoType为AT_CSS_LINE_DETAIL_INFO_HO_BAR时aucLeftBuff里的格式为AT_CSS_CLOUD_LINE_HO_BAR_INFO_STRU结构的格式：
     *     typedef struct
     *     {
     *         VOS_UINT16                                       usLac;
     *         VOS_UINT32                                       usSrcCellId;
     *         VOS_UINT16                                       usSrcArfcn;
     *         VOS_UINT16                                       usSrcBsic;
     *         VOS_UINT16                                       usDstArfcn;
     *         VOS_UINT16                                       usDstBsic;
     *     }AT_CSS_CLOUD_LINE_HO_BAR_CELL_INFO_STRU;
     *     typedef struct
     *     {
     *         VOS_UINT8                                        ucRat;
     *         VOS_UINT8                                        ucCellNum;
     *         //后面必须紧跟ucCellNum个AT_CSS_CLOUD_LINE_HO_BAR_CELL_INFO_STRU的信息。
     *         AT_CSS_CLOUD_LINE_HO_BAR_CELL_INFO_STRU          astCellInfoList[ucCellNum];
     *     }AT_CSS_CLOUD_LINE_HO_BAR_INFO_STRU;
     *   iiii.当enInfoType为AT_CSS_LINE_DETAIL_INFO_NO_INFO时表示该PLMN/线路下没有详细信息，停止解析，modem不再向ap查询该PLMN/线路详细信息。
     *   iiiii.当enInfoType为以上类型的其他无效类型时，停止解析，直接返回失败。
     */
    VOS_UINT32 lineDetailBuffLen;
    VOS_UINT8  lineDetailBuff[4];
} AT_CSS_LineDetailSetReq;


typedef struct {
    VOS_MSG_HEADER         /* _H2ASN_Skip */
    VOS_UINT32     msgId;  /* _H2ASN_Skip */
    VOS_UINT16     clientId;
    VOS_UINT8      seq;    /* 流水号 */
    VOS_UINT8      rsv;
    VOS_UINT32     result; /* VOS_OK表示成功，VOS_ERR表示失败 */
} CSS_AT_LineDetailSetCnf;


typedef struct {
    VOS_MSG_HEADER              /* _H2ASN_Skip */
    VOS_UINT32           msgId; /* _H2ASN_Skip */
    VOS_UINT16           clientId;
    MODEM_ID_ENUM_UINT16 modemId;
} AT_CSS_LineIndexListQueryReq;


typedef struct {
    VOS_MSG_HEADER                 /* _H2ASN_Skip */
    VOS_UINT32          msgId;     /* _H2ASN_Skip */
    VOS_UINT16          clientId;
    VOS_UINT8           versionId[AT_CSS_CLOUD_LINE_VERSION_LEN]; /* 版本号 */
    VOS_UINT8           rsv;
    CSS_AT_PlmnId       rplmnInfo; /* rplmn信息 */
    VOS_UINT32          result;    /* 查询结果是否有效 */
} CSS_AT_LineIndexListQueryCnf;


typedef struct {
    VOS_MSG_HEADER         /* _H2ASN_Skip */
    VOS_UINT32      msgId; /* _H2ASN_Skip */
    VOS_UINT16      clientId;
    VOS_UINT8       rsv[2];
    CSS_AT_PlmnId   rplmnInfo; /* rplmn信息 */
} CSS_AT_LinePlmnNotify;


typedef struct {
    VOS_MSG_HEADER                                            /* _H2ASN_Skip */
    VOS_UINT32 msgId;                                         /* _H2ASN_Skip */
    VOS_UINT16     clientId;                                  /* _H2ASN_Skip */
    VOS_UINT16     lineNum;                                   /* 线路个数 */
    VOS_UINT16     lineIndexList[AT_CSS_TACLAC_MAX_LINE_NUM]; /* 线路列表 */
    CSS_AT_PlmnId  plmnId;                                    /* plmn id */
} CSS_AT_LineIndexNotify;

/* add for CloudLine Feature 2017-6-29 end */


typedef struct {
    VOS_UINT8       entry;
    AT_CSS_RatUint8 rat;
    VOS_UINT16      bandId;
    CSS_AT_PlmnId   plmnId;
} AT_CSS_Mru;

/* ASN解析结构 */
typedef struct {
    VOS_UINT32 msgId; /* _H2ASN_MsgChoice_Export CSS_AT_MsgTypeUint32  */
    VOS_UINT8  msg[4];
    /* _H2ASN_MsgChoice_When_Comment          CSS_AT_MSG_TYPE_ENUM_UINT32 */
} CSS_AT_InterfaceMsgData;
/* _H2ASN_Length UINT32 */

typedef struct {
    VOS_MSG_HEADER
    CSS_AT_InterfaceMsgData msgData;
} CSS_AT_InterfaceMsg;

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
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
