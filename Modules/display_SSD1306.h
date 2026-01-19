#ifndef DISPLAY_SSD1306_H
#define DISPLAY_SSD1306_H
#include "module.h"
#include "i2c.h"

extern "C" {
    #include "screen.h"
}
//using Position = std::array<uint8_t, 2>; // [x, y]

class Display_SSD1306 : public Module
{
public:
    virtual void init() override;
    virtual void loop() override;
    void clear();
    void setCursor(int x, int y);
    void drawChar(char c, uint8);
    void drawText(const char* s);
    struct Position
    {
       int x;
       int y;
       int page : 4;
    };
    
private:
uint8_t buffer[128][8]; // 128 pixels and 8 pages (1 page - 8 bit)
Position _position;

};

#endif