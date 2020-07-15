#ifndef REC_USB_H_
#define REC_USB_H_

#define USB_MASTER 	0   	/* komunikacijska adresa master-ja */
#define USB_BCAST  	0x7F  	/* broadcast adresa */
#define USB_RxMAXLEN 	128   	/* max. dolzina sporocila (rx) */
#define USB_TxMAXLEN 	240  	/* max. dolzina sporocila (tx) */
#define USB_STX  	0x55  	/* STX znak */
#define USB_ETX  	0xAA  	/* ETX znak */
#define USB_ADDR 0xFF
void configure_USB(void);
uint16_t USB_CalcCrc (unsigned char *msg, unsigned char length);
void USB_SendMessage (unsigned char *s, unsigned char length);
unsigned char *USB_GetMessage (unsigned char *Raddr, unsigned char *Saddr, unsigned char *length);
void USB_ReceiveEnable(void);
void USB_ReceiveDisable(void);
unsigned char USB_ReceiveCheck(void);
unsigned char *get_Txbuf(void);
void usart_read_callback(struct usart_module *const usart_module);
void configure_USB_callbacks(void);
void usart_write_callback(struct usart_module *const usart_module);

typedef enum {
    USB_START,
    USB_DESTINATION,
    USB_SOURCE,
    USB_LENGTH,
    USB_DATA,
    USB_CRC_H,
    USB_CRC_L,
    USB_END,
} USB_STATE;

#endif /* REC_USB_H_ */