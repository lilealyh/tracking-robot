#include "main.h"

void vIdleTask(void *pvParameters)
{
	int counter;
	int temp = second_Counter;
	while(1)
	{
		counter++;
		if (second_Counter - temp > 3)
		{
			temp = second_Counter;
			Debug_printf("I am idle!\n");
		}
	}
}
