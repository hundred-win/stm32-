#include "stm32f10x.h"
#include "remote.h"
#include "sys.h"

void remote_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//�õ�PA1����ʱ����ͨ����
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);	

	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//һ��Ƶ���ǲ���Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 10000-1;		//0��1.125ms,1��2.25ms,ͬ������9ms�ĵͣ�4.5ms�ĸߣ�������һ������Ϊ10ms�ܺ���
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72-1;//����Ϊ��Ƶ����period,�ٳ���Ԥ��Ƶ��prescaler��
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_ICInitTypeDef TIM_ICInitStructure;
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;//ѡ��ͨ����
	TIM_ICInitStructure.TIM_ICFilter = 0x03;//�˲������ɼ����ε�ƽ�źţ���Ϊͬһ����ִ��
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//�����ز���
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;//��Ҫ��Ƶ
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//10.20Ҫע��Ѳ�����ȫ��һ��ʼ��û���䵽�����
	TIM_ICInit(TIM2, &TIM_ICInitStructure);	
	
	TIM_Cmd(TIM2, ENABLE);
	//�����ʵ���϶�ʱ�������벶��Ϳ��ƺ��ˣ�����������Ҫ�����жϣ����жϵ�����������ܹ�ȥ��ʱ�ش���ʱ����ʱ���˻��߳��������
	//�������жϵ��������Ȼ������Ҫ��ʼ���ж���������
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM2, TIM_IT_Update|TIM_IT_CC2, ENABLE);//�����жϣ�������������ж�
}
//����һ������������ݽṹ
//ң��������״̬RmtSta
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������								   
//[3:0]:�����ʱ��
u8 	RmtSta=0;	  	  
u16 Dval;		//�½���ʱ��������ֵ
u32 RmtRec=0;	//������յ�������	   		    
u8  RmtCnt=0;	//�������µĴ���,ʵ���Ͼ���������Ĵ���
void TIM2_IRQHandler(void)						//��дһ���ܵ��жϴ�������.s�ļ���ת���������Ҳ��.s�ļ���ֱ�Ӹ��ƣ���ô��
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)//�������룬�ϴ������ݱ����յ���
		{	
			RmtSta&=~0X10;						//ȡ���������Ѿ���������
			if((RmtSta&0X0F)==0X00)RmtSta|=1<<6;//��һ�ν������������Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ���һ�μ��Ľ���
			if((RmtSta&0X0F)<14)RmtSta++;//---------------------------------------------------------û�������룬���13��
			else
			{
				RmtSta&=~(1<<7);//���������ʶ
				RmtSta&=0XF0;	//��ռ�����	
			}						 	   	
		}							    
	}
	if(TIM_GetITStatus(TIM2, TIM_IT_CC2)!=RESET)//		�����жϣ�����ȥ�ж���ʲô���͵��жϣ�,ȥ���ߵ�ƽ���ʱ��
	{
		if(RDATA)//�������жϣ�10.18�磬����ʱ��û���λ��˼�룬�ȰѴ˴���д���˻ع�ͷ��⣩
		{
			//��ʱ�����㣬�ı�Ϊ�����½��أ���־λ����
			TIM_SetCounter(TIM2, 0);
			TIM_OC2PolarityConfig(TIM2, TIM_ICPolarity_Falling);//����ѧ��һ�У�Ҫ�л�Ϊ�½��ز��񣬲�����Ҫ��OCX���Ը��ģ�����Ҫ������д�����_Falling
			RmtSta |= 0x10;
		}
		else{//�������½����ж�
  			 Dval=TIM_GetCapture2(TIM2);//��ʵ�����õ���ʱ
			 TIM_OC2PolarityConfig(TIM2,TIM_ICPolarity_Rising); //	����Ϊ�����ز���,Ϊ��һ����׼��
 			
			if(RmtSta&0X10)					//���һ�θߵ�ƽ���񣬼����û�������ز��� 
			{
 				if(RmtSta&0X80)//���յ���������
				{
					//-------------------------------------------------------------------���ݸߵ�ƽ����ʱ����ȷ������һ����
					if(Dval>300&&Dval<800)			//560Ϊ��׼ֵ,560us
					{
						RmtRec<<=1;	//����һλ.    �Ƚ��յ�����ǰ
						RmtRec|=0;	//���յ�0	   
					}else if(Dval>1400&&Dval<1800)	//1680Ϊ��׼ֵ,1680us
					{
						RmtRec<<=1;	//����һλ.
						RmtRec|=1;	//���յ�1
					}else if(Dval>2200&&Dval<2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms��������----------------������������Ҫ�ٿ�һ��
					{
						RmtCnt++; 		//������������1��
						RmtSta&=0XF0;	//��ռ�ʱ��		
					}
 				}else if(Dval>4200&&Dval<4700)		//4500Ϊ��׼ֵ4.5ms��û�о�ȥȡ������
				{
					RmtSta|=1<<7;	//��ǳɹ����յ���������
					RmtCnt=0;		//�����������������
				}						 
			}
			RmtSta&=~(1<<4);//λ4��0���������ر�Ǹ��ˣ���ʾ���β���Ľ���
		}
	}				 		     	    					   

	TIM_ClearFlag(TIM2, TIM_FLAG_Update|TIM_FLAG_CC2);
}


u8 telecontrol(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
	{ 
	    t1=RmtRec>>24;			//�õ���ַ��
	    t2=(RmtRec>>16)&0xff;	//�õ���ַ���� 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//����ң��ʶ����(ID)����ַ REMOTE_ID����0��ͷ�ļ����壩����Ϊң�����û��뼰����Ϊ00FF
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//��ֵ��ȷ	 
		}   
		if((sta==0)||((RmtSta&0X80)==0))//�������ݴ���(��ֵΪ0��û����һ���sta=t1)/ң���Ѿ�û�а�����
		{
		 	RmtSta&=~(1<<6);//������յ���Ч������ʶ
			RmtCnt=0;		//�����������������
		}
	}  
    return sta;
}
 









