#include "pid.h"
//fRoll	[-180, 180] 度    左右歪头	加速度计 + 陀螺仪（互补滤波）	稳定，无漂移
//fPitch	[-90, 90] 度  点头抬头	加速度计 + 陀螺仪（互补滤波）	稳定，无漂移
//fYaw	无界（可正可负，随积分增大）	仅陀螺仪积分	严重漂移，随时间累积误差

//12
//43

/// @brief  处理原始数据
void vMPU6050_UpdateAngles(MPU6050_DataTDF *Date, MPU6050_HandleDataTDF *Date2, float dt)
{
    // 1. 转换为物理单位
    float ax = (float)Date->sAccelX / ACCEL_SENSITIVITY;
    float ay = (float)Date->sAccelY / ACCEL_SENSITIVITY;
    float az = (float)Date->sAccelZ / ACCEL_SENSITIVITY;

    float gx = (float)Date->sGyroX / GYRO_SENSITIVITY; // °/s
    float gy = (float)Date->sGyroY / GYRO_SENSITIVITY;
    float gz = (float)Date->sGyroZ / GYRO_SENSITIVITY;

    // 2. 从加速度计算角度 (静态倾斜)
    float accelRoll = atan2f(ay, az) * RAD_TO_DEG;
    float accelPitch = atan2f(-ax, sqrtf(ay*ay + az*az)) * RAD_TO_DEG;

    // 3. 互补滤波 (需要历史状态, 使用静态变量模拟)
    static float fusedRoll = 0.0f, fusedPitch = 0.0f;// fusedYaw = 0.0f;
    static uint8_t isFirstRun = 1;

    if (isFirstRun)
    {
        fusedRoll = accelRoll;
        fusedPitch = accelPitch;
//      fusedYaw = 0.0f; // 初始为0
        isFirstRun = 0;
    }

    // 陀螺仪积分
    fusedRoll   += gx * dt;
    fusedPitch  += gy * dt;
//  fusedYaw    += gz * dt; // Yaw 只能靠陀螺仪积分，无法由加速度计校准，会漂移！

    // 互补系数 (0.98 给陀螺仪, 0.02 给加速度计，用于校准水平角度)
    float alpha = 0.98f;
    fusedRoll = alpha * fusedRoll + (1.0f - alpha) * accelRoll;
    fusedPitch = alpha * fusedPitch + (1.0f - alpha) * accelPitch;

    // 写入结构体
    Date2->fRoll = fusedRoll;
    Date2->fPitch = fusedPitch;
//    Date2->fYaw = fusedYaw; // 注意: Yaw 会随时间积累漂移
}


