//
// Created by MorbidArk on 2023/2/26.
//

#pragma once

#include <any>
#include "headerObj.hpp"
#include "ObjString.hpp"
#include "ObjFn.hpp"

enum class MethodType
{
    MT_NONE,     //空方法类型,并不等同于undefined
    MT_PRIMITIVE,    //在vm中用c实现的原生方法
    MT_SCRIPT,    //脚本中定义的方法
    MT_FN_CALL,  //有关函数对象的调用方法,用来实现函数重载
};   //方法类型

#define VT_TO_VALUE(vt) \
   VtToValue(vt)

Value VtToValue(ValueType vt);

#define BOOL_TO_VALUE(boolean) (boolean ? VT_TO_VALUE(ValueType::VT_TRUE) : VT_TO_VALUE(ValueType::VT_FALSE))
#define VALUE_TO_BOOL(value) ((value).type == ValueType::VT_TRUE ? true : false)

#define NUM_TO_VALUE(num) ((Value){ValueType::VT_NUM, {num}})
#define VALUE_TO_NUM(value) value.num

#define OBJ_TO_VALUE(objPtr)  \
   ObjToValue(objPtr)

template<typename Type>
Value ObjToValue(Type *objPtr)
{
    Value value;
    value.type = ValueType::VT_OBJ;
    value.objHeader = (headerObj *)(objPtr);
    return value;
}

#define VALUE_TO_OBJ(value) (value.objHeader)
#define VALUE_TO_OBJSTR(value) ((ObjString*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJFN(value) ((ObjFn*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJRANGE(value) ((ObjRange*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJINSTANCE(value) ((ObjInstance*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJLIST(value) ((ObjList*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJMAP(value) ((ObjMap*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJCLOSURE(value) ((ObjClosure*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJTHREAD(value) ((ObjThread*)VALUE_TO_OBJ(value))
#define VALUE_TO_OBJMODULE(value) ((ObjModule*)VALUE_TO_OBJ(value))
#define VALUE_TO_CLASS(value) ((Class*)VALUE_TO_OBJ(value))

#define VALUE_IS_UNDEFINED(value) ((value).type == ValueType::VT_UNDEFINED)
#define VALUE_IS_NULL(value) ((value).type == ValueType::VT_NULL)
#define VALUE_IS_TRUE(value) ((value).type == ValueType::VT_TRUE)
#define VALUE_IS_FALSE(value) ((value).type == ValueType::VT_FALSE)
#define VALUE_IS_NUM(value) ((value).type == ValueType::VT_NUM)
#define VALUE_IS_OBJ(value) ((value).type == ValueType::VT_OBJ)
#define VALUE_IS_CERTAIN_OBJ(value, objType) (VALUE_IS_OBJ(value) && VALUE_TO_OBJ(value)->type == objType)
#define VALUE_IS_OBJSTR(value) (VALUE_IS_CERTAIN_OBJ(value, ObjType::OT_STRING))
#define VALUE_IS_OBJINSTANCE(value) (VALUE_IS_CERTAIN_OBJ(value, ObjType::OT_INSTANCE))
#define VALUE_IS_OBJCLOSURE(value) (VALUE_IS_CERTAIN_OBJ(value, ObjType::OT_CLOSURE))
#define VALUE_IS_OBJRANGE(value) (VALUE_IS_CERTAIN_OBJ(value, ObjType::OT_RANGE))
#define VALUE_IS_CLASS(value) (VALUE_IS_CERTAIN_OBJ(value, ObjType::OT_CLASS))
#define VALUE_IS_0(value) (VALUE_IS_NUM(value) && (value).num == 0)

// 原生方法指针
[[maybe_unused]]
typedef bool (*Primitive)(VM *vm, Value *args);

typedef struct
{
    MethodType type;  //union中的值由type的值决定
    union
    {
        //指向脚本方法所关联的c实现
        Primitive primFn;
        
        //指向脚本代码编译后的ObjClosure或ObjFn
        ObjClosure *obj;
    };
} Method;

class Class
{
  public:
    headerObj *objHeader;
    Class *superClass; //父类
    uint32_t fieldNum;       //本类的字段数,包括基类的字段数
    Buffer<Method> methods;   //本类的方法
    ObjString *name;   //类名
    
    Class(VM *vm, const char *name, uint32_t fieldNum);
    static Class *newRawClass(VM *vm, const char *name, uint32_t fieldNum);
    static Class *getClassOfObj(VM *vm, Value object);
};

typedef union
{
    uint64_t bits64;
    uint32_t bits32[2];
    double num;
} Bits64;

#define CAPACITY_GROW_FACTOR 4
#define MIN_CAPACITY 64
bool valueIsEqual(Value a, Value b);