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
    OscMessage *message = new OscMessage(path);
    for (int i = 0; i < argc; i++) {
        switch(types[i]) {
        case 'i':
            message->addInteger(*(int32_t *)argv[i]);
            break;
        case 'f':
            message->addFloat(*(float*)argv[i]);
            break;
        case 's':
            message->addString((const char *)argv[i]);
            break;
        case 'T':
            message->addBoolean(true);
            break;
        case 'F':
            message->addBoolean(false);
            break;
        case 'N':
            message->addNull(0);
            break;
        default:
            break;
        }
    }
    ScriptFunction *handler = onReceiveHandler.load();
    if(handler) {
        (new OnReceiveClosure(*handler, message))->dispatch();
    }
}

const char *OscInput::getUrl()
{
    return lo_server_thread_get_url(st);
}

void OscInput::onReceive(ScriptFunction &handler)
{
    if(handler.getNumargs() != 2) {
        throw std::logic_error("onReceive handler should take one argument");
    }
    onReceiveHandler.store(new ScriptFunction(handler));
}

/**
 * script thread
 */
OscInput *OscInputFactory::getOscInput(int port, const char *protocol)
{
    // TODO: needs better hash
    int key = protocol ? std::hash<std::string>()(protocol) : port;
    OscInput *obj = findObject(key);
    if (!obj) {
        obj = new OscInput(port, protocol);
        registerObject(key, obj);
    }
    return obj;
}
