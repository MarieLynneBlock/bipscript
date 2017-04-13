#include "extension.h"

#include <iostream>
#include <dlfcn.h>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace bipscript {

Extension::Extension(const std::string &path)
{
    handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle)
    {
        throw std::logic_error(dlerror());
    }
}

Extension::~Extension()
{
    dlclose(handle);
}

void Extension::bind(HSQUIRRELVM vm)
{
    void (*fn)(HSQUIRRELVM) = (void(*)(HSQUIRRELVM))dlsym(handle, "bindAll");
    char *error;
    if ((error = dlerror()) != NULL)
    {
        throw std::logic_error(error);
    }
    (*fn)(vm);
}

void Extension::shutdown()
{
    void (*fn)() = (void(*)())dlsym(handle, "shutdown");
    char *error;
    if ((error = dlerror()) != NULL)
    {
        throw std::logic_error(error);
    }
    (*fn)();
}

ExtensionManager::ExtensionManager()
{
    fs::path libPath("/usr/local/lib/bipscript/ext");
    if(fs::exists(libPath)) {
        fs::directory_iterator end_itr;
        for (fs::directory_iterator itr(libPath); itr != end_itr; ++itr )
        {
            if (!fs::is_directory(itr->status()) )
            {
                Extension *ext = new Extension(itr->path().string());
                extensions.push_back(ext);
            }
        }
    }
}

void ExtensionManager::bindAll(HSQUIRRELVM vm)
{
    for(int i = 0; i < extensions.size(); i++) {
        extensions[i]->bind(vm);
    }
}

void ExtensionManager::shutdown()
{
    for(int i = 0; i < extensions.size(); i++) {
        extensions[i]->shutdown();
    }
}

bool ExtensionManager::scriptComplete()
{
    bool active = false;
    // TODO: call on extensions
    return active;
}

}