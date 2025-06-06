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

#ifndef _SOCP_IND_DELAY_H
#define _SOCP_IND_DELAY_H
#include <product_config.h>

#include "osl_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INDDELAY_NULL (void *)0

#define SOCP_MEM_VERSION  0x100

typedef struct {
    void *virt_addr;              /* fastboot预留的buffer虚拟BUFFER、在32位系统上是4字节，在64位系统上是8字节 */
    unsigned long phy_addr; /* fastboot预留的buffer物理地址 */
    unsigned int buff_size;     /* fastboot预留的buffer大小 */
    unsigned int timeout;        /* fastboot指定的数据传输超时时间, 只有在fastboot预留内存可用时才生效 */
    unsigned int buff_useable;      /* fastboot预留的buffer是否可用的标志 */
} socp_early_cfg_s;

s32 socp_get_logbuffer_logcfg(void);
void *socp_logbuffer_memremap(unsigned long phys_addr, size_t size);
s32 bsp_socp_ind_delay_init(void);
s32 bsp_socp_dst_init(void);
u32 bsp_socp_read_cur_mode(u32 dst_chan_id);
void bsp_socp_logbuffer_cfgshow(void);
void bsp_deflate_logbuffer_cfgshow(void);
void bsp_socp_logbuffer_early_cfgshow(void);
void bsp_socp_logbuffer_memreserve_cfgshow(void);

#ifdef __cplusplus
}
#endif

#endif
