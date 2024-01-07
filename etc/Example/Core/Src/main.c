/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "dma2d.h"
#include "ltdc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
uint8_t SDRAM_Send_Cmd(uint8_t bankx, uint8_t cmd, uint8_t refresh, uint16_t regval);
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
unsigned char str[] = {"waiting...\r\n"};
unsigned char rec[20];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void UART_Init(void)
{
  LL_USART_SetTransferDirection(USART1, LL_USART_DIRECTION_TX_RX);
  // r
  LL_USART_EnableIT_IDLE(USART1);
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_0, (uint32_t)rec);
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_0, 20);
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_0, (uint32_t)&USART1->RDR);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_0);
  LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_0);
  LL_USART_EnableDMAReq_RX(USART1);
  // t
  LL_DMA_SetPeriphAddress(DMA1, LL_DMA_STREAM_1, (uint32_t)&USART1->TDR);
  LL_USART_EnableDMAReq_TX(USART1);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_STREAM_1);
}

void UART_DMA_Send(USART_TypeDef * USARTx, uint8_t * str, uint16_t size)
{
  LL_DMA_SetMemoryAddress(DMA1, LL_DMA_STREAM_1, (uint32_t)str);
  LL_DMA_SetDataLength(DMA1, LL_DMA_STREAM_1, size);
  LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_1);
}


// uint16_t testsram[250000] __attribute__((section(".ARM.__at_0xC0000000")));
extern SDRAM_HandleTypeDef hsdram1;
#define Bank5_SDRAM_ADDR    ((uint32_t)(0xC0000000)) //SDRAM开始地址
// MAX SIZE 64*1024*1024 = 0x400 0000
// SDRAM配置参数
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)


void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef * hsdram)
{
  uint32_t temp = 0;
  // SDRAM控制器初始化完成以后还需要按照如下顺序初始化SDRAM
  SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_CLK_ENABLE, 1, 0);       // 时钟配置使能
  HAL_Delay(1);
  SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_PALL, 1, 0);             // 对所有存储区预充电
  SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_AUTOREFRESH_MODE, 8, 0); // 设置自刷新次数
  // 配置模式寄存器,SDRAM的bit0~bit2为指定突发访问的长度，
  // bit3为指定突发访问的类型，bit4~bit6为CAS值，bit7和bit8为运行模式
  // bit9为指定的写突发模式，bit10和bit11位保留位
  temp = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_4          | // 设置突发长度:1(可以是1/2/4/8)
    SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL                  | // 设置突发类型:连续(可以是连续/交错)
    SDRAM_MODEREG_CAS_LATENCY_2                          | // 设置CAS值:2(可以是2/3)
    SDRAM_MODEREG_OPERATING_MODE_STANDARD                | // 设置操作模式:0,标准模式
    SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;                  // 设置突发写模式:1,单点访问
  SDRAM_Send_Cmd(0, FMC_SDRAM_CMD_LOAD_MODE, 1, temp);     // 设置SDRAM的模式寄存器
  // 刷新频率计数器(以SDCLK频率计数),计算方法:
  // COUNT=SDRAM刷新周期/行数-20=SDRAM刷新周期(us)*SDCLK频率(Mhz)/行数
  // 我们使用的SDRAM刷新周期为64ms,SDCLK=200/2=100Mhz,行数为8192(2^13).
  // 所以,COUNT=64*1000*120/8192-20=917.5
  HAL_SDRAM_ProgramRefreshRate(&hsdram1, 917);
}

uint8_t SDRAM_Send_Cmd(uint8_t bankx, uint8_t cmd, uint8_t refresh, uint16_t regval)
{
  uint32_t target_bank = 0;
  FMC_SDRAM_CommandTypeDef Command;
  
  if (bankx == 0)
  {
    target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
  }
  else if (bankx == 1)
  {
    target_bank = FMC_SDRAM_CMD_TARGET_BANK2;
  }
  
  Command.CommandMode = cmd;              //命令
  Command.CommandTarget = target_bank;    //目标SDRAM存储区域
  Command.AutoRefreshNumber = refresh;    //自刷新次数
  Command.ModeRegisterDefinition = regval; //要写入模式寄存器的值
  
  if (HAL_SDRAM_SendCommand(&hsdram1, &Command, 0xFFFF) == HAL_OK) //向SDRAM发送命令
  {
    return 0;
  }
  else
  {
    return 1;
  }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	SCB_EnableICache();//使能I-Cache
	SCB_EnableDCache();//使能D-Cache   
	SCB->CACR|=1<<2;
  char Test_Dat[10];
	uint16_t TestLen;
	uint32_t ts;
  /* USER CODE END 1 */
/* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_FMC_Init();
  MX_USART1_UART_Init();
  MX_TIM17_Init();
  MX_SPI2_Init();
  MX_LTDC_Init();
  MX_DMA2D_Init();
  /* USER CODE BEGIN 2 */
  
  SDRAM_Initialization_Sequence(&hsdram1);
  UART_Init();
                                  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1 | LL_GPIO_PIN_0);
    HAL_Delay(100);
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1 | LL_GPIO_PIN_0);
    HAL_Delay(100);

//		*(__IO uint32_t*)(0xC30549D0) = 0x5A5AA5A5;
//		ts = *(__IO uint32_t*)(0xC30549D0);
		UART_DMA_Send(USART1, str, 20);
  }
  
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_4)
  {
  }
  LL_PWR_ConfigSupply(LL_PWR_LDO_SUPPLY);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE0);
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
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
  LL_RCC_PLL1_SetQ(20);
  LL_RCC_PLL1_SetR(2);
  LL_RCC_PLL1_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL1_IsReady() != 1)
  {
  }

   /* Intermediate AHB prescaler 2 when target frequency clock is higher than 80 MHz */
   LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
  {

  }
  LL_RCC_SetSysPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_2);
  LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_2);
  LL_SetSystemCoreClock(480000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_HSE_EnableCSS();
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  LL_RCC_PLL3R_Enable();
  LL_RCC_PLL3_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_1_2);
  LL_RCC_PLL3_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL3_SetM(25);
  LL_RCC_PLL3_SetN(240);
  LL_RCC_PLL3_SetP(2);
  LL_RCC_PLL3_SetQ(2);
  LL_RCC_PLL3_SetR(2);
  LL_RCC_PLL3_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL3_IsReady() != 1)
  {
  }

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  
  while (1)
  {
  }
  
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
