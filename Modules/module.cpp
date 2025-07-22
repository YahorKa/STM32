#include "module.h"
#include "usart.h"
void Module::print_uart(std::string_view text)
{
    if (huart2.gState!= HAL_UART_STATE_READY) return;
    HAL_UART_Transmit(&huart2, reinterpret_cast<const uint8_t*>(text.data()), 17, HAL_MAX_DELAY);
}