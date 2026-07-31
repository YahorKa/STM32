#ifndef DISPLAY_SSD1306_H
#define DISPLAY_SSD1306_H
#include "i2c.h"
#include "module.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef uint8_t uint8;
void OLED_Init(void);
void OLED_Clear(void);
void OLED_SetPos(uint8 x, uint8 y);
void OLED_ShowChar(uint8 x, uint8 y, uint8 chr, uint8 Char_Size);
void SSD1306_WriteData(uint8_t data); 
extern const unsigned char F6x8[][6];
extern const unsigned char F8X16[];
#ifdef __cplusplus
}
#endif
struct FrameBuffer {
    static constexpr int PAGES = 8;
    static constexpr int HEIGHT = 64;
    static constexpr int WIDTH = 128;
    uint8_t data[PAGES][WIDTH]; 
    bool needUpdate[PAGES];
    FrameBuffer()
      {
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
        for (int i = 0; i < PAGES; ++i) {
          needUpdate[i] = true;
      }
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
  void drawIcon16(int x, int y, const unsigned char* icon);


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
