#ifndef _HI_GLOBAL_MEM_MAP_INCLUDE_H_
#define _HI_GLOBAL_MEM_MAP_INCLUDE_H_ 
#ifdef CONFIG_PRODUCT_ARMPC
#define HISI_RESERVED_FASTBOOT_PHYMEM_BASE (0x3A400000)
#ifdef CONFIG_HISI_GCOV_FASTBOOT
#define HISI_RESERVED_FASTBOOT_PHYMEM_SIZE (0x800000)
#else
#define HISI_RESERVED_FASTBOOT_PHYMEM_SIZE (0x603000)
#endif
#define HISI_RESERVED_FASTBOOT_USED_PHYMEM_SIZE (0x4000000)
#define HISI_RESERVED_BL2_PHYMEM_BASE (0x1E400000)
#define HISI_RESERVED_BL2_PHYMEM_SIZE (0x00800000)
#define HISI_RESERVED_BL2_FST_BUFF_BASE (HISI_RESERVED_BL2_PHYMEM_BASE + HISI_RESERVED_BL2_PHYMEM_SIZE)
#define HISI_RESERVED_BL2_FST_BUFF_SIZE (0x01400000)
#define HISI_RESERVED_BL2_VERIFY_BUFF_BASE 0x40000000
#define HISI_RESERVED_BL2_VERIFY_BUFF_SIZE 0x04000000
#define HISI_RESERVED_FASTBOOT_AVB_HEAP_PHYMEM_BASE 0x44000000
#define HISI_RESERVED_FASTBOOT_AVB_HEAP_PHYMEM_SIZE (0x50000000 - 0x44000000)
#define HISI_RESERVED_DTB_PHYMEM_BASE 0x10000000
#define HISI_RESERVED_DTB_PHYMEM_END 0x10400000
#define HISI_RESERVED_DTB_PHYMEM_SIZE (HISI_RESERVED_DTB_PHYMEM_END - HISI_RESERVED_DTB_PHYMEM_BASE)
#define HISI_RESERVED_DTB_CAN_RUN_BASE (HISI_RESERVED_DTB_PHYMEM_BASE)
#define HISI_RESERVED_DTB_CAN_RUN_END (HISI_RESERVED_DTB_PHYMEM_END)
#define HISI_EXTEND_DTB_PHYMEM_BASE 0x10400000
#define HISI_EXTEND_DTB_PHYMEM_END 0x10E00000
#define HISI_EXTEND_DTB_PHYMEM_SIZE (HISI_EXTEND_DTB_PHYMEM_END - HISI_EXTEND_DTB_PHYMEM_BASE)
#define HISI_RESERVED_KERNEL_CAN_RUN_BASE 0
#define HISI_RESERVED_KERNEL_CAN_RUN_END 0x8000000
#define HISI_RESERVED_HHEE_PHYMEM_BASE 0x20000000
#define HISI_RESERVED_HHEE_PHYMEM_SIZE (0x600000)
#define HISI_HHEE_LOG_SIZE (0x20000)
#define HISI_HHEE_LOG_ADDR (HISI_RESERVED_HHEE_PHYMEM_BASE + HISI_RESERVED_HHEE_PHYMEM_SIZE - HISI_HHEE_LOG_SIZE)
#define HISI_RESERVED_SENSORHUB_SHMEMEXT_PHYMEM_BASE 0x20C00000
#define HISI_RESERVED_SENSORHUB_SHMEMEXT_PHYMEM_SIZE (0x200000)
#define HISI_RESERVED_SENSORHUB_SHMEM_PHYMEM_BASE 0x20E00000
#define HISI_RESERVED_SENSORHUB_SHMEM_PHYMEM_SIZE (0x40000)
#define HISI_RESERVED_SENSORHUB_SHARE_MEM_PHYMEM_BASE 0x20E40000
#define HISI_RESERVED_SENSORHUB_SHARE_MEM_PHYMEM_SIZE (0x80000)
#define HISI_RESERVED_LPMX_CORE_PHYMEM_BASE 0x20EC0000
#define HISI_RESERVED_LPMX_CORE_PHYMEM_SIZE (0x80000)
#define HISI_RESERVED_LPMCU_PHYMEM_BASE 0x20F40000
#define HISI_RESERVED_LPMCU_PHYMEM_SIZE 0xC0000
#define HISI_RESERVED_SENSORHUB_PHYMEM_BASE 0x21000000
#define HISI_RESERVED_SENSORHUB_PHYMEM_SIZE (0xD00000)
#define HISI_SENSORHUB_IMAGE_SIZE 0x380000
#define HISI_TEEOS_IMAGE_SIZE 0x600000
#define HISI_RESERVED_BL31_PHYMEM_BASE 0x22000000
#define HISI_RESERVED_BL31_PHYMEM_SIZE (0x800000)
#define HISI_RESERVED_TEEOS_PHYMEN_BASE 0x22808000
#define HISI_RESERVED_TEEOS_PHYMEN_SIZE (0x600000)
#define HISI_RESERVED_SECOS_PHYMEM_BASE 0x22800000
#define HISI_RESERVED_SECOS_PHYMEM_SIZE (0x3000000)
#define HISI_RESERVED_HIFI_PHYMEM_BASE 0x25800000
#define HISI_RESERVED_HIFI_PHYMEM_SIZE (0xF80000)
#define HISI_RESERVED_HIFI_DATA_PHYMEM_BASE 0x26780000
#define HISI_RESERVED_HIFI_DATA_PHYMEM_SIZE (0x580000)
#define HISI_RESERVED_DRV_RESERVE_MEM_PHYMEM_BASE 0x26D00000
#define HISI_RESERVED_DRV_RESERVE_MEM_PHYMEM_SIZE (0x100000)
#define HISI_RESERVED_FAST_KER_AND_PHYMEM_BASE 0x26D00000
#define HISI_RESERVED_FAST_KER_AND_PHYMEM_SIZE (0x40000)
#define HISI_SUB_RESERVED_FASTBOOT_LOG_PYHMEM_BASE 0x26D40000
#define HISI_SUB_RESERVED_FASTBOOT_LOG_PYHMEM_SIZE (0x40000)
#define HISI_SUB_RESERVED_SCHARGE_PYHMEM_BASE 0x26D80000
#define HISI_SUB_RESERVED_SCHARGE_PYHMEM_SIZE (0x1000)
#define HISI_SUB_RESERVED_BL31_SHARE_MEM_PHYMEM_BASE 0x26D81000
#define HISI_SUB_RESERVED_BL31_SHARE_MEM_PHYMEM_SIZE (0x40000)
#define HISI_SUB_RESERVED_LCD_GAMMA_MEM_PHYMEM_BASE 0x26DC1000
#define HISI_SUB_RESERVED_LCD_GAMMA_MEM_PHYMEM_SIZE (0x1000)
#define HISI_SUB_RESERVED_BRIGHTNESS_CHROMA_MEM_PHYMEM_BASE 0x26DC2000
#define HISI_SUB_RESERVED_BRIGHTNESS_CHROMA_MEM_PHYMEM_SIZE (0x1000)
#define HISI_SUB_RESERVED_SEC_RANDNUM_INJECT_PHYMEM_BASE 0x26DC3000
#define HISI_SUB_RESERVED_SEC_RANDNUM_INJECT_PHYMEM_SIZE (0x1000)
#define HISI_SUB_RESERVED_BL2_LOG_MEM_PHYMEM_BASE 0x26DC4000
#define HISI_SUB_RESERVED_BL2_LOG_MEM_PHYMEM_SIZE (0x10000)
#define HISI_SUB_RESERVED_UEFI_DDRINFO_SHARE_BASE 0x26DD4000
#define HISI_SUB_RESERVED_UEFI_DDRINFO_SHARE_SIZE (0x1000)
#define HISI_SUB_RESERVED_UNUSED_PHYMEM_BASE 0x26DD5000
#define HISI_SUB_RESERVED_UNUSED_PHYMEM_SIZE (0x26DFF000 - HISI_SUB_RESERVED_UNUSED_PHYMEM_BASE)
#define HISI_SUB_RESERVED_MNTN_DUMP_PHYMEM_BASE 0x26DFF000
#define HISI_SUB_RESERVED_MNTN_DUMP_PHYMEM_SIZE (0x1000)
#define HISI_RESERVED_PSTORE_PHYMEM_BASE 0x26E00000
#define HISI_RESERVED_PSTORE_PHYMEM_SIZE (0x100000)
#define HISI_RESERVED_MNTN_PHYMEM_BASE 0x26F00000
#define HISI_RESERVED_MNTN_PHYMEM_SIZE (0x860000)
#define HISI_RESERVED_SECOS_S4_BASE 0x27760000
#define HISI_RESERVED_SECOS_S4_SIZE (0x100000)
#define HISI_RESERVED_BL2_UFS_DATA_BASE 0x70000000
#define HISI_RESERVED_BL2_UFS_DATA_SIZE (0x1400000)
#define HISI_RESERVED_LPMX_CORE_PHYMEM_BASE_UNIQUE (HISI_RESERVED_LPMX_CORE_PHYMEM_BASE)
#define HISI_RESERVED_LPMCU_PHYMEM_BASE_UNIQUE (HISI_RESERVED_LPMCU_PHYMEM_BASE)
#define HISI_RESERVED_MNTN_PHYMEM_BASE_UNIQUE (HISI_RESERVED_MNTN_PHYMEM_BASE)
#define HISI_RESERVED_UEFI_RT_BASE 0x76000000ULL
#define HISI_RESERVED_UEFI_RT_SIZE 0x800000ULL
#define HISI_RESERVED_UEFI_RT_END (HISI_RESERVED_UEFI_RT_BASE + HISI_RESERVED_UEFI_RT_SIZE)
#define HISI_RESERVED_UEFI_BASE 0x78000000ULL
#define HISI_RESERVED_UEFI_SIZE 0x8000000ULL
#define HISI_RESERVED_UEFI_END (HISI_RESERVED_UEFI_BASE + HISI_RESERVED_UEFI_SIZE)
#define HISI_RESERVED_KERNEL_DUMP_PROTECT_BASE 0x80000000
#define HISI_RESERVED_KERNEL_DUMP_PRORECT_SIZE (0x20000000)
#ifdef CONFIG_HISI_GCOV_FASTBOOT
#define HISI_RESERVED_4G_DDR_FASTBOOT_GCDA_PHYMEM_BASE (0xC0000000)
#define HISI_RESERVED_6G_DDR_FASTBOOT_GCDA_PHYMEM_BASE (0x100000000ULL)
#define HISI_RESERVED_DDR_FASTBOOT_GCDA_PHYMEM_SIZE (0x200000)
#endif
#ifdef CONFIG_HISI_GCOV_SLT_HIBENCH
#define HISI_RESERVED_4G_DDR_SLT_HIBENCH_GCDA_PHYMEM_BASE (0xC0200000)
#define HISI_RESERVED_6G_DDR_SLT_HIBENCH_GCDA_PHYMEM_BASE (0x100200000ULL)
#define HISI_RESERVED_SLT_HIBENCH_GCDA_PHYMEM_SIZE (0x100000)
#endif
#ifdef HIBENCH_AGING
#define HISI_RESERVED_HIEPS_PHYMEM_BASE 0xA0000000
#define HISI_RESERVED_HIEPS_PHYMEM_SIZE (0x200000)
#define HISI_RESERVED_DP_HDCP2_PHYMEM_BASE 0xA0200000
#define HISI_RESERVED_DP_HDCP2_PHYMEM_SIZE (0x100000)
#endif
#else
#define HISI_RESERVED_FASTBOOT_PHYMEM_BASE (0x3A400000)
#ifdef CONFIG_HISI_GCOV_FASTBOOT
#define HISI_RESERVED_FASTBOOT_PHYMEM_SIZE (0x800000)
#elif defined(CONFIG_PRODUCT_ARMPC)
#define HISI_RESERVED_FASTBOOT_PHYMEM_SIZE (0x603000)
#else
#define HISI_RESERVED_FASTBOOT_PHYMEM_SIZE (0x600000)
#define HISI_RESERVED_FASTBOOT_STACK_BASE (0x3A9FE000)
#define HISI_RESERVED_FASTBOOT_HEAP_CACHE_BASE (0x3AA00000)
#define HISI_RESERVED_FASTBOOT_HEAP_CACHE_TOP (0x3c7FFFFF)
#define HISI_RESERVED_FASTBOOT_HEAP_BASE (0x3C800000)
#define HISI_RESERVED_FASTBOOT_HEAP_TOP (0x3E3FFFFF)
#define HISI_RESERVED_FASTBOOT_LHEAP_TOP (0xA0000000)
#endif
#define HISI_RESERVED_FASTBOOT_USED_PHYMEM_SIZE (0x4000000)
#define HISI_RESERVED_BL2_PHYMEM_BASE (0x3E400000)
#define HISI_RESERVED_BL2_PHYMEM_SIZE (0x00800000)
#define HISI_RESERVED_BL2_FST_BUFF_BASE (HISI_RESERVED_BL2_PHYMEM_BASE + HISI_RESERVED_BL2_PHYMEM_SIZE)
#define HISI_RESERVED_BL2_FST_BUFF_SIZE (0x01400000)
#define HISI_RESERVED_BL2_VERIFY_BUFF_BASE 0x40000000
#define HISI_RESERVED_BL2_VERIFY_BUFF_SIZE 0x04000000
#define HISI_RESERVED_FASTBOOT_AVB_HEAP_PHYMEM_BASE 0x44000000
#define HISI_RESERVED_FASTBOOT_AVB_HEAP_PHYMEM_SIZE (0x50000000 - 0x44000000)
#define HISI_RESERVED_DTB_PHYMEM_BASE 0x66000000
#define HISI_RESERVED_DTB_PHYMEM_END 0x66200000
#define HISI_RESERVED_DTB_PHYMEM_SIZE (HISI_RESERVED_DTB_PHYMEM_END - HISI_RESERVED_DTB_PHYMEM_BASE)
#define HISI_RESERVED_DTB_CAN_RUN_BASE (HISI_RESERVED_DTB_PHYMEM_BASE)
#define HISI_RESERVED_DTB_CAN_RUN_END (HISI_RESERVED_DTB_PHYMEM_END)
#define HISI_RESERVED_KERNEL_CAN_RUN_BASE 0
#define HISI_RESERVED_KERNEL_CAN_RUN_END 0x10000000
#define HISI_RESERVED_NORMAL_RAMDISK_CAN_RUN_BASE 0x66200000
#define HISI_RESERVED_NORMAL_RAMDISK_CAN_RUN_END 0x66A00000
#define HISI_RESERVED_RECOVERY_RAMDISK_CAN_RUN_BASE 0x80000000
#define HISI_RESERVED_RECOVERY_RAMDISK_CAN_RUN_END 0xA0000000
#define HISI_RESERVED_MODEM_NSRO_SHARE_PHYMEM_BASE 0x10000000
#define HISI_RESERVED_MODEM_NSRO_SHARE_PHYMEM_SIZE (0x980000)
#define HISI_RESERVED_MODEM_SHARE_PHYMEM_BASE 0x10980000
#define HISI_RESERVED_MODEM_SHARE_PHYMEM_SIZE (0x180000)
#define HISI_RESERVED_MODEM_MNTN_PHYMEM_BASE 0x10B00000
#define HISI_RESERVED_MODEM_MNTN_PHYMEM_SIZE (0x100000)
#define HISI_RESERVED_DRM_PGTABLE_BASE 0x10D00000
#define HISI_RESERVED_DRM_PGTABLE_SIZE (0x200000)
#define HISI_RESERVED_HHEE_PHYMEM_BASE 0x10F00000
#define HISI_RESERVED_HHEE_PHYMEM_SIZE (0x600000)
#define HISI_HHEE_LOG_SIZE (0x20000)
#define HISI_HHEE_LOG_ADDR (HISI_RESERVED_HHEE_PHYMEM_BASE + HISI_RESERVED_HHEE_PHYMEM_SIZE - HISI_HHEE_LOG_SIZE)
#define HISI_RESERVED_DRV_RESERVE_MEM_PHYMEM_BASE 0x11500000
#define HISI_RESERVED_DRV_RESERVE_MEM_PHYMEM_SIZE (0x100000)
#define HISI_RESERVED_FAST_KER_AND_PHYMEM_BASE 0x11500000
#define HISI_RESERVED_FAST_KER_AND_PHYMEM_SIZE (0x40000)
#define HISI_SUB_RESERVED_FASTBOOT_LOG_PYHMEM_BASE 0x11540000
#define HISI_SUB_RESERVED_FASTBOOT_LOG_PYHMEM_SIZE (0x40000)
#define HISI_SUB_RESERVED_SCHARGE_PYHMEM_BASE 0x11580000
#define HISI_SUB_RESERVED_SCHARGE_PYHMEM_SIZE (0x1000)
#define HISI_SUB_RESERVED_BL31_SHARE_MEM_PHYMEM_BASE 0x11581000
#define HISI_SUB_RESERVED_BL31_SHARE_MEM_PHYMEM_SIZE (0x10000)
#define HISI_SUB_RESERVED_LCD_GAMMA_MEM_PHYMEM_BASE 0x11591000
#define HISI_SUB_RESERVED_LCD_GAMMA_MEM_PHYMEM_SIZE (0x1000)
#define HISI_SUB_RESERVED_BRIGHTNESS_CHROMA_MEM_PHYMEM_BASE 0x11592000
#define HISI_SUB_RESERVED_BRIGHTNESS_CHROMA_MEM_PHYMEM_SIZE (0x1000)
#define HISI_SUB_RESERVED_SEC_RANDNUM_INJECT_PHYMEM_BASE 0x11593000
#define HISI_SUB_RESERVED_SEC_RANDNUM_INJECT_PHYMEM_SIZE (0x1000)
#define HISI_SUB_RESERVED_BL2_LOG_MEM_PHYMEM_BASE 0x11594000
#define HISI_SUB_RESERVED_BL2_LOG_MEM_PHYMEM_SIZE (0x10000)
#define HISI_SUB_RESERVED_UNUSED_PHYMEM_BASE 0x115a4000
#define HISI_SUB_RESERVED_UNUSED_PHYMEM_SIZE (0x115FF000 - HISI_SUB_RESERVED_UNUSED_PHYMEM_BASE)
#define HISI_SUB_RESERVED_MNTN_DUMP_PHYMEM_BASE 0x115FF000
#define HISI_SUB_RESERVED_MNTN_DUMP_PHYMEM_SIZE (0x1000)
#define HISI_RESERVED_IVP_PHYMEM_BASE 0x11600000
#define HISI_RESERVED_IVP_PHYMEM_SIZE (0x100000)
#define HISI_RESERVED_SENSORHUB_SHMEMEXT_PHYMEM_BASE 0x11700000
#define HISI_RESERVED_SENSORHUB_SHMEMEXT_PHYMEM_SIZE (0x200000)
#define HISI_RESERVED_SENSORHUB_SHMEM_PHYMEM_BASE 0x11900000
#define HISI_RESERVED_SENSORHUB_SHMEM_PHYMEM_SIZE (0x40000)
#define HISI_RESERVED_SENSORHUB_SHARE_MEM_PHYMEM_BASE 0x11940000
#define HISI_RESERVED_SENSORHUB_SHARE_MEM_PHYMEM_SIZE (0x80000)
#define HISI_RESERVED_LPMX_CORE_PHYMEM_BASE 0x119C0000
#define HISI_RESERVED_LPMX_CORE_PHYMEM_SIZE (0x80000)
#define HISI_RESERVED_LPMCU_PHYMEM_BASE 0x11A40000
#define HISI_RESERVED_LPMCU_PHYMEM_SIZE 0xC0000
#define HISI_RESERVED_SENSORHUB_PHYMEM_BASE 0x11B00000
#define HISI_RESERVED_SENSORHUB_PHYMEM_SIZE (0xD00000)
#define HISI_RESERVED_NPU_TINY_PHYMEM_BASE 0x12800000
#define HISI_RESERVED_NPU_TINY_PHYMEM_SIZE (0x800000)
#define HISI_RESERVED_BL31_PHYMEM_BASE 0x13000000
#define HISI_RESERVED_BL31_PHYMEM_SIZE (0x600000)
#define HISI_RESERVED_SECOS_PHYMEM_BASE 0x13600000
#define HISI_RESERVED_SECOS_PHYMEM_SIZE (0x3000000)
#define HISI_RESERVED_SECOS_EX_PHYMEM_BASE 0x16600000
#define HISI_RESERVED_SECOS_EX_PHYMEM_SIZE (0x480000)
#define HISI_RESERVED_VOICEID_PHYMEM_BASE 0x16A80000
#define HISI_RESERVED_VOICEID_PHYMEM_SIZE (0x500000)
#define HISI_RESERVED_EPS_EX_PHYMEM_BASE 0x16F80000
#define HISI_RESERVED_EPS_EX_PHYMEM_SIZE (0x100000)
#define HISI_RESERVED_SUPERSONIC_EX_PHYMEM_BASE 0x17080000
#define HISI_RESERVED_SUPERSONIC_EX_PHYMEM_SIZE (0x980000)
#define HISI_RESERVED_TD_FACE_DETECT_PHYMEM_BASE 0x17A00000
#define HISI_RESERVED_TD_FACE_DETECT_PHYMEM_SIZE (0x1900000)
#define HISI_RESERVED_TD_HIAI_RUNNING_PHYMEM_BASE 0x17A00000
#define HISI_RESERVED_TD_HIAI_RUNNING_PHYMEM_SIZE (0x1780000)
#define HISI_RESERVED_TD_HIAI_PAGETABLE_PHYMEM_BASE 0x19180000
#define HISI_RESERVED_TD_HIAI_PAGETABLE_PHYMEM_SIZE (0x180000)
#define HISI_RESERVED_MODEM_PHYMEM_BASE 0x20000000
#define HISI_RESERVED_MODEM_PHYMEM_SIZE (0xBB80000)
#define HISI_RESERVED_SUPERSONIC_PHYMEM_BASE 0x2C200000
#define HISI_RESERVED_SUPERSONIC_PHYMEM_SIZE (0xA00000)
#define HISI_RESERVED_HIEPS_PHYMEM_BASE 0x2CC00000
#define HISI_RESERVED_HIEPS_PHYMEM_SIZE (0x200000)
#define HISI_RESERVED_SEC_CAMERA_PHYMEM_BASE 0x2CE00000
#define HISI_RESERVED_SEC_CAMERA_PHYMEM_SIZE 0x00C00000
#define HISI_RESERVED_HIFI_PHYMEM_BASE 0x2DA00000
#define HISI_RESERVED_HIFI_PHYMEM_SIZE (0xF80000)
#define HISI_RESERVED_HIFI_DATA_PHYMEM_BASE 0x2E980000
#define HISI_RESERVED_HIFI_DATA_PHYMEM_SIZE (0x580000)
#define HISI_RESERVED_DP_HDCP2_PHYMEM_BASE 0x2EF00000
#define HISI_RESERVED_DP_HDCP2_PHYMEM_SIZE (0x100000)
#define HISI_RESERVED_PSTORE_PHYMEM_BASE 0x2F000000
#define HISI_RESERVED_PSTORE_PHYMEM_SIZE (0x100000)
#define HISI_RESERVED_MNTN_PHYMEM_BASE 0x2F100000
#define HISI_RESERVED_MNTN_PHYMEM_SIZE (0x860000)
#define HISI_RESERVED_NPU_AI_SERVER_PHYMEM_BASE 0x2F960000
#define HISI_RESERVED_NPU_AI_SERVER_PHYMEM_SIZE (0xD00000)
#define HISI_RESERVED_NPU_SEC_PHYMEM_BASE 0x30660000
#define HISI_RESERVED_NPU_SEC_PHYMEM_SIZE (0xA00000)
#define HISI_RESERVED_FASTBOOT_CMA_PHYMEM_BASE (0x3A400000)
#define HISI_RESERVED_FASTBOOT_CMA_PHYMEM_SIZE (0x5C00000)
#define HISI_RESERVED_DRM_CMA_BASE 0x40000000
#define HISI_RESERVED_DRM_CMA_SIZE (0x10000000)
#define HISI_RESERVED_IRIS_CMA_BASE 0x50000000
#define HISI_RESERVED_IRIS_CMA_SIZE (0xAC00000)
#define HISI_RESERVED_TINY_CMA_BASE 0x60000000
#define HISI_RESERVED_TINY_CMA_SIZE (0x4000000)
#define HISI_RESERVED_KERNEL_DUMP_PROTECT_BASE 0x80000000
#define HISI_RESERVED_KERNEL_DUMP_PRORECT_SIZE (0x20000000)
#define HISI_RESERVED_MODEM_SOCP_PHYMEM_BASE 0xA0000000
#define HISI_RESERVED_MODEM_SOCP_PHYMEM_SIZE (0x2000000)
#define HISI_RESERVED_MODEM_SAN_PHYMEM_BASE 0xA2000000
#define HISI_RESERVED_MODEM_SAN_PHYMEM_SIZE (0x8000000)
#define HISI_RESERVED_HOTA_PHYMEM_BASE (0xAA000000)
#define HISI_RESERVED_HOTA_PHYMEM_SIZE (0x10000000)
#define HISI_RESERVED_LPMX_CORE_PHYMEM_BASE_UNIQUE (HISI_RESERVED_LPMX_CORE_PHYMEM_BASE)
#define HISI_RESERVED_LPMCU_PHYMEM_BASE_UNIQUE (HISI_RESERVED_LPMCU_PHYMEM_BASE)
#define HISI_RESERVED_MNTN_PHYMEM_BASE_UNIQUE (HISI_RESERVED_MNTN_PHYMEM_BASE)
#ifdef CONFIG_HISI_GCOV_FASTBOOT
#define HISI_RESERVED_4G_DDR_FASTBOOT_GCDA_PHYMEM_BASE (0xC0000000)
#define HISI_RESERVED_6G_DDR_FASTBOOT_GCDA_PHYMEM_BASE (0x100000000ULL)
#define HISI_RESERVED_DDR_FASTBOOT_GCDA_PHYMEM_SIZE (0x200000)
#endif
#ifdef CONFIG_HISI_GCOV_SLT_HIBENCH
#define HISI_RESERVED_4G_DDR_SLT_HIBENCH_GCDA_PHYMEM_BASE (0xC0200000)
#define HISI_RESERVED_6G_DDR_SLT_HIBENCH_GCDA_PHYMEM_BASE (0x100200000ULL)
#define HISI_RESERVED_SLT_HIBENCH_GCDA_PHYMEM_SIZE (0x100000)
#endif
#endif
#endif
