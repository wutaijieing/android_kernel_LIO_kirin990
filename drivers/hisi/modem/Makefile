ifneq ($(choose_modem_mode),eng)
ifeq ($(extra_modem),)
ifneq ($(wildcard $(srctree)/drivers/hisi/modem/config/product/$(OBB_PRODUCT_NAME)/customize/cust_modem_user),)
ifeq ($(TARGET_BUILD_VARIANT),user)
cust_config ?=cust_modem_user
endif
ifneq ($(cust_config),)
OBB_MODEM_CUST_CONFIG_DIR :=customize/$(cust_config)
export OBB_MODEM_CUST_CONFIG_DIR
endif
ifeq ($(cust_config),cust_modem_user)
choose_variant_modem :=user
export choose_variant_modem
endif
else
choose_variant_modem ?= $(TARGET_BUILD_VARIANT)
ifneq ($(choose_variant_modem),)
export choose_variant_modem
endif
endif
else
$(info "P+B kenel is compiling")
choose_variant_modem ?= $(TARGET_BUILD_VARIANT)
ifneq ($(choose_variant_modem),)
export choose_variant_modem
endif
endif
endif

include $(srctree)/drivers/hisi/modem/config/product/$(OBB_PRODUCT_NAME)/$(OBB_MODEM_CUST_CONFIG_DIR)/config/balong_product_config.mk
ifeq ($(strip $(MODEM_FULL_DUMP)),true)
subdir-ccflags-y += -DMODEM_FULL_DUMP_INUSER
endif

ifeq ($(strip $(MODEM_DDR_MINI_DUMP)),true)
subdir-ccflags-y += -DMODEM_DDR_MINI_DUMP_INUSER
endif

ifeq ($(strip $(MODEM_LOGCAT_INUSER)),true)
subdir-ccflags-y += -DMODEM_LOGCAT_INUSER
endif

ifeq ($(strip $(modem_log_filter_nv_control)),true)
subdir-ccflags-y += -DMODEM_LOG_FILTER_INUSER
endif

ifeq ($(strip $(MODEM_CHR_INUSER)),true)
subdir-ccflags-y += -DMODEM_CHR_INUSER
endif

ifeq ($(strip $(MODEM_PM_OM_INUSER)),true)
subdir-ccflags-y += -DMODEM_PM_OM_INUSER
endif

# build drv only config
OBB_SEPARATE        ?=$(separate)
ifeq ($(strip $(OBB_SEPARATE)),true)
subdir-ccflags-y += -DDRV_BUILD_SEPARATE
subdir-asflags-y += -DDRV_BUILD_SEPARATE
endif
# CONFIG_HISI_BALONG_EXTRA_MODEM A+B A�� defconfig �Ƿ����è
# makefile_soc_modem    ��A+B��̬���룬A+B B�����
# makefile_extra_modem  A+B A�����

ifeq ($(strip $(CONFIG_HISI_BALONG_EXTRA_MODEM)),y)
include $(srctree)/drivers/hisi/modem/makefile_extra_modem.mk

else
include $(srctree)/drivers/hisi/modem/makefile_soc_modem.mk

endif


