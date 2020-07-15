#include <asf.h>
#include "config.h"
#include "my_USB.h"

uint8_t USB_RxBuf[USB_RxMAXLEN + 8];
uint8_t USB_TxBuf[USB_TxMAXLEN + 8];

uint8_t USB_flags;


struct usart_module USB_instance;
volatile uint16_t c;

void configure_USB(void) {

    struct usart_config config_usart;

    usart_get_config_defaults(&config_usart);

    config_usart.baudrate    =					32500;
    config_usart.transfer_mode =				USART_TRANSFER_ASYNCHRONOUSLY;
    /*config_usart.mux_setting =					USART_RX_3_TX_2_XCK_3;
    config_usart.character_size =				USART_CHARACTER_SIZE_8BIT;
    config_usart.pinmux_pad0 =					PINMUX_PB10D_SERCOM4_PAD2;
    config_usart.pinmux_pad1 =					PINMUX_PB11D_SERCOM4_PAD3;
    config_usart.pinmux_pad2 =					PINMUX_UNUSED;
    config_usart.pinmux_pad3 =					PINMUX_UNUSED;*/
    config_usart.stopbits =						USART_STOPBITS_2;
    config_usart.parity =						USART_PARITY_NONE;
    config_usart.generator_source =				GCLK_GENERATOR_3;
    config_usart.start_frame_detection_enable = true;
    while (usart_init(&USB_instance, USB_MODULE, &config_usart) != STATUS_OK);

    usart_enable(&USB_instance);
    usart_enable_transceiver(&USB_instance, USART_TRANSCEIVER_TX);
    usart_enable_transceiver(&USB_instance, USART_TRANSCEIVER_RX);
    delay_ms(1);
}


void USB_SendMessage (uint8_t *p_string, uint16_t length) {

    while(usart_write_buffer_job(&USB_instance, p_string, length) != STATUS_OK);
    while(usart_get_job_status(&USB_instance, USART_TRANSCEIVER_TX) != STATUS_OK);
}

void USB_ReceiveEnable(void) {
    USB_flags |= (1 << USB_RXR) | (1 << USB_EN);
}

void USB_ReceiveDisable(void) {
    USB_flags &= ~(1 << USB_RXR) | (1 << USB_EN);
}

// vrne 1, ce ni nobenega prispelega sporocila
uint8_t USB_ReceiveCheck(void) {
    return  USB_flags & (1 << USB_RXR);
}
// vrne pointer na lokacijo tx bufferja, kjer se zacne sporocilo
uint8_t *USB_get_tx_buf(void) {
    return USB_TxBuf;
}
unsigned char *USB_get_rx_buf(void) {
    return USB_RxBuf;
}

void configure_USB_callbacks(void) {

    usart_register_callback(&USB_instance, usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);

    usart_enable_callback(&USB_instance, USART_CALLBACK_BUFFER_RECEIVED);
    usart_read_job(&USB_instance, &c);
}

void usart_read_callback(struct usart_module *const usart_module) {

    usart_read_job(&USB_instance, &c);

}

void USB_change_baudrate(uint16_t baud) {
    float part = 1 - 16.0 * baud / 4000000;
    uint16_t baudrate = (uint16_t)(65536 * part);

    if(baudrate == USB_MODULE->USART.BAUD.reg)return;

    USB_MODULE->USART.CTRLA.reg &= ~(1 << SERCOM_USART_CTRLA_ENABLE_Pos);
    USB_MODULE->USART.CTRLB.bit.TXEN = 0;
    USB_MODULE->USART.CTRLB.bit.RXEN = 0;
    while(USB_MODULE->USART.SYNCBUSY.reg);

    USB_MODULE->USART.BAUD.reg = baudrate;
    USB_MODULE->USART.CTRLB.bit.TXEN = 1;
    USB_MODULE->USART.CTRLB.bit.RXEN = 1;
    USB_MODULE->USART.CTRLA.reg |= (1 << SERCOM_USART_CTRLA_ENABLE_Pos);
    while(USB_MODULE->USART.SYNCBUSY.reg);
}