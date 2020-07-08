#ifndef __IC_W_R_H__
#define __IC_W_R_H__

extern void PcdReset(void);//复位
extern void M500PcdConfigISOType(unsigned char type);//工作方式
extern char PcdRequest(unsigned char req_code,unsigned char *pTagType);//寻卡
extern char PcdAnticoll(unsigned char *pSnr);//读卡号
extern char PcdSelect(unsigned char *pSnr);   //选卡
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr);//验证密码
char PcdWrite(unsigned char addr,unsigned char *pData); //写数据
char PcdRead(unsigned char addr,unsigned char *pData); //读数据
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr);//备份钱包
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue); //充扣款
//char MFRC522_Read_Wallet(uchar block,unsigned char *pData,uint money); //读出钱包的金额

char* Int2String(int num,char *str);

#endif 