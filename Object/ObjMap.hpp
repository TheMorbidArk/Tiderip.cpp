//
// Created by MorbidArk on 2023/3/1.
//

#pragma once

#include "headerObj.hpp"

#define MAP_LOAD_PERCENT 0.8

typedef struct
{
    Value key;
    Value value;
} Entry;   //key->value对儿

class ObjMap
{
  public:
    headerObj *objHeader;
    uint32_t capacity; //Entry的容量(即总数),包括已使用和未使用Entry的数量
    uint32_t count;  //map中使用的Entry的数量
    Entry *entries; //Entry数组
    
    ObjMap(VM *vm);
    static ObjMap *newObjMap(VM *vm);
    
    static void mapSet(VM *vm, ObjMap *objMap, Value key, Value value);
    static Value mapGet(ObjMap *objMap, Value key);
    static void clearMap(VM *vm, ObjMap *objMap);
    static Value removeKey(VM *vm, ObjMap *objMap, Value key);
    
};
