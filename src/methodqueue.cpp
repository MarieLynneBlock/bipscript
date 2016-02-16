#include "methodqueue.h"


void MethodQueue::dispatch(ScriptFunctionClosure *function)
{
    // don't do this in process thread!
    // TODO: make a waiting queue instead
    while(!dispatchQueue.push(function));
}

ScriptFunctionClosure *MethodQueue::next() {
    ScriptFunctionClosure *function;
    return dispatchQueue.pop(function) ? function : 0;
}
