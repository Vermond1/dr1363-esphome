import esphome.codegen as cg
import esphome.config_validation as cv

dr1363_ns = cg.esphome_ns.namespace("dr1363")
DR1363Component = dr1363_ns.class_("DR1363Component", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DR1363Component),
})
