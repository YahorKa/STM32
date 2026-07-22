
/*
 * Kochkin STM project
 */

#include "main.h"
#include "stm32f1xx_hal.h"
#include <cstdint>
#include <string>
#include <stdio.h>
#include <cmath> 
#include <memory>

extern "C" {
#include "screen.h"
}

#include "ws2812.h"
#include "imu.h"
#include "module_adc.h"
#include "internal_sensor.h"
#include "MPU6050.h"
#include "sg90.h"
#include "DTH11.h"
#include "display_ssd1306.h"

#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include <chrono>



// WatchDog
// TODO move to WDG.h *.c
struct Watchdog {
    IWDG_HandleTypeDef hiwdg;
    
    Watchdog() {
        hiwdg.Instance = IWDG;
        hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
        hiwdg.Init.Reload = 0xFFF;

    }
    void init() {HAL_IWDG_Init(&hiwdg);}
    void feed() {
        HAL_IWDG_Refresh(&hiwdg);
    }
};
struct Relay
{
  Relay(GPIO_TypeDef *port, uint16_t pin) : port(port) , pin(pin), workingTimeMs(0), isWorking(0)
  {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
  }
  void setLow(){
    HAL_GPIO_WritePin(port, pin , GPIO_PIN_RESET);
    if (!isWorking){
      isWorking = true;
      startTime = HAL_GetTick();
    }
  }

  void setHight(){
    HAL_GPIO_WritePin(port, pin , GPIO_PIN_SET);
    if (isWorking){
      isWorking = false;
      workingTimeMs += HAL_GetTick() - startTime;
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


// Clock
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System
   */
  HAL_RCC_EnableCSS();
}

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
    printf("error handler");
    break;
  }
}


// Entry Point
int main(void)
{

  /* MCU Configuration--------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
#if (isADC || isDMA_ADC)
  HAL_ADC_MspInit(&hadc1);
#endif

  //WatchDog

  //IWDG_HandleTypeDef watchDogHandler;
    // Initializations
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  //MX_I2C2_Init(); // display
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init(); // Counter 1 us
  MX_SPI1_Init();
  MX_USART2_UART_Init();

  //MPU6050 mpu;;
  //mpu.init();
  Display_SSD1306 display;
  display.init();
  //ServoSG90 servo;
  //servo.init();
  DTH11 dth(GPIOB, GPIO_PIN_1);
  Watchdog iwdg;
  iwdg.init();
  Relay heater(GPIOA, GPIO_PIN_1);
  heater.setHight();
  Relay fan(GPIOA, GPIO_PIN_4);
  fan.setHight();



#if (isPWM)
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
#endif
#if (isIMU)
  while (ADXL345_Init() != HAL_OK)
    ;
#endif
  // turn off led
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
  while (1)
  {
    //HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

    char buf [128];
    uint32_t now = HAL_GetTick();
    display.clear();

    sprintf(buf, "%02lu:%02lu:%02lu", (now/1000/3600)%24, (now/1000/60)%60, (now/1000)%60);
    display.setCursor( 0, 0);
    display.drawText(buf);
    display.setCursor(0, 8);
    
    snprintf(buf, sizeof(buf), "Heat. worked %s",heater.getTimeString().c_str());
    display.drawText(buf);
    if (dth.getLastError() == DHT11_Error::OK) {
        display.setCursor(0, 24);
        snprintf(buf, sizeof(buf), "Temp: %.1f C", dth.get_temperature());
        display.drawText(buf);
      
        display.setCursor(0, 16);
        snprintf(buf, sizeof(buf), "Hum:  %.1f %%", dth.get_humidity());
        display.drawText(buf);
        if (dth.get_humidity() > 85){
          display.drawIcon16(25, 48, ICON_HEATER);
          display.drawIcon16(88, 48, ICON_FAN);
          heater.setLow();
          fan.setLow();
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        } else if(dth.get_humidity() < 80){
          heater.setHight();
          fan.setHight();
          HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        }
    } else {
        // 
        display.setCursor(0, 16);
        snprintf(buf, sizeof(buf), "ERR: %s", dth.getErrorString());
        display.drawText(buf);
    }
    // CHECK WATCHDOG
    // if (now > 30000 ){
    //   while(1){

    //   };
    // }
    display.loop_ms(1000);
    dth.loop_ms(2000);
    iwdg.feed();
    /* Servo TEXT if anable
    // Перемещаем серву в 0°
      htim2.Instance->CCR1 = 500;   // 0.5ms = 0°
      HAL_Delay(1000);
      
      // В 90°
      htim2.Instance->CCR1 = 1500;  // 1.5ms = 90°
      HAL_Delay(1000);
      
      // В 180°
      htim2.Instance->CCR1 = 2500;  // 2.5ms = 180°
      HAL_Delay(1000);
      
      // Плавное движение от 0 до 180:
      for(int angle = 0; angle <= 180; angle += 10) {
          uint16_t pulse = 500 + (angle * 2000 / 180);
          htim2.Instance->CCR1 = pulse;
          HAL_Delay(100);
      }
  */
    
    // MPU
    //mpu.loop_ms(50);
    // if (!mpu.ready) sprintf(buf, "initialization..");
    // else {
    //   sprintf(buf, "%.1f C ", mpu.getTemperature());
    // }

    /* USER CODE BEGIN 3 */
    HAL_Delay(10);
  }
  /* USER CODE END 3 */
}
