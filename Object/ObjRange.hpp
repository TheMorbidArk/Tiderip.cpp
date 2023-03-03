//
// Created by MorbidArk on 2023/3/1.
//

#pragma once

#include "Class.hpp"

class ObjRange
{
  public:
    headerObj *objHeader;
    int from;   //范围的起始
    int to;     //范围的结束
    
    ObjRange(VM *vm, int from, int to);
    static ObjRange *newObjRange(VM *vm, int from, int to);
};
