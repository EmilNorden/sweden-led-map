//
// Created by Emil Nordén on 2025-01-19.
//

#include "sun_position.h"
#include "led_grid.hpp"
#include "led_driver.hpp"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include <cmath>
extern "C" {
#include "solpos00.h"
}
#include "driver/i2c_master.h"

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

extern "C" void sun_position_main(void *pvParameters) {
  //auto grid = (LedGrid<IS31FL3731_Driver, 4> *)pvParameters;
  /*auto bus_handle = (i2c_master_bus_t *)pvParameters;

  i2c_device_config_t dev_config;
  dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  dev_config.device_address = 0x77;
  dev_config.scl_speed_hz = 100 * 1000; // 100KHz
  dev_config.scl_wait_us = 0;

  // Connected to ground  1110100 -> 0x74
  // Connected to VCC     1110111 -> 0x77
  // Connected to SCL     1110101 -> 0x75
  // Connected to SDA     1110110 -> 0x76

  i2c_master_dev_handle_t i2c_handle;
 ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_config, &i2c_handle));*/

  // shutdown

  while(true) {


    //const uint8_t cmd[2] = {0x0A, 0x00};
    //ESP_ERROR_CHECK(i2c_master_transmit(i2c_handle, cmd, sizeof(cmd), 100));
    vTaskDelay(10 / portTICK_PERIOD_MS);
    //ESP_LOGI("sun", "WAIT Grid address is %lu", (unsigned long)grid);
    //grid->set_led(15, 0, 0xA);
    //grid->present();
    //grid->next_frame();
  }
  //
  /*int hour = 15;
  int minute = 12;
  while (true) {
    for (const auto&[x, y] : grid->positions()) {
      auto [lat, lon] = coordinate_to_lat_long(x, y);

      posdata pd{};

      S_init(&pd);
      pd.latitude = lat;
      pd.longitude = lon;
      pd.timezone = 1;
      pd.year = 2025;
      pd.daynum = 19;
      pd.hour = hour;
      pd.minute = minute;
      pd.second = 0;
      pd.temp = 10.0;
      pd.press = 1006.0;
      pd.tilt = 0;
      pd.function = S_ZENETR;

      long retval = S_solpos(&pd);

      float zenith_ratio = pd.zenetr / 96.0f;
      if (zenith_ratio < 0) {
        zenith_ratio = 0;
      }
      if (zenith_ratio > 1) {
        zenith_ratio = 1;
      }

      zenith_ratio = 1.0f - zenith_ratio;

      auto pwm = static_cast<uint8_t>(zenith_ratio * 0xFF);

      grid->set_led(x, y, pwm);
    }
    grid->present();
    //vTaskDelay(10 / portTICK_PERIOD_MS);
    //hour = (hour + 1) % 24;
    vTaskDelay(60000 / portTICK_PERIOD_MS);
    minute = minute + 1;
    if (minute >= 60) {
      minute = 0;
      hour = (hour + 1) % 24;
    }
  }*/
}