#ifndef EXTENSION_H
#define EXTENSION_H

#include "squirrel.h"
#include "jack/jack.h"
#include <vector>
#include <string>

class Extension
{
    void *handle;
public:
    Extension(const std::string &);
    ~Extension();
    void bind(HSQUIRRELVM vm);
    void shutdown();
};

class ExtensionManager
{
    std::vector<Extension*> extensions;
    ExtensionManager();
public:
    static ExtensionManager &instance() {
        static ExtensionManager instance;
        return instance;
    }
    // void setAudioClient(jack_client_t *client);
    void bindAll(HSQUIRRELVM vm);
    void shutdown();
    bool scriptComplete();
};

#endif // EXTENSION_H
