#include "led.h"

#include "main.h" /* CubeMX generated: HAL + pin labels (LED_Pin, LED_GPIO_Port, ...) */

/* Logic level that turns a LED ON.
 * GPIO_PIN_SET   -> active-high (GPIO -> resistor -> LED -> GND)
 * GPIO_PIN_RESET -> active-low  (3V3 -> LED -> resistor -> GPIO)
 * TODO(Member 3): confirm against the schematic (D6 / D11-D14) and change if needed. */
#define LED_ON_LEVEL  GPIO_PIN_SET
#define LED_OFF_LEVEL GPIO_PIN_RESET

typedef struct
{
    GPIO_TypeDef *port;
    uint16_t      pin;
} led_hw_t;

/* Order must match led_id_t */
static const led_hw_t led_table[LED_COUNT] = {
    [LED_STATUS] = {LED_GPIO_Port, LED_Pin},
    [LED_CH1]    = {LED_CH1_GPIO_Port, LED_CH1_Pin},
    [LED_CH2]    = {LED_CH2_GPIO_Port, LED_CH2_Pin},
    [LED_CH3]    = {LED_CH3_GPIO_Port, LED_CH3_Pin},
    [LED_CH4]    = {LED_CH4_GPIO_Port, LED_CH4_Pin},
};

void led_set(led_id_t led, bool state)
{
    if (led >= LED_COUNT)
    {
        return;
    }

    HAL_GPIO_WritePin(led_table[led].port, led_table[led].pin, state ? LED_ON_LEVEL : LED_OFF_LEVEL);
}

void led_toggle(led_id_t led)
{
    if (led >= LED_COUNT)
    {
        return;
    }

    HAL_GPIO_TogglePin(led_table[led].port, led_table[led].pin);
}
