//
// Created by MorbidArk on 2023/2/26.
//

#include "headerObj.hpp"
#include "vm.hpp"

headerObj::headerObj(VM *vm, ObjType objType, Class *thisClass)
{
    this->type = objType;
    this->isDark = false;
    this->thisClass = thisClass;    //设置meta类
    this->next = vm->allObjects;
    vm->allObjects = this;
}
