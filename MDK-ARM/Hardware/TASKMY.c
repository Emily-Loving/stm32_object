#include "TASKMY.h"

void vBlueTask(void)
{
	Fly_ModeTDF FlyDate;
	
    v_Bluetooth_Process();
	
	osMessageQueueGet(BlueQueue01Handle, &FlyDate, NULL, osWaitForever);//接收pid
	
	if (FlyDate.flag)
	{
		char Buf[2];
		//正常状态下在命令
		
		sprintf(Buf,"%d",(int)FlyDate.fPitch);
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[8], (uint16_t)strlen(CommendBlue[8]), 100);
		HAL_UART_Transmit(&huart1, (uint8_t *)Buf, strlen(Buf), 100);
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[7], (uint16_t)strlen(CommendBlue[7]), 100);
		
		sprintf(Buf,"%d",(int)FlyDate.fRoll);	
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[9], (uint16_t)strlen(CommendBlue[9]), 100);
		HAL_UART_Transmit(&huart1, (uint8_t *)Buf, strlen(Buf), 100);
	    HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[6], (uint16_t)strlen(CommendBlue[6]), 100);
	}
}

void v_MPU6050Task(void)
{
	MPU6050_DataTDF MPU6050Date;
	MPU6050_HandleDataTDF MPU6050Date2;
	float dt = 0.01;// 假设你的主循环周期为 10ms (100Hz)
	
	vMPU6050_Read(&MPU6050Date);
	
	vMPU6050_UpdateAngles(&MPU6050Date,&MPU6050Date2,dt);
	
	osMessageQueuePut(MPUQueue02Handle, &MPU6050Date2, 0, 0);//给pid
}

void v_PidTask(void)
{
	MPU6050_HandleDataTDF MPU6050Date2;
	Fly_ModeTDF FlyDate;
	
	osMessageQueueGet(MPUQueue02Handle, &MPU6050Date2, NULL, osWaitForever);
	
	FlyDate.fPitch = MPU6050Date2.fPitch;
	FlyDate.fRoll = MPU6050Date2.fRoll;
	
	uint8_t i = Analyse_Date_pid(&MPU6050Date2);
	
	/* Analyse_Date_pid: 0=正常 1=错误(角度超限)。
	   正常时才把角度经蓝牙发出，超限时不发（保持原保护动作）。 */
	if ( i == 0)
	{
		//姿态正常，允许蓝牙打印角度
		FlyDate.flag = 1;
	}
	else
	{
		//角度超限或异常，不打印
		FlyDate.flag = 0;
	}
	
	volatile int PU1,PU2,PU3;
	PU1 = MPU6050Date2.fPitch;
	PU2 = MPU6050Date2.fRoll;
	
	osMessageQueuePut(BlueQueue01Handle, &FlyDate, 0, 0);//给pid
    osDelay(10);
}

void LedTask(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,GPIO_PIN_SET);
	osDelay(500);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,GPIO_PIN_RESET);
	osDelay(500);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,GPIO_PIN_SET);
	osDelay(500);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0,GPIO_PIN_RESET);
	osDelay(500);
}
