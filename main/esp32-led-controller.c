#include <stdio.h>
#include"freertos/FreeRTOS.h"
#include"freertos/task.h"
#include"driver/gpio.h"

//onboard led pin onesp32
#define LED_PIN GPIO_NUM_2

void app_main(void)
{
    //configure the LED pin as output
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    printf("Esp32 led blinking \n ");
    printf("LED connected to GPIO %d \n", LED_PIN);
    int blink_count = 0;
    while (1) {
        //turn on the LED
        gpio_set_level(LED_PIN, 1);
        printf("LED ON - Blink %d \n",++blink_count);
        vTaskDelay(1000 / portTICK_PERIOD_MS); //delay for 1 second

        //turn off the LED
        gpio_set_level(LED_PIN, 0);
        printf("LED OFF - Blink %d \n",++blink_count);
        vTaskDelay(1000 / portTICK_PERIOD_MS); //delay for 1 second
    }
}
