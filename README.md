# MCU_Scheduler
Scheduler for microcontrollers. Tested with STM32, but should work with others.

As base for this library this blog was used: https://maker.pro/custom/tutorial/a-simple-time-trigger-scheduler-for-stm32-and-msp430-projects

Example pseudo code:
```
void Task_RedLedToggle(){
  //toggle red led
}

int main(){
  //add task, which executes function for toggling red led every 2 cycles and starts immediately
  Adv_Scheduler_Add_Task(Task_RedLedToggle,NULL,0,2);
  
  while(1){
    //check if any of active tasks should be ran
    Adv_Scheduler_Exec();
  }
}

void Timer_Callback(){
  //update state of sheduled tasks
  Adv_Scheduler_Update();
}
```
If you have any questions or suggestions, don't hesitate to contact me.
