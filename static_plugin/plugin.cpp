#include "plugin.hpp"
#include <stdexcept>

PluginRegistry& get_plugin_registry() {
    static PluginRegistry plugin_registry;
    return plugin_registry;
}

bool add_plugin(const std::string& plugin_name, PluginFactory factory_function) {
    PluginRegistry& registry = get_plugin_registry();
    if (registry.find(plugin_name) != registry.end()) {
        return false;
    }
    registry.emplace(plugin_name, factory_function);
    return true;
}

std::vector<std::string> get_plugin_list() {
    std::vector<std::string> list;
    for (const auto&[plugin_name,_] : get_plugin_registry()) {
        list.push_back(plugin_name);
    }
    return list;
}

PluginPtr create_plugin(const std::string& plugin_name) {
    PluginRegistry& registry = get_plugin_registry();
    auto it = registry.find(plugin_name);
    if (it == registry.end()) {
        throw std::runtime_error(plugin_name + ": non-existent plugin");
    }
    return (it->second)();
}
