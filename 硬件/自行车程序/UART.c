#include"ioCC2530.h"
#include"variable.h"

//9600 8N1
void UartInitial()
{ 
    PERCFG = 0x00;				//位置1 P0口
    P0SEL = 0x0c;				//P0用作串口
    P2DIR &= ~0XC0;                             //P0优先作为UART0    
    
    U0CSR |= 0x80;				//串口设置为UART方式
    U0GCR |= 8;				
    U0BAUD |= 59;				//波特率设为9600
    UTX0IF = 1;                                 //UART0 TX中断标志初始置位1 
    
    U0CSR |= 0X40;				//允许接收
    IEN0 |= 0x84;				//开总中断，接收中断
  
}


void UartSend(uchar infor)
{
  U0DBUF = infor;
  while(UTX0IF == 0);
  UTX0IF = 0;
}

//串口发送字符串函数			
void UartSend_String(char *Data,int len)
{
  int j;
  for(j=0;j<len;j++)
  {
    U0DBUF = *Data++;
    while(UTX0IF == 0);
    UTX0IF = 0;
  }
}

void UartSend_Int(int data)
{
  U0DBUF=data;
  while(UTX0IF == 0);
  UTX0IF = 0;  
}
