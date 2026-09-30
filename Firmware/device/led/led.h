#ifndef LED_H
#define LED_H

#include <stdbool.h>
#include <stdint.h>

/* LED indices used by the whole firmware. Pin mapping: see Hardware/05_Report/report_sche.md */
typedef enum
{
    LED_STATUS = 0, /* PC13 - system status LED */
    LED_CH1,        /* PA11 - channel 1 indicator */
    LED_CH2,        /* PA12 - channel 2 indicator */
    LED_CH3,        /* PB9  - channel 3 indicator */
    LED_CH4,        /* PB10 - channel 4 indicator */
    LED_COUNT
} led_id_t;

/* Turn a LED on (true) or off (false). Polarity is handled inside led.c. */
void led_set(led_id_t led, bool state);

/* Invert the current state of a LED. Use this for blinking. */
void led_toggle(led_id_t led);

#endif /* LED_H */
