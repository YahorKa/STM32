#include "sys_manager.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

// SystemManager.cpp
void SystemManager::init()
{
    MX_GPIO_Init();
    MX_DMA_Init();
   
    MX_I2C1_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();
    MX_SPI1_Init();
    MX_USART2_UART_Init();
    // OLED_Init();
    // OLED_Clear();
    // printf("Hello from printf!\n");
    // OLED_ShowString(0, 0, "Hello from i2c!", 8);
    // HAL_UART_Transmit(&huart2, (uint8_t *)"Hello from UART!\n", 17, HAL_MAX_DELAY);
    for (auto *mod : activeModules)
        mod->init();
}

void SystemManager::loop()
{
    //HAL_UART_Transmit(&huart2, (uint8_t *)"Blink!\n", 17, HAL_MAX_DELAY);
    for (auto *mod : activeModules)
        mod->loop();
}
void SystemManager::add(Module *mod)
{
    activeModules.push_back(mod);
}
SystemManager::~SystemManager()
{
    for (auto *mod : activeModules) delete mod;
}