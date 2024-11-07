#include "pico_smp.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>
#include <pico/multicore.h>
#include <hardware/adc.h>
#include <lwip/netif.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "pico_freertos.h"
#include "lwipopts.h"
#include "wifi_credentials.h"

#define TASK_DELAY 500

/* LED toggle semaphore */
SemaphoreHandle_t toggle_semaphore = NULL;

/* Console print mutex */
SemaphoreHandle_t mutex = NULL;
int print_count = 0;

void init_smp()
{
    toggle_semaphore = xSemaphoreCreateBinary();
    mutex = xSemaphoreCreateMutex();
}

void vTaskSMP_demo_delay(void* pv_param)
{
    while (true)
    {
        xSemaphoreGive(toggle_semaphore);
        vTaskDelay(TASK_DELAY / portTICK_PERIOD_MS);
    }
}

void vTaskSMP_demo_led(void* pv_param)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true)
    {
        if (xSemaphoreTake(toggle_semaphore, portMAX_DELAY))
            gpio_put(LED_PIN, !gpio_get_out_level(LED_PIN));
    }
}

void vGuardedPrint(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    xSemaphoreTake(mutex, portMAX_DELAY);
    vprintf(format, args);
    print_count++;
    xSemaphoreGive(mutex);
    va_end(args);
}

void vTaskSMP_print_core(void* pv_param)
{
    while (true)
    {
        vGuardedPrint("%d: Hello task %s from core %d\n", print_count, pcTaskGetName(NULL), get_core_num());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void vTaskSMP_wifi_init(void* pv_param)
{
    vGuardedPrint("Connecting to WiFi...\n");
    cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK);
    
    while (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP)
    {
        if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_FAIL)
        {
            vGuardedPrint("Failed to connect to WiFi. Retrying...\n");
            cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    vGuardedPrint("Connected %s on core %d.\n", pcTaskGetName(NULL), get_core_num());
    extern cyw43_t cyw43_state;
    uint32_t ip_addr = cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr;
    vGuardedPrint("IP Address: %lu.%lu.%lu.%lu\n", ip_addr & 0xFF, (ip_addr >> 8) & 0xFF, (ip_addr >> 16) & 0xFF, ip_addr >> 24);
    
    while (true)
        vTaskDelay(5000 / portTICK_PERIOD_MS);
}
