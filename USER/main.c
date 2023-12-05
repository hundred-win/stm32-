#include "stm32f10x.h"
#include "remote.h"
#include "OLED.h"
#include "pwm.h"
#include "Delay.h"
#include "iravoid.h"
#include "microsonic.h"
#include "turn.h"
#include "blackfollwer.h"
#include "bluetooth.h"
#include <string.h>

u8 key = 0;
int state_flag = 0;

//u8 reclen = 0;
int blue_flag = 0;

void all_init()
{
	remote_init();
	pwm_init();
	microsonic_init();
	turn_init();//有问题的一行
	IRAvoid_Init();
	blackfollower_init();
	usart_init();
	tim3_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

}

void tele_command()
{
	key = telecontrol();
	//key = 168;
		//OLED_ShowNum(1,1,key,3);
		switch(key)
		{
			case  98:
			{
				if(state_flag == 5)
				{
					car_run(100);
				}
				if(state_flag < 5)
				{
					car_run(65);
					delay_ms(100);//减少误差
					state_flag ++;
				}
				break;
			}
			case 168:
			{
				if(state_flag == -5)
				{
					car_back(100);
				}
				if(state_flag > -5)
				{
					car_back(65);
					delay_ms(100);
					state_flag --;
				}
				break;
			}
			case   2:	
			{
				state_flag = 0;
				car_brake();
				break;
			}
			
			case  34:
			{				
				if(state_flag == 0)
				{
					car_left(100);
					delay_ms(20);
					car_brake();
				}
				if((state_flag <= 5)&&(state_flag>0))
				{
					car_left(100);
					while(telecontrol()==34);
					car_run(65);
				}
				if((state_flag >= -5)&&(state_flag<0))
				{
					car_left(100);
					while(telecontrol()==34);
					car_back(65);
				}
				break;
			}
			case 194:	
			{
				if(state_flag == 0)
				{
					car_right(100);
					delay_ms(20);
					car_brake();
				}
				if((state_flag <= 5)&&(state_flag>0))
				{
					car_right(100);
					while(telecontrol()==34);
					car_run(65);
				}
				if((state_flag >= -5)&&(state_flag<0))
				{
					car_right(100);
					while(telecontrol()==34);
					car_back(65);
				}
				break;
			}
				
			case 224:
			{				
				car_spin_left(100);
				while((telecontrol()==224)||(telecontrol()==0));
				break;
			}
			case 144:	
			{
				car_spin_right(100);
				while((telecontrol()==144)||(telecontrol()==0));
				break;
			}
			
//特殊功能按键设置			
			case F_IRA:
			{
				while(telecontrol()!=F_SP)
				{
					iravoid_check();
				}
				car_brake();
				delay_ms(100);
				break;
			}
			case F_MIC:
			{
				while(telecontrol()!=F_SP)
				{
					microsonic_check();
				}
				car_brake();
				delay_ms(100);
				break;
			}
			case F_MIX:
			{
				while(telecontrol()!=F_SP)
				{
					iravoid_check();
					microsonic_check();
				}
				car_brake();
				delay_ms(100);
				break;
			}
			case F_BLF:
			{
				while(telecontrol()!=F_SP)
				{
					black_follow_all();
				}
				car_brake();
				delay_ms(100);
				break;
			}
		}
}

void blue_command()
{
	if(USART2_RX_STA&0x8000)
		{
			reclen = (USART2_RX_STA & 0x7FFF);//得到长度主要两个用处，一是方便看出是不是我们需求的长度。二是方便将该字符置为\0（字符串比较需求）
			USART2_RX_BUF[reclen] = 0;
			if((reclen==3)||(reclen==4))
			{
				if(strcmp((const char*)USART2_RX_BUF, "ONA")==0)//实话说，这个const char*也不是很理解
				{	if(blue_flag == 1)
					{
						car_run(100);
					}
					if(blue_flag == 0)
					{
						car_run(65);
						delay_ms(100);//减少误差
						blue_flag ++;
					}
					USART2_RX_STA = 0;
				}
				else if(strcmp((const char*)USART2_RX_BUF, "ONB")==0)
				{	if(blue_flag == -1)
					{
						car_back(100);
					}
					if(blue_flag == 0)
					{
						car_back(65);
						delay_ms(100);//减少误差
						blue_flag --;
					}
					USART2_RX_STA = 0;
				}
				else if(strcmp((const char*)USART2_RX_BUF, "ONC")==0)
				{
					if(blue_flag == 0)
					{
						car_left(100);
						//delay_ms(20);
						//car_brake();
						USART2_RX_STA = 0;
						reclen = (USART2_RX_STA & 0x7FFF);
						USART2_RX_BUF[reclen] = 0;
						while(strcmp((const char*)USART2_RX_BUF, "ONF")!=0);
						car_brake();
					}
					if(blue_flag == 1)//if((blue_flag <= 5)&&(blue_flag>0))
					{
						car_left(100);
						USART2_RX_STA = 0;
						reclen = (USART2_RX_STA & 0x7FFF);
						USART2_RX_BUF[reclen] = 0;
						while(strcmp((const char*)USART2_RX_BUF, "ONF")!=0);//识别不到ONF,就困在循环
						car_run(65);
					}
					if(blue_flag == -1)//if((blue_flag >= -5)&&(blue_flag<0))
					{
						car_left(100);
						USART2_RX_STA = 0;
						reclen = (USART2_RX_STA & 0x7FFF);
						USART2_RX_BUF[reclen] = 0;
						while(strcmp((const char*)USART2_RX_BUF, "ONF")!=0);
						car_back(65);
					}
					USART2_RX_STA = 0;
				}
				else if(strcmp((const char*)USART2_RX_BUF, "OND")==0)
				{
					if(blue_flag == 0)
					{
						car_right(100);
						USART2_RX_STA = 0;
						reclen = (USART2_RX_STA & 0x7FFF);
						USART2_RX_BUF[reclen] = 0;
						while(strcmp((const char*)USART2_RX_BUF, "ONF")!=0);
						car_brake();
					}
					if(blue_flag == 1)//if((blue_flag <= 5)&&(blue_flag>0))
					{
						car_right(100);
						USART2_RX_STA = 0;
						reclen = (USART2_RX_STA & 0x7FFF);
						USART2_RX_BUF[reclen] = 0;
						while(strcmp((const char*)USART2_RX_BUF, "ONF")!=0);
						car_run(65);
					}
					if(blue_flag == -1)//if((blue_flag >= -5)&&(blue_flag<0))
					{
						car_right(100);
						USART2_RX_STA = 0;
						reclen = (USART2_RX_STA & 0x7FFF);
						USART2_RX_BUF[reclen] = 0;
						while(strcmp((const char*)USART2_RX_BUF, "ONF")!=0);
						car_back(65);
					}
					USART2_RX_STA = 0;
				}
				else if(strcmp((const char*)USART2_RX_BUF, "ONI")==0)//右下角的那个键
				{
					car_brake();
					blue_flag = 0;
					USART2_RX_STA = 0;
				}
				else if(strcmp((const char*)USART2_RX_BUF, "ONG")==0)
				{
					car_spin_left(100);
					USART2_RX_STA = 0;
				}
				else if(strcmp((const char*)USART2_RX_BUF, "ONH")==0)
				{
					car_spin_right(100);
					USART2_RX_STA = 0;
				}
				
//特殊按键处理
				else if(strcmp((const char*)USART2_RX_BUF, "ON1")==0)
				{
						USART2_RX_STA = 0;
						reclen = (USART2_RX_STA & 0x7FFF);
						USART2_RX_BUF[reclen] = 0;
					while(strcmp((const char*)USART2_RX_BUF, "ONI")!=0)
					{
						USART2_RX_STA = 0;
						reclen = (USART2_RX_STA & 0x7FFF);
						USART2_RX_BUF[reclen] = 0;
						iravoid_check();
					}
					USART2_RX_STA = 0;
					car_brake();
				}
				else if(strcmp((const char*)USART2_RX_BUF, "ON2")==0)
				{
						USART2_RX_STA = 0;
						reclen = (USART2_RX_STA & 0x7FFF);
						USART2_RX_BUF[reclen] = 0;
					while(strcmp((const char*)USART2_RX_BUF, "ONI")!=0)
					{
						USART2_RX_STA = 0;
						reclen = (USART2_RX_STA & 0x7FFF);
						USART2_RX_BUF[reclen] = 0;
						microsonic_check();
						
					}
					USART2_RX_STA = 0;
					car_brake();
				}
				else if(strcmp((const char*)USART2_RX_BUF, "ON3")==0)
				{
						USART2_RX_STA = 0;
						reclen = (USART2_RX_STA & 0x7FFF);
						USART2_RX_BUF[reclen] = 0;
					while(strcmp((const char*)USART2_RX_BUF, "ONI")!=0)
					{
						USART2_RX_STA = 0;
						reclen = (USART2_RX_STA & 0x7FFF);
						USART2_RX_BUF[reclen] = 0;
						iravoid_check();
						microsonic_check();
					}
					USART2_RX_STA = 0;
					car_brake();
				}
				else if(strcmp((const char*)USART2_RX_BUF, "ON4")==0)
				{
						USART2_RX_STA = 0;
						reclen = (USART2_RX_STA & 0x7FFF);
						USART2_RX_BUF[reclen] = 0;
					while(telecontrol()!=F_SP)
					{
						
						black_follow_all();
						
					}
					USART2_RX_STA = 0;
					car_brake();
				}
//				char* buffer = (char*)USART2_RX_BUF;
//				OLED_ShowString(2,1,buffer);
			}
			USART2_RX_STA = 0;//果然是在main中清零的
		}
		
}


int main()
{
	all_init();
	car_brake();
	delay_ms(100);

	while(1)
	{
		tele_command();
		blue_command();
	}
}

