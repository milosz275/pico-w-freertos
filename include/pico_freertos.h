#ifndef __TEMPLATE_H
#define __TEMPLATE_H

#define TEMPERATURE_UNITS 'C'

typedef enum
{
    INIT_SUCCESS,
    STDIO_INIT_FAILURE,
    WIFI_INIT_FAILURE
} init_result;

float read_onboard_temperature(const char unit);

void init_queue();

void wifi_init_task(void* pv_param);

void blink_task(void* pv_param);

void usb_task(void* pv_param);

void temperature_task(void* pv_param);

#endif // __TEMPLATE_H
