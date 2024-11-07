#include "pico_freertos.h"

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
#include "lwipopts.h"
#include "wifi_credentials.h"

/* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
const float conversion_factor = 3.3f / (1 << 12);

/* On-board LED state queue */
static QueueHandle_t x_queue = NULL;

/* Semaphore for LED task */
SemaphoreHandle_t count = NULL;

/* Previous LED state */
bool previous_led_state = false;

float read_onboard_temperature(const char unit)
{
    float adc = (float)adc_read() * conversion_factor;
    float temp_c = 27.0f - (adc - 0.706f) / 0.001721f;

    if (unit == 'C')
        return temp_c;
    else if (unit == 'F')
        return temp_c * 9 / 5 + 32;
    else if (unit == 'K')
        return temp_c + 273.15f;

    return -1.0f;
}

void init_freertos()
{
    x_queue = xQueueCreate(1, sizeof(uint));
    count = xSemaphoreCreateCounting(5, 0);
}

void wifi_init_task(void* pv_param)
{
    printf("Connecting to WiFi...\n");
    cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK);
    
    while (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) != CYW43_LINK_UP)
    {
        if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_FAIL)
        {
            printf("Failed to connect to WiFi. Retrying...\n");
            cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    printf("Connected.\n");
    extern cyw43_t cyw43_state;
    uint32_t ip_addr = cyw43_state.netif[CYW43_ITF_STA].ip_addr.addr;
    printf("IP Address: %lu.%lu.%lu.%lu\n", ip_addr & 0xFF, (ip_addr >> 8) & 0xFF, (ip_addr >> 16) & 0xFF, ip_addr >> 24);
    
    while (true)
        vTaskDelay(5000 / portTICK_PERIOD_MS);
}

void led_task(void* pv_param)
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true)
    {
        if (xSemaphoreTake(count, (TickType_t) 10) == pdTRUE)
        {
            if (!previous_led_state)
                printf("Turning LED on\n");
            previous_led_state = true;
            gpio_put(LED_PIN, 1);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        else
        {
            if (previous_led_state)
                printf("Turning LED off\n");
            previous_led_state = false;
            gpio_put(LED_PIN, 0);
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }
}

void button_task(void* pv_param)
{
    gpio_init(LED_TOGGLE_PIN);
    gpio_set_dir(LED_TOGGLE_PIN, GPIO_IN);
    gpio_pull_up(LED_TOGGLE_PIN);
    while (true)
    {
        if (!gpio_get(LED_TOGGLE_PIN))
        {
            xSemaphoreGive(count);
            vTaskDelay(20 / portTICK_PERIOD_MS);
        }
        else
            vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

void blink_task(void* pv_param)
{
    while (true)
    {
        uint led_state = !cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
        xQueueSend(x_queue, &led_state, 0U);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_state);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void usb_task(void* pv_param)
{
    uint received_value;
    while (true)
    {
        BaseType_t result = xQueueReceive(x_queue, &received_value, portMAX_DELAY);
        if (result == errQUEUE_EMPTY)
            continue;
        if (received_value)
            printf("LED is on\n");
        else
            printf("LED is off\n");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void temperature_task(void* pv_param)
{
    while (true)
    {
        float temperature = read_onboard_temperature(TEMPERATURE_UNITS);
        printf("%.02f\n", temperature);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
