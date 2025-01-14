
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "led_grid.hpp"

#include "driver/i2c_master.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


static const char *TAG = "main";

#define PIN_NUM_I2C_SDA     GPIO_NUM_21
#define PIN_NUM_I2C_SCL     GPIO_NUM_17

/*
void i2c_select_bank(i2c_master_dev_handle_t device, uint8_t bank) {
    const uint8_t cmd[2] = {ISSI_COMMANDREGISTER, bank};
    ESP_ERROR_CHECK(i2c_master_transmit(device, cmd, sizeof(cmd), 100));
}
*/
/*
void i2c_write_register8(i2c_master_dev_handle_t device, uint8_t bank, uint8_t reg, const uint8_t data) {
    i2c_select_bank(device, bank);

    const uint8_t cmd[2] = {reg, data};
    ESP_ERROR_CHECK(i2c_master_transmit(device, cmd, sizeof(cmd), 100));
}

void display_frame(i2c_master_dev_handle_t device, uint8_t frame) {
    if (frame > 7) {
        frame = 0;
    }

    i2c_write_register8(device, ISSI_BANK_FUNCTIONREG, ISSI_REG_PICTUREFRAME, frame);
}

void clear(i2c_master_dev_handle_t device, uint8_t frame) {
    i2c_select_bank(device, frame);
    uint8_t erasebuf[25] = {0};

    for (uint8_t i = 0; i < 6; i++) {
        erasebuf[0] = 0x24 + i * 24;
        i2c_master_transmit(device, erasebuf, 25, 100);
    }
}

void audio_sync(i2c_master_dev_handle_t device, bool sync) {
    if (sync) {
        i2c_write_register8(device, ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x1);
    }
    else {
        i2c_write_register8(device, ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x0);
    }
}

void set_led_pwm(i2c_master_dev_handle_t device, uint8_t lednum, uint8_t pwm, uint8_t bank) {
    if (lednum >= 144) {
        return;
    }

    i2c_write_register8(device, bank, 0x24 + lednum, pwm);
}
*/

/*void draw_pixels(i2c_master_dev_handle_t device, int16_t x, int16_t y, uint16_t color, uint8_t bank) {
    if ((x < 0) || (x >= 16))
        return;
    if ((y < 0) || (y >= 9))
        return;
    if (color > 255)
        color = 255; // PWM 8bit max

    set_led_pwm(device, x + y * 16, color, bank);
}

void draw_pixels_index(i2c_master_dev_handle_t device, int16_t index, uint16_t color, uint8_t bank) {
    if(index < 0 || index >= 16*9) {
        return;
    }

    if(color > 255) {color = 255;}

    set_led_pwm(device, index, color, bank);
}

*/
extern "C" int app_main(void) {
    i2c_master_bus_handle_t bus_handle;
    i2c_master_dev_handle_t dev_handle;
    uint8_t frame = 0;

    //LedGrid<IS31FL3731_Driver> grid;

    i2c_master_bus_config_t bus_config;
    bus_config.i2c_port = I2C_NUM_0;
    bus_config.sda_io_num = PIN_NUM_I2C_SDA;
    bus_config.scl_io_num = PIN_NUM_I2C_SCL;
    bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_config.glitch_ignore_cnt = 7;
    bus_config.intr_priority = 0;
    bus_config.trans_queue_depth = 0;
    bus_config.flags.enable_internal_pullup = false;
    bus_config.flags.allow_pd = 0;

    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &bus_handle));

    /*i2c_device_config_t dev_config;
    dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_config.device_address = 0x74;
    dev_config.scl_speed_hz = 100 * 1000; // 100KHz
    dev_config.scl_wait_us = 0;*/


    // Connected to ground  1110100 -> 0x74
    // Connected to VCC     1110111 -> 0x77
    // Connected to SCL     1110101 -> 0x75
    // Connected to SDA     1110110 -> 0x76

    //ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &dev_handle));

    IS31FL3731_Driver driver(bus_handle, 0x77);

    ESP_LOGI(TAG, "I2C initialized successfully");
/*
    // shutdown
    i2c_write_register8(dev_handle, ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00);

    vTaskDelay(10 / portTICK_PERIOD_MS);

    // out of shutdown
    i2c_write_register8(dev_handle, ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x01);

    // picture mode
    i2c_write_register8(dev_handle, ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG, ISSI_REG_CONFIG_PICTUREMODE);

    display_frame(dev_handle, frame);

    // all leds on & PWM 0
    clear(dev_handle, frame);

    for (uint8_t f = 0; f < 8; f++) {
        for (uint8_t i = 0; i <= 0x11; i++)
            i2c_write_register8(dev_handle, f, i, 0xff);
            //writeRegister8(f, i, 0xff); // each 8 LEDs on
    }

    audio_sync(dev_handle, false);*/
////HERE

    int16_t counter = 0;
    int pwme = 0;
    uint8_t next_frame = 0;
    while (true) {
        next_frame = next_frame + 1;
        if (next_frame > 7) next_frame = 0;
        for (int y = 0; y < 9; ++y) {
            for (uint8_t x = 0; x < 16; ++x) {
                driver.set_pwm(x, y, 0xA, next_frame);
                //draw_pixels(dev_handle, x, y, 0xA, next_frame);
            }
        }
        //draw_pixels_index(dev_handle, 2, 0xFF, next_frame);
        driver.display_frame(next_frame);
        //display_frame(dev_handle, next_frame);
        counter += 1;
        if(counter >= 144) {
            counter = 0;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);;
    }

    return 0;
}
