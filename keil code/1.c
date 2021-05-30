#include<reg52.h> 
#include<intrins.h>
#include <stdio.h>

/*------------------------------------------------
           四个电机输出控制口
------------------------------------------------*/
sbit PWM1=P2^0;	  
sbit PWM2=P2^1;
sbit PWM3=P2^2;
sbit PWM4=P2^3;
/*------------------------------------------------
              1602驱动程序
------------------------------------------------*/
sbit RS = P1^5;   //LED定义端口 
sbit RW = P1^6;
sbit EN = P1^7;
#define RS_CLR RS=0 
#define RS_SET RS=1
#define RW_CLR RW=0 
#define RW_SET RW=1 
#define EN_CLR EN=0
#define EN_SET EN=1
#define DataPort P0

/*------------------------------------------------
             自定义变量
------------------------------------------------*/
unsigned int a;
unsigned int b=1;	 // a\b\c\d\e都是标志位，保证wifi模块正确按顺序启动
unsigned int c =1;
unsigned int d =1;
unsigned char ldat;		//变量储存接收位的值
unsigned int e=1,f=0;   
char k[9],flagmode=0;	//k[]接收数据，flagmode模式， 
int v=0,pwmflag=0,pwmv=40;	//v速度  PWM周期  输出PWM占空比
//
/*------------------------------------------------
              简单延时
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
 while(--t);
}
/*------------------------------------------------
 mS延时函数，
------------------------------------------------*/
void DelayMs(unsigned int t)
{
     
 while(t--)
 {
     //大致延时1mS
     DelayUs2x(245);
	   DelayUs2x(245);
 }
}
/*------------------------------------------------
              判忙函数
------------------------------------------------*/
 bit LCD_Check_Busy(void) 
 { 
 DataPort= 0xFF; 
 RS_CLR; 
 RW_SET; 
 EN_CLR; 
 _nop_(); 
 EN_SET;
 return (bit)(DataPort & 0x80);
 }
/*------------------------------------------------
              写入命令函数
------------------------------------------------*/
 void LCD_Write_Com(unsigned char com) 
 {  
 while(LCD_Check_Busy()); //忙则等待
 RS_CLR; 
 RW_CLR; 
 EN_SET; 
 DataPort= com; 
 _nop_(); 
 EN_CLR;
 }
/*------------------------------------------------
              写入数据函数
------------------------------------------------*/
 void LCD_Write_Data(unsigned char Data) 
 { 
 while(LCD_Check_Busy()); //忙则等待
 RS_SET; 
 RW_CLR; 
 EN_SET; 
 DataPort= Data; 
 _nop_();
 EN_CLR;
 }

/*------------------------------------------------
                清屏函数
------------------------------------------------*/
 void LCD_Clear(void) 
 { 
 LCD_Write_Com(0x01); 
 DelayMs(5);
 }
/*------------------------------------------------
              写入字符串函数
------------------------------------------------*/
 void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s) 
 {     
 if (y == 0) 
 	{     
	 LCD_Write_Com(0x80 + x);     //表示第一行
 	}
 else 
 	{      
 	LCD_Write_Com(0xC0 + x);      //表示第二行
 	}        
 while (*s) 
 	{     
 LCD_Write_Data( *s);     
 s ++;     
 	}
 }
/*------------------------------------------------
              写入字符函数
------------------------------------------------*/
 void LCD_Write_Char(unsigned char x,unsigned char y,unsigned char Data) 
 {     
 if (y == 0) 
 	{     
 	LCD_Write_Com(0x80 + x);     
 	}    
 else 
 	{     
 	LCD_Write_Com(0xC0 + x);     
 	}        
 LCD_Write_Data( Data);  
 }
/*------------------------------------------------
              初始化函数
------------------------------------------------*/
 void LCD_Init(void) 
 {
   LCD_Write_Com(0x38);    /*显示模式设置*/ 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38);  
   LCD_Write_Com(0x08);    /*显示关闭*/ 
   LCD_Write_Com(0x01);    /*显示清屏*/ 
   LCD_Write_Com(0x06);    /*显示光标移动设置*/ 
   DelayMs(5); 
   LCD_Write_Com(0x0C);    /*显示开及光标设置*/
 }
 
	
	/*------------------------------------------------
              ESP8266-01初始化
------------------------------------------------*/
	void ESP_Init(void)
	{
		
		 SCON=0x50;		//串口初始化
		 TMOD=0X20;    //设置定时器1为模式2，做为波特率发生器
		 TL1=0xFd;    //9600波特率的初值
		 TH1=0xFd;    
		 TI = 1;
		 TR1=1;     //启动波特率发生
		 DelayMs(1000);
     do{
		   for (a=0; a<50000; a++);  //延时
		   printf ("AT+CIPMUX=1\n");  //启动多连接模式
  	   do{
         if(RI)
		   	 {    //查询是否有数据接收
   					ldat=SBUF;  //数据暂存于变量中
  					RI=0;
   				  if(ldat=='K')
				      {
					    	b=0;
					      d=0;
					    }
				    c++;
				    if(c==1000)
				      {
								c=1;
								d=0;
				      }
		     }
		    }while (d);  
       }while (b) ;

				b=1;
				c=1;
				d=1;

      do{
				for (a=0; a<50000; a++);  //延时
				printf ("AT+CIPSERVER=1,8080\n");  //设置端口号
  		  do{
		    	if(RI)
		    	{    //查询是否有数据接收
						ldat=SBUF;  //数据暂存于变量中
						RI=0;
						if (ldat=='K')
			      {
				    	b=0 ;
				      d=0;
			    	}
			      c++;
			      if (c==1000)
						{
						 c=1;
						 d=0;
						}
		       }
  		   }while (d);  
	     }while(b) ; 

	}
   
/*------------------------------------------------
                    主函数
------------------------------------------------*/ 


void main(void)
{
 
 LCD_Init();  //1602初始化
 LCD_Clear();//清屏
 LCD_Write_String(0,0," Waiting        ");	 
 LCD_Write_String(0,1," WIFI  Control  ");	  
 ESP_Init();
 LCD_Write_String(0,0,"R=0 Stop        ");	 
 while(1)//主循环
   {
		if(RI)	  //查询是否有数据接收
		{   
			PWM1=1;	 //接收到数据时，电机要停止了
			PWM2=1;
			PWM3=1;
			PWM4=1;

			ldat=SBUF;  //数据暂存于变量中
			RI=0;   //重新清0等待接收
		    if(ldat==':')
		    {
				f=0;
				e=1;
				do
				{
					if(RI)
					{    //查询是否有数据接收
						ldat=SBUF;  //数据暂存于变量中
						RI=0;   //重新清0等待接收
						k[f]=ldat;
						f++ ;
						if(ldat=='T'){e=0;}	  //接收到T结束
					}
				} while (e) ;
			//写发送程序
				LCD_Write_Char(0,0,'R');
				LCD_Write_Char(1,0,'=');
				LCD_Write_Char(2,0,k[0]);

				if(k[0]=='5')	  //如果接收到5开头
				{
					flagmode=5;
					LCD_Write_String(4,0,"stop  ");
				}else if(k[0]=='4')	 //如果接收到4开头
				{
					flagmode=4;
					LCD_Write_String(4,0,"right ");
				} else if(k[0]=='3')	//如果接收到3开头
				{
					flagmode=3;
					LCD_Write_String(4,0,"left  ");
				}else if(k[0]=='2')	//如果接收到2开头
				{
					flagmode=2;
					LCD_Write_String(4,0,"back  ");
				}else if(k[0]=='1')	 //如果接收到1开头
				{
					flagmode=1;
					LCD_Write_String(4,0,"forwar");
				}

			}
		} 

		 if(flagmode==1)	  //前进
		 {
		 	if(pwmflag<pwmv) PWM1=1;
			else PWM1=0;
			PWM2=0;
			if(pwmflag<pwmv) PWM3=1;
			else PWM3=0;
			PWM4=0;
		 }else if(flagmode==2)	 //后退
		 {
			PWM1=0;
			if(pwmflag<pwmv) PWM2=1;
			else PWM2=0;
			PWM3=0;
			if(pwmflag<pwmv) PWM4=1;
			else PWM4=0;
		 } else if(flagmode==3)	//左转
		 {
			if(pwmflag<pwmv) PWM1=1;
			else PWM1=0;
			PWM2=0;
			PWM3=0;
			if(pwmflag<pwmv) PWM4=1;
			else PWM4=0;
		 }else if(flagmode==4)	//右转
		 {
		   PWM1=0;
			if(pwmflag<pwmv) PWM2=1;
			else PWM2=0;
			if(pwmflag<pwmv) PWM3=1;
			else PWM3=0;
			PWM4=0;
		 }else if(flagmode==5)	 //停止
		 {
			PWM1=1;
			PWM2=1;
			PWM3=1;
			PWM4=1;
		 }

      if(pwmflag>100)
				pwmflag=0;	 
		  else 
				pwmflag++;
  }
}

  