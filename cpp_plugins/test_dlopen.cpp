#include <dlfcn.h>
#include <iostream>

#include "plugin.hpp"
using namespace std;

typedef const char*(*MessageFunction)();

int main() {
    auto& registry = PluginRegistry::get_registry();
    registry.load("./libhello_world.so");
    auto plugin = registry.create("HelloWorld");
    cout << "refcount: " << registry.refcount("HelloWorld") << endl;
    cout << plugin->get_message() << endl;
    plugin.reset();
    registry.unload("HelloWorld");
    cout << "refcount: " << registry.refcount("HelloWorld") << endl;
    
}
