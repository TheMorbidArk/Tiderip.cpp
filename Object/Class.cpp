//
// Created by MorbidArk on 2023/2/26.
//

#include "Class.hpp"
#include "headerObj.hpp"
#include "vm.hpp"
#include "ObjRange.hpp"
#include <cstring>

Value VtToValue(ValueType vt)
{
    Value value = {
        vt,
        { 0 }
    };
    return value;
}

//判断a和b是否相等
bool valueIsEqual(Value a, Value b)
{
    //类型不同则无须进行后面的比较
    if (a.type != b.type)
    {
        return false;
    }
    
    //同为数字,比较数值
    if (a.type == ValueType::VT_NUM)
    {
        return a.num == b.num;
    }
    
    //同为对象,若所指的对象是同一个则返回true
    if (a.objHeader == b.objHeader)
    {
        return true;
    }
    
    //对象类型不同无须比较
    if (a.objHeader->type != b.objHeader->type)
    {
        return false;
    }
    
    //以下处理类型相同的对象
    //若对象同为字符串
    if (a.objHeader->type == ObjType::OT_STRING)
    {
        ObjString *strA = VALUE_TO_OBJSTR(a);
        ObjString *strB = VALUE_TO_OBJSTR(b);
        return (strA->value.length == strB->value.length &&
            memcmp(strA->value.start, strB->value.start, strA->value.length) == 0);
    }
    
    //若对象同为range
    if (a.objHeader->type == ObjType::OT_RANGE)
    {
        ObjRange *rgA = VALUE_TO_OBJRANGE(a);
        ObjRange *rgB = VALUE_TO_OBJRANGE(b);
        return (rgA->from == rgB->from && rgA->to == rgB->to);
    }
    
    return false;  //其它对象不可比较
}

Class::Class(VM *vm, const char *name, uint32_t fieldNum)
{
    //裸类没有元类
    this->objHeader = new headerObj(vm, ObjType::OT_CLASS, nullptr);
    this->name = new ObjString(vm, name, strlen(name));
    this->fieldNum = fieldNum;
    this->superClass = nullptr;   //默认没有基类
    Buffer<Method>::BufferInit(&this->methods);
}

Class *Class::newRawClass(VM *vm, const char *name, uint32_t fieldNum)
{
    Class *tarClass = ALLOCATE(vm, Class);
    
    //裸类没有元类
    tarClass->objHeader = new headerObj(vm, ObjType::OT_CLASS, nullptr);
    tarClass->name = new ObjString(vm, name, strlen(name));
    tarClass->fieldNum = fieldNum;
    tarClass->superClass = nullptr;   //默认没有基类
    Buffer<Method>::BufferInit(&tarClass->methods);
    
    return tarClass;
}

Class *Class::getClassOfObj(VM *vm, Value object)
{
    switch (object.type)
    {
    case ValueType::VT_NULL:
        return vm->nullClass;
    case ValueType::VT_FALSE:
    case ValueType::VT_TRUE:
        return vm->boolClass;
    case ValueType::VT_NUM:
        return vm->numClass;
    case ValueType::VT_OBJ:
        return VALUE_TO_OBJ(object)->thisClass;
    default:
        NOT_REACHED()
    }
    return nullptr;
}