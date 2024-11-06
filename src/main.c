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
#include "lwipopts.h"
#include "wifi_credentials.h"

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
    init();
    xTaskCreate(wifi_init_task, "wifi_init_task", 256, NULL, 1, NULL);
    xTaskCreate(button_task, "button_task", 256, NULL, 1, NULL);
    xTaskCreate(led_task, "led_task", 256, NULL, 1, NULL);
    xTaskCreate(blink_task, "blink_task", 256, NULL, 1, NULL);
    // xTaskCreate(usb_task, "usb_task", 256, NULL, 1, NULL);
    // xTaskCreate(temperature_task, "temperature_task", 256, NULL, 1, NULL);
    vTaskStartScheduler();
    while (true) {}
}
