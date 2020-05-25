#ifndef DMX_LIB_H_
#define DMX_LIB_H_


#include "dmx_lib.h"
#include <asf.h>
#include "config.h"

void configure_DMX(void);
void DMX_Send(uint8_t *p_data);

void configure_DMX_callbacks(void);
void DMX_write_callback(struct usart_module *const usart_module);
void usart_change_baudrate(uint32_t baudrate);


#endif /* DMX_LIB_H_ */