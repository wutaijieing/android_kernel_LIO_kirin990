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

/*lint -save -e537*/
#include "bsp_nvim.h"
#include "drv_comm.h"
#include "nv_stru_sys.h"
#include "nv_factory_check.h"
/*lint -restore +e537*/

/*lint -save -e958 -e438*/


#define  NV_RESULT_CODE            0xffff

#ifdef CONFIG_NVIM

/*****************************************************************************
* 函 数 名  : mdrv_nv_readex
*
* 功能描述  : 读不同Modem NV项数据
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_readex(unsigned int modemid, unsigned int itemid, void *pdata, unsigned int ulLength)
{
    BSP_U32 card_id = 0;

    if(modemid == MODEM_ID_0)
        card_id = NV_USIMM_CARD_1;
    else if(modemid == MODEM_ID_1)
        card_id = NV_USIMM_CARD_2;
    else if(modemid == MODEM_ID_2)
        card_id = NV_USIMM_CARD_3;
    else
        return BSP_ERR_NV_INVALID_PARAM;
    return bsp_nvm_dcread(card_id, itemid,(BSP_U8*)pdata,ulLength);

}

unsigned int mdrv_nv_readex_factory(unsigned int modemid, unsigned int itemid, void *pdata, unsigned int ulLength)
{
    BSP_U32 card_id = 0;

    if(modemid == MODEM_ID_0)
        card_id = NV_USIMM_CARD_1;
    else if(modemid == MODEM_ID_1)
        card_id = NV_USIMM_CARD_2;
    else if(modemid == MODEM_ID_2)
        card_id = NV_USIMM_CARD_3;
    else
        return BSP_ERR_NV_INVALID_PARAM;
    return nv_readEx_factory(card_id, itemid, 0,(BSP_U8*)pdata,ulLength);

}
EXPORT_SYMBOL(mdrv_nv_readex_factory);

#ifndef FEATURE_NV_SEC_ON
/*****************************************************************************
* 函 数 名  : mdrv_nv_writeex
*
* 功能描述  : 写不同Modem NV项数据
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_writeex(unsigned int modemid, unsigned int itemid,void *pdata, unsigned int ulLength)
{
    BSP_U32 card_id = 0;

    if(modemid == MODEM_ID_0)
        card_id = NV_USIMM_CARD_1;
    else if(modemid == MODEM_ID_1)
        card_id = NV_USIMM_CARD_2;
    else if(modemid == MODEM_ID_2)
        card_id = NV_USIMM_CARD_3;
    else
        return BSP_ERR_NV_INVALID_PARAM;

    return bsp_nvm_dcwrite(card_id, itemid,(BSP_U8*)pdata,ulLength);

}

/*****************************************************************************
* 函 数 名  : mdrv_nv_write_partex
*
* 功能描述  : 写部分NV项数据
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_write_partex(unsigned int modemid, unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength)
{
    BSP_U32 card_id = 0;

    if(modemid == MODEM_ID_0)
        card_id = NV_USIMM_CARD_1;
    else if(modemid == MODEM_ID_1)
        card_id = NV_USIMM_CARD_2;
    else if(modemid == MODEM_ID_2)
        card_id = NV_USIMM_CARD_3;
    else
        return BSP_ERR_NV_INVALID_PARAM;

    return bsp_nvm_dcwritepart(card_id,itemid,ulOffset,(BSP_U8*)pdata,ulLength);
}

/*****************************************************************************
* 函 数 名  : mdrv_nv_write
*
* 功能描述  : 写NV项数据
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_write(unsigned int itemid, void *pdata, unsigned int ulLength)
{
    return bsp_nvm_write(itemid,pdata,ulLength);
}

/*****************************************************************************
* 函 数 名  : mdrv_nv_writepart
*
* 功能描述  : 写部分NV项数据
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_writepart(unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength)
{
    return bsp_nvm_writepart(itemid,ulOffset,pdata,ulLength);
}
/*****************************************************************************
* 函 数 名  : mdrv_nv_write_debug
*
* 功能描述  : 维测写NV接口，HIMS、AT^NVWR、AT^NVWRPART、CBT
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  :
*
*****************************************************************************/
unsigned int mdrv_nv_om_write(unsigned int modemid, unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength)
{
    return bsp_nvm_dcwritepart(modemid+1,itemid,ulOffset,(unsigned char*)pdata,ulLength);
}
#else
unsigned int mdrv_nv_writeex(unsigned int modemid, unsigned int itemid,void *pdata, unsigned int ulLength)
{
    return NV_ERROR;
}

unsigned int mdrv_nv_om_write(unsigned int modemid, unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength)
{
    return NV_ERROR;
}
#endif

/*****************************************************************************
* 函 数 名  : mdrv_nv_get_modem_num
*
* 功能描述  : 获取NV镜像支持的odem个数
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : modem个数
*
* 修改记录  :
*
*****************************************************************************/
unsigned int mdrv_nv_get_modem_num(void)
{
    return bsp_nvm_get_modem_num();
}

/*****************************************************************************
* 函 数 名  : mdrv_nv_get_length
*
* 功能描述  : 获取NV长度
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_get_length(unsigned int itemid, unsigned int *pulLength)
{
    BSP_U32 len = 0;
    BSP_U32 ret = 0;

    ret = bsp_nvm_get_len(itemid,&len);
    if(ret)
    {
        return ret;
    }
    *pulLength = len;
    return 0;
}

/*****************************************************************************
* 函 数 名  : mdrv_nv_read_partex
*
* 功能描述  : 读NV项数据
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_read_partex(unsigned int modemid, unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength)
{
    BSP_U32 card_id = 0;

    if(modemid == MODEM_ID_0)
        card_id = NV_USIMM_CARD_1;
    else if(modemid == MODEM_ID_1)
        card_id = NV_USIMM_CARD_2;
    else if(modemid == MODEM_ID_2)
        card_id = NV_USIMM_CARD_3;
    else
        return BSP_ERR_NV_INVALID_PARAM;

    return bsp_nvm_dcreadpart(card_id,itemid,ulOffset,(BSP_U8*)pdata,ulLength);

}

/*****************************************************************************
* 函 数 名  : mdrv_nv_read
*
* 功能描述  : 读NV项数据
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_read(unsigned int itemid, void *pdata, unsigned int ulLength)
{
    return bsp_nvm_read(itemid,pdata,ulLength);
}

/*****************************************************************************
* 函 数 名  : mdrv_nv_readpart
*
* 功能描述  : 读部分NV项数据
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_readpart(unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength)
{
    return bsp_nvm_readpart(itemid,ulOffset,pdata,ulLength);
}

/*****************************************************************************
* 函 数 名  : mdrv_nv_restore_result
*
* 功能描述  :  恢复结果检查
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_restore_result(void)
{
    BSP_U32 ret = NV_ERROR;
    BSP_U16 resume_flag = 0;

    ret = bsp_nvm_read(NV_ID_DRV_RESUME_FLAG,(BSP_U8*)&resume_flag,sizeof(BSP_U16)); /*to do*/
    if(ret)
    {
        return ret;
    }
    if(0 != resume_flag)
    {
        return NV_ERROR;
    }
    return 0;
}

/*****************************************************************************
* 函 数 名  : mdrv_nv_restore
*
* 功能描述  :  恢复生产NV项
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_restore(void)
{
    return 0;
}
void * mdrv_nv_get_addr(unsigned int modemid, unsigned int itemid)
{
    return bsp_nvm_get_addr(modemid+1,itemid);
}
EXPORT_SYMBOL(mdrv_nv_get_addr);
#ifndef FEATURE_NV_SEC_ON
/*****************************************************************************
* 函 数 名  : mdrv_nv_flush
*
* 功能描述  : 将内存中的nv数据刷到flash 文件系统中
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_flush(void)
{
   return bsp_nvm_flush() & NV_RESULT_CODE;
}

/*****************************************************************************
* 函 数 名  : mdrv_nv_backup
*
* 功能描述  :  备份NV数据
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_backup(void)
{
    return bsp_nvm_backup(NV_FLAG_NEED_CRC) & NV_RESULT_CODE;
}

/*****************************************************************************
* 函 数 名  : mdrv_nv_backup_factorynv
*
* 功能描述  : 备份NV到出厂区，用于AT命令^inforbu
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  :
*
*****************************************************************************/
unsigned int mdrv_nv_backup_factorynv(void)
{
    return bsp_nvm_update_default();
}

unsigned int mdrv_nv_revert_factorynv(void)
{
    return bsp_nvm_revert_default();
}
#else

/*****************************************************************************
* 函 数 名  : mdrv_nv_flush
*
* 功能描述  : 将内存中的nv数据刷到flash 文件系统中
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Yangzhi create
*
*****************************************************************************/
unsigned int mdrv_nv_flush(void)
{
   return NV_ERROR;
}

/*****************************************************************************
* 函 数 名  : mdrv_nv_backup
*
* 功能描述  :  备份NV数据
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : OK
*
*
*****************************************************************************/
unsigned int mdrv_nv_backup(void)
{
   return NV_ERROR;
}
#endif
/*****************************************************************************
* 函 数 名  : mdrv_nv_readex
*
* 功能描述  : 对校准NV进行CRC校验
*
* 输入参数  : u32 mode
*
* 输出参数  : 无
*
* 返 回 值  : OK
*
* 修改记录  : Fuxin create
*
*****************************************************************************/
unsigned int mdrv_nv_check_factorynv(u32 mode)
{
    return nv_check_factory_nv_status(mode);
}

#else

#include <osl_common.h>

unsigned int mdrv_nv_readex(unsigned int modemid, unsigned int itemid, void *pdata, unsigned int ulLength)
{
    return 0;
}

unsigned int mdrv_nv_writeex(unsigned int modemid, unsigned int itemid,void *pdata, unsigned int ulLength)
{
    return 0;
}

unsigned int mdrv_nv_read_partex(unsigned int modemid, unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength)
{
    return 0;
}

unsigned int mdrv_nv_write_partex(unsigned int modemid, unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength)
{
    return 0;
}

unsigned int mdrv_nv_flush(void)
{
    return 0;
}

unsigned int mdrv_nv_get_length(unsigned int itemid, unsigned int *pulLength)
{
    return 0;
}


unsigned int mdrv_nv_read(unsigned int itemid, void *pdata, unsigned int ulLength)
{
    return 0;
}

unsigned int mdrv_nv_write(unsigned int itemid, void *pdata, unsigned int ulLength)
{
    return 0;
}


unsigned int mdrv_nv_readpart(unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength)
{
    return 0;
}


unsigned int mdrv_nv_writepart(unsigned int itemid, unsigned int ulOffset, void *pdata, unsigned int ulLength)
{
    return 0;
}

unsigned int mdrv_nv_backup(void)
{
    return 0;
}

unsigned int mdrv_nv_restore_result(void)
{
    return 0;
}

unsigned int mdrv_nv_restore(void)
{
    return 0;
}

unsigned int mdrv_nv_backup_factorynv(void)
{
    return 0;
}

unsigned int mdrv_nv_revert_factorynv(void)
{
    return 0;
}

unsigned int mdrv_nv_check_factorynv(u32 mode)
{
    return 0;
}

u32 nv_readEx(u32 modem_id, u32 itemid, u32 offset, u8 * pdata, u32 datalen){return 0;}
u32 nv_writeEx(u32 modem_id, u32 itemid, u32 offset, u8 * pdata, u32 datalen){return 0;}

u32 bsp_nvm_dcread(u32 modem_id, u32 itemid, u8 * pdata, u32 datalen){return 0;}
u32 bsp_nvm_dcwrite(u32 modem_id, u32 itemid, u8 * pdata, u32 datalen){return 0;}

u32 bsp_nvm_dcreadpart(u32 modem_id, u32 itemid, u32 offset, u8 * pdata, u32 datalen){return 0;}
u32 bsp_nvm_dcwritepart(u32 modem_id, u32 itemid, u32 offset, u8 * pdata, u32 datalen){return 0;}

u32 bsp_nvm_dcread_direct(u32 modem_id, u32 itemid, u8 * pdata, u32 datalen){return 0;}
u32 bsp_nvm_dcwrite_direct(u32 modem_id, u32 itemid, u8 * pdata, u32 datalen){return 0;}

u32 bsp_nvm_auth_dcread(u32 modem_id, u32 itemid, u8 * pdata, u32 datalen){return 0;}
u32 bsp_nvm_auth_dcwrite(u32 modem_id, u32 itemid, u8 * pdata, u32 datalen){return 0;}
int  modem_nv_init(void){return 0;}

#endif

EXPORT_SYMBOL(mdrv_nv_restore);
EXPORT_SYMBOL(mdrv_nv_flush);
EXPORT_SYMBOL(mdrv_nv_backup);
EXPORT_SYMBOL(mdrv_nv_writeex);
#ifndef FEATURE_NV_SEC_ON
EXPORT_SYMBOL(mdrv_nv_write_partex);
EXPORT_SYMBOL(mdrv_nv_write);
EXPORT_SYMBOL(mdrv_nv_writepart);
EXPORT_SYMBOL(mdrv_nv_backup_factorynv);
EXPORT_SYMBOL(mdrv_nv_revert_factorynv);
#endif
EXPORT_SYMBOL(mdrv_nv_readex);
EXPORT_SYMBOL(mdrv_nv_read_partex);
EXPORT_SYMBOL(mdrv_nv_read);
EXPORT_SYMBOL(mdrv_nv_readpart);
EXPORT_SYMBOL(mdrv_nv_get_length);
EXPORT_SYMBOL(mdrv_nv_restore_result);
EXPORT_SYMBOL(mdrv_nv_check_factorynv);

/*lint -restore*/

