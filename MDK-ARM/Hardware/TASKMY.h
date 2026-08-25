#ifndef __TASKMY_H
#define __TASKMY_H

#include "MPU6050.h"
#include "bluetooth.h"
#include "motor.h"
#include "pid.h"
#include "freertos.h"
#include "gpio.h"
#include "adc.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>
#include "Variable.h"

// 电池电量检测相关参数
#define BAT_VREF_MV          3300   // ADC 参考电压(mV)
#define BAT_ADC_MAX          4095   // 12 位 ADC 满量程原始值
#define BAT_DIV_RATIO        2      // 外部分压比 10k + 10k 实际电压 = 采样电压 × 2
#define BAT_SAMPLE_TIMES     10     // 采样平均次数，抑制抖动
#define BAT_ADC_TIMEOUT      10    // 单次 ADC 转换等待超时(ms)
#define BAT_VOLT_FULL_MV     4200   // 单节锂电满电电压 4.2V
#define BAT_VOLT_EMPTY_MV    3000   // 单节锂电空电电压 3.0V

extern osMessageQueueId_t MPUQueue02Handle;
extern osMessageQueueId_t BlueQueue01Handle;

void vBlueTask(void);
void v_MPU6050Task(void);
void v_PidTask(void);
void LedTask(void);
uint16_t u16_Bat_ReadRaw(void);
uint16_t u16_Bat_GetVoltage(void);
uint8_t  uc_Bat_GetPercent(void);

#endif
