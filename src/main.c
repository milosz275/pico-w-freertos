#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>
#include <pico/multicore.h>
#include <hardware/adc.h>
#include <lwip/netif.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "pico_freertos.h"
#include "pico_smp.h"
#include "lwipopts.h"
#include "wifi_credentials.h"

#define TASK_SIZE 256

bool pico_smp = true; // set to true if using pico_smp.c, otherwise pico_freertos.c

int main()
{
    // init
    stdio_init_all();
    if (cyw43_arch_init())
    {
        printf("Wi-Fi init failed");
        return -1;
    }
    cyw43_arch_enable_sta_mode();
    cyw43_wifi_pm(&cyw43_state, cyw43_pm_value(CYW43_NO_POWERSAVE_MODE, 20, 1, 1, 1));

    // adc init
    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(4);

    // tasks
    sleep_ms(2000);
    
    if (pico_smp)
    {
        init_smp();

        TaskHandle_t handleA;
        TaskHandle_t handleB;

        vGuardedPrint("\033[2J\033[H");

        xTaskCreate(vTaskSMP_print_core, "A", TASK_SIZE, NULL, 1, &handleA);
        xTaskCreate(vTaskSMP_print_core, "B", TASK_SIZE, NULL, 1, &handleB);

        xTaskCreate(vTaskSMP_print_core, "C", TASK_SIZE, NULL, 1, NULL);
        xTaskCreate(vTaskSMP_print_core, "D", TASK_SIZE, NULL, 1, NULL);

        xTaskCreate(vTaskSMP_wifi_init, "WiFi init task", TASK_SIZE, NULL, 1, NULL);

        xTaskCreate(vTaskSMP_demo_delay, "vTaskSMP_demo_delay", TASK_SIZE, NULL, 1, NULL);
        xTaskCreate(vTaskSMP_demo_led, "vTaskSMP_demo_led", TASK_SIZE, NULL, 1, NULL);

        vTaskCoreAffinitySet(handleA, (1 << 0));
        vTaskCoreAffinitySet(handleB, (1 << 1));
    }
    else
    {
        init_freertos();
        xTaskCreate(wifi_init_task, "wifi_init_task", TASK_SIZE, NULL, 1, NULL);
        xTaskCreate(button_task, "button_task", TASK_SIZE, NULL, 1, NULL);
        xTaskCreate(led_task, "led_task", TASK_SIZE, NULL, 1, NULL);
        xTaskCreate(blink_task, "blink_task", TASK_SIZE, NULL, 1, NULL);
        // xTaskCreate(usb_task, "usb_task", TASK_SIZE, NULL, 1, NULL);
        // xTaskCreate(temperature_task, "temperature_task", TASK_SIZE, NULL, 1, NULL);
    }

    vTaskStartScheduler();
    while (true) {}
}
