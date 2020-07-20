#include <asf.h>
#include "config.h"
#include "DMX.h"

uint8_t DMX_RxBuf[DMX_RxMAXLEN + 8];
uint8_t DMX_TxBuf[DMX_TxMAXLEN + 8];

uint8_t DMX_flags;


struct usart_module DMX_instance;
volatile uint16_t c;

void configure_DMX(void) {

    struct usart_config config_usart;

    usart_get_config_defaults(&config_usart);

    config_usart.baudrate    =					250000;
    config_usart.transfer_mode =				USART_TRANSFER_ASYNCHRONOUSLY;
    config_usart.mux_setting =					USART_RX_1_TX_0_XCK_1;
    config_usart.character_size =				USART_CHARACTER_SIZE_8BIT;
    config_usart.pinmux_pad0 =					PINMUX_PB16C_SERCOM5_PAD0;
    config_usart.pinmux_pad1 =					PINMUX_PB17C_SERCOM5_PAD1;
    config_usart.pinmux_pad2 =					PINMUX_UNUSED;
    config_usart.pinmux_pad3 =					PINMUX_UNUSED;
    config_usart.stopbits =						USART_STOPBITS_2;
    config_usart.parity =						USART_PARITY_NONE;
    config_usart.generator_source =				GCLK_GENERATOR_3;
    config_usart.start_frame_detection_enable = true;
    while (usart_init(&DMX_instance, DMX_MODULE, &config_usart) != STATUS_OK);

    usart_enable(&DMX_instance);
    usart_enable_transceiver(&DMX_instance, USART_TRANSCEIVER_TX);
    usart_enable_transceiver(&DMX_instance, USART_TRANSCEIVER_RX);
    delay_ms(1);
}


void DMX_SendMessage (uint8_t *p_string, uint16_t length) {
    DMX_change_baudrate(100000);
    uint8_t null_var = 0;
    DMX_transmit_disable();
    while(usart_write_buffer_job(&DMX_instance, &null_var, 1) != STATUS_OK);
    while(usart_get_job_status(&DMX_instance, USART_TRANSCEIVER_TX) != STATUS_OK);
    DMX_change_baudrate(250000);
    DMX_transmit_disable();
    while(usart_write_buffer_job(&DMX_instance, p_string, length) != STATUS_OK);
    while(usart_get_job_status(&DMX_instance, USART_TRANSCEIVER_TX) != STATUS_OK);
}


void DMX_transmit_callback(struct usart_module *const usart_module) {
    DMX_transmit_enable();
}

void DMX_recieve_enable(void) {
    DMX_flags |= (1 << DMX_RXR);
}

void DMX_recieve_disable(void) {
    DMX_flags &= ~(1 << DMX_RXR);
}

void DMX_transmit_enable(void) {
    DMX_flags |= (1 << DMX_TXR);
}

void DMX_transmit_disable(void) {
    DMX_flags &= ~(1 << DMX_TXR);
}
uint8_t DMX_transmit_ready(void) {
    return DMX_flags & (1 << DMX_TXR);
}

uint8_t DMX_recieve_ready(void) {
    return DMX_flags & (1 << DMX_RXR);
}
// vrne pointer na lokacijo tx bufferja, kjer se zacne sporocilo
uint8_t *DMX_get_tx_buf(void) {
    return DMX_TxBuf;
}
unsigned char *DMX_get_rx_buf(void) {
    return DMX_RxBuf;
}

void configure_DMX_callbacks(void) {

    usart_register_callback(&DMX_instance, usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);

    usart_enable_callback(&DMX_instance, USART_CALLBACK_BUFFER_RECEIVED);
    usart_read_job(&DMX_instance, &c);
}

void usart_read_callback(struct usart_module *const usart_module) {

    usart_read_job(&DMX_instance, &c);

}

void DMX_change_baudrate(uint16_t baud) {
    while(!DMX_transmit_ready());
    DMX_transmit_disable();
    float part = 1 - 16.0 * baud / 4000000;
    uint16_t baudrate = (uint16_t)(65536 * part);

    if(baudrate == DMX_MODULE->USART.BAUD.reg)return;

    DMX_MODULE->USART.CTRLA.reg &= ~(1 << SERCOM_USART_CTRLA_ENABLE_Pos);
    DMX_MODULE->USART.CTRLB.bit.TXEN = 0;
    DMX_MODULE->USART.CTRLB.bit.RXEN = 0;
    while(DMX_MODULE->USART.SYNCBUSY.reg);

    DMX_MODULE->USART.BAUD.reg = baudrate;
    DMX_MODULE->USART.CTRLB.bit.TXEN = 1;
    DMX_MODULE->USART.CTRLB.bit.RXEN = 1;
    DMX_MODULE->USART.CTRLA.reg |= (1 << SERCOM_USART_CTRLA_ENABLE_Pos);
    while(DMX_MODULE->USART.SYNCBUSY.reg);
    DMX_transmit_enable();

}