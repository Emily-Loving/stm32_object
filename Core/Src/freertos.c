/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "TASKMY.h"
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
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for BlueTask01 */
osThreadId_t BlueTask01Handle;
const osThreadAttr_t BlueTask01_attributes = {
  .name = "BlueTask01",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for MPUTask02 */
osThreadId_t MPUTask02Handle;
const osThreadAttr_t MPUTask02_attributes = {
  .name = "MPUTask02",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for PidTask03 */
osThreadId_t PidTask03Handle;
const osThreadAttr_t PidTask03_attributes = {
  .name = "PidTask03",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for BlueQueue01 */
osMessageQueueId_t BlueQueue01Handle;
const osMessageQueueAttr_t BlueQueue01_attributes = {
  .name = "BlueQueue01"
};
/* Definitions for MPUQueue02 */
osMessageQueueId_t MPUQueue02Handle;
const osMessageQueueAttr_t MPUQueue02_attributes = {
  .name = "MPUQueue02"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void BlueStartTask01(void *argument);
void MPUStartTask02(void *argument);
void PidStartTask03(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

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

  /* Create the queue(s) */
  /* creation of BlueQueue01 */
  BlueQueue01Handle = osMessageQueueNew (16, sizeof(uint16_t), &BlueQueue01_attributes);

  /* creation of MPUQueue02 */
  MPUQueue02Handle = osMessageQueueNew (24, sizeof(uint16_t), &MPUQueue02_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of BlueTask01 */
  BlueTask01Handle = osThreadNew(BlueStartTask01, NULL, &BlueTask01_attributes);

  /* creation of MPUTask02 */
  MPUTask02Handle = osThreadNew(MPUStartTask02, NULL, &MPUTask02_attributes);

  /* creation of PidTask03 */
  PidTask03Handle = osThreadNew(PidStartTask03, NULL, &PidTask03_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
	
  for(;;)
  {
    LedTask();
    osDelay(10);

  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_BlueStartTask01 */
/**
* @brief Function implementing the BlueTask01 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_BlueStartTask01 */
void BlueStartTask01(void *argument)
{
  /* USER CODE BEGIN BlueStartTask01 */
  /* Infinite loop */
  
  v_Bluetooth_Init();

  for(;;)
  {
	vBlueTask();
	/*osDelay(10)~osDelay(80)与[1,10]的映射  作用是控制蓝牙发送数据的快慢，见Variable.h*/
    osDelay((int)((70 * BLUE_SEND_TIME)/9 + 0.5)); 
  }
  /* USER CODE END BlueStartTask01 */
}

/* USER CODE BEGIN Header_MPUStartTask02 */
/**
* @brief Function implementing the MPUTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MPUStartTask02 */
void MPUStartTask02(void *argument)
{
  /* USER CODE BEGIN MPUStartTask02 */
  /* Infinite loop */
  vMPU6050_Init();
  for(;;)
  {
    v_MPU6050Task();
    osDelay(10);
  }
  /* USER CODE END MPUStartTask02 */
}

/* USER CODE BEGIN Header_PidStartTask03 */
/**
* @brief Function implementing the PidTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_PidStartTask03 */
void PidStartTask03(void *argument)
{
  /* USER CODE BEGIN PidStartTask03 */
  /* Infinite loop */
	Motor_Init();
  for(;;)
  {
	v_PidTask();
    osDelay(10);
  }
  /* USER CODE END PidStartTask03 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

