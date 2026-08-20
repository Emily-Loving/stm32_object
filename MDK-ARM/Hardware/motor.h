#ifndef __motor_H
#define __motor_H

#include "tim.h"
#include "pid.h"

#define MOTOR_STOP     0.0
#define MOTOR_MIDDLE    0.5  //阈值即起飞的占空比
#define MOTOR_ALL_MIN      0.2  // 电机最小值
#define MOTOR_ALL_MAX      0.95    // 电机最大值 
#define MOTOR_ADD    0.2  //单次加速
//#define MOTOR_PLAY_MAX	   0.8 ///左转
//#define MOTOR_PLAY_MIN		0.3

#define FPITCH_ANGLE_MAX		60
#define FPITCH_ANGLE_MIN		-60



typedef enum
{
	Motor_1 = 1,
	Motor_2,
	Motor_3,
	Motor_4,
}Motor_Number_t;

//0 停止 1加速到阈值 2 加速 3减速 
typedef enum
{	
	Motor_State0 = 0,
	Motor_State1 = 1,
	Motor_State2 = 2,
	Motor_State3 = 3,
}Motor_State_t;

void Motor_Init(void);
void Hanlde_Motorapp(Motor_Number_t Num, Motor_State_t State);
uint32_t return_CCR(Motor_Number_t ch);
void Hanlde_Motor(Motor_Number_t Num, Motor_State_t State);
void Motor_stop(void);
void Motor_middle(void);
void Motor_left(void);
void Motor_right(void);
uint8_t Analyse_Date_pid(MPU6050_HandleDataTDF *Date);

#endif
