#pragma once

struct PIN
{
  PIN(GPIO_TypeDef *port, uint16_t pin) : port(port) , pin(pin), workingTimeMs(0), isWorking(false)
  {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
  }
  bool readState() {
    return HAL_GPIO_ReadPin(port, pin) == GPIO_PinState::GPIO_PIN_SET;
  }
  void setLow(){
     HAL_GPIO_WritePin(port, pin , GPIO_PIN_RESET);
    if (isWorking){
      isWorking = false;
      workingTimeMs += HAL_GetTick() - startTime;
    }
  }
  void setLowWithDelay(uint16_t min){
    HAL_GPIO_WritePin(port, pin , GPIO_PIN_RESET);
    if (!isWorking){
      isWorking = true;
      startTime = HAL_GetTick();
    }
  }

  void setHight(){
    HAL_GPIO_WritePin(port, pin , GPIO_PIN_SET);
    if (!isWorking){
      isWorking = true;
      startTime = HAL_GetTick();
    }
  }
  std::string getTimeString() const {
    std::string str;
    uint32_t sec = workingTimeMs / 1000;
    uint32_t hours = sec / 3600;
    uint32_t minutes = (sec % 3600) / 60;
    uint32_t seconds = sec % 60;
    char buf[16];
    snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu", 
             sec / 3600, (sec % 3600) / 60, sec % 60);
    return std::string(buf);
  }

  uint32_t workingTimeMs;
  uint32_t startTime;
  bool isWorking;
  GPIO_TypeDef *port;
  uint16_t pin;
};
