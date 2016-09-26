#ifndef OSCINPUT_H
#define OSCINPUT_H

#include "objectcache.h"
#include "scripttypes.h"
#include "oscmessage.h"
#include "bindosc.h"

#include "lo/lo.h"
#include <string>
#include <stdexcept>

class OscInput : public Listable
{
    lo_server_thread st;
    std::atomic<ScriptFunction*> onReceiveHandler;
public:
    OscInput(int port);
    OscInput(int port, const char *protocol);
    int handle(const char *path, const char *types,
                lo_arg ** argv, int argc, void *data);
    const char *getUrl();
    void onReceive(ScriptFunction &function);
    void cancel();
};

class OnReceiveClosure : public ScriptFunctionClosure {
    OscMessage *message;
protected:
    bool addParameters() {
        addObject(message, binding::OscMessageObject);
    }
public:
    OnReceiveClosure(ScriptFunction function, OscMessage *message) :
        ScriptFunctionClosure(function), message(message) {}
};

class OscInputFactory : public ObjectCache
{
    std::map<std::string, OscInput*> instanceMap;
    std::set<OscInput*> activeScriptObjects;
    QueueList<OscInput> activeProcessObjects;
    boost::lockfree::spsc_queue<OscInput*> deletedObjects;
    OscInputFactory() : activeProcessObjects(16), deletedObjects(16) {}
    OscInput *createObject(std::string key);
public:
    static OscInputFactory &instance() {
        static OscInputFactory instance;
        return instance;
    }
    void process(bool rolling, jack_position_t &pos, jack_nframes_t nframes, jack_nframes_t time) {}
    bool scriptComplete();
    void reposition();
    bool repositionComplete() { return true; }
    OscInput *getOscInput(int port, const char *protocol);
    OscInput *getOscInput(int port) {
        return getOscInput(port, 0);
    }
    void shutdown();
};

#endif // OSCINPUT_H
