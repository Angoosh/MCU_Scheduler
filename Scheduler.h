/*
 * Adv_Scheduler.h
 *
 *  Created on: Mar 16, 2022
 *      Author: angoosh
 */

#ifndef INC_ADV_SCHEDULER_H_
#define INC_ADV_SCHEDULER_H_

#include "main.h"

#ifdef STM32WBxx_HAL_H
	#include "stm32wbxx_hal.h"
#endif

#include <stdint.h>

#ifdef STM32WBxx_HAL_H
	#define RUNS_ON_STM32
#endif

#define MAX_TASKS	32


typedef struct {
  uint32_t delay;
  uint32_t period;
  uint8_t run;
  void (*exec)(void *handle);
  void *handle;
  char *name;
} tTask;

#ifdef RUNS_ON_STM32

typedef struct {
  TIM_HandleTypeDef *htim;
  uint16_t psc;
  uint32_t period;
  char *name;
} tTimerTask;

extern tTimerTask timerTasks[MAX_TASKS];

#endif

typedef struct{
	uint32_t seconds;
	uint8_t n_o_tasks;
	uint8_t n_o_timer_tasks;
	uint32_t disabled_tasks;
	uint32_t disabled_timer_tasks;
} Adv_Scheduler_Typedef;

extern Adv_Scheduler_Typedef Scheduler;
extern tTask tasks[MAX_TASKS];

void Adv_Scheduler_Init();
void Adv_Scheduler_Add_Task(void (*exec)(void *), void *handle, uint32_t delay, uint32_t period, char *name);
int Adv_Scheduler_Remove_Task(void (*exec)(void *), void *handle);
int Adv_Scheduler_Disable_Task(void (*exec)(void *), void *handle);
int Adv_Scheduler_Enable_Task(void (*exec)(void *), void *handle);
int Adv_Scheduler_Edit_Task_Period(void (*exec)(void *), void *handle, uint32_t period);
void Adv_Scheduler_Update();
void Adv_Scheduler_Exec();

#ifdef RUNS_ON_STM32
    void Adv_Scheduler_Add_Timer_As_Task(TIM_HandleTypeDef *htim, uint16_t psc, uint32_t period, char *name);
	int Adv_Scheduler_Remove_Timer_As_Task(TIM_HandleTypeDef *htim);
	int Adv_Scheduler_Disable_Timer_Task(TIM_HandleTypeDef *htim);
	int Adv_Scheduler_Enable_Timer_Task(TIM_HandleTypeDef *htim);
#endif

#endif /* INC_ADV_SCHEDULER_H_ */
