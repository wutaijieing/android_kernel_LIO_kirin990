HI110X_DRIVER_BUILTIN_PATH ?= $(subst ",,$(CONFIG_HI11XX_DRIVER_PATH))
$(warning HI110X_DRIVER_BUILTIN_PATH $(HI110X_DRIVER_BUILTIN_PATH))
DRIVER_FLODER = wifi
export _PRE_PRODUCT_VERSION ?= 1105
export HI110X_BOARD_VERSION ?= default
export PLAT_DEFCONFIG_FILE  ?= wifi_$(_PRE_PRODUCT_VERSION)_$(HI110X_BOARD_VERSION)_defconfig

HI110X_COMM_DEFCONFIG := hi$(_PRE_PRODUCT_VERSION)_comm_defconfig

CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT ?= no

ifeq (Hi3751, $(findstring Hi3751,$(TARGET_DEVICE)))
PLAT_DEFCONFIG_FILE  := wifi_1105_$(HI110X_BOARD_VERSION)_defconfig
HI110X_COMM_DEFCONFIG := hi1105_comm_defconfig
else ifeq (v818, $(findstring v818,$(TARGET_DEVICE)))
_PRE_PRODUCT_VERSION := 1103
PLAT_DEFCONFIG_FILE  := wifi_1105_$(HI110X_BOARD_VERSION)_defconfig
HI110X_COMM_DEFCONFIG := hi1105_comm_defconfig
BUILDIN_FLAG := $(HI110X_DRIVER_BUILTIN_PATH)
endif

BUILDIN_FLAG ?= $(KERNELRELEASE)

ifeq ($(CONFIG_PANGU_WIFI_DEVICE_HI110X), y)
_PRE_PRODUCT_VERSION := 1103
PLAT_DEFCONFIG_FILE  := wifi_1105_$(HI110X_BOARD_VERSION)_defconfig
HI110X_COMM_DEFCONFIG := hi1105_comm_defconfig
endif

ifneq ($(BUILDIN_FLAG),)
ifeq ($(HI1102_MAKE_FLAG),)
KERNEL_DIR := $(srctree)
HI110X_DRIVER_SRC_PATH=$(KERNEL_DIR)/$(HI110X_DRIVER_BUILTIN_PATH)/hi$(_PRE_PRODUCT_VERSION)
endif

DRIVER_PATH=$(HI110X_DRIVER_SRC_PATH)/$(DRIVER_FLODER)

include $(HI110X_DRIVER_SRC_PATH)/platform/$(HI110X_COMM_DEFCONFIG)
include $(DRIVER_PATH)/$(PLAT_DEFCONFIG_FILE)

$(warning defconfig: $(DRIVER_PATH)/$(PLAT_DEFCONFIG_FILE))

hmac-objs := hmac_main.o hmac_rx_data.o hmac_tx_amsdu.o hmac_tx_data.o hmac_user.o hmac_vap.o hmac_mgmt_bss_comm.o hmac_mgmt_classifier.o hmac_fsm.o hmac_mgmt_ap.o hmac_mgmt_sta.o hmac_encap_frame_ap.o hmac_encap_frame_sta.o hmac_encap_frame.o hmac_sme_sta.o hmac_11i.o hmac_uapsd.o hmac_config.o hmac_protection.o hmac_frag.o hmac_crypto_tkip.o hmac_chan_mgmt.o hmac_smps.o hmac_opmode.o hmac_rx_filter.o hmac_hcc_adapt.o hmac_reset.o hmac_scan.o hmac_p2p.o hmac_device.o hmac_resource.o  hmac_wapi_wpi.o hmac_wapi_sms4.o hmac_wapi.o hmac_cali_mgmt.o
hmac-objs += hmac_scan.o hmac_blockack.o hmac_dfs.o hmac_psm_ap.o hmac_dfx.o hmac_edca_opt.o hmac_arp_offload.o hmac_tcp_opt.o hmac_auto_adjust_freq.o hmac_roam_alg.o hmac_roam_connect.o hmac_roam_main.o hmac_blacklist.o hmac_rr_performance.o
hmac-objs += hmac_traffic_classify.o hmac_proxysta.o hmac_wmmac.o hmac_vowifi.o
hmac-objs += hmac_btcoex.o hmac_11v.o hmac_arp_probe.o hmac_twt.o hmac_mbo.o hmac_hiex.o hmac_dbac.o hmac_tx_msdu_dscr.o hmac_wifi6_self_cure.o hmac_tx_complete.o hmac_host_tx_data.o hmac_hid2d.o hmac_ht_self_cure.o hmac_host_al_tx.o
hmac-objs += hmac_package_params.o
hmac-objs := $(addprefix hmac/,$(hmac-objs))

mac-objs := mac_device.o mac_frame.o mac_user.o mac_vap.o mac_regdomain.o mac_ie.o mac_resource.o mac_board.o mac_data.o mac_hiex.o mac_ftm.o
mac-objs := $(addprefix hmac/,$(mac-objs))
mac-rom-objs := $(addprefix hmac/,$(mac-rom-objs))
hmac-objs += $(mac-objs)
hmac-objs += $(mac-rom-objs)

wal-objs := wal_config.o wal_config_debug.o wal_linux_bridge.o  wal_main.o wal_linux_scan.o wal_linux_rx_rsp.o wal_linux_event.o wal_regdb.o  wal_linux_flowctl.o  wal_linux_ioctl.o wal_linux_vendor.o wal_linux_netdev_ops.o wal_linux_ioctl_debug.o wal_linux_atcmdsrv.o
wal-objs += wal_linux_cfg80211.o wal_dfx.o wal_linux_cfgvendor.o wal_cfg_ioctl.o wal_linux_customize.o

wal-objs := $(addprefix wal/,$(wal-objs))

main-objs := main.o wlan_chip.o wlan_chip_1103.o wlan_chip_1106.o
main-objs := $(addprefix main/,$(main-objs))

hal-objs :=  hal_device.o hal_ring.o hal_main_host.o
hal-objs := $(addprefix hal/common/,$(hal-objs))

chip-objs :=  hal_dscr.o hal_mac.o hal_irq.o hal_main.o
chip-objs := $(addprefix hal/product/hi1106/,$(chip-objs))

#wifi.ko

wifi_$(_PRE_PRODUCT_VERSION)-objs := $(wal-objs)

wifi_$(_PRE_PRODUCT_VERSION)-objs += $(main-objs)

wifi_$(_PRE_PRODUCT_VERSION)-objs += $(hmac-objs)

wifi_$(_PRE_PRODUCT_VERSION)-objs += $(hal-objs)

wifi_$(_PRE_PRODUCT_VERSION)-objs += $(chip-objs)
#main.ko
ifeq ($(CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT), yes)
obj-m += wifi_$(_PRE_PRODUCT_VERSION).o
else
obj-y += wifi_$(_PRE_PRODUCT_VERSION).o
endif

HI110X_INCLUDES := -I$(DRIVER_PATH)/inc/hd_common
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/platform/inc
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/platform/board
HI110X_INCLUDES += -I$(DRIVER_PATH)/inc/hmac
HI110X_INCLUDES += -I$(DRIVER_PATH)/inc/wal
HI110X_INCLUDES += -I$(DRIVER_PATH)/inc
HI110X_INCLUDES += -I$(DRIVER_PATH)/hmac
HI110X_INCLUDES += -I$(DRIVER_PATH)/wal
HI110X_INCLUDES += -I$(DRIVER_PATH)/hal/common
HI110X_INCLUDES += -I$(DRIVER_PATH)/hal/product/hi1106
HI110X_INCLUDES += -I$(DRIVER_PATH)/inc/hal/hi1106
HI110X_INCLUDES += -I$(DRIVER_PATH)/inc/hal
#This is not good
ifneq ($(KERNEL_DIR),)
HI110X_INCLUDES += -I$(KERNEL_DIR)
endif
#HI110X_INCLUDES += -I$(SDK_INC_PATH)

HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/platform/inc/frw
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/platform/inc/oal
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/platform/inc/oal/linux
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/platform/inc/pm
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/platform/inc/oam
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/platform/oam
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/platform/pm
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/platform/oal
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/platform/frw
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/platform/sdt
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/common/customize
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/common/oneimage
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/common/inc
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/common/chr_log
HI110X_INCLUDES += -I$(HI110X_DRIVER_SRC_PATH)/bfgx


# 大屏编译需要该目录下头文件
ifeq (Hi3751, $(findstring Hi3751,$(TARGET_DEVICE)))
KERNEL_DIR := $(srctree)
HI110X_INCLUDES += -I$(SDK_DIR)/source/common/include
HI110X_INCLUDES += -I$(SDK_DIR)/source/msp/drv/include
KBUILD_CFLAGS += -D_PRE_PRODUCT_HI3751V811
endif
ifeq (v818, $(findstring v818,$(TARGET_DEVICE)))
KERNEL_DIR := $(srctree)
HI110X_INCLUDES += -I$(SDK_DIR)/source/common/include
HI110X_INCLUDES += -I$(SDK_DIR)/source/msp/drv/include
KBUILD_CFLAGS += -D_PRE_PRODUCT_HI3751V811
KBUILD_CFLAGS += -D_PRE_PRODUCT_HI3751_PLATO
HI110X_INCLUDES += -I$(SDK_DIR)/source/msp/include
endif

#$(warning INCLUDE: $(HI110X_INCLUDES))

ifeq ($(CONFIG_ARMPC_WIFI_DEVICE_HI110X), y)
KBUILD_CFLAGS += -DPLATFORM_DEBUG_ENABLE
KBUILD_CFLAGS += -DPLATFORM_SSI_FULL_LOG
endif

ifneq ($(TARGET_BUILD_VARIANT),user)
KBUILD_CFLAGS += -DPLATFORM_DEBUG_ENABLE
KBUILD_CFLAGS += -DPLATFORM_SSI_FULL_LOG
endif

EXTRA_CFLAGS = $(HI110X_INCLUDES) $(COPTS)
EXTRA_CFLAGS += $(COPTS)
EXTRA_CFLAGS += $(COMM_COPTS)
ifeq ($(CONN_NON_STRICT_CHECK),y)
$(warning hisi connectivity driver compile strict check disable)
else
EXTRA_CFLAGS += -Werror
endif
EXTRA_CFLAGS +=  -fno-pic
MODFLAGS = -fno-pic
else
#make modules
export HI1102_MAKE_FLAG=MODULES

ifeq (Hi3751, $(findstring Hi3751,$(TARGET_DEVICE)))
# 大屏Hi3751平台编译使用的参数
export HI110X_DRIVER_SRC_PATH ?= $(shell pwd)/..
MODULE_PARAM ?= ARCH=arm64 CROSS_COMPILE=aarch64-hisiv610-linux- CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT=yes
KERNEL_DIR ?= $(LINUX_SRC)
else ifeq (v818, $(findstring v818,$(TARGET_DEVICE)))
MODULE_PARAM ?= ARCH=arm64 CROSS_COMPILE=aarch64-hi100-linux- CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT=no
KERNEL_DIR ?= $(LINUX_SRC)
else
# 非Hi3751平台编译使用的参数
export HI110X_DRIVER_SRC_PATH ?= $(shell pwd)/..

ANDROID_PRODUCT=hi6210sft
ifeq ($(ARCH),arm64)
ANDROID_PATH ?= /home/zhouxinfeng/v8/v8r2-sft-bbit
MODULE_PARAM ?= ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
else
ANDROID_PATH ?= /home/zhouxinfeng/1102/v8r2-checkin
MODULE_PARAM ?= ARCH=arm CROSS_COMPILE=arm-eabi-
endif

KERNEL_DIR ?= $(ANDROID_PATH)/out/target/product/$(ANDROID_PRODUCT)/obj/KERNEL_OBJ
CROSS_DIR ?= $(ANDROID_PATH)/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin:$(ANDROID_PATH)/prebuilts/gcc/linux-x86/arm/gcc-linaro-aarch64-linux-gnu-4.8/bin
PATH := $(CROSS_DIR):$(PATH)

CONFIG_HI110X_KERNEL_MODULES_BUILD_SUPPORT ?= yes
endif # ifeq Hi3751

default:
	$(MAKE) -C $(KERNEL_DIR) $(MODULE_PARAM)  M=$(HI110X_DRIVER_SRC_PATH)/$(DRIVER_FLODER) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(HI110X_DRIVER_SRC_PATH)/$(DRIVER_FLODER) clean
endif
