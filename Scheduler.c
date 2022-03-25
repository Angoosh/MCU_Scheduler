/*
 * Adv_Scheduler.c
 *
 *  Created on: Mar 16, 2022
 *      Author: angoosh
 */

#include "Adv_Scheduler.h"

Adv_Scheduler_Typedef Scheduler;
tTask tasks[MAX_TASKS];


void Adv_Scheduler_Init();
void Adv_Scheduler_Add_Task(void (*exec)(void *), void *handle, uint32_t delay, uint32_t period, char *name);
int Adv_Scheduler_Remove_Task(void (*exec)(void *), void *handle);
int Adv_Scheduler_Disable_Task(void (*exec)(void *), void *handle);
int Adv_Scheduler_Enable_Task(void (*exec)(void *), void *handle);
int Adv_Scheduler_Edit_Task_Period(void (*exec)(void *), void *handle, uint32_t period);
void Adv_Scheduler_Update();
void Adv_Scheduler_Exec();

//STM32 Specific
#ifdef RUNS_ON_STM32
	void Adv_Scheduler_Add_Timer_As_Task(TIM_HandleTypeDef *htim, uint16_t psc, uint32_t period, char *name);
	int Adv_Scheduler_Remove_Timer_As_Task(TIM_HandleTypeDef *htim);
	int Adv_Scheduler_Disable_Timer_Task(TIM_HandleTypeDef *htim);
	int Adv_Scheduler_Enable_Timer_Task(TIM_HandleTypeDef *htim);

	tTimerTask timerTasks[MAX_TASKS];
#endif
/**
 * @brief Initialize sheduler
 * @retval none
 */
void Adv_Scheduler_Init(){
	Scheduler.n_o_tasks = 0;
	Scheduler.n_o_timer_tasks = 0;
	Scheduler.disabled_tasks = 0;
	Scheduler.disabled_timer_tasks = 0;
	Scheduler.seconds = 0;
}

/**
 * @brief Add task to scheduler
 * @param function to be executed
 * @param input parameters for the function
 * @param initial delay after which execution will take place
 * @param delay between executions
 * @param user readable name of task
 * @retval none
 */
void Adv_Scheduler_Add_Task(void (*exec)(void *), void *handle, uint32_t delay, uint32_t period, char *name){
	tasks[Scheduler.n_o_tasks].delay = delay;
	tasks[Scheduler.n_o_tasks].exec = exec;
	tasks[Scheduler.n_o_tasks].handle = handle;
	tasks[Scheduler.n_o_tasks].period = period;
	tasks[Scheduler.n_o_tasks].name = name;
	Scheduler.n_o_tasks ++;
}

/**
 * @brief Remove task from scheduler
 * @param function to be removed
 * @param input parameters for the function, needed to identify concrete process
 * @retval success if 0
 */
int Adv_Scheduler_Remove_Task(void (*exec)(void *), void *handle){
	for(int task = 0; task < Scheduler.n_o_tasks; task++){
		if((tasks[task].exec == exec) && (tasks[task].handle == handle)){
			if(task != (Scheduler.n_o_tasks - 1)){
				for(int i = 0; i < (Scheduler.n_o_tasks - task); i++){
					tasks[task].delay = tasks[task + 1].delay;
					tasks[task].exec = tasks[task + 1].exec;
					tasks[task].handle = tasks[task + 1].handle;
					tasks[task].period = tasks[task + 1].period;
					tasks[task].run = tasks[task + 1].run;
					tasks[task].name = tasks[task + 1].name;
				}
			}
			Scheduler.n_o_tasks --;
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
	for(int task = 0; task < Scheduler.n_o_tasks; task++){
		if((tasks[task].exec == exec) && (tasks[task].handle == handle)){
			Scheduler.disabled_tasks |= (1 << task);
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
	for(int task = 0; task < Scheduler.n_o_tasks; task++){
		if((tasks[task].exec == exec) && (tasks[task].handle == handle)){
			Scheduler.disabled_tasks = Scheduler.disabled_tasks & ~(1 << task);
			return 0;
		}
	}
	return 1;//task does not exist
}

/**
 * @brief Edit period of existing task
 * @param function to be enabled
 * @param input parameters for the function, needed to identify concrete process
 * @param new period
 * @retval success if 0
 */
int Adv_Scheduler_Edit_Task_Period(void (*exec)(void *), void *handle, uint32_t period){
	for(int task = 0; task < Scheduler.n_o_tasks; task++){
		if((tasks[task].exec == exec) && (tasks[task].handle == handle)){
			tasks[task].period = period;
			tasks[task].delay = period;
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
	Scheduler.seconds ++;
	for(int task = 0; task < Scheduler.n_o_tasks; task++){
		if((Scheduler.disabled_tasks & (1 << task)) == 0){
			if(tasks[task].delay == 0){
				if(tasks[task].period == 0){
					Scheduler.disabled_tasks |= (1 << task);
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
	for(int task = 0; task < Scheduler.n_o_tasks; task++){
		if(tasks[task].run != 0){
			tasks[task].exec(tasks[task].handle);
			tasks[task].run --;
		}
	}
}

#ifdef RUNS_ON_STM32

/**
 * @brief Add task which is executed in timer interrupt to scheduler
 * @param timer handle
 * @param value of timer prescaler to set
 * @param value of auto reload register of timer to set
 * @param user readable name of task
 * @retval none
 */
void Adv_Scheduler_Add_Timer_As_Task(TIM_HandleTypeDef *htim, uint16_t psc, uint32_t period, char *name){
	timerTasks[Scheduler.n_o_timer_tasks].htim = htim;
	timerTasks[Scheduler.n_o_timer_tasks].psc = psc;
	timerTasks[Scheduler.n_o_timer_tasks].period = period;
	timerTasks[Scheduler.n_o_timer_tasks].name = name;
	Scheduler.n_o_timer_tasks ++;

	htim->Instance->DIER |= 1 << 0;
	htim->Instance->PSC = psc;
	htim->Instance->ARR = period;

	HAL_TIM_Base_Start_IT(htim);
}

/**
 * @brief Remove task which is executed in timer interrupt
 * @param timer handle
 * @retval success if 0
 */
int Adv_Scheduler_Remove_Timer_As_Task(TIM_HandleTypeDef *htim){
	for(int task = 0; task < Scheduler.n_o_timer_tasks; task++){
		if(timerTasks[task].htim == htim){
			if(task != (Scheduler.n_o_timer_tasks - 1)){
				for(int i = 0; i < (Scheduler.n_o_timer_tasks - task); i++){
					timerTasks[task].htim = timerTasks[task + 1].htim;
					timerTasks[task].period = timerTasks[task + 1].period;
					timerTasks[task].psc = timerTasks[task + 1].psc;
					timerTasks[task].name = timerTasks[task + 1].name;
				}
			}
			Scheduler.n_o_timer_tasks --;

			HAL_TIM_Base_Stop_IT(htim);

			return 0;
		}
	}
	return 1;//task does not exist
}

/**
 * @brief Disable existing timer task. Doesn't remove the task, only disables it for future if needed
 * @param timer to be disabled
 * @retval success if 0
 */
int Adv_Scheduler_Disable_Timer_Task(TIM_HandleTypeDef *htim){
	for(int task = 0; task < Scheduler.n_o_timer_tasks; task++){
		if(timerTasks[task].htim == htim){
			Scheduler.disabled_timer_tasks |= (1 << task);

			HAL_TIM_Base_Stop_IT(htim);

			return 0;
		}
	}
	return 1;//task does not exist
}

/**
 * @brief Enable existing timer task
 * @param timer to be enabled
 * @retval success if 0
 */
int Adv_Scheduler_Enable_Timer_Task(TIM_HandleTypeDef *htim){
	for(int task = 0; task < Scheduler.n_o_timer_tasks; task++){
		if(timerTasks[task].htim == htim){
			Scheduler.disabled_timer_tasks = Scheduler.disabled_timer_tasks & ~(1 << task);

			HAL_TIM_Base_Start_IT(htim);

			return 0;
		}
	}
	return 1;//task does not exist
}

#endif
