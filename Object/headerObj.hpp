//
// Created by MorbidArk on 2023/2/26.
//

#pragma once

#include "utils.hpp"

enum class ObjType : int
{
    OT_CLASS = 0,   //此项是class类型,以下都是object类型
    OT_LIST,
    OT_MAP,
    OT_MODULE,
    OT_RANGE,
    OT_STRING,
    OT_UPVALUE,
    OT_FUNCTION,
    OT_CLOSURE,
    OT_INSTANCE,
    OT_THREAD
};  //对象类型

enum class ValueType : int
{
    VT_UNDEFINED = 0,
    VT_NULL,
    VT_FALSE,
    VT_TRUE,
    VT_NUM,
    VT_OBJ   //值为对象,指向对象头
};     //value类型

class headerObj
{
  public:
    ObjType type;
    bool isDark;       //对象是否可达
    Class *thisClass;   //对象所属的类
    headerObj *next;   //用于链接所有已分配对象
    
    headerObj(VM *vm, ObjType objType, Class *thisClass);
    headerObj();
};

typedef struct
{
    ValueType type;
    union
    {
        double num;
        headerObj *objHeader;
    };
} Value;   //通用的值结构