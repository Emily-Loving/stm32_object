#include "TASKMY.h"

/// @NOTE 蓝牙任务：取命令 + 从队列拿最新一帧姿态角，姿态正常则串口打印角度并驱动电机，异常则停机
/// @param void
/// @return
void vBlueTask(void)
{
	Fly_ModeTDF st_Task_FlyData;
	
    uint8_t uc_Task_FlyCommend = v_Bluetooth_Process();
	
	//收到电池查询命令[B] 发送 SEND_BUTTER_NUM 次电量
	if (uc_Task_FlyCommend == 8)
	{
		char c_Task_BatBuf[8];   
		uint8_t uc_Task_BatLevel = uc_Bat_GetPercent();

		sprintf(c_Task_BatBuf, "%d", uc_Task_BatLevel);

		for (uint8_t uc_Task_i = 0; uc_Task_i < SEND_BUTTER_NUM; uc_Task_i++)
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[12], (uint16_t)strlen(CommendBlue[12]), 100);
			HAL_UART_Transmit(&huart1, (uint8_t *)c_Task_BatBuf, strlen(c_Task_BatBuf), 100);
			HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[13], (uint16_t)strlen(CommendBlue[13]), 100);
			HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[8], (uint16_t)strlen(CommendBlue[8]), 100);
		}
		return;
	}
	
	osMessageQueueGet(BlueQueue01Handle, &st_Task_FlyData, NULL, osWaitForever);//接收pid
	
	// 排空积压的旧帧 只保留最新一帧，消除串口显示延迟 
	while (osMessageQueueGet(BlueQueue01Handle, &st_Task_FlyData, NULL, 0) == osOK)
	{
		// 循环体空：st_Task_FlyData 被不断覆盖为更新的值 
	}
	
	if (st_Task_FlyData.uc_Fly_Flag)
	{
		char c_Task_Buf[16];   // 至少容纳 "-2147483648\0"(12 字节)

		//  MPU 无有效数据时角度会算成 NaN/Inf (int)NaN 会得到 -2147483648 
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
		HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[7], (uint16_t)strlen(CommendBlue[7]), 100);
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
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_SET);
	osDelay(500);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_RESET);
	osDelay(500);
}

/// @NOTE 读取 PA4(ADC1_IN4) 的 ADC 原始值，多次采样取平均以抑制抖动
/// @param void
/// @return 平均后的 12 位 ADC 原始值[0,4095]，转换失败返回 0
uint16_t u16_Bat_ReadRaw(void)
{
	uint32_t u32_Bat_Sum = 0;
	uint8_t  uc_Bat_ValidCnt = 0;

	for (uint8_t uc_Bat_i = 0; uc_Bat_i < BAT_SAMPLE_TIMES; uc_Bat_i++)
	{
		HAL_ADC_Start(&hadc1);

		if (HAL_ADC_PollForConversion(&hadc1, BAT_ADC_TIMEOUT) == HAL_OK)
		{
			u32_Bat_Sum += HAL_ADC_GetValue(&hadc1);
			uc_Bat_ValidCnt++;
		}

		HAL_ADC_Stop(&hadc1);
	}

	/* 全部采样失败则返回 0，避免除零 */
	if (uc_Bat_ValidCnt == 0)
	{
		return 0;
	}

	return (uint16_t)(u32_Bat_Sum / uc_Bat_ValidCnt);
}

/// @NOTE 采集并换算出电池实际电压（已还原外部分压）
/// @param void
/// @return 电池实际电压，单位 mV
uint16_t u16_Bat_GetVoltage(void)
{
	uint16_t u16_Bat_Raw = u16_Bat_ReadRaw();

	/* 采样点电压(mV) = raw / 4095 * Vref */
	uint32_t u32_Bat_PinMv = ((uint32_t)u16_Bat_Raw * BAT_VREF_MV) / BAT_ADC_MAX;

	/* 还原分压：实际电压 = 采样点电压 × 分压比 */
	return (uint16_t)(u32_Bat_PinMv * BAT_DIV_RATIO);
}

/// @NOTE 采集电池电压并线性换算为电量百分比（按单节锂电 3.0~4.2V 区间）
/// @param void
/// @return 电量百分比[0,100]
uint8_t uc_Bat_GetPercent(void)
{
	uint16_t u16_Bat_Mv = u16_Bat_GetVoltage();

	/* 夹在有效区间，防止百分比越界或整型下溢 */
	if (u16_Bat_Mv >= BAT_VOLT_FULL_MV)
	{
		return 100;
	}
	if (u16_Bat_Mv <= BAT_VOLT_EMPTY_MV)
	{
		return 0;
	}

	/* 线性映射：(当前 - 空电) / (满电 - 空电) × 100 */
	uint32_t u32_Bat_Percent = ((uint32_t)(u16_Bat_Mv - BAT_VOLT_EMPTY_MV) * 100U)
	                           / (BAT_VOLT_FULL_MV - BAT_VOLT_EMPTY_MV);

	return (uint8_t)u32_Bat_Percent;
}
