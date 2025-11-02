/*
 * scheduler.c
 *
 *  Created on: Oct 26, 2025
 *      Author: Admin
 */

#include "scheduler.h"

static uint8_t head_idx;
/*unsigned char Error_code_G = 0;*/

static void insert(uint8_t idx, uint32_t delay){
    SCH_Tasks_Array[idx].Delay = delay;
    SCH_Tasks_Array[idx].next  = NO_TASK_ID;
    SCH_Tasks_Array[idx].prev  = NO_TASK_ID;
    if (head_idx == NO_TASK_ID) {
        head_idx = idx;
        return;
    }
    uint8_t cur = head_idx;
    uint8_t prev = NO_TASK_ID;
    //trừ dần delay theo giá trị của các node đi trước
    while (cur != NO_TASK_ID && delay >= SCH_Tasks_Array[cur].Delay) {
        delay = delay - SCH_Tasks_Array[cur].Delay;
        prev = cur;
        cur  = SCH_Tasks_Array[cur].next;
    }
    // set giá trị cho node mới
    SCH_Tasks_Array[idx].Delay = delay;
    // nối vào list
    SCH_Tasks_Array[idx].next = cur;
    SCH_Tasks_Array[idx].prev = prev;
    if (prev == NO_TASK_ID) head_idx = idx;
    	else SCH_Tasks_Array[prev].next = idx;
    if (cur != NO_TASK_ID) {
        SCH_Tasks_Array[cur].prev  = idx;
        //giảm giá trị đi 'delay' node sau vì đã chen node mới trước nó
        SCH_Tasks_Array[cur].Delay = SCH_Tasks_Array[cur].Delay - delay;
    }
}

//remove chỉ tách node khỏi list chứ không xóa khỏi mảng
static void remove(uint8_t idx){
    uint8_t prev = SCH_Tasks_Array[idx].prev;
    uint8_t next = SCH_Tasks_Array[idx].next;
    if (next != NO_TASK_ID){
        SCH_Tasks_Array[next].Delay = SCH_Tasks_Array[next].Delay + SCH_Tasks_Array[idx].Delay;
        SCH_Tasks_Array[next].prev = prev;
    }
    if (prev != NO_TASK_ID) SCH_Tasks_Array[prev].next = next;
    	else head_idx = next;
    SCH_Tasks_Array[idx].next = NO_TASK_ID;
    SCH_Tasks_Array[idx].prev = NO_TASK_ID;
}

void SCH_Init(void){
    head_idx = NO_TASK_ID;
    for (uint8_t i = 0; i < SCH_MAX_TASKS; i++) {
        SCH_Tasks_Array[i].pTask  = 0;
        SCH_Tasks_Array[i].Delay  = 0;
        SCH_Tasks_Array[i].Period = 0;
        SCH_Tasks_Array[i].RunMe  = 0;
        SCH_Tasks_Array[i].TaskID = i;
        //SCH_Tasks_Array[i].used = 0;
        SCH_Tasks_Array[i].next   = NO_TASK_ID;
        SCH_Tasks_Array[i].prev   = NO_TASK_ID;
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

uint8_t SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {
    /*int16_t slot = find_free_slot();
    if (slot < 0) return NO_TASK_ID;*/
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
}

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
    if (head_idx == NO_TASK_ID) return;
    if (SCH_Tasks_Array[head_idx].Delay > 0)
        SCH_Tasks_Array[head_idx].Delay--;
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
    while (head_idx != NO_TASK_ID && SCH_Tasks_Array[head_idx].Delay == 0) {
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
    }
}

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
