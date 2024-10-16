import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID, UNIT_EMPTY, ICON_ACCOUNT_GROUP

from . import MinecraftServerChecker, CONF_MINECRAFT_SERVER_CHECKER_ID

DEPENDENCIES = ["minecraft_server_checker"]

CONFIG_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_EMPTY,
    icon=ICON_ACCOUNT_GROUP,
    accuracy_decimals=0,
).extend({
    cv.GenerateID(CONF_MINECRAFT_SERVER_CHECKER_ID): cv.use_id(MinecraftServerChecker),
})

async def to_code(config):
    parent = await cg.get_variable(config[CONF_MINECRAFT_SERVER_CHECKER_ID])
    var = await sensor.new_sensor(config)
    cg.add(parent.set_player_count_sensor(var))
