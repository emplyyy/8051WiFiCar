#include<reg52.h> 
#include<intrins.h>
#include <stdio.h>

/*------------------------------------------------
           �ĸ����������ƿ�
------------------------------------------------*/
sbit PWM1=P2^0;	  
sbit PWM2=P2^1;
sbit PWM3=P2^2;
sbit PWM4=P2^3;
/*------------------------------------------------
              1602��������
------------------------------------------------*/
sbit RS = P1^5;   //LED����˿� 
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
             �Զ������
------------------------------------------------*/
unsigned int a;
unsigned int b=1;	 // a\b\c\d\e���Ǳ�־λ����֤wifiģ����ȷ��˳������
unsigned int c =1;
unsigned int d =1;
unsigned char ldat;		//�����������λ��ֵ
unsigned int e=1,f=0;   
char k[9],flagmode=0;	//k[]�������ݣ�flagmodeģʽ�� 
int v=0,pwmflag=0,pwmv=40;	//v�ٶ�  PWM����  ���PWMռ�ձ�
//
/*------------------------------------------------
              ����ʱ
------------------------------------------------*/
void DelayUs2x(unsigned char t)
{   
 while(--t);
}
/*------------------------------------------------
 mS��ʱ������
------------------------------------------------*/
void DelayMs(unsigned int t)
{
     
 while(t--)
 {
     //������ʱ1mS
     DelayUs2x(245);
	   DelayUs2x(245);
 }
}
/*------------------------------------------------
              ��æ����
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
              д�������
------------------------------------------------*/
 void LCD_Write_Com(unsigned char com) 
 {  
 while(LCD_Check_Busy()); //æ��ȴ�
 RS_CLR; 
 RW_CLR; 
 EN_SET; 
 DataPort= com; 
 _nop_(); 
 EN_CLR;
 }
/*------------------------------------------------
              д�����ݺ���
------------------------------------------------*/
 void LCD_Write_Data(unsigned char Data) 
 { 
 while(LCD_Check_Busy()); //æ��ȴ�
 RS_SET; 
 RW_CLR; 
 EN_SET; 
 DataPort= Data; 
 _nop_();
 EN_CLR;
 }

/*------------------------------------------------
                ��������
------------------------------------------------*/
 void LCD_Clear(void) 
 { 
 LCD_Write_Com(0x01); 
 DelayMs(5);
 }
/*------------------------------------------------
              д���ַ�������
------------------------------------------------*/
 void LCD_Write_String(unsigned char x,unsigned char y,unsigned char *s) 
 {     
 if (y == 0) 
 	{     
	 LCD_Write_Com(0x80 + x);     //��ʾ��һ��
 	}
 else 
 	{      
 	LCD_Write_Com(0xC0 + x);      //��ʾ�ڶ���
 	}        
 while (*s) 
 	{     
 LCD_Write_Data( *s);     
 s ++;     
 	}
 }
/*------------------------------------------------
              д���ַ�����
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
              ��ʼ������
------------------------------------------------*/
 void LCD_Init(void) 
 {
   LCD_Write_Com(0x38);    /*��ʾģʽ����*/ 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38); 
   DelayMs(5); 
   LCD_Write_Com(0x38);  
   LCD_Write_Com(0x08);    /*��ʾ�ر�*/ 
   LCD_Write_Com(0x01);    /*��ʾ����*/ 
   LCD_Write_Com(0x06);    /*��ʾ����ƶ�����*/ 
   DelayMs(5); 
   LCD_Write_Com(0x0C);    /*��ʾ�����������*/
 }
 
	
	/*------------------------------------------------
              ESP8266-01��ʼ��
------------------------------------------------*/
	void ESP_Init(void)
	{
		
		 SCON=0x50;		//���ڳ�ʼ��
		 TMOD=0X20;    //���ö�ʱ��1Ϊģʽ2����Ϊ�����ʷ�����
		 TL1=0xFd;    //9600�����ʵĳ�ֵ
		 TH1=0xFd;    
		 TI = 1;
		 TR1=1;     //���������ʷ���
		 DelayMs(1000);
     do{
		   for (a=0; a<50000; a++);  //��ʱ
		   printf ("AT+CIPMUX=1\n");  //����������ģʽ
  	   do{
         if(RI)
		   	 {    //��ѯ�Ƿ������ݽ���
   					ldat=SBUF;  //�����ݴ��ڱ�����
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
				for (a=0; a<50000; a++);  //��ʱ
				printf ("AT+CIPSERVER=1,8080\n");  //���ö˿ں�
  		  do{
		    	if(RI)
		    	{    //��ѯ�Ƿ������ݽ���
						ldat=SBUF;  //�����ݴ��ڱ�����
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
                    ������
------------------------------------------------*/ 


void main(void)
{
 
 LCD_Init();  //1602��ʼ��
 LCD_Clear();//����
 LCD_Write_String(0,0," Waiting        ");	 
 LCD_Write_String(0,1," WIFI  Control  ");	  
 ESP_Init();
 LCD_Write_String(0,0,"R=0 Stop        ");	 
 while(1)//��ѭ��
   {
		if(RI)	  //��ѯ�Ƿ������ݽ���
		{   
			PWM1=1;	 //���յ�����ʱ�����Ҫֹͣ��
			PWM2=1;
			PWM3=1;
			PWM4=1;

			ldat=SBUF;  //�����ݴ��ڱ�����
			RI=0;   //������0�ȴ�����
		    if(ldat==':')
		    {
				f=0;
				e=1;
				do
				{
					if(RI)
					{    //��ѯ�Ƿ������ݽ���
						ldat=SBUF;  //�����ݴ��ڱ�����
						RI=0;   //������0�ȴ�����
						k[f]=ldat;
						f++ ;
						if(ldat=='T'){e=0;}	  //���յ�T����
					}
				} while (e) ;
			//д���ͳ���
				LCD_Write_Char(0,0,'R');
				LCD_Write_Char(1,0,'=');
				LCD_Write_Char(2,0,k[0]);

				if(k[0]=='5')	  //������յ�5��ͷ
				{
					flagmode=5;
					LCD_Write_String(4,0,"stop  ");
				}else if(k[0]=='4')	 //������յ�4��ͷ
				{
					flagmode=4;
					LCD_Write_String(4,0,"right ");
				} else if(k[0]=='3')	//������յ�3��ͷ
				{
					flagmode=3;
					LCD_Write_String(4,0,"left  ");
				}else if(k[0]=='2')	//������յ�2��ͷ
				{
					flagmode=2;
					LCD_Write_String(4,0,"back  ");
				}else if(k[0]=='1')	 //������յ�1��ͷ
				{
					flagmode=1;
					LCD_Write_String(4,0,"forwar");
				}

			}
		} 

		 if(flagmode==1)	  //ǰ��
		 {
		 	if(pwmflag<pwmv) PWM1=1;
			else PWM1=0;
			PWM2=0;
			if(pwmflag<pwmv) PWM3=1;
			else PWM3=0;
			PWM4=0;
		 }else if(flagmode==2)	 //����
		 {
			PWM1=0;
			if(pwmflag<pwmv) PWM2=1;
			else PWM2=0;
			PWM3=0;
			if(pwmflag<pwmv) PWM4=1;
			else PWM4=0;
		 } else if(flagmode==3)	//��ת
		 {
			if(pwmflag<pwmv) PWM1=1;
			else PWM1=0;
			PWM2=0;
			PWM3=0;
			if(pwmflag<pwmv) PWM4=1;
			else PWM4=0;
		 }else if(flagmode==4)	//��ת
		 {
		   PWM1=0;
			if(pwmflag<pwmv) PWM2=1;
			else PWM2=0;
			if(pwmflag<pwmv) PWM3=1;
			else PWM3=0;
			PWM4=0;
		 }else if(flagmode==5)	 //ֹͣ
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

  