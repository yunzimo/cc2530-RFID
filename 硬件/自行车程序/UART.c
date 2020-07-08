#include"ioCC2530.h"
#include"variable.h"

//9600 8N1
void UartInitial()
{ 
    PERCFG = 0x00;				//λ��1 P0��
    P0SEL = 0x0c;				//P0��������
    P2DIR &= ~0XC0;                             //P0������ΪUART0    
    
    U0CSR |= 0x80;				//��������ΪUART��ʽ
    U0GCR |= 8;				
    U0BAUD |= 59;				//��������Ϊ9600
    UTX0IF = 1;                                 //UART0 TX�жϱ�־��ʼ��λ1 
    
    U0CSR |= 0X40;				//�������
    IEN0 |= 0x84;				//�����жϣ������ж�
  
}


void UartSend(uchar infor)
{
  U0DBUF = infor;
  while(UTX0IF == 0);
  UTX0IF = 0;
}

//���ڷ����ַ�������			
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
