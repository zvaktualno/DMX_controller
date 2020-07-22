
#ifndef _2004LCD_H_
#define _2004LCD_H_

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En (1<<2)
#define Rw (1<<1)
#define Rs (1<<0)

void lcd_begin(void);
void lcd_clear(void);
void lcd_home(void);
void lcd_setCursor(uint8_t col, uint8_t row);
void lcd_noDisplay(void);
void display(void);
void lcd_noCursor(void);
void lcd_cursor(void);
void noBlink(void);
void blink(void);
void lcd_scrollDisplayLeft(void);
void lcd_scrollDisplayRight(void);
void lcd_leftToRight(void);
void lcd_rightToLeft(void);
void lcd_autoscroll(void);
void lcd_noAutoscroll(void);
void lcd_createChar(uint8_t location, uint8_t charmap[]);
uint8_t lcd_getBacklight(void);
void lcd_command(uint8_t value);
size_t lcd_write(uint8_t value);
void lcd_send(uint8_t value, uint8_t mode);
void lcd_write8bits(uint8_t value);
void lcd_pulseEnable(uint8_t _data);
void lcd_load_custom_character(uint8_t char_num, uint8_t *rows);
void lcd_setBacklight(uint8_t new_val);
void lcd_printstr(char *str);
uint8_t _addr;
uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;
uint8_t _cols;
uint8_t _rows;
uint8_t _charsize;
uint8_t _backlightval;

#endif /* 2004LCD_H_ */