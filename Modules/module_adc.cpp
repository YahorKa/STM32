// module.adc.cpp
#include "module_adc.h"

void Module_ADC::init()
{
    MX_ADC1_Init();
}

void Module_ADC::loop()
{
    //
}

float Module_ADC::readRaw()
{
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
        return HAL_ADC_GetValue(&hadc1) * (3.3f / 4095.0f);
    }
    return 0.0f;
}