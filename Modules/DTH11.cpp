#include "DTH11.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include <cstdint>

// TO DO: MAKE THE PIN A SINGLE CLASS INSTEAD OF "GPIO_TypeDef* port, uint16_t data_pin"
DTH11::DTH11(GPIO_TypeDef* port, uint16_t data_pin, GPIO_TypeDef* vcc_port, uint16_t vcc_pin) :
     _data_port(port), _data_pin(data_pin), _vcc_port(vcc_port), _vcc_pin(vcc_pin)
{
    init();
}

const char* DTH11::getErrorString() const {
    switch (_lastError) {
        case DHT11_Error::OK:                   return "OK";
        case DHT11_Error::ERR_START_SIGNAL:     return "Start Signal Fail";
        case DHT11_Error::ERR_NO_RESPONSE_LOW:  return "No LOW Response";
        case DHT11_Error::ERR_NO_RESPONSE_HIGH: return "No HIGH Response";
        case DHT11_Error::ERR_BIT_TIMEOUT:      return "Bit Timeout";
        case DHT11_Error::ERR_CHECKSUM:         return "Checksum Error";
        case DHT11_Error::ERR_READ_BIT:         return "Read Bit Error";
        case DHT11_Error::ERR_UNKNOWN:          return "Unknown Error";
        case DHT11_Error::ERR_NOVCC:            return "No VCC";
        default:                                return "???";
    }
}

uint32_t DTH11::getMicros(){
    return TIM3->CNT; 
}
void DTH11::delayUs(uint32_t us)
{
   uint32_t start = TIM3->CNT;
   while ((TIM3->CNT - start) < us) {
        __NOP();
    }
}
void DTH11::init(){
    if (_vcc_port && _vcc_pin){
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = _vcc_pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(_vcc_port, &GPIO_InitStruct);
        HAL_GPIO_WritePin(_vcc_port, _vcc_pin, GPIO_PIN_SET);

        // wait on cold start for 1000
        HAL_Delay(1000); 
    } else (_lastError = DHT11_Error::ERR_NOVCC) ;
    setOutputMode();
    setHigh();
    _lastError = DHT11_Error::OK;
}

bool DTH11::read()
{
    _data.fill(0);
    _lastError = DHT11_Error::OK;
    setOutputMode();
    setLow();
    HAL_Delay(20);
    setHigh();
    delayUs(40);
    
    setInputMode();

    // waiting for response (LOW)
    // TODO Rewrite oll timeouts!
    uint16_t timeOut = 5000;
    while (!readPin()){
        delayUs(1);
        if(--timeOut == 0) {
            // doesnt have response -> quit
            _lastError = DHT11_Error::ERR_NO_RESPONSE_LOW;
            return false;
        }
    }
    timeOut = 5000;
    //waiting for hight
    while (readPin()){
        delayUs(1);
        if(--timeOut == 0) {
            // doesnt have response -> quit
            _lastError = DHT11_Error::ERR_NO_RESPONSE_HIGH;
            return false;
        }
    }
    //read 40 bites 
    for (int i = 0 ; i < _data.size(); i++){
        for (int j = 7; j >= 0; j--){
            int bit = readBit();
            if (bit < 0){
                if (_lastError == DHT11_Error::OK) {
                    _lastError = DHT11_Error::ERR_READ_BIT;
                }
                return false; 
            }
            _data[i] |= (bit << j);
        }
    }
    if (_data[0]+ _data[1] + _data[2] + _data[3]  != _data[4]){
        _lastError = DHT11_Error::ERR_CHECKSUM;
        return false;
    }
    return true;

}
int DTH11::readBit()
{
    uint16_t timeOut = 1000;
    while (!readPin()){
        delayUs(1);
        if(--timeOut == 0) {
            // doesnt have response -> quit
            _lastError = DHT11_Error::ERR_BIT_TIMEOUT;
            return -1;
        }
    }
    uint32_t start_us = getMicros();
    timeOut = 1000;
    while (readPin()){
        delayUs(1);
        if(--timeOut == 0) {
            // doesnt have response -> quit
            _lastError = DHT11_Error::ERR_BIT_TIMEOUT;
            return -1;
        }
    }
    uint32_t duration = getMicros() - start_us ;
    if (duration > 60) return 1;
    return 0;
}

void DTH11::setOutputMode() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = _data_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(_data_port, &GPIO_InitStruct);
}

void DTH11::setInputMode() {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = _data_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(_data_port, &GPIO_InitStruct);
}

void DTH11::loop(){
static uint8_t ErrorCount = 0;
if (_lastError == DHT11_Error::OK){
    ErrorCount = 0;
} else {
    if (ErrorCount > 5) {
        //Reload
        if (hardReset())  ErrorCount = 0;
    } else ErrorCount++;
}
_data.fill(0);
read();
}

bool DTH11::hardReset()
{
   // Power off and init
   if (_vcc_port && _vcc_pin){
        HAL_GPIO_WritePin(_vcc_port, _vcc_pin, GPIO_PIN_RESET);
        HAL_Delay(500);
        init();
        return true;
    }
    // TODO: How to reset if we have no power control
    return false;
}
