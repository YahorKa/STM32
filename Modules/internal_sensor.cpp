
#include "internal_sensor.h"

void InternalSensor::init()
{
    ADC1_temp_sensor_Init();
     println("initialization of internal sensor");
}

void InternalSensor::loop()
{
    println("Core temp (%f)", readPhysical());
    println("loop ms intemp (%d)", HAL_GetTick() - _last_seen);
}

float InternalSensor::readRaw()
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    return static_cast<float>(adc_value);
}

float InternalSensor::readPhysical()
{
    float adc_value = readRaw();
    float v_sense = (adc_value / 4095.0f) * 3.3f;

    float temperature = ((1.43f - v_sense) / 0.0043f) + 25.0f;
    return temperature;
}
