#include "oscinput.h"

void osc_error(int num, const char *msg, const char *path)
{
    std::string mesg("osc input error ");
    mesg.append(std::to_string(num));
    mesg.append(": ");
    mesg.append(msg);
    throw std::logic_error(mesg);
}

int osc_handler(const char *path, const char *types, lo_arg ** argv,
                    int argc, void *data, void *user_data)
{
   return ((OscInput*)user_data)->handle(path, types, argv, argc, data);
}

OscInput::OscInput(int port, const char *protocol)
{
    if(protocol) {
        // TODO: implement
        throw std::logic_error("OSC protocol not yet implemented");
    }
    const char *portString = std::to_string(port).c_str();
    st = lo_server_thread_new(portString, osc_error);
    lo_server_thread_add_method(st, NULL, NULL, osc_handler, this);
    lo_server_thread_start(st);
}

int OscInput::handle(const char *path, const char *types, lo_arg **argv, int argc, void *data)
{
    ScriptFunction *handler = onReceiveHandler.load();
    if(handler) {
        (new OnReceiveClosure(*handler, path))->dispatch();
    }
}

const char *OscInput::getUrl()
{
    return lo_server_thread_get_url(st);
}

void OscInput::onReceive(ScriptFunction &handler)
{
    if(handler.getNumargs() != 2) {
        throw std::logic_error("onOnset handler should take one argument");
    }
    onReceiveHandler.store(new ScriptFunction(handler));
}

void OscInput::cancel() {
    lo_server_thread_free(st);
}

/**
 * Called after the script has completed so factory can reset for the next run.
 *
 * Runs in the script thread.
 */
bool OscInputFactory::scriptComplete()
{
    bool activeObjects = false;
    // loop over cached ports
    for(auto iterator = instanceMap.begin(); iterator != instanceMap.end();) {
        OscInput *obj = iterator->second;
        // was used in the last script run?
        auto it = activeScriptObjects.find(obj);
        if(it != activeScriptObjects.end()) {
            activeScriptObjects.erase(it);
            activeObjects = true;
            iterator++;
        } else {
            // remove port from map
            iterator = instanceMap.erase(iterator);
            // delete
            while(!deletedObjects.push(obj));
        }
    }
    return activeObjects;
}

/**
 * Runs in the process thread.
 */
void OscInputFactory::reposition()
{
    OscInput *done;
    while(deletedObjects.pop(done)) {
        activeProcessObjects.remove(done);
        done->cancel();
    }
}

/**
 * script thread
 */
OscInput *OscInputFactory::getOscInput(int port, const char *protocol)
{
    // create key
    std::string key(protocol ? protocol : "");
    key.append(std::to_string(port));
    // check instance map
    OscInput *obj = instanceMap[key];
    if (!obj) {
        obj = new OscInput(port, protocol);
        instanceMap[key] = obj;
        activeProcessObjects.add(obj);
    }
    activeScriptObjects.insert(obj);
    return obj;
}
