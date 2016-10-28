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
    void cancel() {
        lo_server_thread_free(st);
    }
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
    void recycle() { delete this; }
};

class OscInputFactory : public ActiveCache<OscInput>
{
public:
    static OscInputFactory &instance() {
        static OscInputFactory instance;
        return instance;
    }
    OscInput *getOscInput(int port, const char *protocol);
    OscInput *getOscInput(int port) {
        return getOscInput(port, 0);
    }
    void newObject(OscInput *) {}
    void removeObject(OscInput *input) {
        input->cancel();
    }
};

#endif // OSCINPUT_H
