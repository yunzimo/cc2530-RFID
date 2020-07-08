
#include"ioCC2530.h"
#include"variable.h"

/****************************************************************************
��ʱʱ����1us��1ms��1s���ö�ʱ��1�����ò����жϣ����õȴ��ķ�ʽ
ϵͳʱ����32M����ʱ��ʱ��Ϊ32M
�ö�ʱ��1��ģģʽ����

****************************************************************************/

void Delay_1us(unsigned int k)
{/*
  T1CC0L = 0x20;
  T1CC0H = 0x00;
  T1CTL = 0x02;//ģģʽ ����Ƶ
  while(k)
  {
    while(!(T1CNTL >= 0x20));
    k--;
  }
  T1CTL = 0x00;//�رն�ʱ��*/
  uint i,j;
  for(i=0;i<k;i++)
    for(j=0;j<32;j++);
}

void Delay_1ms(unsigned int k)
{
  T1CC0L = 0xe8;
  T1CC0H = 0x03;
  T1CTL = 0x0a;//ģģʽ 32��Ƶ
  while(k)
  {
    while(!((T1CNTL >= 0xe8)&&(T1CNTH >= 0x03)));
    k--;
  }
  T1CTL = 0x00;//�رն�ʱ��
}


void Delay_1s(unsigned int k)
{
  while(k)
  {
    Delay_1ms(1000);
    k--;
  }
}



