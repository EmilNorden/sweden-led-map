//
// Created by Emil Nordén on 2025-01-14.
//

#include "led_grid.hpp"

#define SCL_SPEED_HZ    100 * 1000
/*
void led_grid_init(i2c_master_bus_handle_t bus, led_grid_t* led_grid) {
  const int driver_addresses[4] = {0x74, 0x77, 0x75, 0x76};

  for(int i = 0; i < 4; ++i) {
    i2c_device_config_t dev_config = {
      .dev_addr_length = I2C_ADDR_BIT_LEN_7,
      .device_address = driver_addresses[i],
      .scl_speed_hz = SCL_SPEED_HZ, // 100KHz
      .scl_wait_us = 0
    };

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &dev_config, led_grid->driver[i]));


  }
}*/