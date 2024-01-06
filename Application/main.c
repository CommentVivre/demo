/**
   @copyright None
   @file      main.c
   @author    Comment Vivre
   @date      2023-12-29
   @brief     None
*/
#include <main.h>

/*
 To Do
// 1.LTDC 显示部分需要验证
//  * 测试点刷新与全屏刷新
// 2.需要烧录反客下载算法到内置Flash
//  * 分散加载后续完善
//  * https://blog.csdn.net/tianizimark/article/details/121640588
// 3.测试lvgl demo 确保显示正常
//  * lvgl移植参考教程: https://blog.csdn.net/qq_59953808/article/details/126445608
 4.实现触控功能
  * 参考正点原子代码
 5.移植FreeRTOS 并完成对lvgl的兼容
 6.使用GPU加速传输
  * 测试参考教程 https://blog.csdn.net/a2267542848/article/details/111163633
 ...
*/


// 模拟IIC 会产生时许问题
// 开启cache 注意配置MPU


uint32_t Pen_Color  = 0xFF000000;
uint32_t Back_Color = 0xFFFFFFFF;

/**
   @function     main
   @brief        None
   @param[in]    None
   @return       None
   @date         2023-12-29
*/
int main(void)
{
  // 启用 Cortex-M 处理器的指令缓存
  SCB_EnableICache();
  // 启用 Cortex-M 处理器的数据缓存
  SCB_EnableDCache();
	
	#if 0
  // 在 Cortex-M 处理器的 Cache 控制寄存器中设置某个位
  // 但未提供具体细节
  SCB->CACR |= 1 << 2; 
	#endif
	Configure_Memory_Protection();
  // 初始化 HAL 库
  HAL_Init();
  // 配置系统时钟
  SystemClock_Config();
  // 配置外设共同的时钟参数
  PeriphCommonClock_Config();
  // 初始化 GPIO（通用输入输出端口）
  MX_GPIO_Init();
  // 初始化 FMC（外部存储控制器）
  MX_FMC_Init();
  // 初始化 USART1（通用同步/异步收发传输器）
  MX_USART1_UART_Init();
  // 初始化 DMA（直接存储器访问）
  MX_DMA_Init();
  #if 0
  
  // 清零外部SDRAM
  for (uint32_t t = 0; t < 1024 * 1024 * 16; t++)
  { * (volatile uint32_t *)(0xC0000000 + t * 4) = 0x00000000; }
  
  #endif
  MX_LTDC_Init();
  MX_DMA2D_Init();
  #if 0
  // lvgl 初始化
  lv_init();
  lv_port_disp_init();
  // lv_demo_widgets();
	lv_demo_benchmark();
  #elif 0
  Touch_Init();
  ctp_test();
  #else
  LTDC_Fill_Area(0, 0, 599, 1023, White);
  #endif
	NAND_Init();
  LTDC_Fill_Area(100, 0, 599, 1023, Silver);
  while (1)
  {
    #if 0
    lv_task_handler();
    HAL_Delay(30);
    LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_1 | LL_GPIO_PIN_0);
    #elif 1
    HAL_Delay(500);
    LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_1 | LL_GPIO_PIN_0);
    #endif
  }
  
  return 0;
}


/**
   @function     SystemClock_Config
   @brief        None
   @param[in]    None
   @return       None
   @date         2023-12-29
*/
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
  
  while (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4)
  { }
  
  LL_PWR_ConfigSupply(LL_PWR_LDO_SUPPLY);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
  
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  { }
  
  LL_RCC_HSE_Enable();
  
  /* Wait till HSE is ready */
  while (LL_RCC_HSE_IsReady() != 1)
  { }
  
  LL_RCC_HSE_EnableCSS();
  LL_RCC_PLL_SetSource(LL_RCC_PLLSOURCE_HSE);
  LL_RCC_PLL1P_Enable();
  LL_RCC_PLL1Q_Enable();
  LL_RCC_PLL1R_Enable();
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8);
  LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL1_SetM(5);
  LL_RCC_PLL1_SetN(192);
  LL_RCC_PLL1_SetP(2);
  LL_RCC_PLL1_SetQ(2);
  LL_RCC_PLL1_SetR(2);
  LL_RCC_PLL1_Enable();
  
  /* Wait till PLL is ready */
  while (LL_RCC_PLL1_IsReady() != 1)
  { }
  
  /* Intermediate AHB prescaler 2 when target frequency clock is higher than 80 MHz */
  LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
  
  /* Wait till System clock is ready */
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
  { }
  
  LL_RCC_SetSysPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_2);
  LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_2);
  LL_SetSystemCoreClock(480000000);
  
  /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  { Error_Handler(); }
  
  LL_RCC_HSE_EnableCSS();
}

void PeriphCommonClock_Config(void)
{
  LL_RCC_PLL3R_Enable();
  LL_RCC_PLL3_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_1_2);
  LL_RCC_PLL3_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL3_SetM(25);
  LL_RCC_PLL3_SetN(240);
  LL_RCC_PLL3_SetP(2);
  LL_RCC_PLL3_SetQ(2);
  LL_RCC_PLL3_SetR(4);
  LL_RCC_PLL3_Enable();
  
  /* Wait till PLL is ready */
  while (LL_RCC_PLL3_IsReady() != 1)
  { }
}
/**
 * @function     MPU_Config
 * @brief        None
 * @param[in]    None
 * @return       None
 * @date         2023-12-30
*/
void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;
  HAL_MPU_Disable();
  MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
  MPU_InitStruct.BaseAddress      = 0xC0000000;
  MPU_InitStruct.Size             = MPU_REGION_SIZE_64MB;     // SDRAM
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
  MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.Number           = MPU_REGION_NUMBER1;
  MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
  MPU_InitStruct.SubRegionDisable = 0x00;
  MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
    @brief  This function is executed in case of error occurrence.
    @retval None
*/
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  
  while (1)
  {
  }
}
