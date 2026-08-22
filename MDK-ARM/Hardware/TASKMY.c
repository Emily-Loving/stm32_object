#include "TASKMY.h"

void vBlueTask(void)
{
	Fly_ModeTDF FlyDate;
	
    uint8_t FlyCommend = v_Bluetooth_Process();
	
	osMessageQueueGet(BlueQueue01Handle, &FlyDate, NULL, osWaitForever);//接收pid
	
	// 排空积压的旧帧 只保留最新一帧，消除串口显示延迟 
	while (osMessageQueueGet(BlueQueue01Handle, &FlyDate, NULL, 0) == osOK)
	{
		// 循环体空：FlyDate 被不断覆盖为更新的值 
	}
	
	if (FlyDate.flag)
	{
		char Buf[16];   /* 至少容纳 "-2147483648\0"(12 字节)，取 16 留余量 */

		//  MPU 无有效数据时角度会算成 NaN/Inf (int)NaN 会得到 -2147483648。
		//  这种帧直接丢弃不发，避免上位机收到脏数据
		if (isnan(FlyDate.fPitch) || isinf(FlyDate.fPitch) ||
		    isnan(FlyDate.fRoll)  || isinf(FlyDate.fRoll))
		{
			return;
		}

		//正常状态下在命令
		sprintf(Buf,"%d",(int)FlyDate.fPitch);
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[10], (uint16_t)strlen(CommendBlue[10]), 100);
		HAL_UART_Transmit(&huart1, (uint8_t *)Buf, strlen(Buf), 100);
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[9], (uint16_t)strlen(CommendBlue[9]), 100);
		
		sprintf(Buf,"%d",(int)FlyDate.fRoll);	
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[11], (uint16_t)strlen(CommendBlue[11]), 100);
		HAL_UART_Transmit(&huart1, (uint8_t *)Buf, strlen(Buf), 100);
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[9], (uint16_t)strlen(CommendBlue[9]), 100);
		
	    HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[FlyCommend], (uint16_t)strlen(CommendBlue[FlyCommend]), 100);
		useMotor(FlyCommend);
	    HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[8], (uint16_t)strlen(CommendBlue[8]), 100);
	}
	else
	{
		Motor_stop();
	}
}

void v_MPU6050Task(void)
{
	MPU6050_DataTDF MPU6050Date;
	MPU6050_HandleDataTDF MPU6050Date2;
	float dt = 0.01;// 假设你的主循环周期为 10ms (100Hz)
	
	/* 读失败(MPU 离线/无应答)时直接返回，不解算、不投队列，
	   避免全 0 脏数据经互补滤波算出 NaN 后被当角度发出。 */
	if (vMPU6050_Read(&MPU6050Date) == 0)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[11], (uint16_t)strlen(CommendBlue[11]), 100);
		return;
	}
	
	vMPU6050_UpdateAngles(&MPU6050Date,&MPU6050Date2,dt);
	
	osMessageQueuePut(MPUQueue02Handle, &MPU6050Date2, 0, 0);//给pid
}

void v_PidTask(void)
{
	MPU6050_HandleDataTDF MPU6050Date2;
	Fly_ModeTDF FlyDate;
	
	osMessageQueueGet(MPUQueue02Handle, &MPU6050Date2, NULL, osWaitForever);
	
	//非阻塞排空队列里积压的旧帧，只保留最新一帧，消除延迟 
	while (osMessageQueueGet(MPUQueue02Handle, &MPU6050Date2, NULL, 0) == osOK)
	{
		// 循环体空：MPU6050Date2 被不断覆盖为更新的值 
	}
	
	FlyDate.fPitch = MPU6050Date2.fPitch;
	FlyDate.fRoll = MPU6050Date2.fRoll;
	
	uint8_t i = Analyse_Date_pid(&MPU6050Date2);
	
	if ( i == 0)
	{
		//姿态正常 允许蓝牙打印角度
		FlyDate.flag = 1;
	}
	else
	{
		//角度超限或异常 不打印
		FlyDate.flag = 0;
	}
	
	osMessageQueuePut(BlueQueue01Handle, &FlyDate, 0, 0);//给pid
    osDelay(10);
}

void LedTask(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,GPIO_PIN_SET);
	osDelay(500);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,GPIO_PIN_RESET);
	osDelay(500);

}
