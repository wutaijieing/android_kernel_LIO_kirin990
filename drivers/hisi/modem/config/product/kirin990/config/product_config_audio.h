/* MD5: d79eafb16427ce6a91c3bdb66389c6bb*/
/*
 * copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
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

#if !defined(__PRODUCT_CONFIG_AUDIO_H__)
#define __PRODUCT_CONFIG_AUDIO_H__

#ifndef TENSILICA_BUILDS_HIFI
#define TENSILICA_BUILDS_HIFI "$(ROOT_XTENSA_PATH_W)/XtDevTools/install/builds/RE-2014.5-linux" 
#endif 

#ifndef TENSILICA_TOOLS_HIFI
#define TENSILICA_TOOLS_HIFI "$(ROOT_XTENSA_PATH_W)/XtDevTools/install/tools/RE-2014.5-linux" 
#endif 

#ifndef FEATURE_HIFI
#define FEATURE_HIFI FEATURE_ON 
#endif 

#ifndef FEATURE_HIFI_USE_ICC
#define FEATURE_HIFI_USE_ICC FEATURE_ON 
#endif 

#ifndef FEATURE_MBB_MODULE_AUDIO
#define FEATURE_MBB_MODULE_AUDIO FEATURE_OFF 
#endif 

#ifndef FEATURE_VOICE_UP
#define FEATURE_VOICE_UP FEATURE_OFF 
#endif 

#ifndef PLATFORM_CHIP_TYPE
#define PLATFORM_CHIP_TYPE hi3660 
#endif 

#endif /*__PRODUCT_CONFIG_H__*/ 
