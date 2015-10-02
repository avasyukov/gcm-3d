#ifndef GCM_3D_PLUGIN_HPP
#define GCM_3D_PLUGIN_HPP

namespace xml
{
    class Doc;
}

namespace gcm
{
    class Engine;

    class Plugin
    {
    public:
        virtual ~Plugin();
        virtual void parseTask(xml::Doc& doc);
        virtual void onCalculationStepDone();

    };
};

typedef gcm::Plugin* (*gcm_plugin_create_t)(gcm::Engine*);

#endif //GCM_3D_PLUGIN_HPP
