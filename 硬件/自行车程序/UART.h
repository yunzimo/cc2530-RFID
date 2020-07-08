#ifndef __UART_H__
#define __UART_H__

extern void UartInitial();
extern void UartSend(unsigned char infor);
extern void UartSend_String(char *Data,int len);
extern void UartSend_Int(int data);

#endif