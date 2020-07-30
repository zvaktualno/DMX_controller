#include <asf.h>
#include <stdint.h>
#include "config.h"
#include "2004LCD.h"

uint8_t _displayfunction = 0;
uint8_t _displaycontrol = 0;
uint8_t _displaymode = 0;

void lcd_begin(void)
{

    _displayfunction = LCD_2LINE | LCD_8BITMODE | LCD_5x8DOTS;
    delay_ms(50);
    port_pin_set_output_level(PIN_LCD_RS, 0);
    port_pin_set_output_level(PIN_LCD_RW, 0);
    port_pin_set_output_level(PIN_LCD_EN, 0);

    delay_ms(50);

    lcd_command(LCD_FUNCTIONSET | _displayfunction);
    delay_us(4500);

    lcd_command(LCD_FUNCTIONSET | _displayfunction);
    delay_us(4500);

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

void lcd_clear(void)
{
    lcd_command(LCD_CLEARDISPLAY);
    delay_us(3);
}

void lcd_home(void)
{
    lcd_command(LCD_RETURNHOME);
    delay_us(3);
}

void lcd_setCursor(uint8_t col, uint8_t row)
{
    int row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row > 3 || col > 19)
        return;
    lcd_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void lcd_display(void)
{
    _displaycontrol |= LCD_DISPLAYON;
    lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void lcd_noCursor(void)
{
    _displaycontrol &= ~LCD_CURSORON;
    lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void noBlink(void)
{
    _displaycontrol &= ~LCD_BLINKON;
    lcd_command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void lcd_createChar(uint8_t location, uint8_t *char_arr)
{
    if(location > 7)
        return;
    lcd_command(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++) {
        lcd_write(char_arr[i]);
    }
}

void lcd_command(uint8_t value)
{
    lcd_send(value, 0);
}

void lcd_write(uint8_t value)
{
    lcd_send(value, 1);
}

void lcd_send(uint8_t value, uint8_t mode)
{

    port_pin_set_output_level(PIN_LCD_RS, mode);
    lcd_write8bits(value);
    delay_us(50);
}

void lcd_write8bits(uint8_t value)
{
    port_pin_set_output_level(PIN_LCD_D0, (value >> 0) & 0x01);
    port_pin_set_output_level(PIN_LCD_D1, (value >> 1) & 0x01);
    port_pin_set_output_level(PIN_LCD_D2, (value >> 2) & 0x01);
    port_pin_set_output_level(PIN_LCD_D3, (value >> 3) & 0x01);
    port_pin_set_output_level(PIN_LCD_D4, (value >> 4) & 0x01);
    port_pin_set_output_level(PIN_LCD_D5, (value >> 5) & 0x01);
    port_pin_set_output_level(PIN_LCD_D6, (value >> 6) & 0x01);
    port_pin_set_output_level(PIN_LCD_D7, (value >> 7) & 0x01);
    lcd_pulse_enable();
}

void lcd_pulse_enable(void)
{
    port_pin_set_output_level(PIN_LCD_EN, 1);
    delay_us(1);
    port_pin_set_output_level(PIN_LCD_EN, 0);
    delay_us(50);
}

void lcd_printstr(char *str)
{
    while (*str) {
        lcd_write(*str++);
    }
}
void lcd_create_bar_charts(void)
{
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
void lcd_create_arrow(void)
{
    uint8_t arrow_arr[8] = {0, 0x4, 0x2, 0x1F, 0x2, 0x4, 0, 0};

    lcd_createChar(LCD_ARROW_CHAR, arrow_arr);
}