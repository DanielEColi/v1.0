#ifndef  __UART_H__
#define __UART_H__
extern uint8_t BmpSoundflag1;
extern uint8_t pakege0,pakege1,pakege2,pakege3,pakege4;
void UART3_Send_Str(unsigned char *s);
void UART3_Send_bit(char value);
void Convert_Str( unsigned int num, unsigned char *str );
void bci_send();
void bci_send2();
void Fillbuffer2();
#endif
