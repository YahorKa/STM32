#ifndef INTERNAL_SENSOR
#define INTERNAL_SENSOR

#include "module_adc.h"

class InternalSensor : public Module_ADC 
{
public:
    virtual void init() override;
    virtual void loop() override;
    virtual uint32_t frequency() const override {return 5;}

    virtual float readRaw();
    virtual float readPhysical();
private:

};

#endif //INTERNAL_SENSOR