//
// Created by Emil Nordén on 2025-01-14.
//

#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "driver/i2c_master.h"
#include <unordered_map>

class LedDriverBase{
public:
  virtual ~LedDriverBase() = default;

  virtual void set_pwm(int x, int y, int pwm, uint8_t frame) = 0;
  virtual void set_pwm_by_index(int index, int pwm, uint8_t frame) = 0;
  virtual void display_frame(uint8_t frame) = 0;
};

class IS31FL3731_Driver final : public LedDriverBase {
public:
  IS31FL3731_Driver(i2c_master_bus_handle_t bus, int i2c_address, const std::unordered_map<int, int>& led_index_overrides = {});
  void set_pwm(int x, int y, int pwm, uint8_t frame) override;
  void set_pwm_by_index(int index, int pwm, uint8_t frame) override;
  void display_frame(uint8_t frame) override;
private:

  void i2c_select_bank(uint8_t bank) const;
  void i2c_write_register8(uint8_t bank, uint8_t reg, uint8_t data) const;
  void clear(uint8_t frame) const;
  void audio_sync(bool sync) const;
  void set_led_pwm(uint8_t lednum, uint8_t pwm, uint8_t bank);

  i2c_master_dev_handle_t m_i2c_handle{};
  std::unordered_map<int, int> m_led_index_overrides;
};

#endif //LED_DRIVER_H
