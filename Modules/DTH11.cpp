#include "DTH11.h"
#include "stm32f1xx_hal.h"
#include <cstdint>

DTH11::DTH11(GPIO_TypeDef* port, uint16_t pin) :
     _port(port), _pin(pin)
{
    init();
}

/*
uint32_t DTH11::getMicros()
TO DO: HANDLE SysTick->VAL RELOAD CASE
Mayby use TIM?
*/
uint32_t DTH11::getMicros(){
    // SysTick считает такты
    // 1 такт = 1/72 МГц ≈ 13.9 нс
    return (SysTick->LOAD - SysTick->VAL) * 1000000 / SystemCoreClock;
    
}
void DTH11::delayUs(uint32_t us)
{
    // SystemCoreClock = 72 000 000 (72 МГц)
    // 1 секунда = 72 000 000 тактов
    // 1 микросекунда = 72 такта (при 72 МГц)
    uint32_t ticks = us * (SystemCoreClock / 1000000) / 4; //  18us

    while (ticks--) {
        __NOP();  // ← Один такт процессора но 4 такта вместе с  while (ticks--)
    }
}
void DTH11::init(){

    setOutputMode() ;
    setHigh();
}

bool DTH11::read()
{
    startSignal();
    setInputMode();
    // waiting for response (LOW)
    uint16_t timeOut = 1000;
    while (!readPin()){
        delayUs(1);
        if(--timeOut == 0) {
            // doesnt have response -> quit
            return false;
        }
    }
    timeOut = 1000;
    //waiting for hight
    while (readPin()){
        delayUs(1);
        if(--timeOut == 0) {
            // doesnt have response -> quit
            return false;
        }
    }
    //read 40 bites 
    for (int i = 0 ; i < _data.size(); i++){
        for (int j = 7; j >= 0; j--){
            int bit = readBit();
            if (bit < 0) return false; 
            _data[i] |= (bit << j);
        }
    }
    if (_data[0]+ _data[1] + _data[2] + _data[3]  != _data[4]) return false;
    return true;

}
int DTH11::readBit()
{
    uint16_t timeOut = 1000;
    while (!readPin()){
        delayUs(1);
        if(--timeOut == 0) {
            // doesnt have response -> quit
            return -1;
        }
    }
    uint32_t start_us = getMicros();
    timeOut = 1000;
    while (readPin()){
        delayUs(1);
        if(--timeOut == 0) {
            // doesnt have response -> quit
            return -1;
        }
    }
    uint32_t duration = getMicros() - start_us ;
    if (duration > 60) return 1;
    return 0;
}
void DTH11::startSignal()
{
    setLow();
    HAL_Delay(18);
    setHigh();
}

void DTH11::setOutputMode() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = _pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(_port, &GPIO_InitStruct);
}

void DTH11::setInputMode() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = _pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(_port, &GPIO_InitStruct);
}

 void DTH11::loop(){
    _data.fill(0);
    read();
 }