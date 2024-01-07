#include "main.h"

#include "FreeRTOS.h"
#include "task.h"

TaskHandle_t taskHandle;


void task(void)
{
	while(1)
	{
		vTaskDelay(500);
		LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_1 | LL_GPIO_PIN_0);
	}
}

void OS_Start(void)
{
	xTaskCreate((TaskFunction_t)task,
		(const char*)"task",
		(uint16_t) 32,
		(void*)NULL,
			(UBaseType_t )0,
		(TaskHandle_t *)&taskHandle);
		vTaskStartScheduler();
}

/**
 * @brief 创建任务 并启动任务调度
 */
void StartOperatingSystem(void)
{
    // 创建一个任务
    xTaskCreate((TaskFunction_t)task,
                (const char*)"task",
                (uint16_t)32,
                (void*)NULL,
                (UBaseType_t)0,
                (TaskHandle_t*)&taskHandle);
    
    // 启动 FreeRTOS 的任务调度器
    vTaskStartScheduler();
}
