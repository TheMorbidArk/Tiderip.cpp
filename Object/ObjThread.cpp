//
// Created by MorbidArk on 2023/3/1.
//

#include "ObjThread.hpp"
#include "vm.hpp"

ObjThread::ObjThread(VM *vm, ObjClosure *objClosure)
{
    ASSERT(objClosure != NULL, "objClosure is NULL!");
    
    Frame *frames = ALLOCATE_ARRAY(vm, Frame, INITIAL_FRAME_NUM);
    
    //加1是为接收者的slot
    uint32_t stackCapacity = ceilToPowerOf2(objClosure->fn->maxStackSlotUsedNum + 1);
    Value *newStack = ALLOCATE_ARRAY(vm, Value, stackCapacity);
    
    this->objHeader = new headerObj(vm, ObjType::OT_THREAD, vm->threadClass);
    
    this->frames = frames;
    this->frameCapacity = INITIAL_FRAME_NUM;
    this->stack = newStack;
    this->stackCapacity = stackCapacity;
    
    resetThread(this, objClosure);
}

void ObjThread::prepareFrame(ObjThread *objThread, ObjClosure *objClosure, Value *stackStart)
{
    ASSERT(objThread->frameCapacity > objThread->usedFrameNum, "frame not enough!!");
    //objThread->usedFrameNum是最新可用的frame
    Frame *frame = &(objThread->frames[objThread->usedFrameNum++]);
    
    //thread中的各个frame是共享thread的stack
    //frame用frame->stackStart指向各自frame在thread->stack中的起始地址
    frame->stackStart = stackStart;
    frame->closure = objClosure;
    frame->ip = objClosure->fn->instrStream.datas;
}

ObjThread *ObjThread::newObjThread(VM *vm, ObjClosure *objClosure)
{
    ASSERT(objClosure != NULL, "objClosure is NULL!");
    
    Frame *frames = ALLOCATE_ARRAY(vm, Frame, INITIAL_FRAME_NUM);
    
    //加1是为接收者的slot
    uint32_t stackCapacity = ceilToPowerOf2(objClosure->fn->maxStackSlotUsedNum + 1);
    Value *newStack = ALLOCATE_ARRAY(vm, Value, stackCapacity);
    
    ObjThread *objThread = ALLOCATE(vm, ObjThread);
    objThread->objHeader = new headerObj(vm, ObjType::OT_THREAD, vm->threadClass);
    
    objThread->frames = frames;
    objThread->frameCapacity = INITIAL_FRAME_NUM;
    objThread->stack = newStack;
    objThread->stackCapacity = stackCapacity;
    
    resetThread(objThread, objClosure);
    return objThread;
}

void ObjThread::resetThread(ObjThread *objThread, ObjClosure *objClosure)
{
    objThread->esp = objThread->stack;
    objThread->openUpvalues = nullptr;
    objThread->caller = nullptr;
    objThread->errorObj = VT_TO_VALUE(ValueType::VT_NULL);
    objThread->usedFrameNum = 0;
    
    ASSERT(objClosure != NULL, "objClosure is NULL in function resetThread");
    prepareFrame(objThread, objClosure, objThread->stack);
}
