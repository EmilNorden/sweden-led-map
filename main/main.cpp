
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

constexpr float MinLatitude = 69.0;
constexpr float MaxLatitude = 55.43;
constexpr float MinLongitude = 11.16;
constexpr float MaxLongitude = 23.9;
constexpr float LatitudeRange = MaxLatitude - MinLatitude;
constexpr float LongitudeRange = MaxLongitude - MinLongitude;

constexpr float MinX = 0.0;
constexpr float MaxX = 20.0;
constexpr float MinY = 0.0;
constexpr float MaxY = 47.0;
constexpr float XRange = MaxX - MinX;
constexpr float YRange = MaxY - MinY;

std::pair<int, int> lat_long_to_coordinate(const float lat, const float lon) {
    float lon_ratio = (lon - MinLongitude) / LongitudeRange;
    float lat_ratio = (lat - MinLatitude) / LatitudeRange;

    float x = MinX + (XRange * lon_ratio);
    float y = MinY + (YRange * lat_ratio);

    return {static_cast<int>(std::round(x)), static_cast<int>(std::round(y)) };
}

std::pair<float, float> coordinate_to_lat_long(const int x, const int y) {
    // Convert the x-coordinate back to longitude
    float lon_ratio = (static_cast<float>(x) - MinX) / XRange;
    float lon = MinLongitude + (LongitudeRange * lon_ratio);

    // Convert the y-coordinate back to latitude
    float lat_ratio = (static_cast<float>(y) - MinY) / YRange;
    float lat = MinLatitude + (LatitudeRange * lat_ratio);

    return {lat, lon};
}

i2c_master_bus_handle_t init_i2c() {
    i2c_master_bus_handle_t bus_handle;
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

    ESP_LOGI(TAG, "I2C initialized successfully");

    return bus_handle;
}

LedGrid<IS31FL3731_Driver, 4>  init_led_grid(i2c_master_bus_handle_t i2c_bus) {
    const IS31FL3731_Driver driver1(i2c_bus, 0x74,
        {
            {10, 16},
            {11, 40},
            {12, 32},
            {13, 56},
            {14, 48},
            {15, 64},
            {16, 10},
            {32, 12},
            {40, 11},
            {48, 14},
            {56, 13},
            {64, 15}
        });
    const IS31FL3731_Driver driver2(i2c_bus, 0x77);
    const IS31FL3731_Driver driver3(i2c_bus, 0x75);
    const IS31FL3731_Driver driver4(i2c_bus, 0x76);

    auto grid = LedGrid<IS31FL3731_Driver, 4>({driver1, driver2, driver3, driver4});

    ESP_LOGI(TAG, "LED Grid initialized successfully");

    return grid;
}

extern "C" int app_main(void) {
    const auto bus_handle = init_i2c();
    auto grid = init_led_grid(bus_handle);

    auto [x, y] = lat_long_to_coordinate(57.719528, 11.890335);
    grid.set_led(x, y, 0xA, 0);
    grid.set_led(16, 0, 0xA, 0);

    return 0;
}
