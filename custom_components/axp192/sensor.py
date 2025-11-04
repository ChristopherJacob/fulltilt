import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, binary_sensor
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL

DEPENDENCIES = ["i2c"]

axp192_ns = cg.esphome_ns.namespace("axp192")
AXP192Sensor = axp192_ns.class_("AXP192Sensor", cg.PollingComponent)

CONF_BATTERY_VOLTAGE = "battery_voltage"
CONF_BATTERY_PERCENT = "battery_percent"
CONF_CHARGING = "charging"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(AXP192Sensor),
    cv.Optional(CONF_UPDATE_INTERVAL, default="5s"): cv.update_interval,
    cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
        unit_of_measurement="V",
        accuracy_decimals=2,
        device_class="voltage"
    ),
    cv.Optional(CONF_BATTERY_PERCENT): sensor.sensor_schema(
        unit_of_measurement="%",
        accuracy_decimals=0,
        device_class="battery"
    ),
    cv.Optional(CONF_CHARGING): binary_sensor.binary_sensor_schema(device_class="battery_charging"),
})

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    if CONF_BATTERY_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_VOLTAGE])
        cg.add(var.set_battery_voltage_sensor(sens))
    if CONF_BATTERY_PERCENT in config:
        sens = await sensor.new_sensor(config[CONF_BATTERY_PERCENT])
        cg.add(var.set_battery_percent_sensor(sens))
    if CONF_CHARGING in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_CHARGING])
        cg.add(var.set_charging_sensor(sens))

