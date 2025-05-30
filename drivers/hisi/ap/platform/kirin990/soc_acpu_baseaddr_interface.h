#ifndef __SOC_ACPU_BASEADDR_INTERFACE_H__
#define __SOC_ACPU_BASEADDR_INTERFACE_H__ 
#ifdef __cplusplus
    #if __cplusplus
        extern "C" {
    #endif
#endif
#define SOC_ACPU_BOOTROM_BASE_ADDR (0xFFFE0000)
#define SOC_ACPU_LPMCU_RAM_BASE_ADDR (0xFFF70000)
#define SOC_ACPU_LP_RAM_BASE_ADDR (0xFFF50000)
#define SOC_ACPU_LP_CONFIG_BASE_ADDR (0xFFF0F000)
#define SOC_ACPU_LP_TIMER_BASE_ADDR (0xFFF0E000)
#define SOC_ACPU_LP_WDG_BASE_ADDR (0xFFF0D000)
#define SOC_ACPU_CORE_CRG_B_M_BASE_ADDR (0xFFF09000)
#define SOC_ACPU_CORE_CRG_L_L3_BASE_ADDR (0xFFF08000)
#define SOC_ACPU_LP_PCTRL_BASE_ADDR (0xFFF07000)
#define SOC_ACPU_PERI_CRG_BASE_ADDR (0xFFF05000)
#define SOC_ACPU_PMU_I2C0_BASE_ADDR (0xFFF03000)
#define SOC_ACPU_UART6_BASE_ADDR (0xFFF02000)
#define SOC_ACPU_PMC_BASE_ADDR (0xFFF01000)
#define SOC_ACPU_TSENSORC_BASE_ADDR (0xFFF00000)
#define SOC_ACPU_DMSS_BASE_ADDR (0xFFE80000)
#define SOC_ACPU_DMCPACK3_BASE_ADDR (0xFFE60000)
#define SOC_ACPU_DMCPACK2_BASE_ADDR (0xFFE40000)
#define SOC_ACPU_DMCPACK1_BASE_ADDR (0xFFE20000)
#define SOC_ACPU_DMCPACK0_BASE_ADDR (0xFFE00000)
#define SOC_ACPU_GIC600_BASE_ADDR (0xFE400000)
#define SOC_ACPU_System_Cache_BASE_ADDR (0xFE300000)
#define SOC_ACPU_FD_BUS_Service_Target_BASE_ADDR (0xFE280000)
#define SOC_ACPU_AOBUS_Service_Target_BASE_ADDR (0xFE270000)
#define SOC_ACPU_DMA_NOC_Service_Target_BASE_ADDR (0xFE260000)
#define SOC_ACPU_CFGBUS_Service_Target_BASE_ADDR (0xFE250000)
#define SOC_ACPU_SYS_BUS_Service_Target_BASE_ADDR (0xFE240000)
#define SOC_ACPU_ASP_Service_Target_BASE_ADDR (0xFE230000)
#define SOC_ACPU_Modem_Service_Target_BASE_ADDR (0xFE220000)
#define SOC_ACPU_HSDT_Service_Target_BASE_ADDR (0xFE210000)
#define SOC_ACPU_MMC0_Service_target_BASE_ADDR (0xFE200000)
#define SOC_ACPU_G3D_BASE_ADDR (0xFE140000)
#define SOC_ACPU_CODEC_SSI_BASE_ADDR (0xFE104000)
#define SOC_ACPU_IOC_BASE_ADDR (0xFE102000)
#define SOC_ACPU_IPC_NS_BASE_ADDR (0xFE101000)
#define SOC_ACPU_IPC_BASE_ADDR (0xFE100000)
#define SOC_ACPU_LoadMonitor1_BASE_ADDR (0xFE031000)
#define SOC_ACPU_LoadMonitor0_BASE_ADDR (0xFE030000)
#define SOC_ACPU_CFG_BLPWM_BASE_ADDR (0xFE02F000)
#define SOC_ACPU_PCTRL_BASE_ADDR (0xFE02E000)
#define SOC_ACPU_ATGC_BASE_ADDR (0xFE02D000)
#define SOC_ACPU_TZPC_BASE_ADDR (0xFE02C000)
#define SOC_ACPU_GPUPCR_BASE_ADDR (0xFE028000)
#define SOC_ACPU_WD1_BASE_ADDR (0xFE027000)
#define SOC_ACPU_WD0_BASE_ADDR (0xFE026000)
#define SOC_ACPU_CTF_BASE_ADDR (0xFE025000)
#define SOC_ACPU_PWM_BASE_ADDR (0xFE024000)
#define SOC_ACPU_TIMER12_BASE_ADDR (0xFE01F000)
#define SOC_ACPU_TIMER11_BASE_ADDR (0xFE01E000)
#define SOC_ACPU_TIMER10_BASE_ADDR (0xFE01D000)
#define SOC_ACPU_TIMER9_BASE_ADDR (0xFE01C000)
#define SOC_ACPU_GPIO19_BASE_ADDR (0xFE015000)
#define SOC_ACPU_GPIO18_BASE_ADDR (0xFE014000)
#define SOC_ACPU_GPIO17_BASE_ADDR (0xFE013000)
#define SOC_ACPU_GPIO16_BASE_ADDR (0xFE012000)
#define SOC_ACPU_GPIO15_BASE_ADDR (0xFE011000)
#define SOC_ACPU_GPIO14_BASE_ADDR (0xFE010000)
#define SOC_ACPU_GPIO13_BASE_ADDR (0xFE00F000)
#define SOC_ACPU_GPIO12_BASE_ADDR (0xFE00E000)
#define SOC_ACPU_GPIO11_BASE_ADDR (0xFE00D000)
#define SOC_ACPU_GPIO10_BASE_ADDR (0xFE00C000)
#define SOC_ACPU_GPIO9_BASE_ADDR (0xFE00B000)
#define SOC_ACPU_GPIO8_BASE_ADDR (0xFE00A000)
#define SOC_ACPU_GPIO7_BASE_ADDR (0xFE009000)
#define SOC_ACPU_GPIO6_BASE_ADDR (0xFE008000)
#define SOC_ACPU_GPIO5_BASE_ADDR (0xFE007000)
#define SOC_ACPU_GPIO4_BASE_ADDR (0xFE006000)
#define SOC_ACPU_GPIO3_BASE_ADDR (0xFE005000)
#define SOC_ACPU_GPIO2_BASE_ADDR (0xFE004000)
#define SOC_ACPU_GPIO1_BASE_ADDR (0xFE003000)
#define SOC_ACPU_GPIO0_BASE_ADDR (0xFE002000)
#define SOC_ACPU_GPIO0_SE_BASE_ADDR (0xFE000000)
#define SOC_ACPU_CSSYS_APB_BASE_ADDR (0xFC000000)
#define SOC_ACPU_FD_UL_DMMU2_CFG_BASE_ADDR (0xFAE80000)
#define SOC_ACPU_DAVINCI_TINY_SUBSYSTEM_BASE_ADDR (0xFAE40000)
#define SOC_ACPU_FD_UL_CSI_CFG_BASE_ADDR (0xFAE13000)
#define SOC_ACPU_FD_UL_DMMU1_CFG_BASE_ADDR (0xFAE12000)
#define SOC_ACPU_FD_UL_DMMU0_CFG_BASE_ADDR (0xFAE11000)
#define SOC_ACPU_FD_UL_DMA_CFG_BASE_ADDR (0xFAE10000)
#define SOC_ACPU_FD_UL_IMGCAP_CFG_BASE_ADDR (0xFAE08000)
#define SOC_ACPU_FD_UL_S3_ASC_CFG_BASE_ADDR (0xFAE07000)
#define SOC_ACPU_FD_UL_ETZPC_CFG_BASE_ADDR (0xFAE06000)
#define SOC_ACPU_FD_UL_SUBSYS_ENHC_CFG_BASE_ADDR (0xFAE05000)
#define SOC_ACPU_FD_UL_SUBSYS_SEC_CFG_BASE_ADDR (0xFAE04000)
#define SOC_ACPU_FD_UL_LB_ASC_BASE_ADDR (0xFAE03000)
#define SOC_ACPU_FD_UL_LB_CFG_BASE_ADDR (0xFAE02000)
#define SOC_ACPU_FD_UL_SUBSYS_CFG_BASE_ADDR (0xFAE01000)
#define SOC_ACPU_FD_UL_TZPC_CFG_BASE_ADDR (0xFAE00000)
#define SOC_ACPU_FD_UL_LB_BASE_ADDR (0xFAC00000)
#define SOC_ACPU_AO_TCP_BASE_ADDR (0xFA980000)
#define SOC_ACPU_MAD_DSPIF_BASE_ADDR (0xFA902000)
#define SOC_ACPU_MAD_X2P_BASE_ADDR (0xFA900000)
#define SOC_ACPU_GPIO36_BASE_ADDR (0xFA8B8000)
#define SOC_ACPU_GPIO35_BASE_ADDR (0xFA8B7000)
#define SOC_ACPU_GPIO34_BASE_ADDR (0xFA8B6000)
#define SOC_ACPU_GPIO33_BASE_ADDR (0xFA8B5000)
#define SOC_ACPU_GPIO32_BASE_ADDR (0xFA8B4000)
#define SOC_ACPU_GPIO31_BASE_ADDR (0xFA8B3000)
#define SOC_ACPU_GPIO30_BASE_ADDR (0xFA8B2000)
#define SOC_ACPU_GPIO29_BASE_ADDR (0xFA8B1000)
#define SOC_ACPU_GPIO28_BASE_ADDR (0xFA8B0000)
#define SOC_ACPU_GPIO27_BASE_ADDR (0xFA8AF000)
#define SOC_ACPU_GPIO26_BASE_ADDR (0xFA8AE000)
#define SOC_ACPU_GPIO25_BASE_ADDR (0xFA8AD000)
#define SOC_ACPU_GPIO24_BASE_ADDR (0xFA8AC000)
#define SOC_ACPU_GPIO23_BASE_ADDR (0xFA8AB000)
#define SOC_ACPU_GPIO22_BASE_ADDR (0xFA8AA000)
#define SOC_ACPU_GPIO21_BASE_ADDR (0xFA8A9000)
#define SOC_ACPU_GPIO20_BASE_ADDR (0xFA8A8000)
#define SOC_ACPU_GPIO1_SE_BASE_ADDR (0xFA8A1000)
#define SOC_ACPU_AO_WDG_BASE_ADDR (0xFA8A0000)
#define SOC_ACPU_SPI3_BASE_ADDR (0xFA89F000)
#define SOC_ACPU_AO_TZPC_BASE_ADDR (0xFA89E000)
#define SOC_ACPU_AO_LoadMonitor_BASE_ADDR (0xFA89D000)
#define SOC_ACPU_AO_IOC_BASE_ADDR (0xFA89C000)
#define SOC_ACPU_SCTRL_BASE_ADDR (0xFA89B000)
#define SOC_ACPU_EFUSEC_BASE_ADDR (0xFA89A000)
#define SOC_ACPU_AO_IPC_NS_BASE_ADDR (0xFA899000)
#define SOC_ACPU_AO_IPC_S_BASE_ADDR (0xFA898000)
#define SOC_ACPU_SYS_CNT_BASE_ADDR (0xFA896000)
#define SOC_ACPU_BB_DRX_BASE_ADDR (0xFA894000)
#define SOC_ACPU_SPMI_BASE_ADDR (0xFA890000)
#define SOC_ACPU_SCI1_BASE_ADDR (0xFA88F000)
#define SOC_ACPU_SCI0_BASE_ADDR (0xFA88E000)
#define SOC_ACPU_RTC1_BASE_ADDR (0xFA88D000)
#define SOC_ACPU_RTC0_BASE_ADDR (0xFA88C000)
#define SOC_ACPU_TIMER8_BASE_ADDR (0xFA888000)
#define SOC_ACPU_TIMER7_BASE_ADDR (0xFA887000)
#define SOC_ACPU_TIMER6_BASE_ADDR (0xFA886000)
#define SOC_ACPU_TIMER5_BASE_ADDR (0xFA885000)
#define SOC_ACPU_TIMER4_BASE_ADDR (0xFA884000)
#define SOC_ACPU_TIMER3_BASE_ADDR (0xFA883000)
#define SOC_ACPU_TIMER2_BASE_ADDR (0xFA882000)
#define SOC_ACPU_TIMER1_BASE_ADDR (0xFA881000)
#define SOC_ACPU_TIMER0_BASE_ADDR (0xFA880000)
#define SOC_ACPU_IOMCU_RTC_BASE_ADDR (0xFA87F000)
#define SOC_ACPU_IOMCU_CONFIG_BASE_ADDR (0xFA87E000)
#define SOC_ACPU_IOMCU_TIMER_BASE_ADDR (0xFA87D000)
#define SOC_ACPU_IOMCU_WDG_BASE_ADDR (0xFA87C000)
#define SOC_ACPU_IOMCU_GPIO3_BASE_ADDR (0xFA87B000)
#define SOC_ACPU_IOMCU_GPIO2_BASE_ADDR (0xFA87A000)
#define SOC_ACPU_IOMCU_GPIO1_BASE_ADDR (0xFA879000)
#define SOC_ACPU_IOMCU_GPIO0_BASE_ADDR (0xFA878000)
#define SOC_ACPU_IOMCU_DMAC_BASE_ADDR (0xFA877000)
#define SOC_ACPU_IOMCU_UART7_BASE_ADDR (0xFA876000)
#define SOC_ACPU_IOMCU_BLPWM_BASE_ADDR (0xFA875000)
#define SOC_ACPU_IOMCU_UART3_BASE_ADDR (0xFA874000)
#define SOC_ACPU_IOMCU_I3C1_BASE_ADDR (0xFA873000)
#define SOC_ACPU_IOMCU_I2C1_BASE_ADDR (0xFA872000)
#define SOC_ACPU_IOMCU_I2C0_BASE_ADDR (0xFA871000)
#define SOC_ACPU_IOMCU_SPI0_BASE_ADDR (0xFA870000)
#define SOC_ACPU_IOMCU_I2C5_BASE_ADDR (0xFA86F000)
#define SOC_ACPU_IOMCU_I3C2_BASE_ADDR (0xFA86A000)
#define SOC_ACPU_IOMCU_UART8_BASE_ADDR (0xFA869000)
#define SOC_ACPU_IOMCU_SPI2_BASE_ADDR (0xFA868000)
#define SOC_ACPU_IOMCU_DMMU_BASE_ADDR (0xFA867000)
#define SOC_ACPU_IOMCU_TIMER2_BASE_ADDR (0xFA866000)
#define SOC_ACPU_IOMCU_I3C_BASE_ADDR (0xFA865000)
#define SOC_ACPU_IOMCU_I3C3_BASE_ADDR (0xFA864000)
#define SOC_ACPU_IOMCU_TCP_RAM_BASE_ADDR (0xFA851000)
#define SOC_ACPU_IOMCU_TCP_CFG_BASE_ADDR (0xFA850000)
#define SOC_ACPU_IOMCU_DTCM_BASE_ADDR (0xFA700000)
#define SOC_ACPU_IOMCU_ITCM_BASE_ADDR (0xFA600000)
#define SOC_ACPU_THINKER_BASE_ADDR (0xFA5C0000)
#define SOC_ACPU_DSP_ITCM_BASE_ADDR (0xFA580000)
#define SOC_ACPU_DSP_DTCM_BASE_ADDR (0xFA558000)
#define SOC_ACPU_SoundWire_BASE_ADDR (0xFA554000)
#define SOC_ACPU_ASP_CODEC_BASE_ADDR (0xFA552000)
#define SOC_ACPU_SLIMBUS_BASE_ADDR (0xFA550000)
#define SOC_ACPU_DSD_BASE_ADDR (0xFA54FC00)
#define SOC_ACPU_SIO_BT_BASE_ADDR (0xFA54F800)
#define SOC_ACPU_SIO_THINKER_BASE_ADDR (0xFA54F400)
#define SOC_ACPU_SIO_AUDIO_BASE_ADDR (0xFA54F000)
#define SOC_ACPU_ASP_HDMI_SPDIF_BASE_ADDR (0xFA54EC00)
#define SOC_ACPU_ASP_HDMI_SIO_BASE_ADDR (0xFA54E800)
#define SOC_ACPU_ASP_HDMI_ASP_BASE_ADDR (0xFA54E400)
#define SOC_ACPU_ASP_CFG_BASE_ADDR (0xFA54E000)
#define SOC_ACPU_ASP_WD_BASE_ADDR (0xFA54D000)
#define SOC_ACPU_ASP_IPC_BASE_ADDR (0xFA54C000)
#define SOC_ACPU_ASP_DMAC_BASE_ADDR (0xFA54B000)
#define SOC_ACPU_ASP_TIMER1_BASE_ADDR (0xFA54A000)
#define SOC_ACPU_ASP_TIMER0_BASE_ADDR (0xFA549000)
#define SOC_ACPU_ASP_GPIO_BASE_ADDR (0xFA548000)
#define SOC_ACPU_ASP_DMMU_BASE_ADDR (0xFA547000)
#define SOC_ACPU_SECRAM_BASE_ADDR (0xFA480000)
#define SOC_ACPU_USB_AUDIO_BASE_ADDR (0xFA400000)
#define SOC_ACPU_HISEE_IPC_BASE_ADDR (0xFA230000)
#define SOC_ACPU_HISEE_MAILBOX_BASE_ADDR (0xFA220000)
#define SOC_ACPU_SOCP_BASE_ADDR (0xFA0A0000)
#define SOC_ACPU_IPF_PSAM_BASE_ADDR (0xFA090000)
#define SOC_ACPU_IPF_BASE_ADDR (0xFA088000)
#define SOC_ACPU_IPC_MDM_NS_BASE_ADDR (0xFA081000)
#define SOC_ACPU_IPC_MDM_S_BASE_ADDR (0xFA080000)
#define SOC_ACPU_LAT_STAT_BASE_ADDR (0xFA057000)
#define SOC_ACPU_PERF_STAT_BASE_ADDR (0xFA056000)
#define SOC_ACPU_UART0_BASE_ADDR (0xFA054000)
#define SOC_ACPU_I3C4_BASE_ADDR (0xFA050000)
#define SOC_ACPU_I2C7_BASE_ADDR (0xFA04F000)
#define SOC_ACPU_I2C6_BASE_ADDR (0xFA04E000)
#define SOC_ACPU_I2C4_BASE_ADDR (0xFA04D000)
#define SOC_ACPU_I2C3_BASE_ADDR (0xFA04C000)
#define SOC_ACPU_SPI4_BASE_ADDR (0xFA049000)
#define SOC_ACPU_SPI1_BASE_ADDR (0xFA048000)
#define SOC_ACPU_UART5_BASE_ADDR (0xFA045000)
#define SOC_ACPU_UART2_BASE_ADDR (0xFA044000)
#define SOC_ACPU_UART4_BASE_ADDR (0xFA041000)
#define SOC_ACPU_UART1_BASE_ADDR (0xFA040000)
#define SOC_ACPU_PERI_DMAC_BASE_ADDR (0xFA000000)
#define SOC_ACPU_CS_STM_BASE_ADDR (0xF9000000)
#define SOC_ACPU_SD3_BASE_ADDR (0xF8583000)
#define SOC_ACPU_MMC0_SYS_CTRL_BASE_ADDR (0xF8582000)
#define SOC_ACPU_MMC0_CRG_BASE_ADDR (0xF8581000)
#define SOC_ACPU_IOC_MMC0_BASE_ADDR (0xF8580000)
#define SOC_ACPU_DP_CTRL_BASE_ADDR (0xF8500000)
#define SOC_ACPU_USB_DP_CTRL_BASE_ADDR (0xF8481000)
#define SOC_ACPU_USB_TCA_BASE_ADDR (0xF8480200)
#define SOC_ACPU_USB_MISC_CTRL_BASE_ADDR (0xF8480000)
#define SOC_ACPU_USB_OTG_BASE_ADDR (0xF8400000)
#define SOC_ACPU_UFS_CFG_BASE_ADDR (0xF8200000)
#define SOC_ACPU_UFS_SYS_CTRL_BASE_ADDR (0xF81FF000)
#define SOC_ACPU_HSDT_SYS_CTRL_BASE_ADDR (0xF8181000)
#define SOC_ACPU_HSDT_CRG_BASE_ADDR (0xF8180000)
#define SOC_ACPU_MMC1_IOC_BASE_ADDR (0xF8105000)
#define SOC_ACPU_MMC1_SDIO_BASE_ADDR (0xF8104000)
#define SOC_ACPU_PCIE1_APB_CFG_BASE_ADDR (0xF8103000)
#define SOC_ACPU_PCIE0_APB_CFG_BASE_ADDR (0xF8102000)
#define SOC_ACPU_CCP_BASE_ADDR (0xF8101000)
#define SOC_ACPU_CCS_BASE_ADDR (0xF8100000)
#define SOC_ACPU_PCIEPHY1_BASE_ADDR (0xF8080000)
#define SOC_ACPU_PCIEPHY0_BASE_ADDR (0xF8000000)
#define SOC_ACPU_PCIECtrl1_BASE_ADDR (0xF4000000)
#define SOC_ACPU_PCIECtrl0_BASE_ADDR (0xF0000000)
#define SOC_ACPU_NN_BASE_ADDR (0xE9540000)
#define SOC_ACPU_NN_SMMU_BASE_ADDR (0xE9500000)
#define SOC_ACPU_EPS_RAM_BASE_ADDR (0xE9470000)
#define SOC_ACPU_ICCM_RAM_BASE_ADDR (0xE9448000)
#define SOC_ACPU_EPS_ROM_BASE_ADDR (0xE9440000)
#define SOC_ACPU_EPS_MMU_BASE_ADDR (0xE9430000)
#define SOC_ACPU_EPS_PKE_BASE_ADDR (0xE9422000)
#define SOC_ACPU_EPS_SCE_BASE_ADDR (0xE9421000)
#define SOC_ACPU_EPS_KM_BASE_ADDR (0xE9420000)
#define SOC_ACPU_EPS_IPC_BASE_ADDR (0xE9407000)
#define SOC_ACPU_EPS_UART_BASE_ADDR (0xE9406000)
#define SOC_ACPU_EPS_WD_BASE_ADDR (0xE9405000)
#define SOC_ACPU_EPS_TIMER_BASE_ADDR (0xE9404000)
#define SOC_ACPU_EPS_TRNG_BASE_ADDR (0xE9402000)
#define SOC_ACPU_EPS_ETZPC_BASE_ADDR (0xE9401000)
#define SOC_ACPU_EPS_CONFIG_BASE_ADDR (0xE9400000)
#define SOC_ACPU_EPS_BUS_Service_Target_BASE_ADDR (0xE93D0000)
#define SOC_ACPU_NOC_VENC2_Service_Target_BASE_ADDR (0xE93C0000)
#define SOC_ACPU_NOC_VENC_Service_Target_BASE_ADDR (0xE93B0000)
#define SOC_ACPU_NOC_VDEC_Service_Target_BASE_ADDR (0xE93A0000)
#define SOC_ACPU_NOC_VCODECBUS_Service_Target_BASE_ADDR (0xE9390000)
#define SOC_ACPU_NOC_IVP32_Service_Target_BASE_ADDR (0xE9380000)
#define SOC_ACPU_MEDAI2_LoadMonitor_BASE_ADDR (0xE9301000)
#define SOC_ACPU_MEDIA2_CRG_BASE_ADDR (0xE9300000)
#define SOC_ACPU_VENC2_BASE_ADDR (0xE92C0000)
#define SOC_ACPU_VENC_BASE_ADDR (0xE9280000)
#define SOC_ACPU_VDEC_BASE_ADDR (0xE9200000)
#define SOC_ACPU_IVP32_SMMU_BASE_ADDR (0xE91C0000)
#define SOC_ACPU_DPM_BASE_ADDR (0xE9184000)
#define SOC_ACPU_IVP32_TIMER1_BASE_ADDR (0xE9183000)
#define SOC_ACPU_IVP32_TIMER0_BASE_ADDR (0xE9182000)
#define SOC_ACPU_IVP32_WDG_BASE_ADDR (0xE9181000)
#define SOC_ACPU_IVP32_CFG_BASE_ADDR (0xE9180000)
#define SOC_ACPU_IVP32_IRAM_BASE_ADDR (0xE9100000)
#define SOC_ACPU_IVP32_DRAM1_BASE_ADDR (0xE9040000)
#define SOC_ACPU_IVP32_DRAM0_BASE_ADDR (0xE9000000)
#define SOC_ACPU_NOC_VIVO_Service_Target_BASE_ADDR (0xE86A0000)
#define SOC_ACPU_NOC_ISP_Service_Target_BASE_ADDR (0xE8690000)
#define SOC_ACPU_NOC_DSS_Service_Target_BASE_ADDR (0xE8680000)
#define SOC_ACPU_CSI_E_BASE_ADDR (0xE8608000)
#define SOC_ACPU_CSI_D_BASE_ADDR (0xE8607000)
#define SOC_ACPU_CSI_C_BASE_ADDR (0xE8606000)
#define SOC_ACPU_CSI_B_BASE_ADDR (0xE8605000)
#define SOC_ACPU_CSI_A_BASE_ADDR (0xE8604000)
#define SOC_ACPU_CSI_adapter_BASE_ADDR (0xE8603000)
#define SOC_ACPU_IDI2AXI_BASE_ADDR (0xE8602000)
#define SOC_ACPU_MEDIA1_CRG_BASE_ADDR (0xE8601000)
#define SOC_ACPU_MEDIA1_LoadMonitor_BASE_ADDR (0xE8600000)
#define SOC_ACPU_MEDIA_COMMON_BASE_ADDR (0xE8500000)
#define SOC_ACPU_GMP_LUT1_BASE_ADDR (0xE84EE000)
#define SOC_ACPU_GMP_LUT0_BASE_ADDR (0xE84DE000)
#define SOC_ACPU_DISP_CH2_BASE_ADDR (0xE84C2000)
#define SOC_ACPU_DPM_POST_BASE_ADDR (0xE84BF000)
#define SOC_ACPU_CROSSBAR_BASE_ADDR (0xE84BE000)
#define SOC_ACPU_DISP_CH1_BASE_ADDR (0xE84A2000)
#define SOC_ACPU_DISP_GLB_BASE_ADDR (0xE84A1000)
#define SOC_ACPU_WB_BASE_ADDR (0xE84A0000)
#define SOC_ACPU_DSS_PRO_BASE_ADDR (0xE8490000)
#define SOC_ACPU_DBCU0_SMMU_BASE_ADDR (0xE8480000)
#define SOC_ACPU_DP_ITF1_BASE_ADDR (0xE847F000)
#define SOC_ACPU_DP_ITF0_BASE_ADDR (0xE847E000)
#define SOC_ACPU_DISP_CH0_BASE_ADDR (0xE8462000)
#define SOC_ACPU_DPM_PRE_BASE_ADDR (0xE8461000)
#define SOC_ACPU_OV3_BASE_ADDR (0xE8460C00)
#define SOC_ACPU_OV2_BASE_ADDR (0xE8460800)
#define SOC_ACPU_OV1_BASE_ADDR (0xE8460400)
#define SOC_ACPU_OV0_BASE_ADDR (0xE8460000)
#define SOC_ACPU_WCH1_BASE_ADDR (0xE845C000)
#define SOC_ACPU_WCH0_BASE_ADDR (0xE845A000)
#define SOC_ACPU_RCH_D1_BASE_ADDR (0xE8453000)
#define SOC_ACPU_RCH_D0_BASE_ADDR (0xE8452000)
#define SOC_ACPU_RCH_D3_BASE_ADDR (0xE8451000)
#define SOC_ACPU_RCH_D2_BASE_ADDR (0xE8450000)
#define SOC_ACPU_RCH_G1_BASE_ADDR (0xE8440000)
#define SOC_ACPU_RCH_G0_BASE_ADDR (0xE8438000)
#define SOC_ACPU_RCH_V2_BASE_ADDR (0xE8430000)
#define SOC_ACPU_RCH_V1_BASE_ADDR (0xE8428000)
#define SOC_ACPU_RCH_V0_BASE_ADDR (0xE8420000)
#define SOC_ACPU_GLB_BASE_ADDR (0xE8412000)
#define SOC_ACPU_DBUG_BASE_ADDR (0xE8411000)
#define SOC_ACPU_MCTL_MUTEX_BASE_ADDR (0xE8410800)
#define SOC_ACPU_MCTL_SYS_BASE_ADDR (0xE8410000)
#define SOC_ACPU_DBCU0_MIF_BASE_ADDR (0xE840A000)
#define SOC_ACPU_DBCU1_AIF_BASE_ADDR (0xE8409000)
#define SOC_ACPU_DBCU0_AIF_BASE_ADDR (0xE8407000)
#define SOC_ACPU_CMD_BASE_ADDR (0xE8402000)
#define SOC_ACPU_ASC_BASE_ADDR (0xE8401800)
#define SOC_ACPU_DSI1_BASE_ADDR (0xE8401400)
#define SOC_ACPU_DSI0_BASE_ADDR (0xE8401000)
#define SOC_ACPU_MMBUF_CFG_BASE_ADDR (0xE8400000)
#define SOC_ACPU_SMMUv500_CFG_BASE_ADDR (0xE83F0000)
#define SOC_ACPU_ISP_R8_PERI_BASE_BASE_ADDR (0xE83EE000)
#define SOC_ACPU_DPM_CAP_BASE_ADDR (0xE83E9000)
#define SOC_ACPU_DPM_VID_BASE_ADDR (0xE83E8000)
#define SOC_ACPU_DPM_RT_BASE_ADDR (0xE83E7000)
#define SOC_ACPU_DPM_IPP_BASE_ADDR (0xE83E6000)
#define SOC_ACPU_DPM_NOC_BASE_ADDR (0xE83E5000)
#define SOC_ACPU_ISP_TCMDMA_BASE_ADDR (0xE83E4000)
#define SOC_ACPU_ISP_SUB_CTRL_BASE_ADDR (0xE83E3000)
#define SOC_ACPU_ISP_IPC_BASE_ADDR (0xE83E2000)
#define SOC_ACPU_ISP_TIMER_BASE_ADDR (0xE83E1000)
#define SOC_ACPU_ISP_WDT_BASE_ADDR (0xE83E0000)
#define SOC_ACPU_ISP_R8_DTCM_BASE_ADDR (0xE83D0000)
#define SOC_ACPU_ISP_R8_ITCM_BASE_ADDR (0xE83C0000)
#define SOC_ACPU_ISP_Core_CFG_BASE_ADDR (0xE8200000)
#define SOC_ACPU_FD_BASE_ADDR (0xE8040000)
#define SOC_ACPU_SMMU_BASE_ADDR (0xE8020000)
#define SOC_ACPU_CMP_BASE_ADDR (0xE800E000)
#define SOC_ACPU_RNR_BASE_ADDR (0xE800D000)
#define SOC_ACPU_ORB_BASE_ADDR (0xE800C000)
#define SOC_ACPU_MFNR_BASE_ADDR (0xE800B000)
#define SOC_ACPU_MCF_BASE_ADDR (0xE800A000)
#define SOC_ACPU_VBK_BASE_ADDR (0xE8009000)
#define SOC_ACPU_CVDR_BASE_ADDR (0xE8006000)
#define SOC_ACPU_CMDLST_BASE_ADDR (0xE8005000)
#define SOC_ACPU_TOP_BASE_ADDR (0xE8004000)
#define SOC_ACPU_SMMU_MASTER_BASE_ADDR (0xE8003000)
#define SOC_ACPU_JPGDEC_BASE_ADDR (0xE8001000)
#define SOC_ACPU_JPGENC_BASE_ADDR (0xE8000000)
#define SOC_ACPU_sysdma_smmu_cfg_BASE_ADDR (0xE5F80000)
#define SOC_ACPU_aic_smmu_cfg_BASE_ADDR (0xE5F00000)
#define SOC_ACPU_sysdma_cfg_BASE_ADDR (0xE5EE0000)
#define SOC_ACPU_npu_dpm_BASE_ADDR (0xE5E09000)
#define SOC_ACPU_l2buf_cfg_BASE_ADDR (0xE5E08000)
#define SOC_ACPU_npu_etzpc_BASE_ADDR (0xE5E07000)
#define SOC_ACPU_npu_hw_exp_irq_BASE_ADDR (0xE5E06000)
#define SOC_ACPU_npu_uart_BASE_ADDR (0xE5E05000)
#define SOC_ACPU_npu_sysctrl_BASE_ADDR (0xE5E04000)
#define SOC_ACPU_npu_crg_BASE_ADDR (0xE5E03000)
#define SOC_ACPU_npu_tzpc_BASE_ADDR (0xE5E02000)
#define SOC_ACPU_npu_ipc_ns_BASE_ADDR (0xE5E01000)
#define SOC_ACPU_npu_ipc_s_BASE_ADDR (0xE5E00000)
#define SOC_ACPU_gic_cfg_BASE_ADDR (0xE5C00000)
#define SOC_ACPU_aic_subsys_cfg_BASE_ADDR (0xE5000000)
#define SOC_ACPU_npu_bus_service_target_BASE_ADDR (0xE4D30000)
#define SOC_ACPU_L2BUF_BASE_ADDR (0xE4800000)
#define SOC_ACPU_SRAM_reg_BASE_ADDR (0xE4230000)
#define SOC_ACPU_SRAM_atomicdec_BASE_ADDR (0xE4220000)
#define SOC_ACPU_SRAM_atomicadd_BASE_ADDR (0xE4210000)
#define SOC_ACPU_SRAM_normal_BASE_ADDR (0xE4200000)
#define SOC_ACPU_bs_slv_BASE_ADDR (0xE4110000)
#define SOC_ACPU_doorbell_sts_BASE_ADDR (0xE4100000)
#define SOC_ACPU_doorbell_cfg_BASE_ADDR (0xE4080000)
#define SOC_ACPU_npu_cpu_secure_BASE_ADDR (0xE4037000)
#define SOC_ACPU_axi_mon_soc_BASE_ADDR (0xE4036000)
#define SOC_ACPU_axi_mon_bs_BASE_ADDR (0xE4035000)
#define SOC_ACPU_axi_mon_cpu_BASE_ADDR (0xE4034000)
#define SOC_ACPU_ai_cpu_local_ctrl_BASE_ADDR (0xE4032000)
#define SOC_ACPU_ts_cpu_local_ctrl_BASE_ADDR (0xE4031000)
#define SOC_ACPU_sysctrl_npu_cpu_BASE_ADDR (0xE4030000)
#define SOC_ACPU_fcm_local_ctrl_BASE_ADDR (0xE4022000)
#define SOC_ACPU_ts_cpu_a55_ctrl_BASE_ADDR (0xE4021000)
#define SOC_ACPU_ai_cpu_a55_ctrl_BASE_ADDR (0xE4020000)
#define SOC_ACPU_ts_cpu_timer_BASE_ADDR (0xE4011000)
#define SOC_ACPU_ai_cpu_timer_BASE_ADDR (0xE4010000)
#define SOC_ACPU_ts_cpu_watchdog_BASE_ADDR (0xE4001000)
#define SOC_ACPU_ai_cpu_watchdog_BASE_ADDR (0xE4000000)
#define SOC_ACPU_DSP0_L2C_BASE_ADDR (0xE3A02000)
#define SOC_ACPU_DSP0_L2MEM_BASE_ADDR (0xE3680000)
#define SOC_ACPU_DSP0_IMEM_BASE_ADDR (0xE3500000)
#define SOC_ACPU_DSP0_DMEM_BASE_ADDR (0xE3400000)
#define SOC_ACPU_GUTL_BBP_BASE_ADDR (0xE1000000)
#define SOC_ACPU_CCPU_LOCAL_MEM_BASE_ADDR (0xE0800000)
#define SOC_ACPU_CCPU1_DTCM_BASE_ADDR (0xE0760000)
#define SOC_ACPU_CCPU1_ITCM_BASE_ADDR (0xE0740000)
#define SOC_ACPU_CCPU0_DTCM_BASE_ADDR (0xE0720000)
#define SOC_ACPU_CCPU0_ITCM_BASE_ADDR (0xE0700000)
#define SOC_ACPU_AMON_MDM_BASE_ADDR (0xE0520000)
#define SOC_ACPU_NOC_SERVICE_BASE_ADDR (0xE0500000)
#define SOC_ACPU_DFC_BASE_ADDR (0xE0458000)
#define SOC_ACPU_UPACC_BASE_ADDR (0xE0453000)
#define SOC_ACPU_CIPHER_BASE_ADDR (0xE0452000)
#define SOC_ACPU_RSR_ACC_BASE_ADDR (0xE0444000)
#define SOC_ACPU_CICOM1_BASE_ADDR (0xE0443000)
#define SOC_ACPU_HDLC_BASE_ADDR (0xE0442000)
#define SOC_ACPU_IPCM_BASE_ADDR (0xE0441000)
#define SOC_ACPU_CICOM0_BASE_ADDR (0xE0440000)
#define SOC_ACPU_MDM_POWER_MON3_BASE_ADDR (0xE0227000)
#define SOC_ACPU_MDM_POWER_MON2_BASE_ADDR (0xE0226000)
#define SOC_ACPU_MDM_POWER_MON1_BASE_ADDR (0xE0225000)
#define SOC_ACPU_MDM_POWER_MON0_BASE_ADDR (0xE0224000)
#define SOC_ACPU_DW_AXI_MDM_CCPU_BASE_ADDR (0xE0223000)
#define SOC_ACPU_DW_AXI_MDM_MST_BASE_ADDR (0xE0222000)
#define SOC_ACPU_SYSCNT_MDM_BASE_ADDR (0xE0220000)
#define SOC_ACPU_MDM_RTC_BASE_ADDR (0xE0217000)
#define SOC_ACPU_WDT_CCPU1_BASE_ADDR (0xE0214000)
#define SOC_ACPU_WDT_DSP0_BASE_ADDR (0xE0212000)
#define SOC_ACPU_EDMA1_MDM_BASE_ADDR (0xE0210000)
#define SOC_ACPU_TIMER9_MDM_BASE_ADDR (0xE020F000)
#define SOC_ACPU_TIMER8_MDM_BASE_ADDR (0xE020E000)
#define SOC_ACPU_TIMER7_MDM_BASE_ADDR (0xE020D000)
#define SOC_ACPU_TIMER6_MDM_BASE_ADDR (0xE020C000)
#define SOC_ACPU_TIMER5_MDM_BASE_ADDR (0xE020B000)
#define SOC_ACPU_TIMER4_MDM_BASE_ADDR (0xE020A000)
#define SOC_ACPU_TIMER3_MDM_BASE_ADDR (0xE0209000)
#define SOC_ACPU_TIMER2_MDM_BASE_ADDR (0xE0208000)
#define SOC_ACPU_TIMER1_MDM_BASE_ADDR (0xE0207000)
#define SOC_ACPU_TIMER0_MDM_BASE_ADDR (0xE0206000)
#define SOC_ACPU_UART1_MDM_BASE_ADDR (0xE0205000)
#define SOC_ACPU_EDMA0_MDM_BASE_ADDR (0xE0204000)
#define SOC_ACPU_UART0_MDM_BASE_ADDR (0xE0203000)
#define SOC_ACPU_WDT_CCPU0_BASE_ADDR (0xE0201000)
#define SOC_ACPU_SYSCTRL_MDM_BASE_ADDR (0xE0200000)
#define SOC_ACPU_CCPU_PRIVATE_BASE_ADDR (0xE0100000)
#define SOC_ACPU_CCPU_L2C_BASE_ADDR (0xE0000000)
#define SOC_ACPU_DRAM_BASE_ADDR (0x00000000)
#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif
#endif
