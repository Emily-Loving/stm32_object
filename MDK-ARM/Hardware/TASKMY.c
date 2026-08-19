#include "TASKMY.h"

void vBlueTask(void)
{
    v_Bluetooth_Process();
	
//		osMessageQueueGet(MPUQueue02Handle, &MPU6050Date, NULL, osWaitForever);接收pid
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
	
	osMessageQueueGet(MPUQueue02Handle, &MPU6050Date2, NULL, osWaitForever);
	
	
	
	
	volatile int PU1,PU2,PU3;
	PU1 = MPU6050Date2.fPitch;
	PU2 = MPU6050Date2.fRoll;


	
	
	
//	osMessageQueuePut(MPUQueue02Handle, &, 0, 0);//给blue
//    osDelay(10);
}

void LedTask(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,GPIO_PIN_SET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,GPIO_PIN_RESET);
	HAL_Delay(500);
}
