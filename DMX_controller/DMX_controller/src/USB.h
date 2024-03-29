#ifndef _USB_H_
#define _USB_H_

#define	USB_RXR 1
#define	USB_TXR 2
#define USB_EN 3
#define USB_RX_EN 4

#define USB_TxMAXLEN 512
#define USB_RxMAXLEN 512


#define MIDI_NOTE_ON_MASK (0x9<<4)
#define MIDI_NOTE_OFF_MASK (0x8<<4)


void configure_USB(void);
void USB_SendMessage (uint8_t *p_string, uint16_t length);
void USB_transmit_callback(struct usart_module *const usart_module);
void USB_recieve_enable(void);
void USB_recieve_disable(void);
void USB_transmit_enable(void);
void USB_transmit_disable(void);
uint8_t USB_transmit_ready(void);
uint8_t USB_recieve_ready(void);
uint8_t *USB_get_tx_buf(void);
unsigned char *USB_get_rx_buf(void);
void configure_USB_callbacks(void);
void USB_change_baudrate(uint32_t baud);
void MIDI_send_message(uint8_t midi_ch, uint8_t value, uint8_t velocity);
void MIDI_send_note_on(uint8_t midi_ch, uint8_t note, uint8_t velocity);
void MIDI_send_note_off(uint8_t midi_ch, uint8_t note, uint8_t velocity);
#endif