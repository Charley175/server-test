/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "../../../sources/esp-idf/components/driver/include/driver/gpio.h"
#include "../../../sources/esp-idf/components/driver/include/driver/ledc.h"

void app_main(void)
{
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU cores, WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Free heap: %d\n", esp_get_free_heap_size());

    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    while (1) vTaskDelay(1000 / portTICK_PERIOD_MS);

    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

void app1_main(void)
{
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = 5000,                      // frequency of PWM signal
        .speed_mode = LEDC_HIGH_SPEED_MODE,           // timer mode
        .timer_num = LEDC_TIMER_0,            // timer index
        .clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
    };

    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
            .channel    = LEDC_CHANNEL_0,
            .duty       = 0,
            .gpio_num   = GPIO_NUM_2,
            .speed_mode = LEDC_HIGH_SPEED_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_TIMER_0,
    };
    ledc_channel_config(&ledc_channel);
    ledc_fade_func_install(0);

    for (int i = 10; i >= 0; i--) {
       // ledc_set_duty(ledc_channel.speed_mode, ledc_channel.channel, 255);
       // ledc_update_duty(ledc_channel.speed_mode, ledc_channel.channel);

            ledc_set_fade_with_time(ledc_channel.speed_mode,
                    ledc_channel.channel, 8000, 1000);
            ledc_fade_start(ledc_channel.speed_mode,
                    ledc_channel.channel, LEDC_FADE_NO_WAIT);
 
        vTaskDelay(1000 / portTICK_PERIOD_MS);

			ledc_set_fade_with_time(
				ledc_channel.speed_mode,
				ledc_channel.channel, 
				0, 
				1000);
			// 开始渐变
			ledc_fade_start(
				ledc_channel.speed_mode,
				ledc_channel.channel, 
				LEDC_FADE_NO_WAIT);
	
		// 等待渐变完成
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}


    fflush(stdout);
    esp_restart();
}