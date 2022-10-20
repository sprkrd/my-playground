#pragma once

#include <any>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#define REGISTER_PLUGIN(plugin_name,PluginClass) bool plugin_name ## _registered = add_plugin(#plugin_name,PluginClass::create)

class Plugin;
typedef std::shared_ptr<Plugin> PluginPtr;
typedef std::shared_ptr<const Plugin> PluginConstPtr;
typedef std::function<PluginPtr()> PluginFactory;
typedef std::unordered_map<std::string, PluginFactory> PluginRegistry;

PluginRegistry& get_plugin_registry();

bool add_plugin(const std::string& plugin_name, PluginFactory factory_function);

std::vector<std::string> get_plugin_list();

PluginPtr create_plugin(const std::string& plugin_name);

class Plugin {
    public:
    
        virtual void set_parameter(const std::string& parameter, std::any value) = 0;
    
        virtual std::string get_message() const = 0;
    
        virtual ~Plugin() = default;
};

template <class Derived>
class PluginCRTP : public Plugin {
    public:
        static PluginPtr create() {
            return std::make_shared<Derived>();
        }
};


