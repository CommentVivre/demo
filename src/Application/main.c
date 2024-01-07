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
  // PeriphCommonClock_Config();
  // 初始化 GPIO（通用输入输出端口）
  MX_GPIO_Init();
  // 初始化 FMC（外部存储控制器）
  MX_FMC_Init();
  // 初始化 USART1（通用同步/异步收发传输器）
  MX_USART1_UART_Init();
  // 初始化 DMA（直接存储器访问）
  MX_DMA_Init();

  
  // 清零外部SDRAM
  for (uint32_t t = 0; t < 1024 * 1024 * 16; t++)
  { * (volatile uint32_t *)(0xC0000000 + t * 4) = 0x00000000; }
  
	#if 0
  MX_LTDC_Init();
  MX_DMA2D_Init();
	#endif
	
	#if 1
  #if 1
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
    #if 1
    lv_task_handler();
    HAL_Delay(30);
    LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_1 | LL_GPIO_PIN_0);
    #elif 1
    HAL_Delay(500);
    LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_1 | LL_GPIO_PIN_0);
    #endif
  }
  #endif
  return 0;
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
