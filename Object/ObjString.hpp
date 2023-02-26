//
// Created by MorbidArk on 2023/2/26.
//

#pragma once

#include "headerObj.hpp"

class ObjString
{
  public:
    headerObj *objHeader;
    uint32_t hashCode;  //字符串的哈希值
    CharValue value;
    
    ObjString(VM *vm, const char *str, uint32_t length);
    uint32_t hashString(char *str, uint32_t length);
    void hashObjString(ObjString *objString);
    ObjString *newObjString(VM *vm, const char *str, uint32_t length);
};
