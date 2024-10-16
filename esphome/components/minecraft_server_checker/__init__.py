import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import sensor, text_sensor
from . import minecraft_ore_block_config_ns

DEPENDENCIES = ["minecraft_ore_block_config"]

minecraft_server_checker_ns = cg.esphome_ns.namespace('minecraft_server_checker')
MinecraftServerChecker = minecraft_server_checker_ns.class_('MinecraftServerChecker', cg.PollingComponent)

CONF_CONFIG_ID = "config_id"
CONF_PLAYER_COUNT_ID = "player_count_id"
CONF_SERVER_STATUS_ID = "server_status_id"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(MinecraftServerChecker),
    cv.GenerateID(CONF_CONFIG_ID): cv.use_id(minecraft_ore_block_config_ns.MinecraftOreBlockConfig),
    cv.Optional(CONF_PLAYER_COUNT_ID): cv.use_id(sensor.Sensor),
    cv.Optional(CONF_SERVER_STATUS_ID): cv.use_id(text_sensor.TextSensor),
}).extend(cv.polling_component_schema('60s'))

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
    
    config_var = yield cg.get_variable(config[CONF_CONFIG_ID])
    cg.add(var.set_config(config_var))
    
    if CONF_PLAYER_COUNT_ID in config:
        sens = yield cg.get_variable(config[CONF_PLAYER_COUNT_ID])
        cg.add(var.set_player_count_sensor(sens))
    
    if CONF_SERVER_STATUS_ID in config:
        sens = yield cg.get_variable(config[CONF_SERVER_STATUS_ID])
        cg.add(var.set_server_status_sensor(sens))
