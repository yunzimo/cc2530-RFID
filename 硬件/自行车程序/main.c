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

char Card_mess[32]; //卡片内容

uint money;
uint count;

unsigned char Money[4]={0,0,0,0};
//M1卡的某一块写为如下格式，则该块为钱包，可接收扣款和充值命令
//4字节金额（低字节在前）＋4字节金额取反＋4字节金额＋1字节块地址＋1字节块地址取反＋1字节块地址＋1字节块地址取反




uchar Recdata[30];
uchar RXTXflag=1;
uchar ucTagType[4];
uchar ret;

//定义控制LED灯的端口
#define LED1 P1_0	//定义LED1为P10口控制

uint Transform_16_10(uint  num);

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

//定时器初始化
void InitT1() //系统不配置工作时钟时默认是2分频，即16MHz
{
  T1CTL = 0x0c;          //128分频，自动重装 0X0000-0XFFFF d=1101 c=1100
  T1STAT= 0x21;          //通道0, 中断有效   
}

void InitLed(void)
{
    P1DIR |= 0x03; //P1_0定义为输出
    LED1 = 1;       //LED1灯初始化熄灭
    
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
 * @函数名：MFRC522_Read_Wallet
 * @描  述：读出钱包的金额
 * @说  明：
 * @参  数：block 钱包的位置（第几块）
 * @返回值：成功返回MI_OK
 *********************************/
char MFRC522_Read_Wallet(uchar block,unsigned char *pData)
{
  uchar status=MI_ERR;
  //uint money;  //最后的结果
  status = PcdRead(block,pData); //读块
  if(status==0x26){
    money=pData[1];
    money<<=8;  //扩大8倍
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
    T1CTL = 0x0d;  //启动定时器
  }
  //PcdValue(0xc0,2,Money);
  return status;
  
}


/*********************************
 * @函数名：Transform_16_10
 * @描  述：16进制装换成10进制
 * @说  明：
 * @参  数：num 要转换的数据
 * @返回值：无 
 *********************************/
uint Transform_16_10(uint num)
{
	uint temp,result;
	temp = num&0x000f;//获取最后一位 
	result = temp;	
	temp = (num>>4)&0x000f;//获取倒数第二位
	result += temp*16;	
	temp = (num>>8)&0x000f;
	result += temp*16*16;	
	temp = (num>>12)&0x000f;//获取最高位
	result += temp*16*16*16; //最后的结果
	return  result; //十六进制转换为十进制
}





void main()
{


  InitIO();
  InitLed();
  
  LCD_Init();                      //oled 初始化  
  LCD_CLS();                      //屏全亮 
  LCD_welcome();  
  
  InitT1();
  PcdReset();
  M500PcdConfigISOType('A');//设置工作方式
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
        DHT11();             //获取温湿度        
        //将温湿度的转换成字符串
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
