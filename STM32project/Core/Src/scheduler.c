/*
 * scheduler.c
 *
 *  Created on: Oct 26, 2025
 *      Author: Admin
 */

#include "scheduler.h"

sTasks SCH_Tasks_Array[SCH_MAX_TASKS];
uint8_t current_index_tasks;

void SCH_Init(void){
	current_index_tasks = 0;
}

void SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	SCH_Tasks_Array[current_index_tasks].pTask = pFunction;
	SCH_Tasks_Array[current_index_tasks].Delay = DELAY;
	SCH_Tasks_Array[current_index_tasks].Period = PERIOD;
	SCH_Tasks_Array[current_index_tasks].RunMe = 0;
	SCH_Tasks_Array[current_index_tasks].TaskID = current_index_tasks;

	current_index_tasks++;
}

void SCH_Update(void){
	for (int i = 0; i < current_index_tasks; i++){
		if (SCH_Tasks_Array[i].Delay > 0) SCH_Tasks_Array[i].Delay --;
		else {
			SCH_Tasks_Array[i].Delay = SCH_Tasks_Array[i].Period;
			SCH_Tasks_Array[i].RunMe ++;
		}
	}
}

void SCH_Dispatch_Tasks(void){
	for (int i = 0; i < current_index_tasks; i++)
		if (SCH_Tasks_Array[i].RunMe > 0){
			SCH_Tasks_Array[i].RunMe --;
			(*SCH_Tasks_Array[i].pTask)();
		}
}
