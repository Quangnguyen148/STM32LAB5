/*
 * scheduler.h
 *
 *  Created on: Oct 26, 2025
 *      Author: Admin
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>


typedef struct{
	void (*pTask)(void);
	uint32_t 	Delay;
	uint32_t	Period;
	uint8_t		RunMe;

    //uint8_t   used;   // slot có task hay không
    uint8_t   next;     // chỉ số node kế tiếp trong delta-list (NO_TASK_ID nếu none)
    uint8_t   prev;     // để xóa O(1)
    uint32_t  TaskID;
}sTasks;

#define SCH_MAX_TASKS 			40
#define	NO_TASK_ID				0xFF
/*sTasks SCH_tasks_G[SCH_MAX_TASKS];*/
sTasks SCH_Tasks_Array[SCH_MAX_TASKS];
//uint8_t current_index_tasks;

void SCH_Init(void);

//void SCH_Add_Task(void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD);
uint8_t SCH_Add_Task(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD);

void SCH_Update(void);

void SCH_Dispatch_Tasks(void);

void SCH_Delete_Task (uint8_t TASK_INDEX);
/*unsigned char SCH_Delete_Task(const uint8_t TASK_INDEX);*/

/*void SCH_Report_Status(void);*/

#endif /* INC_SCHEDULER_H_ */
