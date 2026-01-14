#include "MPU6050.h"
#include <array>

void MPU6050::init()
{
    MX_I2C1_Init();
    wake_up();
}

void MPU6050::loop()
{
    _last_seen = HAL_GetTick();
    count_loop ++;
    if (!ready)
    {
        if (HAL_I2C_IsDeviceReady(&hi2c1, MPU_ADDR, 3, 100) == HAL_OK)
        {
            printf("MPU6050 is ready!\r\n");
            ready = true;
        }
        else
        {
            printf("not connected MPU6050\r\n");
            ready = false;
            wake_up();
        }
    }
    else
    {
        
        const auto accel = getAccel();
        auto gyro = getGyro();
// temperature
        uint8_t buffer[2];
        HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
            &hi2c1,
            MPU_ADDR,
            0x41,  // TEMP_OUT_H
            I2C_MEMADD_SIZE_8BIT,
            buffer,
            2,
            200
        );
        int16_t raw_temp = (buffer[0] << 8) | buffer[1];
        _temperature = (raw_temp / 340.0f) + 36.53f;
        println("IMU temp (%f)", _temperature);
        // println("Accel (g): x=%.2f y=%.2f z=%.2f", accel.x, accel.y, accel.z);
        // println("Gyro (raw): x=%f y=%f z=%f", gyro.x, gyro.y, gyro.z);
        
    }
}

void MPU6050::wake_up()
{
    uint8_t data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU_ADDR, 0x6B, 1, &data, 1, 100);
}

float MPU6050::getTemperature()
{
   
    return _temperature;
}

MPU6050::AccelData MPU6050::getAccel()
{
    std::array<uint8_t, 6> raw{};
    HAL_I2C_Mem_Read(&hi2c1, MPU_ADDR, 0x3B, 1, raw.data(), raw.size(), 100);

    auto to_i16 = [](uint8_t hi, uint8_t lo)
    {
        return static_cast<int16_t>((hi << 8) | lo);
    };

    return {
        filterEMA(_accel.x, (to_i16(raw[0], raw[1]) / ACCEL_SENSITIVITY)),
        filterEMA(_accel.x, (to_i16(raw[2], raw[3]) / ACCEL_SENSITIVITY)),
        filterEMA(_accel.x, ( to_i16(raw[4], raw[5]) / ACCEL_SENSITIVITY))};
}

MPU6050::GyroData MPU6050::getGyro()
{
    std::array<uint8_t, 6> raw{};
    HAL_I2C_Mem_Read(&hi2c1, MPU_ADDR, 0x43, 1, raw.data(), raw.size(), 100);

    auto to_i16 = [](uint8_t hi, uint8_t lo)
    {
        return static_cast<int16_t>((hi << 8) | lo);
    };

    return {
        to_i16(raw[0], raw[1]) / GYRO_SENSITIVITY,
        to_i16(raw[2], raw[3]) / GYRO_SENSITIVITY,
        to_i16(raw[4], raw[5]) / GYRO_SENSITIVITY};
}

 uint32_t MPU6050::count_loop = 0;