#include "variable.h"
#include"delay.h"
#include "UART.h"
#include"rc522.h"
#include "IC_w_r.h"
#include <string.h>
#include <stdio.h>
#include "DHT11.H"
#include "LCD.h"

int String2Int(char *str);

unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 

unsigned char RFID1[16]={0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x01,0xFE,0x01,0xFE};

uchar asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

unsigned char pdata[32];

char Card_mess[32]; //��Ƭ����

uint money;
uint count;

unsigned char Money[4]={0,0,0,0};
//M1����ĳһ��дΪ���¸�ʽ����ÿ�ΪǮ�����ɽ��տۿ�ͳ�ֵ����
//4�ֽڽ����ֽ���ǰ����4�ֽڽ��ȡ����4�ֽڽ�1�ֽڿ��ַ��1�ֽڿ��ַȡ����1�ֽڿ��ַ��1�ֽڿ��ַȡ��




uchar Recdata[30];
uchar RXTXflag=1;
uchar ucTagType[4];
uchar ret;

//�������LED�ƵĶ˿�
#define LED1 P1_0	//����LED1ΪP10�ڿ���

uint Transform_16_10(uint  num);

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

//��ʱ����ʼ��
void InitT1() //ϵͳ�����ù���ʱ��ʱĬ����2��Ƶ����16MHz
{
  T1CTL = 0x0c;          //128��Ƶ���Զ���װ 0X0000-0XFFFF d=1101 c=1100
  T1STAT= 0x21;          //ͨ��0, �ж���Ч   
}

void InitLed(void)
{
    P1DIR |= 0x03; //P1_0����Ϊ���
    LED1 = 1;       //LED1�Ƴ�ʼ��Ϩ��
    
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
          ret=PcdAuthState(0x60,2,DefaultKey,ucTagType);
          
          if(ret==0x26){
            UartSend_String("auth 2 sucess",13);
            break;
            }
                        
          }
        }

        find = 0x00;
      }
    }
}

/*********************************
 * @��������MFRC522_Read_Wallet
 * @��  ��������Ǯ���Ľ��
 * @˵  ����
 * @��  ����block Ǯ����λ�ã��ڼ��飩
 * @����ֵ���ɹ�����MI_OK
 *********************************/
char MFRC522_Read_Wallet(uchar block,unsigned char *pData)
{
  uchar status=MI_ERR;
  //uint money;  //���Ľ��
  status = PcdRead(block,pData); //����
  if(status==0x26){
    money=pData[1];
    money<<=8;  //����8��
    money+=pData[0];
    money=Transform_16_10(money);
    
    Money[0]=money;
    char yue[20];
    Int2String(money,yue);
    //UartSend(block);
    UartSend_String("over:",5);
    UartSend_String(yue,strlen(yue));
    UartSend_String("\n",1);
    LED1=!LED1;
    T1CTL = 0x0d;  //������ʱ��
  }
  //PcdValue(0xc0,2,Money);
  return status;
  
}


/*********************************
 * @��������Transform_16_10
 * @��  ����16����װ����10����
 * @˵  ����
 * @��  ����num Ҫת��������
 * @����ֵ���� 
 *********************************/
uint Transform_16_10(uint num)
{
	uint temp,result;
	temp = num&0x000f;//��ȡ���һλ 
	result = temp;	
	temp = (num>>4)&0x000f;//��ȡ�����ڶ�λ
	result += temp*16;	
	temp = (num>>8)&0x000f;
	result += temp*16*16;	
	temp = (num>>12)&0x000f;//��ȡ���λ
	result += temp*16*16*16; //���Ľ��
	return  result; //ʮ������ת��Ϊʮ����
}





void main()
{


  InitIO();
  InitLed();
  
  LCD_Init();                      //oled ��ʼ��  
  LCD_CLS();                      //��ȫ�� 
  LCD_welcome();  
  
  InitT1();
  PcdReset();
  M500PcdConfigISOType('A');//���ù�����ʽ
  while(1){
    IC_test();
    ret=MFRC522_Read_Wallet(2,pdata);
    if(ret==0x26){
    while(1){
      if(IRCON>0){        
        IRCON=0;
        uchar temp[3]; 
        uchar humidity[3]; 
        uchar strTemp[15]="T:";
        uchar strHumidity[5]="H:";
        memset(temp, 0, 3);
        memset(humidity, 0, 3);
        DHT11();             //��ȡ��ʪ��        
        //����ʪ�ȵ�ת�����ַ���
        temp[0]=wendu_shi+0x30;
        temp[1]=wendu_ge+0x30;
        humidity[0]=shidu_shi+0x30;
        humidity[1]=shidu_ge+0x30; 
        strcat(strTemp,temp);
        strcat(strTemp,"  ");
        strcat(strHumidity,humidity);
        
        strcat(strTemp,strHumidity);
        
        LCD_P8x16Str(0, 4,strTemp);        
          if(++count>=money){
          count=0;
          LED1=!LED1;
          T1CTL = 0x0c;
          break;
          } 
        }
      }      
    }
  }
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
