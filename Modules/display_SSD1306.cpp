#include "display_ssd1306.h"

void Display_SSD1306::init()
{
    MX_I2C2_Init();
    OLED_Init();
    OLED_Clear();
}
void Display_SSD1306::loop()
{
    //
}

void Display_SSD1306::setCursor(int x, int y) // page 1-8
{
    _position.x = x;
    _position.y = y;
}
//char_Size 8 - 16 bit
void Display_SSD1306::drawChar(char c ,uint8 char_Size)
{
    OLED_ShowChar(_position.x, _position.y, c, char_Size);
    
    setCursor(_position.x+char_Size, _position.y);
}
void Display_SSD1306::drawText(const char *s) 
{
    unsigned char j = 0;

    while (s[j] != '\0')
    {
        OLED_ShowChar(_position.x, _position.y, s[j], 8);
        _position.x += 8;
        if (_position.x > 120)
        {
            _position.x = 0;
            _position.y += 1;
        }
        _position.page++; // next page
    }

}

void Display_SSD1306::clear()
{
    setCursor(0, 0);
}
