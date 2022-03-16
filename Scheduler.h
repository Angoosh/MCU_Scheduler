/*
 * Adv_Scheduler.h
 *
 *  Created on: Mar 16, 2022
 *      Author: angoosh
 */

#ifndef INC_ADV_SCHEDULER_H_
#define INC_ADV_SCHEDULER_H_

#include "stm32l0xx_hal.h"

#define MAX_TASKS	32


typedef struct {
  uint32_t delay;
  uint32_t period;
  uint8_t run;
  void (*exec)(void *handle);
  void *handle;
} tTask;


void Adv_Scheduler_Init();
void Adv_Scheduler_Add_Task(void (*exec)(void *), void *handle, uint32_t delay, uint32_t period);
int Adv_Scheduler_Remove_Task(void (*exec)(void *), void *handle);
int Adv_Scheduler_Disable_Task(void (*exec)(void *), void *handle);
int Adv_Scheduler_Enable_Task(void (*exec)(void *), void *handle);
void Adv_Scheduler_Update();
void Adv_Scheduler_Exec();

#endif /* INC_ADV_SCHEDULER_H_ */
