#include "MPU6050.h"


#define MPU6050_RAW_SIZE    14   /* ACCEL_XOUT_H 起连续 14 字节（含 2 字节温度） */
#define MPU6050_TIMEOUT   100   /* I2C 操作超时(ms)，防器件掉线卡死 */

static uint8_t MPURawBuffer[MPU6050_RAW_SIZE];   /* 原始数据接收缓冲 */

/// @brief  写 MPU6050 单字节寄存器
///
/// @param  ucReg  : 寄存器地址
/// @param  ucData : 要写入的数据
void vMPU6050_WriteReg(uint8_t ucReg, uint8_t ucData)
{
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ucReg, I2C_MEMADD_SIZE_8BIT, &ucData, 1, HAL_MAX_DELAY);
}

/// @brief  初始化 MPU6050：唤醒 + 设量程（最简开启方式）
void vMPU6050_Init(void)
{
    vMPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00);
    vMPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x07);
    vMPU6050_WriteReg(MPU6050_CONFIG, 0x00);
    vMPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
    vMPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x00);
}

/// @brief  读取 MPU6050 原始数据（阻塞方式，返回时数据已就绪）
/// @param  Data : 输出，解析后的原始数据（角度字段暂未填充）
/// @return 1=成功 0=失败
uint8_t vMPU6050_Read(MPU6050_DataTDF *Data)   //vMPU6050_Read(MPU6050_DataTDF);
{

	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, MPU6050_ACCEL_XOUT_H,
                         I2C_MEMADD_SIZE_8BIT, MPURawBuffer,
                         MPU6050_RAW_SIZE, MPU6050_TIMEOUT);
	

    /* 大端 -> 小端拼装，跳过温度字节 [6][7] */
    Data->sAccelX = (int16_t)((MPURawBuffer[0]  << 8) | MPURawBuffer[1]);
    Data->sAccelY = (int16_t)((MPURawBuffer[2]  << 8) | MPURawBuffer[3]);
    Data->sAccelZ = (int16_t)((MPURawBuffer[4]  << 8) | MPURawBuffer[5]);
    Data->sGyroX  = (int16_t)((MPURawBuffer[8]  << 8) | MPURawBuffer[9]);
    Data->sGyroY  = (int16_t)((MPURawBuffer[10] << 8) | MPURawBuffer[11]);
    Data->sGyroZ  = (int16_t)((MPURawBuffer[12] << 8) | MPURawBuffer[13]);

    volatile int16_t A,B,C;
    A = Data->sAccelX;
    B = Data->sAccelY;
    C = Data->sAccelZ;
    return 1;
}

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
//    fusedYaw    += gz * dt; // Yaw 只能靠陀螺仪积分，无法由加速度计校准，会漂移！

    // 互补系数 (0.98 给陀螺仪, 0.02 给加速度计，用于校准水平角度)
    float alpha = 0.98f;
    fusedRoll = alpha * fusedRoll + (1.0f - alpha) * accelRoll;
    fusedPitch = alpha * fusedPitch + (1.0f - alpha) * accelPitch;

    // 写入结构体
    Date2->fRoll = fusedRoll;
    Date2->fPitch = fusedPitch;
//    Date2->fYaw = fusedYaw; // 注意: Yaw 会随时间积累漂移
}
