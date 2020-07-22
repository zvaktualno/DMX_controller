#include <asf.h>
#include <stdint.h>
#include "config.h"
#include "2004LCD.h"


void lcd_begin(void) {
    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

    if (_rows > 1) {
        _displayfunction |= LCD_2LINE;
    }

    // for some 1 line displays you can select a 10 pixel high font
    if ((_charsize != 0) && (_rows == 1)) {
        _displayfunction |= LCD_5x10DOTS;
    }

    // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
    // according to datasheet, we need at least 40ms after power rises above 2.7V
    // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
    delay_ms(50);

    // Now we pull both RS and R/W low to begin commands

    lcd_write(_backlightval);
    delay_ms(1000);

    // finally, set to 4-bit interface
    lcd_write8bits(0x02 << 4);

    // set # lines, font size, etc.
    lcd_command(LCD_FUNCTIONSET | _displayfunction);

    // turn the display on with no cursor or blinking default
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    //lcd_display();

    // clear it off
    lcd_clear();

    // Initialize to default text direction (for roman languages)
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

    // set the entry mode
    lcd_command(LCD_ENTRYMODESET | _displaymode);

    lcd_home();
}

/********** high level commands, for the user! */
void lcd_clear() {
    lcd_command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
    delay_us(2000);  // this command takes a long time!
}

void lcd_home() {
    lcd_command(LCD_RETURNHOME);  // set cursor position to zero
    delay_us(2000);  // this command takes a long time!
}

void lcd_setCursor(uint8_t col, uint8_t row) {
    int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if (row > _rows) {
        row = _rows - 1;  // we count rows starting w/0
    }
    lcd_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void lcd_noDisplay(void) {
    _displaycontrol &= ~LCD_DISPLAYON;
    lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void display(void) {
    _displaycontrol |= LCD_DISPLAYON;
    lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void lcd_noCursor(void) {
    _displaycontrol &= ~LCD_CURSORON;
    lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcd_cursor(void) {
    _displaycontrol |= LCD_CURSORON;
    lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void noBlink(void) {
    _displaycontrol &= ~LCD_BLINKON;
    lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void blink(void) {
    _displaycontrol |= LCD_BLINKON;
    lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void lcd_scrollDisplayLeft(void) {
    lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void lcd_scrollDisplayRight(void) {
    lcd_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcd_leftToRight(void) {
    _displaymode |= LCD_ENTRYLEFT;
    lcd_command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void lcd_rightToLeft(void) {
    _displaymode &= ~LCD_ENTRYLEFT;
    lcd_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void lcd_autoscroll(void) {
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    lcd_command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void lcd_noAutoscroll(void) {
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    lcd_command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcd_createChar(uint8_t location, uint8_t charmap[]) {
    location &= 0x7; // we only have 8 locations 0-7
    lcd_command(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++) {
        lcd_write(charmap[i]);
    }
}

uint8_t lcd_getBacklight(void) {
    return _backlightval == LCD_BACKLIGHT;
}

inline void lcd_command(uint8_t value) {
    lcd_send(value, 0);
}

inline size_t lcd_write(uint8_t value) {
    lcd_send(value, Rs);
    return 1;
}

void lcd_send(uint8_t value, uint8_t mode) {
    port_pin_set_output_level(PIN_LCD_RS, mode);
    lcd_write8bits(value);
}

void lcd_write8bits(uint8_t value) {
    port_pin_set_output_level(PIN_LCD_D0, value & (1 << 0));
    port_pin_set_output_level(PIN_LCD_D1, value & (1 << 1));
    port_pin_set_output_level(PIN_LCD_D2, value & (1 << 2));
    port_pin_set_output_level(PIN_LCD_D3, value & (1 << 3));
    port_pin_set_output_level(PIN_LCD_D4, value & (1 << 4));
    port_pin_set_output_level(PIN_LCD_D5, value & (1 << 5));
    port_pin_set_output_level(PIN_LCD_D6, value & (1 << 6));
    port_pin_set_output_level(PIN_LCD_D7, value & (1 << 7));
    lcd_pulseEnable(value);
}



void lcd_pulseEnable(uint8_t _data) {
    port_pin_set_output_level(PIN_LCD_EN, 1);
    delay_us(1);
    port_pin_set_output_level(PIN_LCD_EN, 0);
    delay_us(50);
}

void lcd_load_custom_character(uint8_t char_num, uint8_t *rows) {
    lcd_createChar(char_num, rows);
}

void lcd_setBacklight(uint8_t new_val) {
    /*manka*/
}

void lcd_printstr(char *str) {
    while(*str) {
        lcd_write(*str++);
    }
}