/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId foc_taskHandle;
osThreadId dbug_infoHandle;
osThreadId setupTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void foc_task_fun(void const * argument);
void dbug_info_fun(void const * argument);
void setupTaskFun(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of foc_task */
  osThreadDef(foc_task, foc_task_fun, osPriorityRealtime, 0, 1000);
  foc_taskHandle = osThreadCreate(osThread(foc_task), NULL);

  /* definition and creation of dbug_info */
  osThreadDef(dbug_info, dbug_info_fun, osPriorityLow, 0, 1000);
  dbug_infoHandle = osThreadCreate(osThread(dbug_info), NULL);

  /* definition and creation of setupTask */
  osThreadDef(setupTask, setupTaskFun, osPriorityRealtime, 0, 256);
  setupTaskHandle = osThreadCreate(osThread(setupTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_foc_task_fun */
/**
  * @brief  Function implementing the foc_task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_foc_task_fun */
__weak void foc_task_fun(void const * argument)
{
  /* USER CODE BEGIN foc_task_fun */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END foc_task_fun */
}

/* USER CODE BEGIN Header_dbug_info_fun */
/**
* @brief Function implementing the dbug_info thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_dbug_info_fun */
__weak void dbug_info_fun(void const * argument)
{
  /* USER CODE BEGIN dbug_info_fun */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END dbug_info_fun */
}

/* USER CODE BEGIN Header_setupTaskFun */
/**
* @brief Function implementing the setupTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_setupTaskFun */
__weak void setupTaskFun(void const * argument)
{
  /* USER CODE BEGIN setupTaskFun */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END setupTaskFun */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
