#include <dlfcn.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

#define PLUGIN_API_VERSION 1


class Plugin {
    public:
        virtual std::string get_message() const = 0;
        
        virtual ~Plugin() = default;
};

typedef Plugin* (*PluginFactory)();
typedef void (*PluginDeleter)(Plugin*);

typedef std::shared_ptr<Plugin> PluginPtr;

struct PluginInfo {
    const char* plugin_name;
    int plugin_api_version;
};

struct PluginEntry {
    PluginInfo* plugin_info;
    void* dl_handler;
    PluginFactory factory_function;
    PluginDeleter deleter_function;
    std::string filename;
    int refcount;
};

class PluginRegistry {
    
    public:
    
        static PluginRegistry& get_registry() {
            static PluginRegistry registry;
            return registry;
        }
        
        PluginRegistry(const PluginRegistry& other) = delete;
        
        PluginRegistry(PluginRegistry&& other) = delete;
        
        PluginRegistry& operator=(const PluginRegistry& other) = delete;
        
        PluginRegistry& operator=(PluginRegistry&& other) = delete;
        
        void load(const std::string& filename) {
            void* dl_handler = dlopen(filename.c_str(), RTLD_LAZY);
            check_dl_error(dl_handler, filename);
            
            auto info = reinterpret_cast<PluginInfo*>(dlsym(dl_handler, "PLUGIN_INFO"));
            check_dl_error(dl_handler, filename);
            
            if (info->plugin_api_version != PLUGIN_API_VERSION) {
                auto error_msg = filename + ": compiled with different plugin API version ("+
                    std::to_string(PLUGIN_API_VERSION) + " vs " + std::to_string(info->plugin_api_version) + ")";
                dlclose(dl_handler);
                throw std::runtime_error(std::move(error_msg));
            }
            
            std::string name(info->plugin_name);
            
            PluginEntry entry;
            entry.plugin_info = info;
            entry.dl_handler = dl_handler;
            
            entry.factory_function = reinterpret_cast<PluginFactory>(dlsym(dl_handler, "create_new_plugin"));
            check_dl_error(dl_handler, filename);
            
            entry.deleter_function = reinterpret_cast<PluginDeleter>(dlsym(dl_handler, "delete_plugin"));
            check_dl_error(dl_handler, filename);
            
            entry.filename = filename;
            entry.refcount = 0;
            
            bool registered = m_registered_plugins.emplace(name, entry).second;
            if (!registered) {
                dlclose(dl_handler);
                throw std::runtime_error(filename + ": duplicated plugin name (" + name + ")");
            }
        }
        
        void unload(const std::string& plugin_name) {
            auto& entry = get_entry(plugin_name);
            if (entry.refcount != 0) {
                throw std::runtime_error(plugin_name + ": there still exist instance(s) of this plugin in memory");
            }
            m_registered_plugins.erase(plugin_name);
            
        }
        
        PluginPtr create(const std::string& plugin_name) {
            // TODO thread safety?
            
            auto& entry = get_entry(plugin_name);
            
            PluginPtr new_plugin(entry.factory_function(), [&](Plugin* ptr) {
                --entry.refcount;
                entry.deleter_function(ptr);
            });
            ++entry.refcount;

            return new_plugin;
        }
        
        int refcount(const std::string& plugin_name) {
            auto entry = get_entry(plugin_name);
            return entry.refcount;
        }
    
    
    private:
    
        PluginEntry& get_entry(const std::string& name) {
            auto it = m_registered_plugins.find(name);
            if (it == m_registered_plugins.end()) {
                throw std::runtime_error(name + ": plugin not loaded");
            }
            return it->second;
        }
    
        void check_dl_error(void* dl_handler, const std::string& head) {
            char* error = dlerror();
            if (error != NULL) {
                if (dl_handler != NULL) {
                    dlclose(dl_handler);
                }
                throw std::runtime_error(head + ": " + error);
            }
        }
    
        PluginRegistry() = default;
        
        std::unordered_map<std::string, PluginEntry> m_registered_plugins;
    
};

namespace detail {
    
    
struct PluginEntry {
    void* dl_handler;
    std::function<PluginPtr()> factory_function;
};
    
}

