/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *

  * Copyright (c) 2022 STMicroelectronics.
=======
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
osThreadId StartINSTaskHandle;
osThreadId statetaskHandle;
osThreadId gimbaltaskHandle;
osThreadId firetaskHandle;
osThreadId covertaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void INS_Task(void const * argument);
void stateTask(void const * argument);
void gimbalTask(void const * argument);
void fireTask(void const * argument);
void coverTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

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
  /* definition and creation of StartINSTask */
  osThreadDef(StartINSTask, INS_Task, osPriorityRealtime, 0, 128);
  StartINSTaskHandle = osThreadCreate(osThread(StartINSTask), NULL);

  /* definition and creation of statetask */
  osThreadDef(statetask, stateTask, osPriorityIdle, 0, 128);
  statetaskHandle = osThreadCreate(osThread(statetask), NULL);

  /* definition and creation of gimbaltask */
  osThreadDef(gimbaltask, gimbalTask, osPriorityIdle, 0, 128);
  gimbaltaskHandle = osThreadCreate(osThread(gimbaltask), NULL);

  /* definition and creation of firetask */
  osThreadDef(firetask, fireTask, osPriorityIdle, 0, 128);
  firetaskHandle = osThreadCreate(osThread(firetask), NULL);

  /* definition and creation of covertask */
  osThreadDef(covertask, coverTask, osPriorityIdle, 0, 128);
  covertaskHandle = osThreadCreate(osThread(covertask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_INS_Task */
/**
  * @brief  Function implementing the StartINSTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_INS_Task */
__weak void INS_Task(void const * argument)
{
  /* USER CODE BEGIN INS_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END INS_Task */
}

/* USER CODE BEGIN Header_stateTask */
/**
* @brief Function implementing the statetask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_stateTask */
__weak void stateTask(void const * argument)
{
  /* USER CODE BEGIN stateTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END stateTask */
}

/* USER CODE BEGIN Header_gimbalTask */
/**
* @brief Function implementing the gimbaltask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_gimbalTask */
__weak void gimbalTask(void const * argument)
{
  /* USER CODE BEGIN gimbalTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END gimbalTask */
}

/* USER CODE BEGIN Header_fireTask */
/**
* @brief Function implementing the firetask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_fireTask */
__weak void fireTask(void const * argument)
{
  /* USER CODE BEGIN fireTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END fireTask */
}

/* USER CODE BEGIN Header_coverTask */
/**
* @brief Function implementing the covertask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_coverTask */
__weak void coverTask(void const * argument)
{
  /* USER CODE BEGIN coverTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END coverTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
