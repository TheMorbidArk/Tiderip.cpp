//
// Created by MorbidArk on 2023/3/1.
//

#pragma once

#include "Class.hpp"
#include "vm.hpp"

class ObjList
{
  public:
    headerObj *objHeader;
    Buffer<Value> elements;  //list中的元素
    
    ObjList(VM *vm, uint32_t elementNum);
    static ObjList *newObjList(VM *vm, uint32_t elementNum);
    static Value removeElement(VM *vm, ObjList *objList, uint32_t index);
    static void insertElement(VM *vm, ObjList *objList, uint32_t index, Value value);
    
};