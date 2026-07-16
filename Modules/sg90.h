#pragma once
#include "module.h"
#include "tim.h" 
class ServoSG90 : public Module
{
public:

    virtual void init() override;
    virtual void loop() override;
private:
 using Module::loop_ms;
};
