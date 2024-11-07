#ifndef __PICO_SMP_H
#define __PICO_SMP_H

void init_smp();

void vTaskSMP_demo_delay(void* pv_param);

void vTaskSMP_demo_led(void* pv_param);

void vGuardedPrint(const char* format, ...);

void vTaskSMP_print_core(void* pv_param);

void vTaskSMP_wifi_init(void* pv_param);

#endif // __PICO_SMP_H
