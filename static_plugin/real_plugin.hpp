#pragma once

#include "plugin.hpp"

class RealPlugin : public PluginCRTP<RealPlugin> {
    public:
        RealPlugin() : m_greeting("Hello") {
        }
    
        void set_parameter(const std::string& parameter, std::any value) override;
        
        virtual std::string get_message() const override;
    
    private:
    
        std::string m_greeting;
};




