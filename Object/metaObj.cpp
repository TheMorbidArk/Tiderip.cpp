//
// Created by MorbidArk on 2023/2/26.
//

#include "metaObj.hpp"
#include "vm.hpp"
#include "Class.hpp"

ObjModule::ObjModule(VM *vm, const char *modName)
{
    // ObjModule是元信息对象,不属于任何一个类
    this->objHeader = new headerObj(vm, ObjType::OT_MODULE, nullptr);
    
    Buffer<String>::BufferInit(&this->moduleVarName);
    Buffer<Value>::BufferInit(&this->moduleVarValue);
    
    this->name = nullptr;   // 核心模块名为NULL
    if (modName != nullptr)
    {
        this->name = new ObjString(vm, modName, (uint32_t)strlen(modName));
    }
}
ObjModule *ObjModule::newObjModule(VM *vm, const char *modName)
{
    ObjModule *objModule = ALLOCATE(vm, ObjModule);
    if (objModule == nullptr)
    {
        MEM_ERROR("allocate ObjModule failed!");
    }
    
    // ObjModule是元信息对象,不属于任何一个类
    objModule->objHeader = new headerObj(vm, ObjType::OT_MODULE, nullptr);
    
    Buffer<String>::BufferInit(&objModule->moduleVarName);
    Buffer<Value>::BufferInit(&objModule->moduleVarValue);
    
    objModule->name = nullptr;   // 核心模块名为NULL
    if (modName != nullptr)
    {
        objModule->name = new ObjString(vm, modName, strlen(modName));
    }
    
    return objModule;
}
ObjInstance::ObjInstance(VM *vm, Class *tarClass)
{
//在此关联对象的类为参数class
    this->objHeader = new headerObj(vm, ObjType::OT_INSTANCE, tarClass);
    //初始化field为NULL
    uint32_t idx = 0;
    while (idx < tarClass->fieldNum)
    {
        Value field = { ValueType::VT_NULL, { 0 }};
        this->fields[idx++] = field;
    }
}
ObjInstance *ObjInstance::newObjInstance(VM *vm, Class *tarClass)
{
    //参数class主要作用是提供类中field的数目
    ObjInstance *objInstance = ALLOCATE_EXTRA(vm, ObjInstance, sizeof(Value) * tarClass->fieldNum);
    
    //在此关联对象的类为参数class
    objInstance->objHeader = new headerObj(vm, ObjType::OT_INSTANCE, tarClass);
    //初始化field为NULL
    uint32_t idx = 0;
    while (idx < tarClass->fieldNum)
    {
        Value field = { ValueType::VT_NULL, { 0 }};
        objInstance->fields[idx++] = field;
    }
    return objInstance;
}
