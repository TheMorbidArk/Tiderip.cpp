//
// Created by MorbidArk on 2023/2/26.
//

#pragma once

#include "ObjString.hpp"

//模块对象
class ObjModule
{
  public:
    headerObj *objHeader;
    SymbolTable moduleVarName;   //模块中的模块变量名
    Buffer<Value> moduleVarValue;  //模块中的模块变量值
    ObjString *name;   //模块名
    
    ObjModule(VM *vm, const char *modName);
    static ObjModule *newObjModule(VM *vm, const char *modName);
};

//对象实例
class ObjInstance
{
  public:
    headerObj *objHeader;
    //具体的字段
    Value fields[0];
    
    ObjInstance(VM *vm, Class *tarClass);
    static ObjInstance *newObjInstance(VM *vm, Class *tarClass);
};


