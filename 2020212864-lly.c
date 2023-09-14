#include <reg52.h>		//头文件
#define uc unsigned char
#define ui unsigned int	  //宏定义

sbit RS=P1^4;	
sbit RW=P1^5;
sbit E=P1^6;			
sbit D0=P1^3;	
sbit D1=P1^2;	
sbit D2=P1^1;	
sbit D3=P1^0;			
sbit bg_1602=P2^6;		
sbit key=P3^3;			
sbit Beep=P3^4;			

uc code tab0[]=" 123456789#";  
uc code tab1[]="Calling!!!";   
uc code tab2[]="I Know!";	   
uc code tab3[]="Happy Every Day!"; 

uc			num_D0=0,num_D1=0,num_D2=0,num_D3=0;	
bit bdata	fu_D0=0, fu_D1=0, fu_D2=0, fu_D3=0,flag;

void delay(ui x)		
{
	ui i,j;
	for(i=0;i<x;i++)
	for(j=0;j<121;j++);	 
}

void init()				//初始化函数
{
	bg_1602=0;			//开启背光
	TMOD=0x01;			//定时器0工作方式1
 	TH0=0x4b;
 	TL0=0xff;		  //赋初值
 	EA=1;			   //开总中断
 	ET0=1;			   //定时器中断开
 	TR0=1;			   //定时器0中断开

	D0=0;
	D1=0;
	D2=0;
	D3=0;				//清零床号引脚
}
/******LCD1602******/
void com_1602(uc com)	//写命令
{
	RS=0;
	P0=com;
	delay(1);
	E=1;
	delay(1);
	E=0;
	RS=1;
}

void data_1602(uc date)	//写数据
{
	RS=1;
	P0=date;
	delay(1);
	E=1;
	delay(1);
	E=0;
	RS=0;
}

void dis_1602(uc word_num,uc hang,uc lie,uc tab_num,uc num)
/**word_num:显示字符个数，hang,lie:显示坐标，****
***tab_num：显示数组号，num：从第几字符开始显示****/
{
	uc n;
	for(n=0;n<word_num;n++)
	{
		com_1602(0x80+hang*0x40+lie);
		if(tab_num==0)
			data_1602(tab0[num+n]);
		else if(tab_num==1)
			data_1602(tab1[num+n]);
		else if(tab_num==2)
			data_1602(tab2[num+n]);
		else if(tab_num==3)
			data_1602(tab3[num+n]);
		delay(5);
		lie++;
	}
}
void init_1602()	//初始化1602
{
	RW=0;			//写命令
	com_1602(0x38);//设置1602
	com_1602(0x0c);//设置显示，光标不闪烁
	com_1602(0x06);//不移动，指针加1
	com_1602(0x01);//清屏
}

void know()			//应答函数
{
	if(D0!=fu_D0)
	num_D0=1;
	if(D1!=fu_D1)
	num_D1=2;
	if(D2!=fu_D2)
	num_D2=3;
	if(D3!=fu_D3)
	num_D3=4; 		//数据脚状态改变，将床号赋给变量

	if(key==0)		//检测按键
	{
		delay(5);
		if(key==0)
		{
			if((num_D0!=0)||(num_D1!=0)||(num_D2!=0)||(num_D3!=0))//如果有呼叫时
			{
				bg_1602=0;							//打开背光
				Beep=1;								//关闭蜂鸣器
				flag=0;								//标志位清零
				num_D0=0;num_D1=0;num_D2=0;num_D3=0;
				fu_D0=D0;fu_D1=D1;fu_D2=D2;fu_D3=D3;//将床位清零，并记录当前引脚状态
				com_1602(0x01);						//清屏
				dis_1602(7,1,4,2,0);				//显示I Know 
			}
			else		//如果没有呼叫时
			bg_1602=0;	//只点亮背光
		}
		while(!key);	//按键释放
		delay(2000);
		com_1602(0x01);//清屏
	}
}

void display()		//显示函数
{
	if((num_D0==0)&&(num_D1==0)&&(num_D2==0)&&(num_D3==0))	//当没有床位呼叫时
	dis_1602(16,0,0,3,0);									//显示Happy 
	else							//有床位呼叫时
	{
		if(flag==0)
		{
			flag=1;
			com_1602(0x01);//清屏
		}
		if(num_D0==1)
		{
			dis_1602(1,0,2,0,num_D0);
			dis_1602(1,0,3,0,10);
		}
		if(num_D1==2)
		{
			dis_1602(1,0,5,0,num_D1);
			dis_1602(1,0,6,0,10);
		}
		if(num_D2==3)
		{
			dis_1602(1,0,8,0,num_D2);
			dis_1602(1,0,9,0,10);
		}
		if(num_D3==4)
		{
			dis_1602(1,0,11,0,num_D3);
			dis_1602(1,0,12,0,10);
		}
		dis_1602(10,1,3,1,0);		//在对应床位显示床位号
	}
}

void LED()		//背光函数
{
	if((D0!=fu_D0)||(D1!=fu_D1)||(D2!=fu_D2)||(D3!=fu_D3))	//如果有呼叫
	bg_1602=0;				//打开背光
}	

void main()		//主函数
{
	init();
	init_1602();//初始化
	while(1)	//死循环
	{
		display();
		know();
		LED();
	}
}

void T0_time() interrupt 1	//定时器T0
{
	ui m,n;
	TH0=0x4b;
 	TL0=0xff;		//重新赋初值
	m++;
	if(m==10)
	{
		m=0;
		if((num_D0!=0)||(num_D1!=0)||(num_D2!=0)||(num_D3!=0))		//如果有呼叫时开启蜂鸣器
		Beep=~Beep;
	}
	if((bg_1602==0)&&(D0==fu_D0)&&(D1==fu_D1)&&(D2==fu_D2)&&(D3==fu_D3))
	{
		n++;
		if(n==300)		//背光点亮时计时15秒关闭背光
		{
			n=0;
			bg_1602=1;
		}
	}
}