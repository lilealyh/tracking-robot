#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#include "main.h"






//Position t = {40.0,50.0,60.0};

int HaveData = 0;
void vTraceForecastTask(void *pvParameters) 
{


	int totol_num = 0;
	int MyHaveData = 0;
	Position SeenPos;
	time_pos Observ;

	Pred out_predict[FORECAST_LEN];

	int flag = 0,j,preFlag = 0;

	for(;;)	
	{
		xSemaphoreTake(TimerxSyn, portMAX_DELAY);
		//time_cache = time_cache + 1;
		
		
		
		
		Result = AssignResult[OrderIndex[SELFADDRESS]];
		Debug_printf("Result: %d\r\n",Result);
		
		
		/*Determine whether there is data passed down*/

		if(Cache1[Result].x >= (float)(0.03) ||  Cache1[Result].y >= (float)(0.03) || Cache2[Result].x >= (float)(0.03) ||  Cache2[Result].y >= (float)(0.03))
		{
			MyHaveData = 1;
			Debug_printf("have data\r\n");
			if(abs(T_TIMER - Cache1Time) > abs(T_TIMER - Cache2Time))
			{
				Observ.x = Cache2[Result].x;
				Observ.y = Cache2[Result].y;
				Observ.angle = Cache2[Result].Angle;
				Observ.time = Cache2Time;
			}
			else
			{
				Observ.x = Cache1[Result].x;
				Observ.y = Cache1[Result].y;
				Observ.angle = Cache1[Result].Angle;
				Observ.time = Cache1Time;
			}
		}
		else
		{
			MyHaveData = 0;
			Debug_printf("no data\r\n");
		}
		
		

		/*Do part of the decision*/
		if(MyHaveData == 0 && preFlag == 0)//û����Ҳû�����˲�
		{
			Debug_printf("0 0\r\n");
			HaveData = 0;
			totol_num = 0;
		}
		else if(MyHaveData == 1 && preFlag == 0)//���洫�������ݣ�����û�����˲�
		{
			HaveData = 1;
			SeenPos.Angle = Observ.angle;
			SeenPos.x = Observ.x;
			SeenPos.y = Observ.y;
			printf("SeenPos: %f %f %f\r\n",SeenPos.Angle,SeenPos.x,SeenPos.y);
			Debug_printf("1 0\r\n");
			if(GlobleCurProcedure.which_step != seenTarget)
			{
				Debug_printf("I see you\r\n");
				
				GlobleCurProcedure.which_step = seenTarget;
				GlobleCurProcedure.pvParameter.pos = SeenPos;
				
				if (pdTRUE == xSemaphoreTake(MoveStopEnable, 10))
				{
					Debug_printf("needStop\r\n");
					USART2->SR;
					USART_SendData(UART4, '!');
					xSemaphoreGive(MoveStopEnable);
					
					xSemaphoreGive(FoundTargetSyn);					//??vMovingControlTask??,????????
				}
				
	//			vTaskDelay(100);
			}
			GlobleCurProcedure.which_step = seenTarget;
			GlobleCurProcedure.pvParameter.pos = SeenPos;
			
			
			totol_num = 0;
		}
		else if(MyHaveData == 0 && preFlag == 1)   //����û�������������Ѿ�����kalman�˲�
		{
			Debug_printf("0 1\r\n");
			printf("SeenPos: %f %f %f\r\n",SeenPos.Angle,SeenPos.x,SeenPos.y);
			Observ.x = out_predict[0].x;										//�ƺ�������
			Observ.y = out_predict[0].y;
			Observ.time = out_predict[0].time;
			
			
			HaveData = 1;
			SeenPos.Angle = Observ.angle;
			SeenPos.x = Observ.x;
			SeenPos.y = Observ.y;
			
			if(GlobleCurProcedure.which_step != seenTarget)
			{
				Debug_printf("I see you\r\n");
				
				GlobleCurProcedure.which_step = seenTarget;
				GlobleCurProcedure.pvParameter.pos = SeenPos;
			
				if (pdTRUE == xSemaphoreTake(MoveStopEnable, 10))
				{
					Debug_printf("needStop\r\n");
					USART2->SR;
					USART_SendData(UART4, '!');
					xSemaphoreGive(MoveStopEnable);
					
					xSemaphoreGive(FoundTargetSyn);					//??vMovingControlTask??,????????
				}
	//			vTaskDelay(100);
			}
			GlobleCurProcedure.which_step = seenTarget;
			GlobleCurProcedure.pvParameter.pos = SeenPos;
			
			totol_num++;
			if(totol_num >= FORECAST_LEN)
			{
				HaveData = 0;
				totol_num = 0;
				GlobleCurProcedure.which_step = looseTarget;
				
			}
			
			State_prediction(&Observ,out_predict,&flag);
			preFlag = flag;
		}
		else																				//���������������Ѿ�����kalman�˲�
		{
			Debug_printf("1 1\r\n");
			HaveData = 1;
			SeenPos.Angle = Observ.angle;
			SeenPos.x = Observ.x;
			SeenPos.y = Observ.y;
			
			if(GlobleCurProcedure.which_step != seenTarget)
			{
				Debug_printf("I see you\r\n");
				
				GlobleCurProcedure.which_step = seenTarget;
				GlobleCurProcedure.pvParameter.pos = SeenPos;
				
				if (pdTRUE == xSemaphoreTake(MoveStopEnable, 10))
				{
					Debug_printf("needStop\r\n");
					USART2->SR;
					USART_SendData(UART4, '!');
					xSemaphoreGive(MoveStopEnable);
					
					xSemaphoreGive(FoundTargetSyn);					//??vMovingControlTask??,????????
				}

	//			vTaskDelay(100);
			}
			GlobleCurProcedure.which_step = seenTarget;
				GlobleCurProcedure.pvParameter.pos = SeenPos;
			
			
			State_prediction(&Observ,out_predict,&flag);
			preFlag = flag;
		}
		
		xSemaphoreGive(ComunicationWithTraceSyn);
	}	
		

		
	
}


