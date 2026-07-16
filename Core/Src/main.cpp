
/*
 * Kochkin STM project
 */

#include "main.h"
#include <string>
#include <stdio.h>
#include <cmath> 
#include <memory>
extern "C" {
#include "screen.h"
}


#include "ws2812.h"
#include "imu.h"
#include "sys_manager.h"
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

#define isblink 1
#define DMAdemo 0
#define isDMA 0
#define isADC 0
#define isDMA_ADC 0
#define isPWM 0
#define isWS2812 0
#define isIMU 0
#define isUARTdemo 0

void frequency(int Hz)
{
  uint32_t period_ms = 1000U / Hz;
  HAL_Delay(period_ms);
}
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

void increment(uint8_t *a)
{
  for (int i = 0; a[i] != NULL; i++)
  {
    a[i]++;
  }
}

const uint8_t a = 0x4C; // ????
// Entry Point
int main(void)
{

  char buffer[20];
#if (isDMA)
  uint8_t DataA[] = {0x01, 0x02, 0x03, 0x04};
  uint8_t DataB[] = {0, 0, 0, 0};
#endif

#if (isADC || isDMA_ADC)
  uint32_t adcValue;
#endif
  /* MCU Configuration--------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
#if (isADC || isDMA_ADC)
  HAL_ADC_MspInit(&hadc1);
#endif
  /* USER CODE END SysInit */

    // Initializations
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();

  OLED_Init();
  OLED_Clear();

  //MPU6050 mpu;;
  //mpu.init();
  Display_SSD1306 display;
  display.init();
  //AL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  ServoSG90 servo;
  servo.init();
  DTH11 dth(GPIOB, GPIO_PIN_9);

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
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

    char buf [128];
    uint32_t now = HAL_GetTick();

    // MPU
    //mpu.loop_ms(50);
    // if (!mpu.ready) sprintf(buf, "initialization..");
    // else {
    //   sprintf(buf, "%.1f C ", mpu.getTemperature());
    // }

    display.loop_ms(1000);
    
    sprintf(buf,  "%lu:%lu:%lu", now / 1000 / 60 / 60, (now / 1000 / 60) % 60 ,(now / 1000) % 60);
    display.setCursor( 0, 0);
    display.drawText(buf);

    display.setCursor(0, 8);
    snprintf(buf, sizeof(buf), "SYSCLK: %lu MHz", HAL_RCC_GetSysClockFreq() / 1000000);
    display.drawText(buf);

    if (dth.read()) {
        // Только если успешно!
        display.setCursor(0, 16);
        snprintf(buf, sizeof(buf), "humidity: %.1f %%", dth.get_humidity());
        display.drawText(buf);
        
        display.setCursor(0, 25);
        snprintf(buf, sizeof(buf), "temp: %.1f C", dth.get_temperature());
        display.drawText(buf);
    } else {
        display.setCursor(0, 16);
        display.drawText("DHT11 ERROR!");
    }
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
    

#if (isDMA)
    OLED_ShowString(0, 0, "Data A", 8);
    sprintf(buffer, "ADDR: %02x", &DataA);
    OLED_ShowString(0, 1, buffer, 8);
    sprintf(buffer, "%02x %02x %02x %02x", DataA[0], DataA[1], DataA[2], DataA[3]);
    OLED_ShowString(0, 2, buffer, 8);
    // start DMA transfer
    HAL_DMA_Start(&hdma_memtomem_dma1_channel2, (uint32_t)DataA, (uint32_t)DataB, 4);
    // wait for DMA transfer complete
    while (HAL_DMA_PollForTransfer(&hdma_memtomem_dma1_channel2, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY) != HAL_OK)
      ;
    // after DMA transfer
    OLED_ShowString(0, 3, "Data B", 8);
    sprintf(buffer, "ADDR: %02x", &DataB);
    OLED_ShowString(0, 4, buffer, 8);
    sprintf(buffer, "%02x %02x %02x %02x", DataB[0], DataB[1], DataB[2], DataB[3]);
    OLED_ShowString(0, 5, buffer, 8);
    increment(DataA);
#endif

#if (isADC)
    HAL_ADC_Start(&hadc1);
    while (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY))
      ;
    adcValue = HAL_ADC_GetValue(&hadc1);
    OLED_ShowString(0, 0, "ADC Value", 8);
    sprintf(buffer, "%d", adcValue);
    OLED_ShowString(0, 1, buffer, 8);
#endif

/* DMA ADC begin */
#if (isDMA_ADC)
    HAL_ADC_Start_DMA(&hadc1, &adcValue, 1);
    OLED_ShowString(0, 0, "ADC Value", 8);
    adcValue = (adcValue & 0x0000FFFF);
    sprintf(buffer, "%04d", adcValue);
    OLED_ShowString(0, 1, buffer, 8);
#endif

#if (isPWM)
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&TIM1->CCR1, 1);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&TIM1->CCR2, 1);
#endif

#if (isWS2812)
    // ws2812_example();
    ws2812_test();
#endif

#if (isIMU)
    ADXL345_Test();
#endif

#if (isUARTdemo)
    if (HAL_UART_Receive(&huart2, (uint8_t *)buffer, 1, HAL_MAX_DELAY) == HAL_OK)
    {
      HAL_UART_Transmit(&huart2, (uint8_t *)buffer, 1, HAL_MAX_DELAY);
      HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n\r", 1, HAL_MAX_DELAY);
    }
#endif
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HAL_Delay(100);
  }
  /* USER CODE END 3 */
}
