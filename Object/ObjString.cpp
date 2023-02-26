//
// Created by MorbidArk on 2023/2/26.
//

#include <cstring>
#include <cstdlib>
#include "ObjString.hpp"
#include "vm.hpp"
#include "utils.hpp"
#include "common.hpp"

uint32_t ObjString::hashString(char *str, uint32_t length)
{
    uint32_t hashCode = 2166136261, idx = 0;
    while (idx < length)
    {
        hashCode ^= str[idx];
        hashCode *= 16777619;
        idx++;
    }
    return hashCode;
}

void ObjString::hashObjString(ObjString *objString)
{
    objString->hashCode =
        hashString(objString->value.start, objString->value.length);
}

ObjString *ObjString::newObjString(VM *vm, const char *str, uint32_t length)
{
    // length为0时str必为NULL length不为0时str不为NULL
    ASSERT(length == 0 || str != NULL, "str length don`t match str!");
    
    //+1是为了结尾的'\0'
    ObjString *objString = ALLOCATE_EXTRA(vm, ObjString, length + 1);
    
    if (objString != nullptr)
    {
        objString->objHeader = new headerObj(vm, ObjType::OT_STRING, vm->stringClass);
        objString->value.length = length;
        
        //支持空字符串: str为null,length为0
        //如果非空则复制其内容
        if (length > 0)
        {
            memcpy(objString->value.start, str, length);
        }
        objString->value.start[length] = '\0';
        hashObjString(objString);
    }
    else
    {
        MEM_ERROR("Allocating ObjString failed!");
    }
    return objString;
}

ObjString::ObjString(VM *vm, const char *str, uint32_t length)
{
// length为0时str必为NULL length不为0时str不为NULL
    ASSERT(length == 0 || str != NULL, "str length don`t match str!");
    
    //+1是为了结尾的'\0'
    //ObjString *objString = ALLOCATE_EXTRA(vm, ObjString, length + 1);
    
    this->objHeader = new headerObj(vm, ObjType::OT_STRING, vm->stringClass);
    this->value.length = length;
    
    //支持空字符串: str为null,length为0
    //如果非空则复制其内容
    if (length > 0)
    {
        memcpy(this->value.start, str, length);
    }
    this->value.start[length] = '\0';
    hashObjString(this);
    
}