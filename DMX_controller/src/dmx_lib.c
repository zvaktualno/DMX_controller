#include "dmx_lib.h"
#include <asf.h>
#include "config.h"

uint8_t DMX_TxBuf[DMX_UNIVERSE_SIZE]; // message buffer (Tx)
volatile uint8_t DMX_sent_flag = 0;

struct usart_module DMX_instance;
volatile uint16_t c;
struct usart_config config_usart;
void configure_DMX(void) {
    usart_get_config_defaults(&config_usart);

    config_usart.baudrate = 250000;
    config_usart.transfer_mode = USART_TRANSFER_ASYNCHRONOUSLY;
    config_usart.mux_setting = DMX_MUX;
    config_usart.character_size = USART_CHARACTER_SIZE_8BIT;
    config_usart.pinmux_pad0 = DMX_TX_PINMUX;
    config_usart.pinmux_pad1 = DMX_RX_PINMUX;
    config_usart.pinmux_pad2 = PINMUX_UNUSED;
    config_usart.pinmux_pad3 = PINMUX_UNUSED;
    config_usart.stopbits = USART_STOPBITS_2;
    config_usart.parity = USART_PARITY_NONE;
    config_usart.generator_source = GCLK_GENERATOR_3;
    config_usart.start_frame_detection_enable = true;
    while (usart_init(&DMX_instance, DMX_MODULE, &config_usart) != STATUS_OK)
        ;
    usart_enable(&DMX_instance);
    usart_enable_transceiver(&DMX_instance, USART_TRANSCEIVER_TX);
    delay_ms(1);
}

//usart_async_set_baud_rate(struct usart_async_descriptor *const descr, const uint32_t baud_rate);

void DMX_Send(uint8_t *p_data) {
    DMX_sent_flag = 0;

    usart_change_baudrate(57600);
    while (usart_write_buffer_job(&DMX_instance, &DMX_sent_flag, 1) != STATUS_OK);
    while (usart_get_job_status(&DMX_instance, USART_TRANSCEIVER_TX) != STATUS_OK);
    while (!DMX_sent_flag);

    usart_change_baudrate(250000);
    while (usart_write_buffer_job(&DMX_instance, p_data, DMX_UNIVERSE_SIZE) != STATUS_OK);
    while (usart_get_job_status(&DMX_instance, USART_TRANSCEIVER_TX) != STATUS_OK);
    while (!DMX_sent_flag);
}

void configure_DMX_callbacks(void) {
    usart_register_callback(&DMX_instance, DMX_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
    usart_enable_callback(&DMX_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
}

void DMX_write_callback(struct usart_module *const usart_module) {
    DMX_sent_flag = 1;
}

void usart_change_baudrate(uint32_t baudrate) {
    config_usart.baudrate = 250000;
    usart_disable_transceiver(&DMX_instance);
    while (usart_init(&DMX_instance, USB_MODULE, &config_usart) != STATUS_OK);

    usart_enable(&DMX_instance);
    usart_enable_transceiver(&DMX_instance, USART_TRANSCEIVER_TX);
    usart_enable_transceiver(&DMX_instance, USART_TRANSCEIVER_RX);
    delay_ms(1);
}
