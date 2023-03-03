//
// Created by MorbidArk on 2023/3/2.
//

#include "core.hpp"

#define CORE_MODULE VT_TO_VALUE(ValueType::VT_NULL)

//执行模块,目前为空,桩函数
VMResult executeModule(VM *vm, Value moduleName, const char *moduleCode)
{
    return VM_RESULT_ERROR;
}

//编译核心模块
void buildCore(VM *vm)
{
    //创建核心模块,录入到vm->allModules
    ObjModule *coreModule = new ObjModule(vm, nullptr); //NULL为核心模块.name
    ObjMap::mapSet(vm, vm->allModules, CORE_MODULE, OBJ_TO_VALUE(coreModule));
}