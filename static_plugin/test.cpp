#include <iostream>
#include "plugin.hpp"
using namespace std;

int main() {
    auto plugin_list = get_plugin_list();
    if (plugin_list.empty()) {
        cout << "no plugins" << endl;
    }
    else {
        for (const auto& plugin : get_plugin_list()) {
            cout << plugin << endl;
        }
    }
    
    auto plugin = create_plugin("real_plugin");
    
    plugin->set_parameter("greeting", std::string("Bye"));
    
    cout << plugin->get_message() << endl;
}
