import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import uart
from esphome.const import CONF_ID

from . import DR1363Component

AUTO_LOAD = ["uart"]

dr1363_ns = cg.esphome_ns.namespace("dr1363")

# BELANGRIJK: UART schema eerst, daarna je eigen ID
CONFIG_SCHEMA = uart.UART_DEVICE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(DR1363Component),
})

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
