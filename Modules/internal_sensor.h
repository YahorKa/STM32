#ifndef INTERNAL_SENSOR
#define INTERNAL_SENSOR

#include "module_adc.h"

class InternalSensor : public Module_ADC 
{
public:
    virtual void init() override;
    virtual void loop() override;

    virtual float readRaw();
    virtual float readPhysical();
private:
    static uint32_t count_loop;
};

#endif //INTERNAL_SENSOR