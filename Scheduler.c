/*
 * Adv_Scheduler.c
 *
 *  Created on: Mar 16, 2022
 *      Author: angoosh
 */

#include "Adv_Scheduler.h"
#include "main.h"

tTask tasks[MAX_TASKS];
uint8_t n_o_tasks;
uint32_t disabled_tasks;


void Adv_Scheduler_Init();
void Adv_Scheduler_Add_Task(void (*exec)(void *), void *handle, uint32_t delay, uint32_t period);
int Adv_Scheduler_Remove_Task(void (*exec)(void *), void *handle);
int Adv_Scheduler_Disable_Task(void (*exec)(void *), void *handle);
int Adv_Scheduler_Enable_Task(void (*exec)(void *), void *handle);
void Adv_Scheduler_Update();
void Adv_Scheduler_Exec();

/**
 * @brief Initialize sheduler
 * @retval none
 */
void Adv_Scheduler_Init(){
	n_o_tasks = 0;
	disabled_tasks = 0;
}

/**
 * @brief Add task to scheduler
 * @param function to be executed
 * @param input parameters for the function
 * @param initial delay after which execution will take place
 * @param delay between executions
 * @retval none
 */
void Adv_Scheduler_Add_Task(void (*exec)(void *), void *handle, uint32_t delay, uint32_t period){
	tasks[n_o_tasks].delay = delay;
	tasks[n_o_tasks].exec = exec;
	tasks[n_o_tasks].handle = handle;
	tasks[n_o_tasks].period = period;
	n_o_tasks ++;
}

/**
 * @brief Remove task from scheduler
 * @param function to be removed
 * @param input parameters for the function, needed to identify concrete process
 * @retval success if 0
 */
int Adv_Scheduler_Remove_Task(void (*exec)(void *), void *handle){
	for(int task = 0; task < n_o_tasks; task++){
		if((tasks[task].exec == exec) && (tasks[task].handle == handle)){
			if(task != (n_o_tasks - 1)){
				for(int i = 0; i < (n_o_tasks - task); i++){
					tasks[task].delay = tasks[task + 1].delay;
					tasks[task].exec = tasks[task + 1].exec;
					tasks[task].handle = tasks[task + 1].handle;
					tasks[task].period = tasks[task + 1].period;
					tasks[task].run = tasks[task + 1].run;
				}
			}
			n_o_tasks --;
			return 0;
		}
	}
	return 1;//task does not exist
}

/**
 * @brief Disable task. Doesn't remove the task, only disables it for future if needed
 * @param function to be disabled
 * @param input parameters for the function, needed to identify concrete process
 * @retval success if 0
 */
int Adv_Scheduler_Disable_Task(void (*exec)(void *), void *handle){
	for(int task = 0; task < n_o_tasks; task++){
		if((tasks[task].exec == exec) && (tasks[task].handle == handle)){
			disabled_tasks |= (1 << task);
			return 0;
		}
	}
	return 1;//task does not exist
}

/**
 * @brief Enable existing task
 * @param function to be enabled
 * @param input parameters for the function, needed to identify concrete process
 * @retval success if 0
 */
int Adv_Scheduler_Enable_Task(void (*exec)(void *), void *handle){
	for(int task = 0; task < n_o_tasks; task++){
		if((tasks[task].exec == exec) && (tasks[task].handle == handle)){
			disabled_tasks = disabled_tasks & ~(1 << task);
			return 0;
		}
	}
	return 1;//task does not exist
}

/**
 * @brief Update scheduler state, should be executed by timer periodically.
 * Period dictates how long will delay values be.
 * @retval none
 */
void Adv_Scheduler_Update(){
	for(int task = 0; task < n_o_tasks; task++){
		if((disabled_tasks & (1 << task)) == 0){
			if(tasks[task].delay == 0){
				if(tasks[task].period == 0){
					disabled_tasks |= (1 << task);
				}
				else{
					tasks[task].delay = tasks[task].period;
				}
				tasks[task].run ++;
			}
			else{
				tasks[task].delay --;
			}
		}
	}
}

/**
 * @brief Execute all tasks which have to be executed (run value > 0)
 * Task will be executed run number of times.
 * @retval none
 */
void Adv_Scheduler_Exec(){
	for(int task = 0; task < n_o_tasks; task++){
		if(tasks[task].run != 0){
			tasks[task].exec(tasks[task].handle);
			tasks[task].run --;
		}
	}
}
