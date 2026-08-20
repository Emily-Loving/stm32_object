#ifndef __pid_H
#define __pid_H

#include "MPU6050.h"


typedef struct 
{
    float   fRoll;    // Roll 角度 (度)
    float   fPitch;   // Pitch 角度 (度)
	uint8_t flag;
} Fly_ModeTDF;


void vMPU6050_UpdateAngles(MPU6050_DataTDF *Date, MPU6050_HandleDataTDF *Date2, float dt);


#endif
