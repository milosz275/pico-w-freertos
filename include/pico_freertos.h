#ifndef __PICO_FREERTOS_H
#define __PICO_FREERTOS_H

#define TEMPERATURE_UNITS 'C'
#define LED_PIN 14
#define LED_TOGGLE_PIN 15

typedef enum
{
    INIT_SUCCESS,
    STDIO_INIT_FAILURE,
    WIFI_INIT_FAILURE
} init_result;

float read_onboard_temperature(const char unit);

void init_freertos();

void wifi_init_task(void* pv_param);

void led_task(void* pv_param);

void button_task(void* pv_param);

void blink_task(void* pv_param);

void usb_task(void* pv_param);

void temperature_task(void* pv_param);

#endif // __PICO_FREERTOS_H
