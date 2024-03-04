/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: ivp atf function header file
 * Author: chenweiyu
 * Create: 2018-02-09
 */

#ifndef _IVP_ATF_SUBSYS_H_
#define _IVP_ATF_SUBSYS_H_

#define IVP_SLV_SECMODE     0xC500BB00
#define IVP_MST_SECMODE     0xC500BB01
#define IVP_REMAP_SECMODE   0xC500BB02

enum secmode {
	IVP_SEC = 0,
	IVP_NONSEC
};

int ivpatf_change_slv_secmod(unsigned int mode);
int ivpatf_change_mst_secmod(unsigned int mode);
int ivpatf_poweron_remap_secmod(unsigned int ivp_addr,
	unsigned int len, unsigned long ddr_addr);

#endif /* _IVP_ATF_SUBSYS_H_ */
