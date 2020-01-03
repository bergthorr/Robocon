#ifndef UART_H_
#define UART_H_

/* RXD0: PE0, TXD0: PE1 */
void usart0_init(uint32_t baud);
void usart0_send_char(uint8_t ch);
uint8_t usart0_recv_char();
void usart0_send_int16(int16_t val);
void usart0_send_string(char *str);

/* RXD1: PD2, TXD1: PD3 */
void usart1_init(uint32_t baud);
void usart1_send_char(uint8_t ch);
uint8_t usart1_recv_char();
void usart1_send_int16(int16_t val);
void usart1_send_string(char *str);

/* RXD2: PH0, TXD2: PH1 */
void usart2_init(uint32_t baud);
void usart2_send_char(uint8_t ch);
uint8_t usart2_recv_char();
void usart2_send_int16(int16_t val);
void usart2_send_string(char *str);

/* RXD3: PJ0, TXD3: PJ1 */
void usart3_init(uint32_t baud);
void usart3_send_char(uint8_t ch);
uint8_t usart3_recv_char();
void usart3_send_int16(int16_t val);
void usart3_send_string(char *str);

#endif /* UART_H_ */