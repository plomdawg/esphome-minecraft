import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, text_sensor
from esphome.const import CONF_ID, CONF_UPDATE_INTERVAL, UNIT_EMPTY, ICON_ACCOUNT_GROUP, ICON_SERVER_NETWORK

DEPENDENCIES = ['network']
AUTO_LOAD = ['sensor', 'text_sensor']

minecraft_ns = cg.esphome_ns.namespace('minecraft')
MinecraftServerChecker = minecraft_ns.class_('MinecraftServerChecker', cg.Component)

CONF_SERVER_ADDRESS = 'server_address'
CONF_SERVER_PORT = 'server_port'
CONF_PLAYER_COUNT = 'player_count'
CONF_SERVER_STATUS = 'server_status'

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(MinecraftServerChecker),
    cv.Optional(CONF_UPDATE_INTERVAL, default='60s'): cv.update_interval,
    cv.Required(CONF_SERVER_ADDRESS): cv.string,
    cv.Required(CONF_SERVER_PORT): cv.port,
    cv.Optional(CONF_PLAYER_COUNT): sensor.sensor_schema(
        unit_of_measurement=UNIT_EMPTY,
        icon=ICON_ACCOUNT_GROUP,
        accuracy_decimals=0
    ),
    cv.Optional(CONF_SERVER_STATUS): text_sensor.text_sensor_schema(
        icon=ICON_SERVER_NETWORK
    ),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_update_interval(config[CONF_UPDATE_INTERVAL]))
    cg.add(var.set_server_address(config[CONF_SERVER_ADDRESS]))
    cg.add(var.set_server_port(config[CONF_SERVER_PORT]))

    if CONF_PLAYER_COUNT in config:
        sens = await sensor.new_sensor(config[CONF_PLAYER_COUNT])
        cg.add(var.set_player_count_sensor(sens))

    if CONF_SERVER_STATUS in config:
        sens = await text_sensor.new_text_sensor(config[CONF_SERVER_STATUS])
        cg.add(var.set_server_status_sensor(sens))

    # Add library dependencies
    cg.add_library("ArduinoJson", "6.19.4")
