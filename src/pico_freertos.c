#include "pico_freertos.h"

#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/cyw43_arch.h>
#include <pico/multicore.h>
#include <hardware/adc.h>
#include <lwip/netif.h>

#include "FreeRTOS.h"
#include "task.h"
#include "lwipopts.h"
#include "wifi_credentials.h"

/* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
const float conversion_factor = 3.3f / (1 << 12);

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

void wifi_init_task()
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
        vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void blink_task()
{
    while (true)
    {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        vTaskDelay(250 / portTICK_PERIOD_MS);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}

void temperature_task()
{
    while (true)
    {
        float temperature = read_onboard_temperature(TEMPERATURE_UNITS);
        printf("%.02f\n", temperature);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
