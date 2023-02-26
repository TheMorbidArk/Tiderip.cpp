//
// Created by MorbidArk on 2023/2/25.
//

#pragma once

#include "common.hpp"
#include "Class.hpp"

class vm
{
  public:
    Class *classOfClass;
    Class *objectClass;
    Class *stringClass;
    Class *mapClass;
    Class *rangeClass;
    Class *listClass;
    Class *nullClass;
    Class *boolClass;
    Class *numClass;
    Class *fnClass;
    Class *threadClass;
    uint32_t allocatedBytes;// 累计已分配的内存量
    headerObj *allObjects;  // 所有已分配对象链表
    Parser *curParser;
    
    vm();
    static void initVM(VM *vm);
    static VM *newVM();
    
};
