#include "real_plugin.hpp"

void RealPlugin::set_parameter(const std::string& parameter, std::any value) {
    if (parameter == "greeting") {
        m_greeting = std::any_cast<std::string>(value);
    }
    else {
        throw std::invalid_argument(parameter + ": this plugin does admit this parameter");
    }
}

std::string RealPlugin::get_message() const {
    return m_greeting + " world!";
}

REGISTER_PLUGIN(real_plugin, RealPlugin);
