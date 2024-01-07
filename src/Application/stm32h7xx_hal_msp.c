/**
 * @copyright None
 * @file      stm32h7xx_hal_msp.c
 * @author    Comment Vivre 
 * @date      2024-01-06
 * @brief     None
 */

#include "main.h"


/**
    Initializes the Global MSP.
*/
void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  /* System interrupt init*/
}
