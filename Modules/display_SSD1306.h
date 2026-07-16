#ifndef DISPLAY_SSD1306_H
#define DISPLAY_SSD1306_H
#include "i2c.h"
#include "module.h"
#include <stdint.h>

extern "C" {
typedef uint8_t uint8;
void OLED_Init(void);
void OLED_Clear(void);
void OLED_SetPos(uint8 x, uint8 y);
void OLED_ShowChar(uint8 x, uint8 y, uint8 chr, uint8 Char_Size);
void SSD1306_WriteData(uint8_t data); 
extern const unsigned char F6x8[][6];
extern const unsigned char F8X16[];
}
struct FrameBuffer {
    static constexpr int PAGES = 8;
    static constexpr int HEIGHT = 64;
    static constexpr int WIDTH = 128;
    uint8_t data[PAGES][WIDTH]; 
    bool needUpdate[PAGES];
    FrameBuffer(){
      clear();
    }

    inline uint8_t& at(int page, int col) {
      return data[page][col];
    }
    void clear(){
        memset(data, 0, sizeof(data));
        setNeedUpdate();
    }
    inline void setNeedUpdate() {
      std::fill(std::begin(needUpdate), std::end(needUpdate), true);
    }
  };
class Display_SSD1306 : public Module {
public:
  struct Position {
        int x = 0;
        int y = 0;
        int page = 0;
    };

  virtual void init() override;
  virtual void loop() override;
  void clear();
  void setCursor(int x, int y);
  void drawChar(char , uint8 = 8);
  void drawText(const char *s,uint8 = 8);
  void drawTextCenter(const char* s, uint8_t size = 8);


  void update();
  // Рисование примитивов
    void drawPixel(int x, int y, bool on = true);
    void drawLine(int x1, int y1, int x2, int y2, bool on = true);
    void drawRect(int x, int y, int w, int h, bool fill = false);

    FrameBuffer buffer;
private:
    
  void SendCommand(uint8_t cmd);
  void SendData(const uint8_t* data, size_t len);
  Position _position;
  
};

#endif

/*
  SendCommand(0xAE);  // Display OFF
    SendCommand(0xD5);  // Set oscillator frequency
    SendCommand(0x80);
    SendCommand(0xA8);  // Set multiplex ratio
    SendCommand(0x3F);  // 64 lines
    SendCommand(0xD3);  // Set display offset
    SendCommand(0x00);
    SendCommand(0x40);  // Set start line
    SendCommand(0x8D);  // Charge pump
    SendCommand(0x14);  // Enable
    SendCommand(0x20);  // Memory mode
    SendCommand(0x00);  // Horizontal
    SendCommand(0xA1);  // Segment remap
    SendCommand(0xC8);  // COM scan direction
    SendCommand(0xDA);  // COM pins hardware config
    SendCommand(0x12);
    SendCommand(0x81);  // Set contrast
    SendCommand(0xCF);  // Max contrast
    SendCommand(0xD9);  // Pre-charge period
    SendCommand(0xF1);
    SendCommand(0xDB);  // VCOM detect
    SendCommand(0x40);
    SendCommand(0xA4);  // Display all on resume
    SendCommand(0xA6);  // Normal display (не инвертированный)
    SendCommand(0x2E);  // Deactivate scroll
    SendCommand(0xAF);  // Display ON*/