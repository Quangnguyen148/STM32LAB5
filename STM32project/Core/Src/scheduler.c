/*
 * scheduler.c
 *
 *  Created on: Oct 26, 2025
 *      Author: Admin
 */

#include "scheduler.h"

/*unsigned char Error_code_G = 0;*/
static uint32_t newTaskID = 0;
static uint32_t Get_New_Task_ID(void);

uint32_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	uint8_t cur_idx = 0;
	while (cur_idx < SCH_MAX_TASKS && SCH_Tasks_Array[cur_idx].pTask != 0 && DELAY >= SCH_Tasks_Array[cur_idx].Delay){
		DELAY = DELAY - SCH_Tasks_Array[cur_idx].Delay;
		cur_idx++;
	}
	uint8_t end = cur_idx;
	while (end < SCH_MAX_TASKS && SCH_Tasks_Array[end].pTask != 0) end++;
	if (end == SCH_MAX_TASKS) return 0xFF;
	for (uint8_t i = end; i > cur_idx; i--)
		SCH_Tasks_Array[i] = SCH_Tasks_Array[i - 1];
	SCH_Tasks_Array[cur_idx].pTask = pFunction;
	SCH_Tasks_Array[cur_idx].Delay = DELAY;
	SCH_Tasks_Array[cur_idx].Period = PERIOD;
	if(SCH_Tasks_Array[cur_idx].Delay == 0)
		SCH_Tasks_Array[cur_idx].RunMe = 1;
	else SCH_Tasks_Array[cur_idx].RunMe = 0;
	SCH_Tasks_Array[cur_idx].TaskID = Get_New_Task_ID();
	if (cur_idx < end) SCH_Tasks_Array[cur_idx + 1].Delay -= DELAY;
	return SCH_Tasks_Array[cur_idx].TaskID;
}

uint32_t SCH_Add_Task_KeepID(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD, uint32_t TASK_ID){
    uint8_t cur_idx = 0;
    while (cur_idx < SCH_MAX_TASKS && SCH_Tasks_Array[cur_idx].pTask != 0 && DELAY >= SCH_Tasks_Array[cur_idx].Delay){
    	DELAY = DELAY - SCH_Tasks_Array[cur_idx].Delay;
        cur_idx++;
    }
    uint8_t end = cur_idx;
    while (end < SCH_MAX_TASKS && SCH_Tasks_Array[end].pTask != 0) end++;
    if (end == SCH_MAX_TASKS) return 0xFF;
    for (uint8_t i = end; i > cur_idx; i--)
        SCH_Tasks_Array[i] = SCH_Tasks_Array[i - 1];
    SCH_Tasks_Array[cur_idx].pTask   = pFunction;
    SCH_Tasks_Array[cur_idx].Delay   = DELAY;
    SCH_Tasks_Array[cur_idx].Period  = PERIOD;
	if(SCH_Tasks_Array[cur_idx].Delay == 0)
		SCH_Tasks_Array[cur_idx].RunMe = 1;
	else SCH_Tasks_Array[cur_idx].RunMe = 0;
    SCH_Tasks_Array[cur_idx].TaskID  = TASK_ID;
    if (cur_idx < end) SCH_Tasks_Array[cur_idx + 1].Delay -= DELAY;
    return SCH_Tasks_Array[cur_idx].TaskID;
}


uint8_t SCH_Delete_Task(uint32_t taskID){
	uint8_t Return_code = 0;
	uint8_t deleted_task;
	if (taskID != NO_TASK_ID){
		for (deleted_task = 0; deleted_task < SCH_MAX_TASKS; deleted_task++){
			if (SCH_Tasks_Array[deleted_task].TaskID == taskID){
				Return_code = 1;
				if (deleted_task < SCH_MAX_TASKS - 1 && SCH_Tasks_Array[deleted_task+1].pTask != 0)
					SCH_Tasks_Array[deleted_task+1].Delay = SCH_Tasks_Array[deleted_task+1].Delay + SCH_Tasks_Array[deleted_task].Delay;

				uint8_t task_idx = deleted_task;
				while (task_idx + 1 < SCH_MAX_TASKS && SCH_Tasks_Array[task_idx + 1].pTask != 0) {
					SCH_Tasks_Array[task_idx] = SCH_Tasks_Array[task_idx + 1];
					task_idx++;
				}
				SCH_Tasks_Array[task_idx].pTask  = 0;
				SCH_Tasks_Array[task_idx].Delay  = 0;
				SCH_Tasks_Array[task_idx].Period = 0;
				SCH_Tasks_Array[task_idx].RunMe  = 0;
				SCH_Tasks_Array[task_idx].TaskID = NO_TASK_ID;
				return Return_code;
			}
		}
	}
	return Return_code;
}

void SCH_Init(void){
    for (uint8_t i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_Tasks_Array[i].pTask  = 0;
        SCH_Tasks_Array[i].Delay  = 0;
        SCH_Tasks_Array[i].Period = 0;
        SCH_Tasks_Array[i].RunMe  = 0;
        SCH_Tasks_Array[i].TaskID = NO_TASK_ID;
    }
}

//need to fix: phai chay dong thoi tat ca cac task dang co flag runme > 0
void SCH_Update(void) {
	if (SCH_Tasks_Array[0].pTask != 0 && SCH_Tasks_Array[0].RunMe == 0) {
		if(SCH_Tasks_Array[0].Delay > 0){
			SCH_Tasks_Array[0].Delay = SCH_Tasks_Array[0].Delay - 1;
		}
		else{
			uint8_t idx = 0;
			while (idx < SCH_MAX_TASKS && SCH_Tasks_Array[idx].pTask != 0 && SCH_Tasks_Array[idx].Delay == 0) {
			            SCH_Tasks_Array[idx].RunMe = 1;
			            idx++;
			        }
		}
	}
}

void SCH_Dispatch_Tasks(void) {
/*	if(SCH_Tasks_Array[0].RunMe > 0) {
		(*SCH_Tasks_Array[0].pTask)();
		SCH_Tasks_Array[0].RunMe = 0;
		sTasks temtask = SCH_Tasks_Array[0];
		SCH_Delete_Task(temtask.TaskID);
		if (temtask.Period != 0) {
			//need to fix: luu lai taskID cu khi add lai task
			SCH_Add_Task(temtask.pTask, temtask.Period, temtask.Period); //goi nhu vay la da add taskID moi
		}
	}*/
    if (SCH_Tasks_Array[0].RunMe > 0){
    	sTasks temtask = SCH_Tasks_Array[0];
        SCH_Tasks_Array[0].RunMe = 0;
    	(*temtask.pTask)();
        SCH_Delete_Task(temtask.TaskID);
        if (temtask.Period > 0)
            SCH_Add_Task_KeepID(temtask.pTask, temtask.Period, temtask.Period, temtask.TaskID);
    }
}

static uint32_t Get_New_Task_ID(void){
	newTaskID++;
	if(newTaskID == NO_TASK_ID){
		newTaskID++;
	}
	return newTaskID;
}
