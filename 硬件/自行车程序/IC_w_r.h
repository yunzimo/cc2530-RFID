#ifndef __IC_W_R_H__
#define __IC_W_R_H__

extern void PcdReset(void);//��λ
extern void M500PcdConfigISOType(unsigned char type);//������ʽ
extern char PcdRequest(unsigned char req_code,unsigned char *pTagType);//Ѱ��
extern char PcdAnticoll(unsigned char *pSnr);//������
extern char PcdSelect(unsigned char *pSnr);   //ѡ��
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr);//��֤����
char PcdWrite(unsigned char addr,unsigned char *pData); //д����
char PcdRead(unsigned char addr,unsigned char *pData); //������
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr);//����Ǯ��
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue); //��ۿ�
//char MFRC522_Read_Wallet(uchar block,unsigned char *pData,uint money); //����Ǯ���Ľ��

char* Int2String(int num,char *str);

#endif 