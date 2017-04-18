#ifndef OSCINPUT_H
#define OSCINPUT_H

#include "objectcache.h"
#include "scripttypes.h"
#include "oscmessage.h"
#include "bindosc.h"

#include "lo/lo.h"
#include <atomic>
#include <string>
#include <stdexcept>

namespace bipscript {
namespace osc {

class Input : public Listable
{
    lo_server_thread st;
    std::atomic<ScriptFunction*> onReceiveHandler;
public:
    Input(int port);
    Input(int port, const char *protocol);
    int handle(const char *path, const char *types,
                lo_arg ** argv, int argc, void *data);
    const char *getUrl();
    void onReceive(ScriptFunction &function);
    void cancel() {
        lo_server_thread_free(st);
    }
};

class OnReceiveClosure : public ScriptFunctionClosure {
    Message *message;
protected:
    void addParameters() {
        addObject(message, binding::OscMessageObject);
    }
public:
    OnReceiveClosure(ScriptFunction function, Message *message) :
        ScriptFunctionClosure(function), message(message) {}
    void recycle() { delete this; }
};

class InputFactory : public ActiveCache<Input>
{
public:
    static InputFactory &instance() {
        static InputFactory instance;
        return instance;
    }
    Input *getOscInput(int port, const char *protocol);
    Input *getOscInput(int port) {
        return getOscInput(port, 0);
    }
    void newObject(Input *) {}
    void removeObject(Input *input) {
        input->cancel();
    }
};

}}

#endif // OSCINPUT_H
