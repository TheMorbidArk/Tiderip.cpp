//
// Created by MorbidArk on 2023/2/25.
//

#pragma once

#include "common.hpp"

class vm
{
  public:
    uint32_t allocatedBytes;  //累计已分配的内存量
    Parser *curParser;
    
    vm();
    static void initVM(VM *vm);
    static VM *newVM();
    
};
