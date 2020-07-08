#include "variable.h"
#include"delay.h"
#include "UART.h"
#include "IC_w_r.h"
#include <string.h>
#include <stdio.h>


#define LED1 P1_0            //����P1.0��ΪLED1���ƶ�
#define DATA_PIN P1_5        //����P1.5��Ϊ�����������3

int String2Int(char *str);

unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 

unsigned char RFID1[16]={0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x01,0xFE,0x01,0xFE};

uchar asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

unsigned char pdata[32];

char Card_mess[32]; //��Ƭ����

unsigned char Money[4]={0,0,0,0};
//M1����ĳһ��дΪ���¸�ʽ����ÿ�ΪǮ�����ɽ��տۿ�ͳ�ֵ����
//4�ֽڽ����ֽ���ǰ����4�ֽڽ��ȡ����4�ֽڽ�1�ֽڿ��ַ��1�ֽڿ��ַȡ����1�ֽڿ��ַ��1�ֽڿ��ַȡ��


uchar temp;
uint datanumber=0;
uchar Recdata[30];
uchar RXTXflag=1;
uchar ucTagType[4];
uchar ret;
int k=1;

/****************************************************************************
* ��    ��: InitLed()
* ��    ��: ����LED����Ӧ��IO��
* ��ڲ���: ��
* ���ڲ���: ��
****************************************************************************/
void InitLed(void)
{
    P1DIR |= 0x01;           //P1.0����Ϊ�����
}


void InitIO()
{
  CLKCONCMD &= ~0x40;         //����ϵͳʱ��ԴΪ32MHZ���� 
  while(CLKCONSTA & 0x40);    //�ȴ������ȶ�Ϊ32M 
  CLKCONCMD &= ~0x47;         //����ϵͳ��ʱ��Ƶ��Ϊ32MHZ     
  UartInitial();
  
  // IC_SDA P2_0
  P2DIR |= 1<<0;
  P2INP |= 1<<0;
  P2SEL &= ~(1<<0);
  
  // IC_SCK  P0_7
  P0DIR |= 1<<7;
  P0INP |= 1<<7;
  P0SEL &= ~(1<<7);
  
  // IC_MOSI P0_6
  P0DIR |= 1<<6;
  P0INP |= 1<<6;
  P0SEL &= ~(1<<6);
  
  // IC_MISO P0_5
  P0DIR |= 1<<5;
  P0INP |= 1<<5;
  P0SEL &= ~(1<<5);  

  // IC_RST P0_4
  P0DIR &= ~(1<<4);
  P0INP &= ~(1<<4);
  P0SEL &= ~(1<<4);
  
  IC_SCK = 1;
  IC_SDA = 1;
}



void IC_test()
{
  
  uchar find=0xaa;
  
  while(1)
  {
    //16����תASC��
    char i;  
    char Card_Id[8]; //���32λ����
    char Card_type[8];
   
  
    ret = PcdRequest(0x52,ucTagType);//Ѱ��
    
    if(ret != 0x26)
      ret = PcdRequest(0x52,ucTagType);
    if(ret != 0x26)
      find = 0xaa;
    if((ret == 0x26)&&(find == 0xaa))
    {
        for(i=0;i<4;i++)
        {
          Card_type[i*2]=asc_16[ucTagType[i]/16];
          Card_type[i*2+1]=asc_16[ucTagType[i]%16];        
        }
        UartSend_String(Card_type,4); 
      if(PcdAnticoll(ucTagType) == 0x26)//����ײ
      {
        
        UartSend_String("The Card ID is: ",16);   
        
        //16����תASC��
        for(i=0;i<4;i++)
        {
          Card_Id[i*2]=asc_16[ucTagType[i]/16];
          Card_Id[i*2+1]=asc_16[ucTagType[i]%16];        
        }  
        UartSend_String(Card_Id,8); 
        UartSend_String("\n",1);
        
        ret=PcdSelect(ucTagType);//ѡ��

        if(ret==0x26){
          UartSend_String("select sucess",13);
          ret=PcdAuthState(0x60,1,DefaultKey,ucTagType);
          
          if(ret==0x26){
            UartSend_String("auth 1 sucess",13);
            break;
            }
                        
          }
        }

        find = 0x00;
      }
    }

  

}
void main()
{
  InitIO();
  PcdReset();
  M500PcdConfigISOType('A');//���ù�����ʽ
  InitLed();
  P1DIR &= ~0x20;          //P1.5����Ϊ�����
  while(1)
  {
        
        if(DATA_PIN == 1)    //���������账�ںڰ���ʱP0.5�ߵ�ƽ,LED1Ϩ��
        {
            LED1 = 0;
            Delay_1ms(1000);
        }
        else
        {
            LED1 = 1;       //��⵽����ʱP0.5Ϊ�͵�ƽLED1��
            Delay_1ms(1000);
        }  
        
    
         if(RXTXflag == 1)			     //����״̬
          {
            
            if(temp != 0)
            {
                				     //����״ָ̬ʾ
                if((temp!='#')&&(datanumber<50))     //������������Ϊ�����ַ�������ܽ���50���ַ�
                {          
                  Recdata[datanumber++] = temp;
                }
                else
                {
                  RXTXflag = 3;                      //���뷢��״̬
                }
                
                if(datanumber == 50)
                  RXTXflag = 3;
              temp  = 0;	
            }
          }
          if(RXTXflag == 3)			//����״̬
          {
            UartSend_String("send:",5);                
            U0CSR &= ~0x40;			//��������
            UartSend_String(Recdata,datanumber);
            UartSend_String("\n",1);
            U0CSR |= 0x40;			//�������
            
            if(Recdata[0]=='0'){
              char mm[50];
              int i,j=1;
              for(i=0;i<datanumber;i++){
                mm[i]=Recdata[j];
                j++;
              }
              datanumber--;
              
              U0CSR &= ~0x40;
              UartSend_String("down",4);
              UartSend_String("\n",1);
              UartSend_String(mm,datanumber);
              UartSend_String("\n",1);
              U0CSR |= 0x40;

              Money[0]=String2Int(mm);

              IC_test();              //���IC��
              
              ret=PcdValue(0xc0,1,Money);

              MFRC522_Read_Wallet(1,pdata);
              if(ret==0x26){
                UartSend_String("yue sucess",10);
              }
              PcdWrite(2,RFID1);
              ret=PcdValue(0xc1,2,Money);
              if(ret==0x26)
              {
                  MFRC522_Read_Wallet(2,pdata);
              }
            }
            else if(Recdata[0]=='1'){
              char mm[50];
              int i,j=1;
              for(i=0;i<datanumber;i++){
                mm[i]=Recdata[j];
                j++;
              }
              datanumber--;
              UartSend_String("up",2);
              UartSend_String("\n",1);
              UartSend_String(mm,datanumber);
              UartSend_String("\n",1);

              Money[0]=String2Int(mm);

              IC_test();              //���IC��
              ret=PcdValue(0xc1,1,Money);
              if(ret==0x26){
                ret=PcdRead(1,pdata);
                if(ret==0x26){
                   for(i=0;i<32;i++)
                   {
                        Card_mess[i*2]=asc_16[pdata[i]/16];
                        Card_mess[i*2+1]=asc_16[pdata[i]%16];     
                   }  
                   UartSend_String(Card_mess,32);
                   UartSend_String("\n",1);
                 }                
              }
              MFRC522_Read_Wallet(1,pdata);
              if(ret==0x26){
                UartSend_String("yue sucess",10);
              }
            }
            else if(Recdata[0]=='c'){
              IC_test();              //���IC��
              ret=PcdWrite(1,RFID1);  
              if(ret==0x26){
                UartSend_String("clear sucess",12);
                ret=PcdRead(1,pdata);
                if(ret==0x26){
                   int i;
                   for(i=0;i<32;i++)
                   {
                        Card_mess[i*2]=asc_16[pdata[i]/16];
                        Card_mess[i*2+1]=asc_16[pdata[i]%16];     
                   }  
                   UartSend_String(Card_mess,32);
                   UartSend_String("\n",1);
                 }
                MFRC522_Read_Wallet(1,pdata);
              }
            }
            RXTXflag = 1;		        //�ָ�������״̬
            datanumber = 0;			//ָ���0
            memset(Recdata, 0, sizeof(Recdata));
          }
    
  }
}

/****************************************************************
���ڽ���һ���ַ�:һ�������ݴӴ��ڴ���CC2530,������жϣ������յ������ݸ�ֵ������temp.
****************************************************************/
#pragma vector = URX0_VECTOR
 __interrupt void UART0_ISR(void)
 {
 	URX0IF = 0;				//���жϱ�־
	temp = U0DBUF;                          
 }


int String2Int(char *str)//�ַ���ת���� 
{
	char flag = '+';//ָʾ����Ƿ������ 

        long res = 0;
	
	
        if(*str=='-')//�ַ��������� 

	{
		
         ++str;//ָ����һ���ַ� 
		
         flag = '-';//����־��Ϊ���� 

	} 
	
	
        sscanf(str, "%ld", &res);
	
        if(flag == '-'){
		
          res = -res;
	
        }
	
        return (int)res;

}
