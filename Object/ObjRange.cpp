//
// Created by MorbidArk on 2023/3/1.
//

#include "ObjRange.hpp"
#include "utils.hpp"
#include "vm.hpp"

ObjRange::ObjRange(VM *vm, int from, int to)
{
    this->objHeader = new headerObj(vm, ObjType::OT_RANGE, vm->rangeClass);
    this->from = from;
    this->to = to;
}

ObjRange *ObjRange::newObjRange(VM *vm, int from, int to)
{
    ObjRange *objRange = ALLOCATE(vm, ObjRange);
    objRange->objHeader = new headerObj(vm, ObjType::OT_RANGE, vm->rangeClass);
    objRange->from = from;
    objRange->to = to;
    return objRange;
}
