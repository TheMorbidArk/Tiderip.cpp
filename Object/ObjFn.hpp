//
// Created by MorbidArk on 2023/3/1.
//

#pragma once

#include "utils.hpp"
#include "metaObj.hpp"

#define INITIAL_FRAME_NUM 4

typedef struct
{
    char *fnName;     // 函数名
    Buffer<Int> lineNo; // 行号
} FnDebug;   // 在函数中的调试结构

class ObjFn
{
  public:
    headerObj *objHeader;
    Buffer<Byte> instrStream;  //函数编译后的指令流
    Buffer<Value> constants;   // 函数中的常量表
    
    ObjModule *module;    //本函数所属的模块
    
    //本函数最多需要的栈空间,是栈使用空间的峰值
    uint32_t maxStackSlotUsedNum;
    uint32_t upvalueNum;      //本函数所涵盖的upvalue数量
    uint8_t argNum;   //函数期望的参数个数
#if DEBUG
    FnDebug* debug;
#endif
    
    ObjFn(VM *vm, ObjModule *objModule, uint32_t maxStackSlotUsedNum);
    static ObjFn *newObjFn(VM *vm, ObjModule *objModule, uint32_t maxStackSlotUsedNum);
};

class ObjUpvalue
{
  public:
    headerObj *objHeader;
    
    //栈是个Value类型的数组,localVarPtr指向upvalue所关联的局部变量
    Value *localVarPtr;
    
    //已被关闭的upvalue
    Value closedUpvalue;
    
    ObjUpvalue *next;   //用以链接openUpvalue链表
    
    ObjUpvalue(VM *vm, Value *localVarPtr);
    static ObjUpvalue *newObjUpvalue(VM *vm, Value *localVarPtr);
};   //upvalue对象

class ObjClosure
{
  public:
    headerObj *objHeader;
    ObjFn *fn;   //闭包中所要引用的函数
    
    ObjUpvalue *upvalues[0]; //用于存储此函数的 "close upvalue"
    
    ObjClosure(VM *vm, ObjFn *objFn);
    static ObjClosure *newObjClosure(VM *vm, ObjFn *objFn);
};      //闭包对象

typedef struct
{
    uint8_t *ip;    //程序计数器 指向下一个将被执行的指令
    
    //在本frame中执行的闭包函数
    ObjClosure *closure;
    
    //frame是共享therad.stack
    //此项用于指向本frame所在thread运行时栈的起始地址
    Value *stackStart;
} Frame;  //调用框架


