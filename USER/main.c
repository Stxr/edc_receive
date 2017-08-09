#include "sys.h"
#include "delay.h"
#include "string.h"
#include "Tool.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "libincludes.h"
#include <stdarg.h>
#include <stdlib.h>
#include "chinese.h"
extern LCD_POINT LCD_Point;
extern LCD_POINT LCD_POINT_DB;
extern LCD_POINT LCD_POINT_HZ;
void callbackReceive(u8 *rec);
u32 times=0;
u8 TIM3_Count=0;
u16 tt=0;
u32 temp=0;
u32 frequceny=0;
u8 buff_hz[20];
u8 buff_db[20];
int main(void){	
	int step=200;	
	delay_init();	    	   
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	
	USARTX_Init(9600);
	LED_Init();
	AD9854_Init();
	HSMP3816_Init();
	PE43503_Init();
	LCD_Show_Init();
//	menu_init();
	Key_Init();
//	Input_Init(10000,7299);//500ms
//	TIM3_Cap_Init(5000,7200-1);//10ms

	LCD_show_int(124,"hz",&LCD_POINT_HZ);
	LCD_show_int(23,"db",&LCD_POINT_DB);
	frequceny=10000000;
	AD9854_SetSine(10000000,3300);

	printf("ok!\r\n");
	while(1){
		AD9854_SetSine(frequceny+=100000,3300);
//		AD9854_SetSine(frequceny,3300);
//		frequceny+=1000;
	//	HSMP3816_Control(frequceny/100*7);
//		receiveMatch((u8*)"hello",callbackReceive);
////		printf("tim3:%d\r\n",TIM_GetCounter(TIM3));
		LED1 = !LED1;		
//		AD9854_UPDATE_LOCK=!AD9854_UPDATE_LOCK;
//		AD9854_CS=!AD9854_CS;
//		AD9854_RESET=!AD9854_RESET;
//		LCD_show_int(frequceny,"hz",&LCD_POINT_HZ);
//		frequceny+=100000;
		delay_ms(1000);
	}
}
 /*
 	回调函数
 */
void callbackReceive(u8 *rec){
	Data receive = 	afterDeal(rec);
	if(receive.argc>0){
		if(!strcmp((const char*)receive.command,"H")){ //HSMP3816 :0-3299
			HSMP3816_Control(atoi((const char*)receive.argv[0]));
			printf("HSMP3816 Received\r\n");
		}else if(!strcmp((const char*)receive.command,"P")){//PE43503  0-31;
			PE43503_Send(atoi((const char*)receive.argv[0]));
			printf("PE43503 Received\r\n");
		}else if(!strcmp((const char*)receive.command,"A")){//AD9854 
			if(receive.argc>=2){
				AD9854_SetSine(atoi((const char*)receive.argv[0]),atoi((const char*)receive.argv[1]));
				printf("AD9854 Received\r\n");
			}
		}
	}
//	delay_ms(1000);
//	printf("command:%s\r\n",receive.command);
//	delay_ms(1000);
//	for(int i=0;i<receive.argc;i++){
//		printf("argv[%d]: %s\r\n",i,receive.argv[i]);
//		delay_ms(1000);
//	}
//	printf("argc:%d\r\n",receive.argc);
//	delay_ms(1000);
}

void EXTI15_10_IRQHandler(void)
{
	LED3 =!LED3;
	times++;
	EXTI_ClearITPendingBit(EXTI_Line15);  
}

void TIM3_IRQHandler(void)   
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
	{
		TIM3_Count++;
		LED4=!LED4;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  
	}
}
	 
void TIM5_IRQHandler(void)
{ 
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) 
	{
		EXTI->IMR&=0XF7FFF;//关15脚中断
		LED2=!LED2;
//		printf("F:%dHZ\r\n",times);
		times=0;
		EXTI->IMR|=1<<15;
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update); 
	}
}
/*
误差补偿: 8s           1.00609
					0.5s         1.04301

*/
void TIM4_IRQHandler(void)   //TIM3
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) 
	{
		TIM_Cmd(TIM3, DISABLE); 
		frequceny = (int)(TIM3->CNT+TIM3_Count*65536)*16*1.04301;
//		printf("tim3->cnt:%dHZ\r\n",frequceny); 
		TIM3->CNT=0;
		TIM3_Count=0;
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  
		TIM_Cmd(TIM3, ENABLE);
	}
}
