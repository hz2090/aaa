#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"


void LED_IO_Init(void)
{ 	 		 
  GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��PORTbʱ��
  
	
	//GPIO��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
}


/**************************************************************
OLEDд����
void OLED_WR_Byte(���ݻ������A0ѡ��)
****************************************************************/
void OLED_WR_Byte(u8 dat,u8 cmd)
{
 u8 i;
 OLED_A0=cmd;//����CMD=0ʱ��ʾ�����CMD=1ʱ��ʾ��������
 OLED_CS=0; 
 for(i=0;i<8;i++)
 {
   OLED_SCL=0;
   OLED_SI=(dat&0x80)==0?0:1;
   dat<<=1;
   // delay_us(1);//delay50ns Tsds>+50ns
   OLED_SCL=1;
 }
 OLED_CS=1; 
}

/**************************************************************
OLED��ʼ������
****************************************************************/
void OLED_Init(void) 
{uchar i;
LED_IO_Init();
OLED_RST=0; //OLED(�͵�ƽ)��λ
for(i=100;i>0;i--);
OLED_RST=1;    // RES�ø�
for(i=100;i>0;i--);
OLED_WR_Byte(0xa2,0);// OLEDƫѹ���ã�1/9 BIAS
OLED_WR_Byte(0xa1,0);//�е�ַѡ��Ϊ���ҵ����Ӧ0~127;0xa1:�е�ַ�����ң�0xa0:�е�ַ���ҵ���
OLED_WR_Byte(0xc0,0);// �е�ַ���ϵ���Ϊ0~63;0xc0:�е�ַ���ϵ��£�0xc8:�е�ַ���µ���	
OLED_WR_Byte(0x26,0);// V5��ѹ�ڲ������������
OLED_WR_Byte(0x81,0);//���ȵ�������0~63(������)
OLED_WR_Byte(0x10,0);   //���ȵ���Ϊ˫�ֽ�����,ǰһ�����������ֵ���ǳ
OLED_WR_Byte(0x2f,0); //�ϵ���ƣ��򿪵�ѹ������ѹ���͵�ѹ����
OLED_WR_Byte(0xaf,0); //0xae set display off��0xaf:set display on
OLED_WR_Byte(0x40,0);//��ʼ���������д����01xxxxxx ����λ�ǵ�ַ(0-63)
}
//ҳ��ַ�������д����1011xxxx ��4λ�ǵ�ַ(0-8)
//�е�ַ���������4λ��д����0001xxxx ��4λ�ǵ�ַ(0-8)
//�е�ַ���������4λ��д����0000xxxx ��4λ�ǵ�ַ(0-8)
//------------------------------------------------------------------------------



//��8ҳ��һҳ��8��128�У�һҳ��ÿһ����һ���ֽڣ��±��Ǹ�λ��

u8 OLED_GRAM[128][8];	 

//�����Դ浽OLED		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		OLED_WR_Byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ  
		OLED_WR_Byte (0x04,OLED_CMD);      //������ʾλ�á��е͵�ַ!!!!!!!!
		 
		for(n=0;n<128;n++)
		OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}

//����OLED��ʾ    
void OLED_Display_On(void)
{
	//OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	//OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void OLED_Display_Off(void)
{
	//OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC����
	//OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)
		for(n=0;n<128;n++)
			OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//������ʾ
}
//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���	
/*
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;  //PAGE
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
*/
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;  //PAGE
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//mode:0,������ʾ;1,������ʾ				 
//size:ѡ������ 12/16/24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//����1206����
		else if(size==16)temp=asc2_1608[chr][t];	//����1608����
		else if(size==24)temp=asc2_2412[chr][t];	//����2412����
		else return;								//û�е��ֿ�
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}
//m^n����
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//size:�����С
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 
//��ʾ�ַ���
//x,y:�������  
//size:�����С 
//*p:�ַ�����ʼ��ַ 
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}	




