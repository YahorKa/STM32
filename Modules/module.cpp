// module.cpp
#include "module.h"
#include "usart.h"
#include "stdio.h"
#include <cstring>
#include <string>


extern "C" int __io_putchar(int ch) {
    if (huart2.gState!= HAL_UART_STATE_READY) return 0;
    HAL_UART_Transmit(&huart2, reinterpret_cast<uint8_t*>(&ch), 1, HAL_MAX_DELAY);
    return ch;
}


void Module::loop(const uint32_t& freq)
{
    if (freq == 0) return;
    uint32_t now = HAL_GetTick();
    if (now > (_last_seen + 1000/freq))
    {
        loop();
        _last_seen = now;
    }
    
}
