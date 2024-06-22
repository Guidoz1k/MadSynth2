#include "main.h"

 // RUNS ON CORE 1
void core1Task(void* parameter){
    timer_core1_setup(); // initiate CORE 1 interrupt

    task_core1();

    // core 1 task termination
    vTaskDelete(NULL);
}

 // RUNS ON CORE 0
void app_main(){
    delay_milli(200); // so our UART configuration won't mess with the startup dump
    serial_setup();
    led_setup();

    timer_core0_setup(); // initiate CORE 0 interrupt

    // core 1 task creation
    xTaskCreatePinnedToCore(core1Task, "timer1Creator", 10000, NULL, 1, NULL, 1);

    task_core0();

    // core 0 task termination
    vTaskDelete(NULL);
}
