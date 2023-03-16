//
// Created by MorbidArk on 2023/3/1.
//

#pragma once

#include "ObjFn.hpp"

class ObjThread
{
  public:
    headerObj *objHeader;
    
    Value *stack;  //运行时栈的栈底
    Value *esp;    //运行时栈的栈顶
    uint32_t stackCapacity;  //栈容量
    
    Frame *frames;   //调用框架
    uint32_t usedFrameNum;   //已使用的frame数量
    uint32_t frameCapacity;  //frame容量
    
    //"打开的upvalue"的链表首结点
    ObjUpvalue *openUpvalues;
    
    //当前thread的调用者
    ObjThread *caller;
    
    //导致运行时错误的对象会放在此处,否则为空
    Value errorObj;
    
    ObjThread(VM *vm, ObjClosure *objClosure);
    
    static void prepareFrame(ObjThread *objThread, ObjClosure *objClosure, Value *stackStart);
    static ObjThread *newObjThread(VM *vm, ObjClosure *objClosure);
    static void resetThread(ObjThread *objThread, ObjClosure *objClosure);
    
};