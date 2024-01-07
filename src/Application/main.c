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
 7.NAND Flash 读写教程
  * 参考例程 https://blog.csdn.net/qq_15181569/article/details/111312027
 ...
*/


// 模拟IIC 会产生时许问题
// 开启cache 注意配置MPU


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
	// 配置MPU
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
	// 创建任务并开始调度
	StartOperatingSystem();
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
