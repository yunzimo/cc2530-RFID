#include "variable.h"
#include"delay.h"
#include "UART.h"
#include "IC_w_r.h"
#include <string.h>
#include <stdio.h>


#define LED1 P1_0            //定义P1.0口为LED1控制端
#define DATA_PIN P1_5        //定义P1.5口为传感器输入端3

int String2Int(char *str);

unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 

unsigned char RFID1[16]={0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x01,0xFE,0x01,0xFE};

uchar asc_16[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

unsigned char pdata[32];

char Card_mess[32]; //卡片内容

unsigned char Money[4]={0,0,0,0};
//M1卡的某一块写为如下格式，则该块为钱包，可接收扣款和充值命令
//4字节金额（低字节在前）＋4字节金额取反＋4字节金额＋1字节块地址＋1字节块地址取反＋1字节块地址＋1字节块地址取反


uchar temp;
uint datanumber=0;
uchar Recdata[30];
uchar RXTXflag=1;
uchar ucTagType[4];
uchar ret;
int k=1;

/****************************************************************************
* 名    称: InitLed()
* 功    能: 设置LED灯相应的IO口
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void InitLed(void)
{
    P1DIR |= 0x01;           //P1.0定义为输出口
}


void InitIO()
{
  CLKCONCMD &= ~0x40;         //设置系统时钟源为32MHZ晶振 
  while(CLKCONSTA & 0x40);    //等待晶振稳定为32M 
  CLKCONCMD &= ~0x47;         //设置系统主时钟频率为32MHZ     
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
    //16进制转ASC码
    char i;  
    char Card_Id[8]; //存放32位卡号
    char Card_type[8];
   
  
    ret = PcdRequest(0x52,ucTagType);//寻卡
    
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
      if(PcdAnticoll(ucTagType) == 0x26)//防冲撞
      {
        
        UartSend_String("The Card ID is: ",16);   
        
        //16进制转ASC码
        for(i=0;i<4;i++)
        {
          Card_Id[i*2]=asc_16[ucTagType[i]/16];
          Card_Id[i*2+1]=asc_16[ucTagType[i]%16];        
        }  
        UartSend_String(Card_Id,8); 
        UartSend_String("\n",1);
        
        ret=PcdSelect(ucTagType);//选卡

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
  M500PcdConfigISOType('A');//设置工作方式
  InitLed();
  P1DIR &= ~0x20;          //P1.5定义为输入口
  while(1)
  {
        
        if(DATA_PIN == 1)    //当光敏电阻处于黑暗中时P0.5高电平,LED1熄灭
        {
            LED1 = 0;
            Delay_1ms(1000);
        }
        else
        {
            LED1 = 1;       //检测到光线时P0.5为低电平LED1亮
            Delay_1ms(1000);
        }  
        
    
         if(RXTXflag == 1)			     //接收状态
          {
            
            if(temp != 0)
            {
                				     //接收状态指示
                if((temp!='#')&&(datanumber<50))     //’＃‘被定义为结束字符，最多能接收50个字符
                {          
                  Recdata[datanumber++] = temp;
                }
                else
                {
                  RXTXflag = 3;                      //进入发送状态
                }
                
                if(datanumber == 50)
                  RXTXflag = 3;
              temp  = 0;	
            }
          }
          if(RXTXflag == 3)			//发送状态
          {
            UartSend_String("send:",5);                
            U0CSR &= ~0x40;			//不能收数
            UartSend_String(Recdata,datanumber);
            UartSend_String("\n",1);
            U0CSR |= 0x40;			//允许接收
            
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

              IC_test();              //检测IC卡
              
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

              IC_test();              //检测IC卡
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
              IC_test();              //检测IC卡
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
            RXTXflag = 1;		        //恢复到接收状态
            datanumber = 0;			//指针归0
            memset(Recdata, 0, sizeof(Recdata));
          }
    
  }
}

/****************************************************************
串口接收一个字符:一旦有数据从串口传至CC2530,则进入中断，将接收到的数据赋值给变量temp.
****************************************************************/
#pragma vector = URX0_VECTOR
 __interrupt void UART0_ISR(void)
 {
 	URX0IF = 0;				//清中断标志
	temp = U0DBUF;                          
 }


int String2Int(char *str)//字符串转数字 
{
	char flag = '+';//指示结果是否带符号 

        long res = 0;
	
	
        if(*str=='-')//字符串带负号 

	{
		
         ++str;//指向下一个字符 
		
         flag = '-';//将标志设为负号 

	} 
	
	
        sscanf(str, "%ld", &res);
	
        if(flag == '-'){
		
          res = -res;
	
        }
	
        return (int)res;

}
