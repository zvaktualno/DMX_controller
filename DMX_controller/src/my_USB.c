#include <asf.h>
#include "config.h"
#include "my_USB.h"

unsigned char USB_RxBuf[USB_RxMAXLEN + 8]; // message buffer (Rx)
unsigned char USB_TxBuf[USB_TxMAXLEN + 8]; // message buffer (Tx)

uint8_t USB_flags;
#define	USB_RXR 1
#define USB_EN 3
#define USB_RX_EN 4

struct usart_module USB_instance;
volatile uint16_t c;

//bit USB_RxReady;    //when a message has been read, it is set to 1, that the next message can be received

void configure_USB(void) {

    struct usart_config config_usart;

    usart_get_config_defaults(&config_usart);

    config_usart.baudrate    =					31500;
    config_usart.transfer_mode =				USART_TRANSFER_ASYNCHRONOUSLY;
    config_usart.mux_setting =					USART_RX_3_TX_2_XCK_3;
    config_usart.character_size =				USART_CHARACTER_SIZE_8BIT;
    config_usart.pinmux_pad0 =					PINMUX_PB10D_SERCOM4_PAD2;
    config_usart.pinmux_pad1 =					PINMUX_PB11D_SERCOM4_PAD3;
    config_usart.pinmux_pad2 =					PINMUX_UNUSED;
    config_usart.pinmux_pad3 =					PINMUX_UNUSED;
    config_usart.stopbits =						USART_STOPBITS_1;
    config_usart.parity =						USART_PARITY_NONE;
    config_usart.generator_source =				GCLK_GENERATOR_3;
    config_usart.start_frame_detection_enable = true;
    while (usart_init(&USB_instance, USB_MODULE, &config_usart) != STATUS_OK);

    usart_enable(&USB_instance);
    usart_enable_transceiver(&USB_instance, USART_TRANSCEIVER_TX);
    usart_enable_transceiver(&USB_instance, USART_TRANSCEIVER_RX);
    delay_ms(1);
}

// update crc reverse polynome 0xA001 (crc-16 mask)
uint16_t USB_CalcCrc (unsigned char *msg, unsigned char length) {
    unsigned char n, tmp;
    uint16_t crc = 0;
    unsigned char *p_msg;
    unsigned int *p_tab;

    /*p_msg = msg;
    for (n = 0; n < length; n++) {
        tmp = ((unsigned char)crc^*p_msg++);
        p_tab = USB_crc16tab + tmp;
        crc = (unsigned int)(crc >> 8)^*p_tab;
    }*/

    return crc;
}


// formira in poslje slave message
// opremi z STX Saddr Raddr len msg CRC16 ETX+

void USB_SendMessage (unsigned char *p_string, uint16_t length) {

    char transmit_message[USB_TxMAXLEN + 6];
    char *p_to_msg_start, *p_to_msg_last;
    uint16_t crc;
    p_to_msg_last = p_to_msg_start = transmit_message;
    *(p_to_msg_last++) = USB_STX;
    *(p_to_msg_last++) = USB_MASTER;  // source: our address
    *(p_to_msg_last++) = USB_ADDR; // destination: 0 = PC
    *(p_to_msg_last++) = length >> 8; // block length
    *(p_to_msg_last++) = length & 0xFF; // block length

    // copy message to output buffer
    for (uint16_t n = 0; n < length; n++) *(p_to_msg_last++) = *(p_string + n);
    /*
        crc = USB_CalcCrc(p_to_msg_start + 1, length + 3);
        *(p_to_msg_last++) = (unsigned char)(crc >> 8); // CRC MSB
        *(p_to_msg_last++) = (unsigned char)(crc & 0xFF); // CRC LSB*/
    *(p_to_msg_last++) = USB_ETX;  // ETX

    while(usart_write_buffer_job(&USB_instance, p_to_msg_start, p_to_msg_last - p_to_msg_start) != STATUS_OK);
    while(usart_get_job_status(&USB_instance, USART_TRANSCEIVER_TX) != STATUS_OK);
}
/*
//vrne parametre sprejetega sporocila in pointer na message v RxBuff
unsigned char *USB_GetMessage (unsigned char *Raddr, unsigned char *Saddr, unsigned char *length) {
    *Raddr = *USB_RxBuf;
    *Saddr = *(USB_RxBuf + 1);
    *length = *(USB_RxBuf + 2);
    return USB_RxBuf + 3;
}*/

void USB_ReceiveEnable(void) {
    USB_flags |= (1 << USB_RXR) | (1 << USB_EN);
}

void USB_ReceiveDisable(void) {
    USB_flags &= ~(1 << USB_RXR) | (1 << USB_EN);
}

// vrne 1, ce ni nobenega prispelega sporocila
unsigned char USB_ReceiveCheck(void) {
    return  USB_flags & (1 << USB_RXR);
}
// vrne pointer na lokacijo tx bufferja, kjer se zacne sporocilo
unsigned char *get_Txbuf(void) {
    return ( USB_TxBuf + 3 );
}

void configure_USB_callbacks(void) {

    usart_register_callback(&USB_instance, usart_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
    usart_register_callback(&USB_instance, usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);

    usart_enable_callback(&USB_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
    usart_enable_callback(&USB_instance, USART_CALLBACK_BUFFER_RECEIVED);
    usart_read_job(&USB_instance, &c);
}
void usart_write_callback(struct usart_module *const usart_module) {
    BREAKPOINT;
}


void usart_read_callback(struct usart_module *const usart_module) {

    static USB_STATE state = USB_START;
    static uint8_t count;
    static uint16_t crc;
    static uint8_t length;
    static uint8_t *p_RxBuf;

    usart_read_job(&USB_instance, &c);
    if(!(USB_flags & (1 << USB_RXR)))
        state = USB_START;
    else
        switch (state) {
            case USB_START:
                //until RxReady, ignore received chars
                if (USB_flags & ((1 << USB_RXR) | (1 << USB_EN))) {
                    if (c == USB_STX) {
                        p_RxBuf = USB_RxBuf;
                        state = USB_SOURCE;
                    }
                }
                break;

            case USB_SOURCE:
                if (c == USB_ADDR || c == USB_BCAST) {
                    *p_RxBuf++ = c;
                    state = USB_DESTINATION;
                }
                else state = USB_START;
                break;

            case USB_DESTINATION:
                if (c == USB_MASTER) {
                    *p_RxBuf++ = c;
                    state = USB_LENGTH;
                }
                else state = USB_START;
                break;

            case USB_LENGTH:
                if (c <= USB_RxMAXLEN) {
                    *p_RxBuf++ = c;
                    count = c;
                    length = c;
                    state = USB_DATA;
                    if (count == 0) state = USB_CRC_H;
                }
                else state = USB_START;
                break;

            case USB_DATA:
                *p_RxBuf++ = c;
                if (--count == 0) state = USB_CRC_H;
                break;

            case USB_CRC_H:
                crc = (unsigned int)c << 8;
                state = USB_CRC_L;
                break;

            case USB_CRC_L:
                crc |= (unsigned int)c;
                if (crc == USB_CalcCrc(USB_RxBuf, length + 3)) state = USB_END;
                else state = USB_START;
                break;

            case USB_END:
                *p_RxBuf++ = 0;  //insert null character
                if (c == USB_ETX) {
                    USB_flags |= (1 << USB_RXR);
                }
                state = USB_START;
                break;

            default:
                state = USB_START;
        }    //end of switch (Rx_mode)

}