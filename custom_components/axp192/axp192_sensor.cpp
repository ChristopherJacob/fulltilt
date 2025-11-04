#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include <Wire.h>

#define AXP192_ADDR 0x34

using namespace esphome;

class AXP192Sensor : public PollingComponent {
 public:
  sensor::Sensor *battery_voltage{nullptr};
  sensor::Sensor *battery_percent{nullptr};
  binary_sensor::BinarySensor *charging{nullptr};

  void set_battery_voltage_sensor(sensor::Sensor *s) { battery_voltage = s; }
  void set_battery_percent_sensor(sensor::Sensor *s) { battery_percent = s; }
  void set_charging_sensor(binary_sensor::BinarySensor *s) { charging = s; }

  AXP192Sensor() : PollingComponent(5000) {}

  void setup() override {
    Wire.begin(21, 22);
  }

  void update() override {
    // --- Battery voltage ---
    Wire.beginTransmission(AXP192_ADDR);
    Wire.write(0x78);
    Wire.endTransmission(false);
    Wire.requestFrom(AXP192_ADDR, 2);
    if (Wire.available() < 2) return;
    uint16_t vh = (Wire.read() << 4) | (Wire.read() & 0x0F);
    float vbat = vh * 1.1f / 1000.0f;

    // --- Charging status ---
    Wire.beginTransmission(AXP192_ADDR);
    Wire.write(0x00);
    Wire.endTransmission(false);
    Wire.requestFrom(AXP192_ADDR, 1);
    if (!Wire.available()) return;
    uint8_t status = Wire.read();
    bool chg = status & 0x04;

    // --- Derived battery percent ---
    float pct = (vbat - 3.3f) / (4.2f - 3.3f) * 100.0f;
    if (pct < 0.0f) pct = 0.0f;
    if (pct > 100.0f) pct = 100.0f;

    // --- Publish states ---
    if (battery_voltage != nullptr)
      battery_voltage->publish_state(vbat);
    if (battery_percent != nullptr)
      battery_percent->publish_state(pct);
    if (charging != nullptr)
      charging->publish_state(chg);
  }
};
