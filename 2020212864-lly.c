#include <reg52.h>		//ͷ�ļ�
#define uc unsigned char
#define ui unsigned int	  //�궨��

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

void init()				//��ʼ������
{
	bg_1602=0;			//��������
	TMOD=0x01;			//��ʱ��0������ʽ1
 	TH0=0x4b;
 	TL0=0xff;		  //����ֵ
 	EA=1;			   //�����ж�
 	ET0=1;			   //��ʱ���жϿ�
 	TR0=1;			   //��ʱ��0�жϿ�

	D0=0;
	D1=0;
	D2=0;
	D3=0;				//���㴲������
}
/******LCD1602******/
void com_1602(uc com)	//д����
{
	RS=0;
	P0=com;
	delay(1);
	E=1;
	delay(1);
	E=0;
	RS=1;
}

void data_1602(uc date)	//д����
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
/**word_num:��ʾ�ַ�������hang,lie:��ʾ���꣬****
***tab_num����ʾ����ţ�num���ӵڼ��ַ���ʼ��ʾ****/
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
void init_1602()	//��ʼ��1602
{
	RW=0;			//д����
	com_1602(0x38);//����1602
	com_1602(0x0c);//������ʾ����겻��˸
	com_1602(0x06);//���ƶ���ָ���1
	com_1602(0x01);//����
}

void know()			//Ӧ����
{
	if(D0!=fu_D0)
	num_D0=1;
	if(D1!=fu_D1)
	num_D1=2;
	if(D2!=fu_D2)
	num_D2=3;
	if(D3!=fu_D3)
	num_D3=4; 		//���ݽ�״̬�ı䣬�����Ÿ�������

	if(key==0)		//��ⰴ��
	{
		delay(5);
		if(key==0)
		{
			if((num_D0!=0)||(num_D1!=0)||(num_D2!=0)||(num_D3!=0))//����к���ʱ
			{
				bg_1602=0;							//�򿪱���
				Beep=1;								//�رշ�����
				flag=0;								//��־λ����
				num_D0=0;num_D1=0;num_D2=0;num_D3=0;
				fu_D0=D0;fu_D1=D1;fu_D2=D2;fu_D3=D3;//����λ���㣬����¼��ǰ����״̬
				com_1602(0x01);						//����
				dis_1602(7,1,4,2,0);				//��ʾI Know 
			}
			else		//���û�к���ʱ
			bg_1602=0;	//ֻ��������
		}
		while(!key);	//�����ͷ�
		delay(2000);
		com_1602(0x01);//����
	}
}

void display()		//��ʾ����
{
	if((num_D0==0)&&(num_D1==0)&&(num_D2==0)&&(num_D3==0))	//��û�д�λ����ʱ
	dis_1602(16,0,0,3,0);									//��ʾHappy 
	else							//�д�λ����ʱ
	{
		if(flag==0)
		{
			flag=1;
			com_1602(0x01);//����
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
		dis_1602(10,1,3,1,0);		//�ڶ�Ӧ��λ��ʾ��λ��
	}
}

void LED()		//���⺯��
{
	if((D0!=fu_D0)||(D1!=fu_D1)||(D2!=fu_D2)||(D3!=fu_D3))	//����к���
	bg_1602=0;				//�򿪱���
}	

void main()		//������
{
	init();
	init_1602();//��ʼ��
	while(1)	//��ѭ��
	{
		display();
		know();
		LED();
	}
}

void T0_time() interrupt 1	//��ʱ��T0
{
	ui m,n;
	TH0=0x4b;
 	TL0=0xff;		//���¸���ֵ
	m++;
	if(m==10)
	{
		m=0;
		if((num_D0!=0)||(num_D1!=0)||(num_D2!=0)||(num_D3!=0))		//����к���ʱ����������
		Beep=~Beep;
	}
	if((bg_1602==0)&&(D0==fu_D0)&&(D1==fu_D1)&&(D2==fu_D2)&&(D3==fu_D3))
	{
		n++;
		if(n==300)		//�������ʱ��ʱ15��رձ���
		{
			n=0;
			bg_1602=1;
		}
	}
}