//
// Created by MorbidArk on 2023/2/25.
//

#include "vm.hpp"
#include "utils.hpp"

vm::vm()
{
    this->allocatedBytes = 0;
    this->allObjects = nullptr;
    this->curParser = nullptr;
    Buffer<String>::BufferInit(&this->allMethodNames);
    this->allModules = new ObjMap(this);
    this->curParser = nullptr;
}

void vm::initVM(VM *vm)
{
    vm->allocatedBytes = 0;
    vm->allObjects = nullptr;
    vm->curParser = nullptr;
    Buffer<String>::BufferInit(&vm->allMethodNames);
    vm->allModules = new ObjMap(vm);
    vm->curParser = nullptr;
}

VM *vm::newVM()
{
    VM *vm = (VM *)malloc(sizeof(VM));
    if (vm == nullptr)
    {
        MEM_ERROR("allocate VM failed!");
    }
    initVM(vm);
    return vm;
}
