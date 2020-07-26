#ifndef _DMX_H_
#define _DMX_H_

#define	DMX_RXR 1
#define	DMX_TXR 2
#define DMX_EN 3
#define DMX_RX_EN 4

#define DMX_TxMAXLEN 512
#define DMX_RxMAXLEN 512


void configure_DMX(void);
void DMX_SendMessage (uint8_t *p_string, uint16_t length);

void DMX_recieve_enable(void);
void DMX_recieve_disable(void);
void DMX_transmit_enable(void);
void DMX_transmit_disable(void);
uint8_t DMX_transmit_ready(void);
uint8_t DMX_recieve_ready(void);
uint8_t *DMX_get_tx_buf(void);
unsigned char *DMX_get_rx_buf(void);
void configure_DMX_callbacks(void);
void usart_read_callback(struct usart_module *const usart_module);
void DMX_change_baudrate(uint32_t baud);

#endif