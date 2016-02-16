#ifndef METHODQUEUE_H
#define METHODQUEUE_H

#include <boost/lockfree/spsc_queue.hpp>

class ScriptFunctionClosure;

/**
 * A global queue that passes methods from process thread to script thread for
 * immediate execution (e.g. callbacks)
 *
 */
class MethodQueue
{    
    boost::lockfree::spsc_queue<ScriptFunctionClosure*> dispatchQueue; // process thread -> script thread
    MethodQueue() : dispatchQueue(512) {}
public:
    static MethodQueue &instance() {
        static MethodQueue instance;
        return instance;
    }
    void dispatch(ScriptFunctionClosure *function);
    ScriptFunctionClosure *next();
};

#endif // METHODQUEUE_H
