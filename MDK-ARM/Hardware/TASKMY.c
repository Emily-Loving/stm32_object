#include "TASKMY.h"

/// @NOTE 蓝牙任务：取命令 + 从队列拿最新一帧姿态角，姿态正常则串口打印角度并驱动电机，异常则停机
/// @param void
/// @return
void vBlueTask(void)
{
	Fly_ModeTDF st_Task_FlyData;
	
    uint8_t uc_Task_FlyCommend = v_Bluetooth_Process();
	
	osMessageQueueGet(BlueQueue01Handle, &st_Task_FlyData, NULL, osWaitForever);//接收pid
	
	// 排空积压的旧帧 只保留最新一帧，消除串口显示延迟 
	while (osMessageQueueGet(BlueQueue01Handle, &st_Task_FlyData, NULL, 0) == osOK)
	{
		// 循环体空：st_Task_FlyData 被不断覆盖为更新的值 
	}
	
	if (st_Task_FlyData.uc_Fly_Flag)
	{
		char c_Task_Buf[16];   /* 至少容纳 "-2147483648\0"(12 字节)，取 16 留余量 */

		//  MPU 无有效数据时角度会算成 NaN/Inf (int)NaN 会得到 -2147483648。
		//  这种帧直接丢弃不发，避免上位机收到脏数据
		if (isnan(st_Task_FlyData.f_Fly_Pitch) || isinf(st_Task_FlyData.f_Fly_Pitch) ||
		    isnan(st_Task_FlyData.f_Fly_Roll)  || isinf(st_Task_FlyData.f_Fly_Roll))
		{
			return;
		}

		//正常状态下在命令
		sprintf(c_Task_Buf,"%d",(int)st_Task_FlyData.f_Fly_Pitch);
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[10], (uint16_t)strlen(CommendBlue[10]), 100);
		HAL_UART_Transmit(&huart1, (uint8_t *)c_Task_Buf, strlen(c_Task_Buf), 100);
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[9], (uint16_t)strlen(CommendBlue[9]), 100);
		
		sprintf(c_Task_Buf,"%d",(int)st_Task_FlyData.f_Fly_Roll);	
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[11], (uint16_t)strlen(CommendBlue[11]), 100);
		HAL_UART_Transmit(&huart1, (uint8_t *)c_Task_Buf, strlen(c_Task_Buf), 100);
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[9], (uint16_t)strlen(CommendBlue[9]), 100);
		
	    HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[uc_Task_FlyCommend], (uint16_t)strlen(CommendBlue[uc_Task_FlyCommend]), 100);
		useMotor(uc_Task_FlyCommend);
	    HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[8], (uint16_t)strlen(CommendBlue[8]), 100);
	}
	else
	{
		Motor_stop();
	}
}

/// @NOTE MPU6050 采集任务：读原始数据 -> 解算角度 -> 投递到 PID 队列；读失败则直接返回不投递
/// @param void
/// @return
void v_MPU6050Task(void)
{
	MPU6050_DataTDF st_Task_Raw;
	MPU6050_HandleDataTDF st_Task_Angle;
	float f_Task_Dt = 0.01;// 假设你的主循环周期为 10ms (100Hz)
	
	/* 读失败(MPU 离线/无应答)时直接返回，不解算、不投队列，
	   避免全 0 脏数据经互补滤波算出 NaN 后被当角度发出。 */
	if (vMPU6050_Read(&st_Task_Raw) == 0)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[11], (uint16_t)strlen(CommendBlue[11]), 100);
		return;
	}
	
	vMPU6050_UpdateAngles(&st_Task_Raw,&st_Task_Angle,f_Task_Dt);
	
	osMessageQueuePut(MPUQueue02Handle, &st_Task_Angle, 0, 0);//给pid
}

/// @NOTE PID 任务：从队列取最新姿态角，做超限保护判断后置 flag，再投递给蓝牙任务
/// @param void
/// @return
void v_PidTask(void)
{
	MPU6050_HandleDataTDF st_Task_Angle;
	Fly_ModeTDF st_Task_FlyData;
	
	osMessageQueueGet(MPUQueue02Handle, &st_Task_Angle, NULL, osWaitForever);
	
	//非阻塞排空队列里积压的旧帧，只保留最新一帧，消除延迟 
	while (osMessageQueueGet(MPUQueue02Handle, &st_Task_Angle, NULL, 0) == osOK)
	{
		// 循环体空：st_Task_Angle 被不断覆盖为更新的值 
	}
	
	st_Task_FlyData.f_Fly_Pitch = st_Task_Angle.f_Mpu_Pitch;
	st_Task_FlyData.f_Fly_Roll = st_Task_Angle.f_Mpu_Roll;
	
	uint8_t uc_Task_Ret = Analyse_Date_pid(&st_Task_Angle);
	
	if ( uc_Task_Ret == 0)
	{
		//姿态正常 允许蓝牙打印角度
		st_Task_FlyData.uc_Fly_Flag = 1;
	}
	else
	{
		//角度超限或异常 不打印
		st_Task_FlyData.uc_Fly_Flag = 0;
	}
	
	osMessageQueuePut(BlueQueue01Handle, &st_Task_FlyData, 0, 0);//给pid
    osDelay(10);
}

/// @NOTE LED 任务：PC13 每 500ms 翻转一次用作运行心跳指示
/// @param void
/// @return
void LedTask(void)
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,GPIO_PIN_SET);
	osDelay(500);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,GPIO_PIN_RESET);
	osDelay(500);

}
