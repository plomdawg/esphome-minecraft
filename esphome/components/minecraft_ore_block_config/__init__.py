import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["network"]

minecraft_ore_block_config_ns = cg.esphome_ns.namespace('minecraft_ore_block_config')
MinecraftOreBlockConfig = minecraft_ore_block_config_ns.class_('MinecraftOreBlockConfig', cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(MinecraftOreBlockConfig)
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    yield cg.register_component(var, config)
