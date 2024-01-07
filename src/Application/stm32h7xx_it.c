#include "main.h"
#include "stm32h7xx_it.h"

#include "FreeRTOS.h"
#include "task.h"

void xPortSysTickHandler( void );
//BaseType_t xTaskGetSchedulerState( void );
/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
    @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  while (1)
  {
  }
}

/**
    @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
    @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  while (1)
  {
  }
}

/**
    @brief This function handles Pre-fetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  while (1)
  {
  }
}

/**
    @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  while (1)
  {
  }
}
#if (!defined(FREERTOS_CONFIG_H))
/**
    @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
}
#endif
/**
    @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
}
#if (!defined(FREERTOS_CONFIG_H))
/**
    @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
}
#endif
/**
    @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  HAL_IncTick();
	lv_tick_inc(1);
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)
	{ xPortSysTickHandler();}
}
extern LTDC_HandleTypeDef hltdc;
/**
  * @brief  This function handles LTDC global interrupt request.
  * @param  None
  * @retval None
  */
void LTDC_IRQHandler(void)
{
  HAL_LTDC_IRQHandler(&hltdc);
}
/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/
