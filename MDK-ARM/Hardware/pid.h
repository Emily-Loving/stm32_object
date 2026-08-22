#ifndef __pid_H
#define __pid_H

#include "MPU6050.h"

typedef struct 
{
    float   f_Fly_Roll;    // Roll 角度 (度)
    float   f_Fly_Pitch;   // Pitch 角度 (度)
	uint8_t uc_Fly_Flag;
} Fly_ModeTDF;

void vMPU6050_UpdateAngles(MPU6050_DataTDF *Date, MPU6050_HandleDataTDF *Date2, float dt);

#endif
