//
// Created by MorbidArk on 2023/3/1.
//

#include "ObjFn.hpp"
#include "headerObj.hpp"
#include "vm.hpp"
ObjFn::ObjFn(VM *vm, ObjModule *objModule, uint32_t slotNum)
{
    this->objHeader = new headerObj(vm, ObjType::OT_FUNCTION, vm->fnClass);
    Buffer<Byte>::BufferInit(&this->instrStream);
    Buffer<Value>::BufferInit(&this->constants);
    this->module = objModule;
    this->maxStackSlotUsedNum = slotNum;
    this->upvalueNum = this->argNum = 0;
#ifdef DEBUG
    this->debug = ALLOCATE(vm, FnDebug);
    this->debug->fnName = NULL;
    Buffer<Int>BufferInit(&objFn->debug->lineNo);
#endif
}

ObjFn *ObjFn::newObjFn(VM *vm, ObjModule *objModule, uint32_t slotNum)
{
    ObjFn *objFn = ALLOCATE(vm, ObjFn);
    if (objFn == nullptr)
    {
        MEM_ERROR("allocate ObjFn failed!");
    }
    objFn->objHeader = new headerObj(vm, ObjType::OT_FUNCTION, vm->fnClass);
    Buffer<Byte>::BufferInit(&objFn->instrStream);
    Buffer<Value>::BufferInit(&objFn->constants);
    objFn->module = objModule;
    objFn->maxStackSlotUsedNum = slotNum;
    objFn->upvalueNum = objFn->argNum = 0;
#ifdef DEBUG
    objFn->debug = ALLOCATE(vm, FnDebug);
    objFn->debug->fnName = NULL;
    Buffer<Int>BufferInit(&objFn->debug->lineNo);
#endif
    return objFn;
}

ObjUpvalue::ObjUpvalue(VM *vm, Value *localVarPtr)
{
    this->objHeader = new headerObj(vm, ObjType::OT_UPVALUE, nullptr);
    this->localVarPtr = localVarPtr;
    Value closedUpvalue = { ValueType::VT_NULL, { 0 }};
    this->closedUpvalue = closedUpvalue;
    this->next = nullptr;
}

ObjUpvalue *ObjUpvalue::newObjUpvalue(VM *vm, Value *localVarPtr)
{
    ObjUpvalue *objUpvalue = ALLOCATE(vm, ObjUpvalue);
    objUpvalue->objHeader = new headerObj(vm, ObjType::OT_UPVALUE, nullptr);
    objUpvalue->localVarPtr = localVarPtr;
    Value closedUpvalue = { ValueType::VT_NULL, { 0 }};
    objUpvalue->closedUpvalue = closedUpvalue;
    objUpvalue->next = nullptr;
    return objUpvalue;
}

ObjClosure::ObjClosure(VM *vm, ObjFn *objFn)
{
    this->objHeader = new headerObj(vm, ObjType::OT_CLOSURE, vm->fnClass);
    this->fn = objFn;
    
    //清除upvalue数组做 以避免在填充upvalue数组之前触发GC
    uint32_t idx = 0;
    while (idx < objFn->upvalueNum)
    {
        this->upvalues[idx] = nullptr;
        idx++;
    }
    
}

ObjClosure *ObjClosure::newObjClosure(VM *vm, ObjFn *objFn)
{
    ObjClosure *objClosure = ALLOCATE_EXTRA(vm, ObjClosure, sizeof(ObjUpvalue *) * objFn->upvalueNum);
    objClosure->objHeader = new headerObj(vm, ObjType::OT_CLOSURE, vm->fnClass);
    objClosure->fn = objFn;
    
    //清除upvalue数组做 以避免在填充upvalue数组之前触发GC
    uint32_t idx = 0;
    while (idx < objFn->upvalueNum)
    {
        objClosure->upvalues[idx] = nullptr;
        idx++;
    }
    
    return objClosure;
}
