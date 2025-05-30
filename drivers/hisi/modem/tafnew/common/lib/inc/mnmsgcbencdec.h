/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
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
#ifndef __MNMSGCBENCDEC_H__
#define __MNMSGCBENCDEC_H__

#include "vos.h"
#include "mn_msg_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if ((FEATURE_GCBS == FEATURE_ON) || (FEATURE_WCBS == FEATURE_ON))

#pragma pack(4)

#define MN_MSG_CB_GS_MASK 0xC000
#define MN_MSG_CB_MSG_CODE_HIGH_MASK 0x3F00
#define MN_MSG_CB_MSG_CODE_LOW_MASK 0x00F0
#define MN_MSG_CB_UPDATE_NUM_MASK 0x000F

#define MN_MSG_MAX_LANG_NUM 20

/* ISO639 language code */
enum MN_MSG_Iso639LangCode {
    MN_MSG_ISO639_LANG_GERMAN     = 0x6465,
    MN_MSG_ISO639_LANG_ENGLISH    = 0x656e,
    MN_MSG_ISO639_LANG_ITALIAN    = 0x6974,
    MN_MSG_ISO639_LANG_FRENCH     = 0x6672,
    MN_MSG_ISO639_LANG_SPANISH    = 0x6573,
    MN_MSG_ISO639_LANG_DUTCH      = 0x6e6c,
    MN_MSG_ISO639_LANG_SWEDISH    = 0x7376,
    MN_MSG_ISO639_LANG_DANISH     = 0x6461,
    MN_MSG_ISO639_LANG_PORTUGUESE = 0x7074,
    MN_MSG_ISO639_LANG_FINNISH    = 0x6569,
    MN_MSG_ISO639_LANG_NORWEGIAN  = 0x6e6f,
    MN_MSG_ISO639_LANG_GREEK      = 0x6772,
    MN_MSG_ISO639_LANG_TURKISH    = 0x7472,
    MN_MSG_ISO639_LANG_HUNGARIAN  = 0x6875,
    MN_MSG_ISO639_LANG_POLISH     = 0x706c,
    MN_MSG_ISO639_LANG_CZECH      = 0x6373,
    MN_MSG_ISO639_LANG_HEBREW     = 0x6865,
    MN_MSG_ISO639_LANG_ARABIC     = 0x6172,
    MN_MSG_ISO639_LANG_RUSSIAN    = 0x7275,
    MN_MSG_ISO639_LANG_ICELANDIC  = 0x6973,
    MN_MSG_ISO639_LANG_MAX        = 0xFFFF
};
typedef VOS_UINT16 MN_MSG_Iso639LangUint16;

/* Each CBS Page Info */

/* Refer to 23041 9.4.1.2 */
typedef struct {
    VOS_UINT8 snHigh;
    VOS_UINT8 snLow;
    VOS_UINT8 mIdHigh;
    VOS_UINT8 mIdLow;
    VOS_UINT8 dcs;
    /* bits 0 - 3 indicates the total number of pages in the CBS message */
    VOS_UINT8 pagesbit4 : 4;
    /* bits 4 - 7 indicates binary the page number within that sequence */
    VOS_UINT8 pagebit4 : 4;
    VOS_UINT8 content[TAF_CBA_MAX_CBDATA_LEN];
} MN_MSG_Cbgsmpage;

/* CBS����Ϣͷ */
typedef struct {
    VOS_UINT8  numOfPages;
    VOS_UINT8  dcs;
    VOS_UINT8  updateNum;
    VOS_UINT8  gs;
    VOS_UINT16 msgCode;
    VOS_UINT16 msgId;
} MN_MSG_CbPageHeader;

VOS_UINT32 MN_MSG_DecodeCbsDcs(VOS_UINT8 dcs, VOS_UINT8 *content, VOS_UINT32 contentLength, MN_MSG_CbdcsCode *dcsInfo);

#endif /* end of MnMsgCbEncDec.h */

extern VOS_UINT32 MN_MSG_DecodeDcsIf01(VOS_UINT8 dcs, VOS_UINT8 *content, VOS_UINT32 contentLength,
                                       MN_MSG_CbdcsCode *dcsInfo);

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

#endif /* __MNMSGCBENCDEC_H__ */
