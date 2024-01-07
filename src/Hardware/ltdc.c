/**
   @copyright None
   @file      ltdc.c
   @author    Comment Vivre
   @date      2023-12-29
   @brief     None
*/
#include <main.h>

LTDC_HandleTypeDef hltdc;

/**
   @function     MX_LTDC_Init
   @brief        None
   @param[in]    None
   @return       None
   @date         2023-12-30
*/
void MX_LTDC_Init(void)
{
  LTDC_LayerCfgTypeDef pLayerCfg = {0};
  LTDC_LayerCfgTypeDef pLayerCfg1 = {0};
  // ltdc 初始化
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync     = HSW - 1;
  hltdc.Init.VerticalSync       = VSW - 1 ;
  hltdc.Init.AccumulatedHBP     = HBP + HSW - 1;
  hltdc.Init.AccumulatedVBP     = VBP + VSW - 1;
  hltdc.Init.AccumulatedActiveW = Width  + HSW + HBP - 1;
  hltdc.Init.AccumulatedActiveH = Height + VSW + VBP - 1;
  hltdc.Init.TotalWidth         = Width  + HSW + HBP + HFP - 1;
  hltdc.Init.TotalHeigh         = Height + VSW + VBP + VFP - 1;
  hltdc.Init.Backcolor.Blue     = 0;
  hltdc.Init.Backcolor.Green    = 0;
  hltdc.Init.Backcolor.Red      = 0;
  
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  { Error_Handler(); }
  
  // Layer 0
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = Width;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = Height;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 255;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = LCD_Layer_0_Addr;
  pLayerCfg.ImageWidth = 0;
  pLayerCfg.ImageHeight = 0;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  { Error_Handler(); }
  
  HAL_LTDC_SetWindowPosition(&hltdc, 0, 0, 0);      // 设置窗口位置
  HAL_LTDC_SetWindowSize(&hltdc, Width, Height, 0); // 设置窗口大小
  HAL_LTDC_EnableDither(&hltdc);                    // 颜色抖动
  #if 0
  // Layer 1
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = Width;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = Height;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg1.Alpha = 128;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg1.FBStartAdress = LCD_Layer_1_Addr;
  pLayerCfg1.ImageWidth = 0;
  pLayerCfg1.ImageHeight = 0;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1) != HAL_OK)
  { Error_Handler(); }
  
  HAL_LTDC_SetWindowPosition(&hltdc, 0, 0, 1);      // 设置窗口位置
  HAL_LTDC_SetWindowSize(&hltdc, Width, Height, 1); // 设置窗口大小
  // 颜色抖动
  HAL_LTDC_EnableDither(&hltdc);
  #endif
  HAL_LTDC_ProgramLineEvent(&hltdc, 0 );
  HAL_NVIC_SetPriority(LTDC_IRQn, 0xE, 0);
  HAL_NVIC_EnableIRQ(LTDC_IRQn);
  LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_5);
}

/**
   @function     HAL_LTDC_MspInit
   @brief        None
   @param[in]    ltdcHandle: [输入/出]
   @return       None
   @date         2023-12-30
*/
void HAL_LTDC_MspInit(LTDC_HandleTypeDef * ltdcHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  if (ltdcHandle->Instance == LTDC)
  {
    /* USER CODE BEGIN LTDC_MspInit 0 */
    /* USER CODE END LTDC_MspInit 0 */
    /* LTDC clock enable */
    __HAL_RCC_LTDC_CLK_ENABLE();
    __HAL_RCC_GPIOK_CLK_ENABLE();
    __HAL_RCC_GPIOJ_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();
    /**LTDC GPIO Configuration
      PK5     ------> LTDC_B6       PK4     ------> LTDC_B5       PJ15    ------> LTDC_B3
      PK6     ------> LTDC_B7       PK3     ------> LTDC_B4       PK7     ------> LTDC_DE
      PJ14    ------> LTDC_B2       PJ12    ------> LTDC_B0       PJ13    ------> LTDC_B1
      PI12    ------> LTDC_HSYNC    PI13    ------> LTDC_VSYNC    PI14    ------> LTDC_CLK
      PK2     ------> LTDC_G7       PK0     ------> LTDC_G5       PK1     ------> LTDC_G6
      PJ11    ------> LTDC_G4       PJ10    ------> LTDC_G3       PJ9     ------> LTDC_G2
      PJ0     ------> LTDC_R1       PJ8     ------> LTDC_G1       PJ7     ------> LTDC_G0
      PJ6     ------> LTDC_R7       PI15    ------> LTDC_R0       PJ1     ------> LTDC_R2
      PJ5     ------> LTDC_R6       PJ2     ------> LTDC_R3       PJ3     ------> LTDC_R4
      PJ4     ------> LTDC_R5
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_3
      | GPIO_PIN_7 | GPIO_PIN_2 | GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOK, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_12 | GPIO_PIN_13
      | GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_9 | GPIO_PIN_0
      | GPIO_PIN_8 | GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_1
      | GPIO_PIN_5 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOJ, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
    // 背光引脚初始化
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}


