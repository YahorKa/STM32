#ifndef EEPROM_H
#define EEPROM_H

/*
 Эмуляция EEPROM во Flash:
- Используем страницу Flash
- Пишем редко (только важные данные)
- Пример: калибровки, счетчики, настройки

DEEPSEEK GENERATED CODE !!!
EEPROM EMULATOR used last flash page
*/
#include "stm32f1xx_hal_i2c.h"
#include <stdint.h>
#include "display_ssd1306.h" 

#define END_OF_FLASH 0x08010000
#define FLASH_ADDR   0x0800F800
#define PAGE_SIZE    0x400
#define DATA_SIZE    4 // 4 BYTES

// TO DO 
// struct EEPROM_DATA {...}

enum EEPROM_IDs : uint16_t {
    RESERV = 0,
    WATCHDOG_RESTART,    // uint32_t
    DH11_RESTART,        // uint32_t
    HEATER_TIME,         // uint32_t
    FAN_TIME,            // uint32_t
    HUMIDITY_THRESHOLD,  // float
    TEMPERATURE_THRESHOLD,  // float
    EEPROM_LAST
};

extern Display_SSD1306 display;
class EEPROM_Emulator {
private:
    uint32_t startAddr;
    uint32_t endAddr;
    HAL_StatusTypeDef _last_status;

    const char* HAL_StatusToString(HAL_StatusTypeDef status) {
        switch (status) {
            case HAL_OK:       return "OK";
            case HAL_ERROR:    return "ERROR";
            case HAL_BUSY:     return "BUSY";
            case HAL_TIMEOUT:  return "TIMEOUT";
            default:           return "UNKNOWN";
        }
    }   

    HAL_StatusTypeDef Erase_Memory(uint8_t pages = 1){
        uint32_t pageError = 0;
        FLASH_EraseInitTypeDef FlashErase; 
        FlashErase.TypeErase = FLASH_TYPEERASE_PAGES;
        FlashErase.PageAddress = startAddr; 
        FlashErase.NbPages = pages;
        _last_status = HAL_FLASHEx_Erase(&FlashErase, &pageError);
        if (_last_status != HAL_OK)
        {
            HAL_FLASH_Lock(); 
            __enable_irq();
            return _last_status;
        } else return HAL_OK;
    }
    
public:
    EEPROM_Emulator(uint32_t start, uint16_t pages) {
        startAddr = start;
        endAddr = start + pages * PAGE_SIZE;
        if (endAddr > END_OF_FLASH) {
            Error_Handler();
        }
    };
    template<typename T>
    HAL_StatusTypeDef WriteEEPROM(EEPROM_IDs id, T value) 
    {
        static_assert(sizeof(T) <= 4, "EEPROM supports max 4 bytes");
        uint32_t adr = startAddr + id * DATA_SIZE;
        HAL_StatusTypeDef res;
         //HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, adr, value);
         // We should erase the memory before write
        __disable_irq();
        _last_status = HAL_FLASH_Unlock();
        if (_last_status != HAL_OK)  {
            __enable_irq();
            return _last_status;
        }
        _last_status = Erase_Memory(1);
        if (_last_status)
        {
          display.setCursor(60, 40);
          char buf[64];
          snprintf(buf, sizeof(buf), "EEPROM: %s ", (HAL_StatusToString(_last_status)));
          display.drawText(buf);
          return _last_status;
        } else {
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, adr, value);
            // no error - ok
        }
        HAL_FLASH_Lock();
        __enable_irq();
        return  HAL_OK;
    }
    template<typename T>
    T ReadEEPROM(EEPROM_IDs id) 
    {
        uint32_t adr = startAddr + id * DATA_SIZE;
        return *(T*)adr;
    }
};

inline EEPROM_Emulator eeprom(FLASH_ADDR, 2);

inline void eeprom_write(EEPROM_IDs id, uint32_t value) {
    eeprom.WriteEEPROM(id, value);
}

template<typename T>
inline T eeprom_read(EEPROM_IDs id) {
    return eeprom.ReadEEPROM<T>(id);
}
#endif