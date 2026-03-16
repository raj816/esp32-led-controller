#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

// Hardware
#define LED_PIN GPIO_NUM_2

// Task priorities
#define PRIORITY_LED    3
#define PRIORITY_UART   2
#define PRIORITY_SENSOR 1

// Queue handle - for sending commands to LED task
QueueHandle_t led_queue;

// LED Commands
typedef enum {
    CMD_ON,
    CMD_OFF,
    CMD_TOGGLE,
    CMD_FAST,
    CMD_SLOW
} led_command_t;

//======================
// TASK 1: LED Control
//======================
void led_task(void *pvParameters) {
    // Setup LED
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    led_command_t cmd;
    int blink_delay = 1000;

    printf("[LED Task] Started!\n");

    while(1) {
        // Wait for command from queue (100ms timeout)
        if(xQueueReceive(led_queue, &cmd, pdMS_TO_TICKS(100))) {
            switch(cmd) {
                case CMD_ON:
                    gpio_set_level(LED_PIN, 1);
                    printf("[LED] ON\n");
                    break;
                case CMD_OFF:
                    gpio_set_level(LED_PIN, 0);
                    printf("[LED] OFF\n");
                    break;
                case CMD_TOGGLE:
                    gpio_set_level(LED_PIN, !gpio_get_level(LED_PIN));
                    printf("[LED] TOGGLED\n");
                    break;
                case CMD_FAST:
                    blink_delay = 200;
                    printf("[LED] Fast blink mode\n");
                    break;
                case CMD_SLOW:
                    blink_delay = 1000;
                    printf("[LED] Slow blink mode\n");
                    break;
            }
        } else {
            // No command received - auto blink
            gpio_set_level(LED_PIN, !gpio_get_level(LED_PIN));
            vTaskDelay(pdMS_TO_TICKS(blink_delay));
        }
    }
}

//======================
// TASK 2: UART Monitor
//======================
void uart_task(void *pvParameters) {
    printf("[UART Task] Started! Send commands:\n");
    printf("  1=ON  2=OFF  3=TOGGLE  4=FAST  5=SLOW\n");

    while(1) {
        int ch = getchar();
        if(ch != EOF) {
            led_command_t cmd;
            switch(ch) {
                case '1': cmd = CMD_ON;     break;
                case '2': cmd = CMD_OFF;    break;
                case '3': cmd = CMD_TOGGLE; break;
                case '4': cmd = CMD_FAST;   break;
                case '5': cmd = CMD_SLOW;   break;
                default: continue;
            }
            // Send command to LED task via queue
            xQueueSend(led_queue, &cmd, 0);
            printf("[UART] Command sent!\n");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

//======================
// TASK 3: Sensor Monitor
//======================
void sensor_task(void *pvParameters) {
    printf("[Sensor Task] Started!\n");
    int count = 0;

    while(1) {
        // Simulate sensor reading
        count++;
        printf("[Sensor] Reading #%d - Value: %d\n",
               count, (count * 37) % 100);

        vTaskDelay(pdMS_TO_TICKS(3000));  // read every 3 seconds
    }
}

//======================
// MAIN
//======================
void app_main(void) {
    printf("\n================================\n");
    printf("  ESP32 FreeRTOS LED Controller\n");
    printf("  3 tasks running concurrently\n");
    printf("================================\n\n");

    // Create queue for LED commands
    led_queue = xQueueCreate(10, sizeof(led_command_t));

    // Create tasks
    xTaskCreate(led_task,    "LED",    2048, NULL, PRIORITY_LED,    NULL);
    xTaskCreate(uart_task,   "UART",   2048, NULL, PRIORITY_UART,   NULL);
    xTaskCreate(sensor_task, "SENSOR", 2048, NULL, PRIORITY_SENSOR, NULL);

    printf("[Main] All tasks created!\n");
}