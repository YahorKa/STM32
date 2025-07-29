// module_adc.h
#ifndef MODULE_ADC
#define MODULE_ADC
#include "module.h"
#include "adc.h"

class Module_ADC : public Module
{
public:
virtual ~Module_ADC() = default;
    virtual void init() override;
    virtual void loop() override;
    virtual uint32_t frequency() const override {return 50;}

    virtual float readRaw();
    virtual float readPhysical();
};

#endif // MODULE_ADC