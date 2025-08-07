/* MPU6050.h */
#ifndef MPU6050_H
#define MPU6050_H

#include "module.h"
#include "i2c.h"
#include "filter.h"

constexpr uint8_t MPU_ADDR = 0x68 << 1;

class MPU6050 : public Module // Module_I2C
{
    struct AccelData;
    struct GyroData;

public:
    virtual void init() override;
    virtual void loop() override;
    float getTemperature();
    AccelData getAccel();
    GyroData getGyro();
    virtual uint32_t frequency() const override { return 20; }

private:
    static constexpr float ACCEL_SENSITIVITY = 16384.0f; // ±2g
    static constexpr float GYRO_SENSITIVITY = 131.0f;    // Для ±250°/s

    void wake_up();
    bool ready = {false};
    uint16_t _temperature;
    struct AccelData
    {
        float x, y, z;
    } _accel;
    struct GyroData
    {
        float x, y, z;
    } _gyro;
    static uint32_t count_loop;
};

#endif //  MPU6050_H
