#include "display_ssd1306.h"

void Display_SSD1306::init()
{
    MX_I2C2_Init();
    OLED_Init();
    OLED_Clear();
}

void Display_SSD1306::setCursor(int x, int y) // page 1-8
{
    if (x >= 0 && x < FrameBuffer::WIDTH) _position.x = x;
    if (y >= 0 && y < FrameBuffer::HEIGHT) {_position.y = y; _position.page = y / 8;}
}

void Display_SSD1306::drawPixel(int x, int y, bool on) {

    if (x < 0 || x >= FrameBuffer::WIDTH) return;
    if (y < 0 || y >= FrameBuffer::HEIGHT) return;
    
    int page = y / 8;      // 0..7
    
    int bit = y % 8;       // 0..7
    
    if (on) {
        // Включаем: OR с единицей
        buffer.data[page][x] |= (1 << bit);
    } else {
        // Выключаем: AND с нулем
        buffer.data[page][x] &= ~(1 << bit);
    }

    buffer.needUpdate[page] = true;
}
//char_Size 8 - 16 bit
void Display_SSD1306::drawChar(char chr ,uint8 char_Size)
{
    uint8 c = chr - ' ';
    switch (char_Size){
    case 8: {
        if (c > 92) return;
         if (_position.x > (FrameBuffer::WIDTH - 6)) {
                _position.x = 0;
                _position.y += 8;
                _position.page = _position.y / 8;
            }
            if (_position.page >= FrameBuffer::PAGES) {
                _position.page = 0;
                _position.y = 0;
            }
        for (int i = 0; i < 6; i++)
        {
            buffer.data[_position.page][_position.x + i] = F6x8[c][i];
        }
        _position.x = _position.x + 6;
        buffer.needUpdate[_position.page] = true;
        break;
    }
    case 16: {
        if (_position.x > (FrameBuffer::WIDTH - 8)) {
            _position.x = 0;
            _position.y += 16;
            _position.page = _position.y / 8;
        }
        
        if (_position.page >= FrameBuffer::PAGES - 1) {
            _position.page = 0;
            _position.y = 0;
        }
        
        // Верхняя половина (8 байт)
        for (int i = 0; i < 8; i++) {
            if (_position.x + i < FrameBuffer::WIDTH) {
                buffer.data[_position.page][_position.x + i] = F8X16[c * 16 + i];
            }
        }
        
        // Нижняя половина (следующие 8 байт)
        for (int i = 0; i < 8; i++) {
            if (_position.x + i < FrameBuffer::WIDTH) {
                buffer.data[_position.page + 1][_position.x + i] = F8X16[c * 16 + i + 8];
            }
        }
        buffer.needUpdate[_position.page] = true;
        buffer.needUpdate[_position.page + 1] = true;
        _position.x += 8;
        break;
    }
    default:
    // somethind wrong font
    break;
    }
}

void Display_SSD1306::drawText(const char *s, uint8 size ) 
{   
    //if (!s || *s == '\0') return;
    while (*s)
    {
        drawChar( *s, size);
        s++;
    }
}
void Display_SSD1306::drawTextCenter(const char* s, uint8_t size) {
    if (!s || *s == '\0') return;  // Пустая строка
    
    int len = strlen(s);
    int char_width = (size == 16) ? 8 : 6;
    int max_chars_per_line = FrameBuffer::WIDTH / char_width;
    int total_width = len * char_width;
    
    // ====== 1. Текст помещается в одну строку ======
    if (total_width <= FrameBuffer::WIDTH) {
        int start_x = (FrameBuffer::WIDTH - total_width) / 2;
        if (start_x < 0) start_x = 0;
        setCursor(start_x, (FrameBuffer::HEIGHT - size) / 2);
        drawText(s);
        return;
    }
    
    // ====== 2. Текст не помещается - перенос ======
    int total_lines = (len + max_chars_per_line - 1) / max_chars_per_line;
    int total_height = total_lines * size;
    
    // Если не влезает по высоте - уменьшаем количество строк
    if (total_height > FrameBuffer::HEIGHT) {
        total_lines = FrameBuffer::HEIGHT / size;
        if (total_lines == 0) total_lines = 1;
    }
    
    // Рисуем по строкам
    int pos = 0;
    char line[64];
    
    for (int line_num = 0; line_num < total_lines && pos < len; line_num++) {
        int chars_in_line = 0;
        while (pos < len && chars_in_line < max_chars_per_line) {
            line[chars_in_line++] = s[pos++];
        }
        line[chars_in_line] = '\0';
        
        // Центрируем строку
        int line_width = chars_in_line * char_width;
        int start_x = (FrameBuffer::WIDTH - line_width) / 2;
        if (start_x < 0) start_x = 0;
        
        int start_y = (FrameBuffer::HEIGHT - total_lines * size) / 2 + line_num * size;
        if (start_y < 0) start_y = 0;
        
        setCursor(start_x, start_y);
        drawText(line);
    }
}
// Рисуем иконку размером 16x16 по координатам x, y
void Display_SSD1306::drawIcon16(int x, int y, const unsigned char* icon) {
    // Проверка выхода за границы
    if (x < 0 || x > FrameBuffer::WIDTH - 16) return;
    if (y < 0 || y > FrameBuffer::HEIGHT - 16) return;
    
    int page = y / 8;      // Страница для верхней половины
    int page2 = page + 1;  // Страница для нижней половины (y+8)
    
    for (int col = 0; col < 16; col++) {
        // Верхняя половина (строки 0-7)
        buffer.data[page][x + col] = icon[col];
        // Нижняя половина (строки 8-15)
        buffer.data[page2][x + col] = icon[col + 16];
    }
    
    // Помечаем страницы для обновления
    buffer.needUpdate[page] = true;
    buffer.needUpdate[page2] = true;
}

void Display_SSD1306::clear()
{
    buffer.clear();
    _position = {};

}
void Display_SSD1306::update(){

    for (int page = 0; page < FrameBuffer::PAGES; page++) {
        if (!buffer.needUpdate[page]) continue;
        OLED_SetPos(0, page);
        for (int col = 0; col < FrameBuffer::WIDTH; col++) {
            SSD1306_WriteData(buffer.data[page][col]);
        }
    }
}

void Display_SSD1306::loop() {
    update();
}

