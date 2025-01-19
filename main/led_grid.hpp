//
// Created by Emil Nordén on 2025-01-14.
//

#ifndef LED_GRID_H
#define LED_GRID_H

#include <concepts>
#include <array>
#include <unordered_map>
#include "driver/i2c_master.h"
#include "led_driver.hpp"
#include "led_physical_config.h"

template <typename T>
concept DerivedFromBase = std::is_base_of_v<LedDriverBase, T>;

struct LedIdentifier {
  size_t driver_index;
  size_t led_index;
};

template <DerivedFromBase LedDriver, size_t NumDrivers>
class LedGrid {
public:

  explicit LedGrid(std::array<LedDriver, NumDrivers> drivers)
  : m_drivers(drivers) {

    init_led_driver(0, driver1_leds);
    init_led_driver(1, driver2_leds);
    init_led_driver(2, driver3_leds);
    init_led_driver(3, driver4_leds);
  }


  void set_led(const int x, const int y, uint8_t pwm, uint8_t frame) {
    const int index = (y*LedGridWidth) + x;
    const auto it = m_leds.find(index);
    if (it == m_leds.end()) {
      return;
    }

    m_drivers[it->second.driver_index].set_pwm_by_index(it->second.led_index, pwm, frame);
  }

private:
  void init_led_driver(const size_t driver_index, uint8_t driver_leds[][LedGridWidth]) {
    size_t led_index = 0;
    for (int y = 0; y < Driver_LedGridHeights[driver_index]; ++y) {
      for (int x = 0; x < LedGridWidth; ++x) {
        if (driver_leds[y][x] == 1) {
          int global_y = Driver_YOffsets[driver_index] + y;
          m_leds.emplace((global_y * LedGridWidth) + x, LedIdentifier{driver_index, led_index++});
        }
      }
    }
  }

  std::array<LedDriver, NumDrivers> m_drivers;
  std::unordered_map<int, LedIdentifier> m_leds;
};

#endif //LED_GRID_H
