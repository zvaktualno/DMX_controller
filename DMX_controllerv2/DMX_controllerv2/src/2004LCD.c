#include <asf.h>
#include <stdint.h>
#include "config.h"
#include "2004LCD.h"

uint8_t _displayfunction = 0;
uint8_t _displaycontrol = 0;
uint8_t _displaymode = 0;

void lcd_begin(void) {

    _displayfunction = LCD_2LINE | LCD_8BITMODE | LCD_5x8DOTS;
    delay_ms(50);
    port_pin_set_output_level(PIN_LCD_RS, 0);
    port_pin_set_output_level(PIN_LCD_RW, 0);
    port_pin_set_output_level(PIN_LCD_EN, 0);

    delay_ms(50);

    lcd_command(LCD_FUNCTIONSET | _displayfunction);
    delay_us(4500);

    lcd_command(LCD_FUNCTIONSET | _displayfunction);
    delay_us(150);

    // third go
    lcd_command(LCD_FUNCTIONSET | _displayfunction);

    // finally, set # lines, font size, etc.
    lcd_command(LCD_FUNCTIONSET | _displayfunction);
    _displaycontrol = LCD_DISPLAYON | LCD_CURSORON | LCD_BLINKOFF;
    lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);

    lcd_clear();

    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    lcd_command(LCD_ENTRYMODESET | _displaymode);

    lcd_home();
}

void lcd_clear() {
    lcd_command(LCD_CLEARDISPLAY);
    delay_us(3);
}

void lcd_home() {
    lcd_command(LCD_RETURNHOME);
    delay_us(3);
}

void lcd_setCursor(uint8_t col, uint8_t row) {
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > 3 || col > 19)
        return;
    lcd_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void lcd_noDisplay(void) {
    _displaycontrol &= ~LCD_DISPLAYON;
    lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void lcd_display(void) {
    _displaycontrol |= LCD_DISPLAYON;
    lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

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

void lcd_command(uint8_t value) {
    lcd_send(value, 0);
}

void lcd_write(uint8_t value) {
    lcd_send(value, 1);
}

void lcd_send(uint8_t value, uint8_t mode) {

    port_pin_set_output_level(PIN_LCD_RS, mode);
    lcd_write8bits(value);
    delay_us(50);
}

void lcd_write8bits(uint8_t value) {
    port_pin_set_output_level(PIN_LCD_D0, (value >> 0) & 0x01);
    port_pin_set_output_level(PIN_LCD_D1, (value >> 1) & 0x01);
    port_pin_set_output_level(PIN_LCD_D2, (value >> 2) & 0x01);
    port_pin_set_output_level(PIN_LCD_D3, (value >> 3) & 0x01);
    port_pin_set_output_level(PIN_LCD_D4, (value >> 4) & 0x01);
    port_pin_set_output_level(PIN_LCD_D5, (value >> 5) & 0x01);
    port_pin_set_output_level(PIN_LCD_D6, (value >> 6) & 0x01);
    port_pin_set_output_level(PIN_LCD_D7, (value >> 7) & 0x01);
    lcd_pulseEnable();
}

void lcd_pulseEnable(void) {
    port_pin_set_output_level(PIN_LCD_EN, 1);
    delay_us(1);
    port_pin_set_output_level(PIN_LCD_EN, 0);
    delay_us(50);
}


void lcd_setBacklight(uint8_t new_val) {
    /*manka*/
}

void lcd_printstr(char *str) {
    while (*str) {
        lcd_write(*str++);
    }
}
void lcd_create_bar_charts(void) {
    uint8_t char_arr[8];
    for(uint8_t i = 0; i < 8; i++) {
        char_arr[i] = 0;
    }
    for(uint8_t i = 0; i < 5; i++) {
        for(uint8_t j = 0; j < 8; j++) {
            char_arr[j] |= 1 << 4 - i;
        }
        lcd_createChar(i + 1, char_arr);
    }
}
