#ifndef SENSOR_TEMPERATURE_KTY81
#define SENSOR_TEMPERATURE_KTY81
#include "module_adc.h"
class Sensor_temperature_KTY81 : public Module_ADC
{
public:
    virtual float readRaw() override;
    virtual float readPhysical() override;
    virtual void loop() override;
};

#endif // SENSOR_TEMPERATURE_KTY81