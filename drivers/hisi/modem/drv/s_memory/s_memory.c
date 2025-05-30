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
 
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <soc_memmap.h>
#include <bsp_s_memory.h>
#include <bsp_shared_ddr.h>
#include <bsp_ddr.h>
#include <linux/of_reserved_mem.h>
#include <linux/of_fdt.h>
#include <linux/of.h>
#include <linux/vmalloc.h>
#include <securec.h>
#include <bsp_print.h>
#include <product_config.h>

#undef THIS_MODU
#define THIS_MODU mod_s_mem

/*lint -e528*/

#define s_mem_pr_err(fmt, ...) \
    (bsp_err("<%s> "fmt, __FUNCTION__, ##__VA_ARGS__))

struct mem_ctrl g_mem_ctrl;

int bsp_shared_mem_init(void)
{
    if (memset_s(&g_mem_ctrl, sizeof(struct mem_ctrl), 0x0, sizeof(struct mem_ctrl))) {
        s_mem_pr_err("memset g_mem_ctrl failed\n");
        return -1;
    }
    g_mem_ctrl.sddr_phy_addr  = (void*)HI_SHARED_DDR_BASE_ADDR;
    g_mem_ctrl.sddr_mem_size  = HI_SHARED_DDR_SIZE;
    g_mem_ctrl.sddr_virt_addr = (void*)ioremap_wc(
        (unsigned long)(uintptr_t)g_mem_ctrl.sddr_phy_addr, g_mem_ctrl.sddr_mem_size);
    s_mem_pr_err("shared_ddr phy=%10pK, virt=%10lx, size=%10x\n",
        g_mem_ctrl.sddr_phy_addr, (unsigned long)(uintptr_t)g_mem_ctrl.sddr_virt_addr, HI_SHARED_DDR_SIZE);
    s_mem_pr_err("ok!\n");
    return 0;
}

#ifndef CONFIG_HISI_BALONG_MODEM_MODULE
core_initcall(bsp_shared_mem_init); /*lint !e528*/
#endif
EXPORT_SYMBOL_GPL(g_mem_ctrl);

