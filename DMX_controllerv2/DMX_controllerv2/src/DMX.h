#ifndef REC_USB_H_
#define REC_USB_H_

#define	USB_RXR 1
#define USB_EN 3
#define USB_RX_EN 4

#define USB_TxMAXLEN 512
#define USB_RxMAXLEN 512

void configure_USB(void);
void USB_SendMessage (uint8_t *p_string, uint16_t length);
void USB_ReceiveEnable(void);
void USB_ReceiveDisable(void);
uint8_t USB_ReceiveCheck(void);
uint8_t *USB_get_tx_buf(void);
unsigned char *USB_get_rx_buf(void);
void configure_USB_callbacks(void);
void usart_read_callback(struct usart_module *const usart_module);
void USB_change_baudrate(uint16_t baud);

#endif