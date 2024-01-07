/**
 * @copyright None
 * @file      test.c
 * @author    Comment Vivre
 * @date      2024-01-07
 * @brief     None
 */
#include "main.h"
// 测试文件

uint32_t Pen_Color  = 0xFF000000;
uint32_t Back_Color = 0xFFFFFFFF;

void lvgl_test(void)
{
	#if 0
  // 在 Cortex-M 处理器的 Cache 控制寄存器中设置某个位
  // 但未提供具体细节
  SCB->CACR |= 1 << 2; 
	#endif
  // 清零外部SDRAM
  for (uint32_t t = 0; t < 1024 * 1024 * 16; t++)
  { * (volatile uint32_t *)(0xC0000000 + t * 4) = 0x00000000; }
  
  MX_LTDC_Init();
  MX_DMA2D_Init();
  #if 0
  // lvgl 初始化
  lv_init();
  lv_port_disp_init();
  // lv_demo_widgets();
  lv_demo_benchmark();
  #elif 1
  Touch_Init();
  ctp_test();
  LTDC_Fill_Area(0, 0, 599, 1023, White);
  #endif
  
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
}
