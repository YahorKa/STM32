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


class EEPROM_Emulator {
private:
    uint32_t startAddr;
    uint32_t endAddr;
    
public:
    EEPROM_Emulator(uint32_t start, uint32_t end) : 
        startAddr(start), endAddr(end) {}
    
    void write(uint32_t address, uint32_t data) {
        HAL_FLASH_Unlock();
        
        // Стираем страницу
        FLASH_EraseInitTypeDef erase = {0};
        erase.TypeErase = FLASH_TYPEERASE_PAGES;
        erase.PageAddress = startAddr;
        erase.NbPages = 1;
        uint32_t pageError;
        HAL_FLASHEx_Erase(&erase, &pageError);
        
        // Пишем данные
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, startAddr, data);
        
        HAL_FLASH_Lock();
    }
    
    uint32_t read(uint32_t address) {
        return *((uint32_t*)address);
    }
};

#endif