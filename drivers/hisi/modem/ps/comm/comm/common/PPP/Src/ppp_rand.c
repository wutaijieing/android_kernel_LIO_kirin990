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


#include "ppp_rand.h"
#include "securec.h"

#define THIS_FILE_ID PS_FILE_ID_PPP_RAND_C

/*
 * �������·������о���任������������С�
 *       C0  C1  C2  C3              C0  C1  C2  C3
 *  R0 | 0 | 1 | 2 | 3 |        R0 | 3 | 6 | 9 | C |
 *     -----------------           -----------------
 *  R1 | 4 | 5 | 6 | 7 |        R1 | 7 | A | D | 0 |
 *     -----------------   ==>     -----------------
 *  R2 | 8 | 9 | A | B |        R2 | B | E | 1 | 4 |
 *     -----------------           -----------------
 *  R3 | C | D | E | F |        R3 | F | 2 | 5 | 8 |
 */
#define PPP_MATRIX_TRANSFORM(dst, src) do { \
    dst[0]  = src[0][3];           \
    dst[1]  = src[1][2];           \
    dst[2]  = src[2][1];           \
    dst[3]  = src[3][0];           \
    dst[4]  = src[1][3];           \
    dst[5]  = src[2][2];           \
    dst[6]  = src[3][1];           \
    dst[7]  = src[0][0];           \
    dst[8]  = src[2][3];           \
    dst[9]  = src[3][2];           \
    dst[10] = src[0][1];           \
    dst[11] = src[1][0];           \
    dst[12] = src[3][3];           \
    dst[13] = src[0][2];           \
    dst[14] = src[1][1];           \
    dst[15] = src[2][0];           \
} while (0)

/* �������� */
VOS_UINT32 PPP_GenerateSeed(
    VOS_UINT8 aucSerial[], VOS_UINT8 serialLen, VOS_UINT32 startIndex, VOS_UINT32 disturbaceFactor)
{
    VOS_UINT32 loop;
    VOS_UINT32 seedVal;
    VOS_UINT8 *seed      = VOS_NULL_PTR;
    VOS_UINT8  tempValue = 0xFF;

    /* ����澯���ⲿ���뱣֤ucSerialLen����Ϊ0 */
    serialLen = TTF_MAX(1, serialLen);

    /* �����������ѡ�ĸ��ֽ���Ϊ���� */
    seed = (VOS_UINT8*)&seedVal;
    for (loop = 0; loop < sizeof(seedVal); loop++) {
        *seed++ = aucSerial[(VOS_UINT8)((startIndex + loop) % serialLen)];
    }

    /* ���� */
    seedVal += disturbaceFactor;

    /* �滻���ֽ� */
    seed = (VOS_UINT8*)&seedVal;
    for (loop = 0; loop < sizeof(seedVal); loop++) {
        if (*seed == 0) {
            *seed = tempValue--;
        }
        seed++;
    }

    return seedVal;
}

/* ����ȡϵͳ����(Tick/Slice/TaskId/HdlcHardStatics)�任����������С� */
VOS_VOID PPP_Get16ByteSerial(VOS_UINT8 aucSerial[])
{
    VOS_UINT8  srcSerial[4][4];
    VOS_UINT32 tick    = VOS_GetTick();
    VOS_UINT32 slice   = VOS_GetSlice();
    VOS_UINT32 taskId  = VOS_GetCurrentTaskID();
    VOS_UINT32 statSum = PPP_RAND_GET_STAT_SUM();

    /* ���ϵͳ����Ϊ0��������������� */
    tick    = (0x00UL != tick) ? tick : 0x12345678;
    slice   = (0x00UL != slice) ? slice : 0x9ABCDEF1;
    taskId  = (0x00UL != taskId) ? taskId : 0xFDB97531;
    statSum = (0x00UL != statSum) ? statSum : 0xECA8642F;

    /* ��ϵͳ������ϳ�һ������ */
    *((VOS_UINT32*)&srcSerial[0][0]) = tick;
    *((VOS_UINT32*)&srcSerial[1][0]) = slice;
    *((VOS_UINT32*)&srcSerial[2][0]) = taskId;
    *((VOS_UINT32*)&srcSerial[3][0]) = statSum;

    /* ���о���任�����µ�������� */
    PPP_MATRIX_TRANSFORM(aucSerial, srcSerial);

    return;
}

/* ��ȡ��ȫ����� */
VOS_VOID PPP_GetSecurityRand(VOS_UINT8 randByteLen, VOS_UINT8 *randData)
{
    VOS_UINT8        dictionary[256] = {0};
    VOS_UINT8        serial[16]      = {0};
    VOS_UINT8        value           = 0;
    VOS_UINT8        tempValue;
    VOS_UINT32       seed;
    VOS_UINT32       randIndex;
    VOS_UINT32       swapIndex;
    VOS_UINT32       loop;
    VOS_UINT32       start;
    VOS_UINT32       randSum;
    const VOS_UINT32 max = 0x100; /* ���ڻ�ȡ�����, �����ȡֵ��Χ��[0x00..0xFF] */

    /*
     * ����������ѧ�ڶ����ɵ�������ԭ����Ҫ��˼���ǽ����������ͨ��һ���Ľ�����
     * ����������˼���������˿��Ƶ�ϴ�ơ������˿����Ǵ�С�����˳��źõģ�ͨ
     * ����ε�ϴ��֮���˿��Ƶ�˳���Ծ�����ʧ�������������仰˵��ϵͳ����ֵ��
     * �󡣶������ֹ����ǲ�����ģ�ֻ�ܴ����������򣬲��ܴ�����������ʹ��һ
     * Щϵͳ���ݣ�ʱ��ֵ��TICKֵ���ڴ�ʹ������ϵͳ���á��жϡ���/�߳�ID��������
     * ����ԣ�����һ����ʽ����ȡ����Ũ��������������������������������������
     * �������������������
     */

    /*
     * ���ɰ�ȫ�������Ϊ����:
     * 1�������ֵ�
     * 2�������ֵ�Ԫ��
     */

    /* ��ȡ16byte������� */
    PPP_Get16ByteSerial(serial);

    /* �������� */
    seed = PPP_GenerateSeed(serial, (VOS_UINT8)sizeof(serial), 0, 0);
    VOS_SetSeed(seed);

    /* �����ֵ� */

    /* a�����������ʼλ��, ���ɵķ�Χ:[0..0xFF] */
    start   = VOS_Rand(max);
    randSum = start;

    /* b�������ֵ�ĺ�벿��: [ulStart��0xFF] */
    for (loop = start; loop < max; loop++) {
        dictionary[loop] = value;
        value++;
    }

    /* c�������ֵ��ǰ�벿��: [0, ulStart) */
    for (loop = 0; loop < start; loop++) {
        dictionary[loop] = value;
        value++;
    }

    /*
     * 2�������ֵ�Ԫ��
     * ����ucRandByteLen�ֽ���������У��������±�ulLoop = 0��ʼ��
     * ��������ֵ�Ԫ��(ulLoop��[ulLoop, 0xFF]����)�������ֵ����С�
     */
    for (loop = 0; loop < randByteLen; loop++) {
        /* �������� */
        seed = PPP_GenerateSeed(serial, (VOS_UINT8)sizeof(serial), loop, randSum);
        VOS_SetSeed(seed);

        /* ���������, ���ɵķ�Χ:[0..0xFF] */
        randIndex = VOS_Rand(max);
        randSum += randIndex;

        /* ���㽻����λ�ã���Χ:[ulLoop..0xFF] */
        swapIndex = (randIndex % (max - loop)) + loop;

        /* ����aucDictionary[ulLoop]��aucDictionary[ulSwapIndex] */
        tempValue             = dictionary[loop];
        dictionary[loop]      = dictionary[swapIndex];
        dictionary[swapIndex] = tempValue;
    }

    /* ��ȡucRandByteLen�ֽ�������� */
    (VOS_VOID)memcpy_s(randData, randByteLen, &(dictionary[0]), randByteLen);
    return;
}

