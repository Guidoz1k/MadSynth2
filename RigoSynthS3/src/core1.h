#ifndef __CORE1_H
#define __CORE1_H

#include <stdio.h>
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include <esp_intr_alloc.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "delay.h"
#include "serial.h"

#define PERIOD1     100     // interrupt period in µs
#define SYNCPIN1    2       // interruption 1 work pin

void task_core1(void);

void timer_core1_setup();

#endif /* __CORE1_H */