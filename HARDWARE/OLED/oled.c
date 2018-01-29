#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  	 
#include "delay.h"


void LED_IO_Init(void)
{ 	 		 
  GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能PORTb时钟
  
	
	//GPIO初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
}


/**************************************************************
OLED写函数
void OLED_WR_Byte(数据或者命令，A0选择)
****************************************************************/
void OLED_WR_Byte(u8 dat,u8 cmd)
{
 u8 i;
 OLED_A0=cmd;//参数CMD=0时表示发命令，CMD=1时表示发送数据
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
OLED初始化函数
****************************************************************/
void OLED_Init(void) 
{uchar i;
LED_IO_Init();
OLED_RST=0; //OLED(低电平)复位
for(i=100;i>0;i--);
OLED_RST=1;    // RES置高
for(i=100;i>0;i--);
OLED_WR_Byte(0xa2,0);// OLED偏压设置：1/9 BIAS
OLED_WR_Byte(0xa1,0);//列地址选择为从右到左对应0~127;0xa1:列地址从左到右；0xa0:列地址从右到左
OLED_WR_Byte(0xc0,0);// 行地址从上到下为0~63;0xc0:行地址从上到下；0xc8:行地址从下到上	
OLED_WR_Byte(0x26,0);// V5电压内部电阻调整设置
OLED_WR_Byte(0x81,0);//亮度调整命令0~63(暗到亮)
OLED_WR_Byte(0x10,0);   //亮度调节为双字节命令,前一个参数控制字的深浅
OLED_WR_Byte(0x2f,0); //上电控制，打开调压器、稳压器和电压跟随
OLED_WR_Byte(0xaf,0); //0xae set display off。0xaf:set display on
OLED_WR_Byte(0x40,0);//起始行设置命令，写命令01xxxxxx 后六位是地址(0-63)
}
//页地址设置命令，写命令1011xxxx 后4位是地址(0-8)
//列地址设置命令高4位，写命令0001xxxx 后4位是地址(0-8)
//列地址设置命令低4位，写命令0000xxxx 后4位是地址(0-8)
//------------------------------------------------------------------------------



//共8页，一页有8行128列，一页的每一列是一个字节，下边是高位，

u8 OLED_GRAM[128][8];	 

//更新显存到OLED		 
void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址  
		OLED_WR_Byte (0x04,OLED_CMD);      //设置显示位置—列低地址!!!!!!!!
		 
		for(n=0;n<128;n++)
		OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}

//开启OLED显示    
void OLED_Display_On(void)
{
	//OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	//OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	//OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	//OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)
		for(n=0;n<128;n++)
			OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//更新显示
}
//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空	
/*
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
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
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;  //PAGE
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,清空;1,填充	  
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
	{
		for(y=y1;y<=y2;y++)OLED_DrawPoint(x,y,dot);
	}													    
	OLED_Refresh_Gram();//更新显示
}
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 12/16/24
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	chr=chr-' ';//得到偏移后的值		 
    for(t=0;t<csize;t++)
    {   
		if(size==12)temp=asc2_1206[chr][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[chr][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[chr][t];	//调用2412字体
		else return;								//没有的字库
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
//m^n函数
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
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
//显示字符串
//x,y:起点坐标  
//size:字体大小 
//*p:字符串起始地址 
void OLED_ShowString(u8 x,u8 y,const u8 *p,u8 size)
{	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>(128-(size/2))){x=0;y+=size;}
        if(y>(64-size)){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,size,1);	 
        x+=size/2;
        p++;
    }  
	
}	




