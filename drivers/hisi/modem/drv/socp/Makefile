#gcov
ifeq ($(strip $(llt_gcov)),y)
GCOV_PROFILE := y
GCOV_PROFILE_socp_balong_test.o := n
GCOV_PROFILE_socp_api_test.o := n
endif

-include $(srctree)/drivers/hisi/modem/config/product/$(OBB_PRODUCT_NAME)/$(OBB_MODEM_CUST_CONFIG_DIR)/config/balong_product_config.mk
subdir-ccflags-y += -I$(srctree)/drivers/hisi/modem/drv/socp/encode/
subdir-ccflags-y += -I$(srctree)/drivers/hisi/modem/drv/socp/deflate/
KBUILD_CPPFLAGS += -Idrivers/hisi/modem/drv/socp/deflate
KBUILD_CPPFLAGS += -Idrivers/hisi/modem/drv/socp/encode

ifeq ($(strip $(CFG_DIAG_SYSTEM_5G)),YES)
obj-y       += socp_enc.o
obj-y       += encode/
else
obj-y       += socp_balong.o
endif
obj-y       += socp_ind_delay.o socp_debug.o
#socp_balong_test.o socp_balong_test_llt.o 

ifeq ($(strip $(CFG_CONFIG_DEFLATE)),YES)
obj-y       += deflate/
endif


ifneq ($(strip $(OBB_SEPARATE)),true) 
ifeq ($(strip $(CFG_CONFIG_DIAG_SYSTEM)),YES)
obj-y				+= soft_decode/
endif
endif

subdir-ccflags-y := -Werror
