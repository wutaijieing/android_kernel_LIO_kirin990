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
#ifndef _BSP_SYSCTRL_H
#define _BSP_SYSCTRL_H

#ifdef __cplusplus /* __cplusplus */
extern "C" {
#endif /* __cplusplus */

#include <product_config.h>
#include <bsp_print.h>

typedef enum {
    SYSCTRL_AO = 0x0, /* SYSCTRL_AO(V722/V711/V765)、sctrl(phone/M535/M533) */
    SYSCTRL_PD,
    SYSCTRL_MDM,
    SYSCTRL_AP_PERI, /* peri_sysctrl(V765)  pctrl (phone/M535/M533) */
    SYSCTRL_AP_PERICRG,
    CRG_MODEM5G,
    SYSCTRL_MODEM5G,
    SYSCTRL_CCPU5G,
    SYSCTRL_LL1C,
    SYSCTRL_L2HAC,
    CRG_AO,
    CRG_MDM,
    SYSCTRL_RESERVE1, /* 预留系统控制器 */
    SYSCTRL_RESERVE2, /* 预留系统控制器 */
    SYSCTRL_RESERVE3, /* 预留系统控制器 */

    SYSCTRL_MAX
} sysctrl_index_e;

#define sc_err(fmt, ...) (bsp_err(fmt, ##__VA_ARGS__))
#define sc_wrn(fmt, ...) (bsp_wrn(fmt, ##__VA_ARGS__))
#define sc_info(fmt, ...) (bsp_info(fmt, ##__VA_ARGS__))
#define sc_debug(fmt, ...) (bsp_debug(fmt, ##__VA_ARGS__))

#define sc_err_func(fmt, ...) (bsp_err("<%s> " fmt, __FUNCTION__, ##__VA_ARGS__))
#define sc_wrn_func(fmt, ...) (bsp_wrn("<%s> " fmt, __FUNCTION__, ##__VA_ARGS__))
#define sc_info_func(fmt, ...) (bsp_info("<%s> " fmt, __FUNCTION__, ##__VA_ARGS__))
#define sc_debug_func(fmt, ...) (bsp_debug("<%s> " fmt, __FUNCTION__, ##__VA_ARGS__))

#ifdef CONFIG_SYSCTRL
void *bsp_sysctrl_addr_get(void *phy_addr);
void *bsp_sysctrl_addr_byindex(sysctrl_index_e index);
void *bsp_sysctrl_addr_phy_byindex(sysctrl_index_e index);
#else /* CONFIG_SYSCTRL */
static inline void *bsp_sysctrl_addr_get(void *phy_addr)
{
    return 0;
}
static inline void *bsp_sysctrl_addr_byindex(sysctrl_index_e index)
{
    return 0;
}
static inline void *bsp_sysctrl_addr_phy_byindex(sysctrl_index_e index)
{
    return 0;
}
#endif

void system_status_init(void);

/*****************************************************************************
 * 函 数 名  : get_system_status
 * 功能描述  : 判断芯片当前状态
 * 输入参数  : 无
 * 输出参数  : 无
 * 返 回 值  : 1:  系统没有掉电情况下发生过重启
 *             0:  掉电重启
 *            -1:  没有初始化、或芯片不支持该功能、或魔数被其他组件误改
 *****************************************************************************/
int get_system_status(void);

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif
