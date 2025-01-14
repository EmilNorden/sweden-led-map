//
// Created by Emil Nordén on 2025-01-14.
//

#ifndef LED_GRID_H
#define LED_GRID_H

#include <concepts>
#include <array>
#include "driver/i2c_master.h"
#include "led_driver.hpp"

template <typename T>
concept DerivedFromBase = std::is_base_of<LedDriverBase, T>::value;

template <DerivedFromBase LedDriver>
class LedGrid {
public:
  LedGrid(std::array<LedDriver, 4> drivers) : m_drivers(drivers) {}
private:
  std::array<LedDriver, 4> m_drivers;
};

#endif //LED_GRID_H
