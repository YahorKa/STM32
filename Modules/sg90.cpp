#include "sg90.h"

void ServoSG90::init()
{
      HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

void ServoSG90::loop()
{
    printf("servo loop");
}