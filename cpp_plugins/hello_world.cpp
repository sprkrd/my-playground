#include "plugin.hpp"
#include <iostream>

class HelloWorld : public Plugin {
    public:
        std::string get_message() const override {
            return "hello world!";
        }
};

extern "C" {
    
PluginInfo PLUGIN_INFO = { "HelloWorld",  PLUGIN_API_VERSION };

Plugin* create_new_plugin() {
    return new HelloWorld();
}

void delete_plugin(Plugin* plugin) {
    delete plugin;
}
    
    
}
