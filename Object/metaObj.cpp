//
// Created by MorbidArk on 2023/2/26.
//

#include "metaObj.hpp"
#include "vm.hpp"
#include "Class.hpp"

ObjModule::ObjModule(VM *vm, const char *modName)
{

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
        objInstance->fields[idx++] = VT_TO_VALUE(ValueType::VT_NULL);
    }
    return objInstance;
}
