#include "stm32f10x.h"
#include "remote.h"
#include "sys.h"

void remote_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//用的PA1，定时器五通道二
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);	

	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//一分频就是不分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10000-1;		//0是1.125ms,1是2.25ms,同步码是9ms的低，4.5ms的高，所以让一个周期为10ms很合理
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;//周期为主频除以period,再除以预分频（prescaler）
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;//选择通道二
	TIM_ICInitStructure.TIM_ICFilter = 0x03;//滤波器，采集几次电平信号，都为同一个就执行
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//上升沿捕获
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;//不要分频
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//10.20要注意把参数配全，一开始就没有配到这个参
	TIM_ICInit(TIM2, &TIM_ICInitStructure);	
	
	TIM_Cmd(TIM2, ENABLE);
	//到这里，实际上定时器、输入捕获就控制好了，接下来就是要设置中断，而中断的意义就在于能够去及时地处理定时器计时到了或者超出的情况
	//首先用中断的情况，必然是首先要初始化中断向量管理
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM2, TIM_IT_Update|TIM_IT_CC2, ENABLE);//更新中断（溢出），捕获中断
}
//定义一个很巧妙的数据结构
//遥控器接收状态RmtSta
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获								   
//[3:0]:溢出计时器
u8 	RmtSta=0;	  	  
u16 Dval;		//下降沿时计数器的值
u32 RmtRec=0;	//红外接收到的数据	   		    
u8  RmtCnt=0;	//按键按下的次数,实际上就是连发码的次数
void TIM2_IRQHandler(void)						//先写一个总的中断处理，便于.s文件跳转，名字最好也从.s文件里直接复制，免得打错
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)//有引导码，上次有数据被接收到了
		{	
			RmtSta&=~0X10;						//取消上升沿已经被捕获标记
			if((RmtSta&0X0F)==0X00)RmtSta|=1<<6;//第一次进入溢出，标记已经完成一次按键的键值信息采集，一次键的结束
			if((RmtSta&0X0F)<14)RmtSta++;//---------------------------------------------------------没有连发码，溢出13次
			else
			{
				RmtSta&=~(1<<7);//清空引导标识
				RmtSta&=0XF0;	//清空计数器	
			}						 	   	
		}							    
	}
	if(TIM_GetITStatus(TIM2, TIM_IT_CC2)!=RESET)//		捕获中断（具体去判断是什么类型的中断）,去检测高电平间的时间
	{
		if(RDATA)//上升沿中断（10.18早，我暂时还没理解位带思想，等把此代码写完了回过头理解）
		{
			//计时器清零，改变为捕获下降沿，标志位更改
			TIM_SetCounter(TIM2, 0);
			TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Falling);//必须学的一招，要切换为下降沿捕获，不仅需要调OCX极性更改，还需要单独地写入参数_Falling
			RmtSta |= 0x10;
		}
		else{//捕获到了下降沿中断
  			 Dval=TIM_GetCapture2(TIM2);//其实就是拿到计时
			 TIM_OC2PolarityConfig(TIM2,TIM_ICPolarity_Rising); //	设置为上升沿捕获,为下一次作准备
 			
			if(RmtSta&0X10)					//完成一次高电平捕获，检查有没有上升沿捕获 
			{
 				if(RmtSta&0X80)//接收到了引导码
				{
					//-------------------------------------------------------------------根据高电平持续时间来确定是哪一种码
					if(Dval>300&&Dval<800)			//560为标准值,560us
					{
						RmtRec<<=1;	//左移一位.    先接收到的在前
						RmtRec|=0;	//接收到0	   
					}else if(Dval>1400&&Dval<1800)	//1680为标准值,1680us
					{
						RmtRec<<=1;	//左移一位.
						RmtRec|=1;	//接收到1
					}else if(Dval>2200&&Dval<2600)	//得到按键键值增加的信息 2500为标准值2.5ms，连发码----------------连发码这里需要再看一看
					{
						RmtCnt++; 		//按键次数增加1次
						RmtSta&=0XF0;	//清空计时器		
					}
 				}else if(Dval>4200&&Dval<4700)		//4500为标准值4.5ms，没有就去取引导码
				{
					RmtSta|=1<<7;	//标记成功接收到了引导码
					RmtCnt=0;		//清除按键次数计数器
				}						 
			}
			RmtSta&=~(1<<4);//位4置0，把上升沿标记改了，表示本次捕获的结束
		}
	}				 		     	    					   

	TIM_ClearFlag(TIM2, TIM_FLAG_Update|TIM_FLAG_CC2);
}


u8 telecontrol(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//得到一个按键的所有信息了
	{ 
	    t1=RmtRec>>24;			//得到地址码
	    t2=(RmtRec>>16)&0xff;	//得到地址反码 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//检验遥控识别码(ID)及地址 REMOTE_ID就是0（头文件定义），因为遥控器用户码及反码为00FF
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//键值正确	 
		}   
		if((sta==0)||((RmtSta&0X80)==0))//按键数据错误(初值为0，没走上一层的sta=t1)/遥控已经没有按下了
		{
		 	RmtSta&=~(1<<6);//清除接收到有效按键标识
			RmtCnt=0;		//清除按键次数计数器
		}
	}  
    return sta;
}
 









