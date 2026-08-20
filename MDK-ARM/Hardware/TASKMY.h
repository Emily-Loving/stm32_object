#ifndef __TASKMY_H
#define __TASKMY_H

#include "MPU6050.h"
#include "bluetooth.h"
#include "motor.h"
#include "pid.h"
#include "freertos.h"
#include "gpio.h"
#include "cmsis_os2.h"
#include <string.h>
#include <stdio.h>

extern osMessageQueueId_t MPUQueue02Handle;
extern osMessageQueueId_t BlueQueue01Handle;

void vBlueTask(void);
void v_MPU6050Task(void);
void v_PidTask(void);
void LedTask(void);


#endif
