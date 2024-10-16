import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import light, sensor
from . import minecraft_ore_block_config_ns

DEPENDENCIES = ["minecraft_ore_block_config", "light"]

minecraft_led_control_ns = cg.esphome_ns.namespace('minecraft_led_control')
MinecraftLEDControl = minecraft_led_control_ns.class_('MinecraftLEDControl', cg.Component)

CONF_CONFIG_ID = "config_id"
CONF_LIGHT_ID = "light_id"
CONF_PLAYER_COUNT_SENSOR_ID = "player_count_sensor_id"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(MinecraftLEDControl),
    cv.GenerateID(CONF_CONFIG_ID): cv.use_id(minecraft_ore_block_config_ns.MinecraftOreBlockConfig),
    cv.GenerateID(CONF_LIGHT_ID): cv.use_id(light.LightState),
    cv.GenerateID(CONF_PLAYER_COUNT_SENSOR_ID): cv.use_id(sensor.Sensor),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    
    config_var = yield cg.get_variable(config[CONF_CONFIG_ID])
    cg.add(var.set_config(config_var))
    
    light_var = yield cg.get_variable(config[CONF_LIGHT_ID])
    cg.add(var.set_light(light_var))
    
    sensor_var = yield cg.get_variable(config[CONF_PLAYER_COUNT_SENSOR_ID])
    cg.add(var.set_player_count_sensor(sensor_var))
