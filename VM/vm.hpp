//
// Created by MorbidArk on 2023/2/25.
//

#pragma once

#include "common.hpp"
#include "Class.hpp"
#include "ObjMap.hpp"
#include "ObjThread.hpp"

//为定义在opcode.inc中的操作码加上前缀"OPCODE_"
#define OPCODE_SLOTS(opcode, effect) OPCODE_##opcode,
typedef enum {
    #include "opcode.include"
} OpCode;
#undef OPCODE_SLOTS

typedef enum vmResult
{
    VM_RESULT_SUCCESS,
    VM_RESULT_ERROR
} VMResult;   //虚拟机执行结果
//如果执行无误,可以将字符码输出到文件缓存,避免下次重新编译

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
    ObjMap *allModules;
    uint32_t allocatedBytes;// 累计已分配的内存量
    SymbolTable allMethodNames;    // (所有)类的方法名
    headerObj *allObjects;  // 所有已分配对象链表
    ObjThread *curThread;   // 当前正在执行的线程2
    Parser *curParser;
    
    vm();
    static void initVM(VM *vm);
    static VM *newVM();
    
};
