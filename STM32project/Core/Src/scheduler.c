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


/*void SCH_Init ( void ) {
	unsigned char i ;
	for (i = 0; i < SCH_MAX_TASKS; i++)
		SCH_Delete_Task(i);
	Error_code_G = 0 ;
	Timer_init();
	Watchdog_init();
}*/

/*void SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	SCH_Tasks_Array[current_index_tasks].pTask = pFunction;
	SCH_Tasks_Array[current_index_tasks].Delay = DELAY;
	SCH_Tasks_Array[current_index_tasks].Period = PERIOD;
	SCH_Tasks_Array[current_index_tasks].RunMe = 0;
	SCH_Tasks_Array[current_index_tasks].TaskID = current_index_tasks;

	current_index_tasks++;
}

unsigned char SCH_Add_Task(void (* pFunction)(), unsigned int DELAY, unsigned int PERIOD)
{
    unsigned char Index = 0;
    // First find a gap in the array (if there is one)
    while ((SCH_tasks_G[Index].pTask != 0) && (Index < SCH_MAX_TASKS))
    {
       Index++;
    }
    // Have we reached the end of the list?
    if (Index == SCH_MAX_TASKS)
    {
        // Task list is full
        // Set the global error variable
        Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
        // Also return an error code
        return SCH_MAX_TASKS;
    }
    // If we're here, there is a space in the task array
    SCH_tasks_G[Index].pTask = pFunction;
    SCH_tasks_G[Index].Delay = DELAY;
    SCH_tasks_G[Index].Period = PERIOD;
    SCH_tasks_G[Index].RunMe = 0;
    // return position of task (to allow later deletion)
    return Index;
}*/

/*uint8_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {
    int16_t slot = find_free_slot();
    if (slot < 0) return NO_TASK_ID;
	uint8_t index = 0;
	while((SCH_Tasks_Array[index].pTask != 0) && (index < SCH_MAX_TASKS))
		index++;
	if(index < 0) return NO_TASK_ID;
    //uint8_t idx = (uint8_t)slot;
    sTasks *t = &SCH_Tasks_Array[index];
    t->pTask  = pFunction;
    t->Period = PERIOD;
    t->RunMe  = 0;
    //t->in_use = 1;

    insert(index, DELAY);
    return index;
}*/

/*void SCH_Update(void){
	for (int i = 0; i < current_index_tasks; i++){
		if (SCH_Tasks_Array[i].Delay > 0) SCH_Tasks_Array[i].Delay --;
		else {
			SCH_Tasks_Array[i].Delay = SCH_Tasks_Array[i].Period;
			SCH_Tasks_Array[i].RunMe ++;
		}
	}
}

void SCH_Update(void){
    unsigned char Index;
    // NOTE: calculations are in *TICKS* (not milliseconds)
    for (Index = 0; Index < SCH_MAX_TASKS; Index++) {
        // Check if there is a task at this location
        if (SCH_tasks_G[Index].pTask){
            if (SCH_tasks_G[Index].Delay == 0) {
                // The task is due to run
                // Inc. the 'RunMe' flag
                SCH_tasks_G[Index].RunMe += 1;
                if (SCH_tasks_G[Index].Period) {
                    // Schedule periodic tasks to run again
                    SCH_tasks_G[Index].Delay = SCH_tasks_G[Index].Period;
                }
            } else {
                // Not yet ready to run: just decrement the delay
                SCH_tasks_G[Index].Delay -= 1;
            }
        }
    }
}*/

void SCH_Update(void) {
	if (SCH_Tasks_Array[0].pTask != 0 && SCH_Tasks_Array[0].RunMe == 0) {
		if(SCH_Tasks_Array[0].Delay > 0){
			SCH_Tasks_Array[0].Delay = SCH_Tasks_Array[0].Delay - 1;
		}
		if (SCH_Tasks_Array[0].Delay == 0) {
			SCH_Tasks_Array[0].RunMe = 1;
		}
	}
}

/*void SCH_Dispatch_Tasks(void){
	uint8_t i = 0;
    while (i < current_index_tasks){
        if (SCH_Tasks_Array[i].RunMe > 0){
            SCH_Tasks_Array[i].RunMe--;
            (*SCH_Tasks_Array[i].pTask)();
            if (SCH_Tasks_Array[i].Period == 0){
                SCH_Delete_Task(i);
                continue;
            }
        }
        i++;
    }
}

void SCH_Dispatch_Tasks(void)
{
    unsigned char Index;
    // Dispatches (runs) the next task (if one is ready)
    for (Index = 0; Index < SCH_MAX_TASKS; Index++){
        if (SCH_tasks_G[Index].RunMe > 0) {
            (*SCH_tasks_G[Index].pTask)(); // Run the task
            SCH_tasks_G[Index].RunMe -= 1; // Reset / reduce RunMe flag
            // Periodic tasks will automatically run again
            // - if this is a 'one shot' task, remove it from the array
            if (SCH_tasks_G[Index].Period == 0)
            {
                SCH_Delete_Task(Index);
            }
        }
    }
    // Report system status
    SCH_Report_Status();
    // The scheduler enters idle mode at this point
    SCH_Go_To_Sleep();
}*/

void SCH_Dispatch_Tasks(void) {
/*    while (head_idx != NO_TASK_ID && SCH_Tasks_Array[head_idx].Delay == 0) {
        uint8_t idx = head_idx;
        // tách node khỏi list trước khi chạy
        remove(idx);
        sTasks *t = &SCH_Tasks_Array[idx];
        t->RunMe++;
        if (t->RunMe) {
            t->RunMe--;
            if (t->pTask) (*t->pTask)();
        }
        if (t->Period > 0)
            // periodic: chèn lại theo chu kỳ
            insert(idx, t->Period);
        else {
            // one-shot: giải phóng
            t->pTask  = 0;
            t->Delay  = 0;
            t->Period = 0;
            //t->used = 0;
            t->next   = NO_TASK_ID;
            t->prev   = NO_TASK_ID;
        }
    }*/
	if(SCH_Tasks_Array[0].RunMe > 0) {
		(*SCH_Tasks_Array[0].pTask)(); // Run the task
		SCH_Tasks_Array[0].RunMe = 0; // Reset / reduce RunMe flag
		sTasks temtask = SCH_Tasks_Array[0];
		SCH_Delete_Task(temtask.TaskID);
		if (temtask.Period != 0) {
			SCH_Add_Task(temtask.pTask, temtask.Period, temtask.Period);
		}
	}
}

static uint32_t Get_New_Task_ID(void){
	newTaskID++;
	if(newTaskID == NO_TASK_ID){
		newTaskID++;
	}
	return newTaskID;
}

/*
void SCH_Delete_Task (uint8_t TASK_INDEX) {
    if (TASK_INDEX >= SCH_MAX_TASKS) return;
    sTasks *t = &SCH_Tasks_Array[TASK_INDEX];
    if (!t->pTask) return;
    remove(TASK_INDEX);
    t->pTask  = 0;
    t->Delay  = 0;
    t->Period = 0;
    t->RunMe  = 0;
    //t->used = 0;
    t->next   = NO_TASK_ID;
    t->prev   = NO_TASK_ID;
}
*/

/*void SCH_Delete_Task (uint8_t TASK_INDEX){
    for (uint8_t i = TASK_INDEX; i < current_index_tasks - 1; i++){
        SCH_Tasks_Array[i] = SCH_Tasks_Array[i + 1];
        SCH_Tasks_Array[i].TaskID = i;
    }
    SCH_Tasks_Array[current_index_tasks - 1].pTask  = 0;
    SCH_Tasks_Array[current_index_tasks - 1].Delay  = 0;
    SCH_Tasks_Array[current_index_tasks - 1].Period = 0;
    SCH_Tasks_Array[current_index_tasks - 1].RunMe  = 0;
    SCH_Tasks_Array[current_index_tasks - 1].TaskID = 0;
    current_index_tasks--;
}

unsigned char SCH_Delete_Task(const uint8_t TASK_INDEX){
    unsigned char Return_code;
    if (SCH_tasks_G[TASK_INDEX].pTask == 0) {
        // No task at this location...
        //
        // Set the global error variable
        Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;

        // ...also return an error code
        Return_code = RETURN_ERROR;
    } else {
        Return_code = RETURN_NORMAL;
    }
    SCH_tasks_G[TASK_INDEX].pTask = 0x0000;
    SCH_tasks_G[TASK_INDEX].Delay = 0;
    SCH_tasks_G[TASK_INDEX].Period = 0;
    SCH_tasks_G[TASK_INDEX].RunMe = 0;
    return Return_code; // return status
}
*/
