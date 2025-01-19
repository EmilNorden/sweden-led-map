//
// Created by Emil Nordén on 2025-01-14.
//
#include "led_driver.hpp"
#include "freertos/FreeRTOS.h"

#define ISSI_COMMANDREGISTER    0xFD

#define ISSI_BANK_FUNCTIONREG 0x0B
#define ISSI_COMMANDREGISTER 0xFD

#define ISSI_REG_SHUTDOWN 0x0A
#define ISSI_REG_AUDIOSYNC 0x06

#define ISSI_REG_PICTUREFRAME 0x01

#define ISSI_REG_CONFIG 0x00
#define ISSI_REG_CONFIG_PICTUREMODE 0x00
#define ISSI_REG_CONFIG_AUTOPLAYMODE 0x08
#define ISSI_REG_CONFIG_AUDIOPLAYMODE 0x18

IS31FL3731_Driver::IS31FL3731_Driver(i2c_master_bus_handle_t bus, const int i2c_address, const std::unordered_map<int, int>& led_index_overrides)
    : m_led_index_overrides(led_index_overrides){
    i2c_device_config_t dev_config;
    dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_config.device_address = i2c_address;
    dev_config.scl_speed_hz = 100 * 1000; // 100KHz
    dev_config.scl_wait_us = 0;

    // Connected to ground  1110100 -> 0x74
    // Connected to VCC     1110111 -> 0x77
    // Connected to SCL     1110101 -> 0x75
    // Connected to SDA     1110110 -> 0x76

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &dev_config, &m_i2c_handle));

    // shutdown
    i2c_write_register8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00);

    vTaskDelay(10 / portTICK_PERIOD_MS);

    // out of shutdown
    i2c_write_register8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x01);

    // picture mode
    i2c_write_register8(ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG, ISSI_REG_CONFIG_PICTUREMODE);

    display_frame(0);

    // all leds on & PWM 0
    clear(0);

    for (uint8_t f = 0; f < 8; f++) {
        for (uint8_t i = 0; i <= 0x11; i++)
            i2c_write_register8(f, i, 0xff);
    }

    audio_sync(false);
}

void IS31FL3731_Driver::set_pwm(const int x, const int y, const int pwm, const uint8_t frame) {
    if ((x < 0) || (x >= 16))
        return;
    if ((y < 0) || (y >= 9))
        return;

    set_pwm_by_index(x + y * 16, pwm, frame);
}

void IS31FL3731_Driver::set_pwm_by_index(const int index, int pwm, const uint8_t frame) {
    if (pwm > 255)
        pwm = 255; // PWM 8bit

    set_led_pwm(index, pwm, frame);
}

void IS31FL3731_Driver::display_frame(uint8_t frame) {
    if (frame > 7) {
        frame = 0;
    }

    i2c_write_register8(ISSI_BANK_FUNCTIONREG, ISSI_REG_PICTUREFRAME, frame);
}

void IS31FL3731_Driver::i2c_select_bank(const uint8_t bank) const {
    const uint8_t cmd[2] = {ISSI_COMMANDREGISTER, bank};
    ESP_ERROR_CHECK(i2c_master_transmit(m_i2c_handle, cmd, sizeof(cmd), 100));
}

void IS31FL3731_Driver::i2c_write_register8(const uint8_t bank, const uint8_t reg, const uint8_t data) const {
    i2c_select_bank(bank);

    const uint8_t cmd[2] = {reg, data};
    ESP_ERROR_CHECK(i2c_master_transmit(m_i2c_handle, cmd, sizeof(cmd), 100));
}

void IS31FL3731_Driver::clear(uint8_t frame) const {
    i2c_select_bank(frame);
    uint8_t erasebuf[25] = {0};

    for (uint8_t i = 0; i < 6; i++) {
        erasebuf[0] = 0x24 + i * 24;
        i2c_master_transmit(m_i2c_handle, erasebuf, 25, 100);
    }
}

void IS31FL3731_Driver::audio_sync(bool sync) const {
    if (sync) {
        i2c_write_register8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x1);
    }
    else {
        i2c_write_register8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x0);
    }
}

void IS31FL3731_Driver::set_led_pwm(uint8_t lednum, const uint8_t pwm, const uint8_t bank) {
    constexpr auto LedCount = 144;
    constexpr auto Matrix_Size = 72;
    constexpr auto MatrixA_StartOffset = 0x24;
    constexpr auto MatrixB_StartOffset = 0x2C;
    constexpr auto Matrix_RowSize = 0x10;

    if (const auto it = m_led_index_overrides.find(lednum); it != m_led_index_overrides.end()) {
        lednum = it->second;
    }

    if (lednum >= LedCount) {
        return;
    }


    auto led_index_offset = MatrixA_StartOffset;
    if (lednum >= Matrix_Size) {
        led_index_offset = MatrixB_StartOffset;
        lednum -= Matrix_Size;
    }
    auto [matrix_row, matrix_col] = std::div(lednum, 8);

    const int index = led_index_offset + (matrix_row * Matrix_RowSize) + matrix_col;

    i2c_write_register8(bank, index, pwm);
}

