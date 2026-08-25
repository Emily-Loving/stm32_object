#include "pid.h"

/// @NOTE 处理原始数据：物理单位换算 + 加速度求角 + 互补滤波，输出 Roll/Pitch
/// @param p_Pid_Raw 输入，MPU6050 原始数据
/// @param p_Pid_Angle 输出，滤波后的角度
/// @param f_Pid_Dt 采样周期(s)
/// @return
void vMPU6050_UpdateAngles(MPU6050_DataTDF *p_Pid_Raw, MPU6050_HandleDataTDF *p_Pid_Angle, float f_Pid_Dt)
{
    // 转换为物理单位
    float f_Pid_Ax = (float)p_Pid_Raw->s_Mpu_AccelX / ACCEL_SENSITIVITY;
    float f_Pid_Ay = (float)p_Pid_Raw->s_Mpu_AccelY / ACCEL_SENSITIVITY;
    float f_Pid_Az = (float)p_Pid_Raw->s_Mpu_AccelZ / ACCEL_SENSITIVITY;

    float f_Pid_Gx = (float)p_Pid_Raw->s_Mpu_GyroX / GYRO_SENSITIVITY; // °/s
    float f_Pid_Gy = (float)p_Pid_Raw->s_Mpu_GyroY / GYRO_SENSITIVITY;
    float f_Pid_Gz = (float)p_Pid_Raw->s_Mpu_GyroZ / GYRO_SENSITIVITY;

    // 从加速度计算角度 (静态倾斜)
    float f_Pid_AccelRoll = atan2f(f_Pid_Ay, f_Pid_Az) * RAD_TO_DEG;
    float f_Pid_AccelPitch = atan2f(-f_Pid_Ax, sqrtf(f_Pid_Ay*f_Pid_Ay + f_Pid_Az*f_Pid_Az)) * RAD_TO_DEG;

    // 互补滤波 (需要历史状态, 使用静态变量模拟)
    static float f_Pid_FusedRoll = 0.0f, f_Pid_FusedPitch = 0.0f;// fusedYaw = 0.0f;
    static uint8_t uc_Pid_IsFirstRun = 1;

    if (uc_Pid_IsFirstRun)
    {
        f_Pid_FusedRoll = f_Pid_AccelRoll;
        f_Pid_FusedPitch = f_Pid_AccelPitch;
//      fusedYaw = 0.0f; // 初始为0
        uc_Pid_IsFirstRun = 0;
    }

    // 陀螺仪积分
    f_Pid_FusedRoll   += f_Pid_Gx * f_Pid_Dt;
    f_Pid_FusedPitch  += f_Pid_Gy * f_Pid_Dt;
//  fusedYaw    += gz * dt; // Yaw 只能靠陀螺仪积分，无法由加速度计校准，会漂移！

    // 互补系数 (0.98 给陀螺仪, 0.02 给加速度计，用于校准水平角度)
    float f_Pid_Alpha = 0.98f;
    f_Pid_FusedRoll = f_Pid_Alpha * f_Pid_FusedRoll + (1.0f - f_Pid_Alpha) * f_Pid_AccelRoll;
    f_Pid_FusedPitch = f_Pid_Alpha * f_Pid_FusedPitch + (1.0f - f_Pid_Alpha) * f_Pid_AccelPitch;

    // 写入结构体
    p_Pid_Angle->f_Mpu_Roll = f_Pid_FusedRoll;
    p_Pid_Angle->f_Mpu_Pitch = f_Pid_FusedPitch;
//    Date2->fYaw = fusedYaw; // 注意: Yaw 会随时间积累漂移
}
