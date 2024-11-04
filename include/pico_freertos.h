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

void wifi_init_task();

void blink_task();

void temperature_task();

#endif // __TEMPLATE_H
