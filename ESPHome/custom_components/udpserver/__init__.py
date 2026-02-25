import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart
from esphome.const import (
    CONF_ID,
    CONF_TRIGGER_ID,
)

CONF_ALLOWED_IPS = "allowed_ips"
CONF_TEXT_FILTER = "text_filter"
CONF_FILTER_MODE = "filter_mode"

DEPENDENCIES = ['network']

udpserver_ns = cg.esphome_ns.namespace('udpserver')
UdpserverComponent = udpserver_ns.class_('UdpserverComponent', cg.Component)
UDPContext = udpserver_ns.class_("UDPContext")
OnStringDataTrigger = udpserver_ns.class_("OnStringDataTrigger",
                                 automation.Trigger.template(cg.std_string, UDPContext))


CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(UdpserverComponent),
    cv.Required("listen_port"): cv.int_range(0, 65535),
    cv.Optional(CONF_ALLOWED_IPS): cv.ensure_list(cv.string),
    cv.Optional("on_string_data"): automation.validate_automation(
        {
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnStringDataTrigger),
            cv.Optional(CONF_TEXT_FILTER): cv.string,
            cv.Optional(CONF_FILTER_MODE): cv.enum(
                {
                    "NONE": "none",
                    "CONTAINS": "contains",
                    "STARTS_WITH": "starts_with",
                    "ENDS_WITH": "ends_with",
                    "EQUALS": "equals",
                },
                upper=True,
            ),
        }
    ),
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add(var.set_listen_port(config["listen_port"]))
    yield cg.register_component(var, config)

    # Configure IP filtering
    if CONF_ALLOWED_IPS in config:
        for ip in config[CONF_ALLOWED_IPS]:
            cg.add(var.add_allowed_ip(ip))
        cg.add(var.set_allow_all_ips(False))

    for conf in config.get("on_string_data", []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        cg.add(var.add_string_trigger(trigger))

        # Configure text filtering
        if CONF_TEXT_FILTER in conf:
            cg.add(trigger.set_text_filter(conf[CONF_TEXT_FILTER]))
            filter_mode = conf.get(CONF_FILTER_MODE, "contains")
            cg.add(trigger.set_filter_mode(filter_mode))

        yield automation.build_automation(trigger, [(cg.std_string, "data"), (UDPContext, "udp")], conf)
