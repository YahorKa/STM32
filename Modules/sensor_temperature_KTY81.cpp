#include "sensor_temperature_KTY81.h"

float Sensor_temperature_KTY81::readRaw()
{
     HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK)
    {
        return HAL_ADC_GetValue(&hadc1);
    }
    return 0.0f;
}

float Sensor_temperature_KTY81::readPhysical()
{
    float voltage = (readRaw() / 4095.0f) * 3.3f;
    float r_fixed = 1000.0f;
    float r_kty = (voltage * r_fixed) / (3.3f - voltage);
    float temperature = (r_kty - 1000.0f) / 2.0f + 25.0f;

    return temperature;
}
void Sensor_temperature_KTY81::loop()
{
    println("temp = %f",readPhysical());
}