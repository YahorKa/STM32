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

