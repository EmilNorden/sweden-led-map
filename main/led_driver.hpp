//
// Created by Emil Nordén on 2025-01-14.
//

#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "driver/i2c_master.h"

class LedDriverBase{
public:
  virtual void set_pwm(int x, int y, int pwm, uint8_t frame) = 0;
};

class IS31FL3731_Driver : public LedDriverBase {
public:
  IS31FL3731_Driver(i2c_master_bus_handle_t bus, int i2c_address);
  void set_pwm(int x, int y, int pwm, uint8_t frame);
  void display_frame(uint8_t frame);
private:

  void i2c_select_bank(uint8_t bank);
  void i2c_write_register8(uint8_t bank, uint8_t reg, const uint8_t data);
  void clear(uint8_t frame);
  void audio_sync(bool sync);
  void set_led_pwm(uint8_t lednum, uint8_t pwm, uint8_t bank);

  i2c_master_dev_handle_t m_i2c_handle;
};

#endif //LED_DRIVER_H
