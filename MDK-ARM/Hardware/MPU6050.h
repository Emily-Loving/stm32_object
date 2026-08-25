#ifndef __MPU6050_H
#define __MPU6050_H

#include "cmsis_os2.h"
#include "i2c.h"
#include <math.h>

// MPU6050 传感器的寄存器地址
#define MPU6050_ADDR     0xD0

#define MPU6050_SMPLRT_DIV   0x19
#define MPU6050_CONFIG       0x1A
#define MPU6050_GYRO_CONFIG  0x1B
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_WHO_AM_I     0x75
#define MPU6050_PWR_MGMT_1   0x6B

#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_ACCEL_XOUT_L 0x3C
#define MPU6050_ACCEL_YOUT_H 0x3D
#define MPU6050_ACCEL_YOUT_L 0x3E
#define MPU6050_ACCEL_ZOUT_H 0x3F
#define MPU6050_ACCEL_ZOUT_L 0x40
#define MPU6050_TEMP_OUT_H   0x41
#define MPU6050_TEMP_OUT_L   0x42
#define MPU6050_GYRO_XOUT_H  0x43
#define MPU6050_GYRO_XOUT_L  0x44
#define MPU6050_GYRO_YOUT_H  0x45
#define MPU6050_GYRO_YOUT_L  0x46
#define MPU6050_GYRO_ZOUT_H  0x47
#define MPU6050_GYRO_ZOUT_L  0x48

#define ACCEL_SENSITIVITY 16384.0f  // ±2g
#define GYRO_SENSITIVITY 16.4f      // ±2000°/s
#define RAD_TO_DEG 57.3f

typedef struct {
    int16_t s_Mpu_AccelX;  // 加速度 X 轴原始值 
    int16_t s_Mpu_AccelY;  // 加速度 Y 轴原始值 
    int16_t s_Mpu_AccelZ;  // 加速度 Z 轴原始值 
    int16_t s_Mpu_GyroX;   // 陀螺仪 X 轴原始值 
    int16_t s_Mpu_GyroY;   // 陀螺仪 Y 轴原始值 
    int16_t s_Mpu_GyroZ;   // 陀螺仪 Z 轴原始值 
//    float   fYaw;     // Yaw 角度 (度)  此项目用不到
} 
MPU6050_DataTDF;

typedef struct {
    float   f_Mpu_Roll;    // Roll 角度 (度)
    float   f_Mpu_Pitch;   // Pitch 角度 (度)
//    float   fYaw;     // Yaw 角度 (度)  此项目用不到
} 
MPU6050_HandleDataTDF;

extern I2C_HandleTypeDef hi2c1;

void vMPU6050_WriteReg(uint8_t ucReg, uint8_t ucData);
uint8_t ucMPU6050_Probe(void);  
uint8_t ucI2C_Scan(void);     
void vMPU6050_Init(void);
uint8_t vMPU6050_Read(MPU6050_DataTDF *Data);

#endif
