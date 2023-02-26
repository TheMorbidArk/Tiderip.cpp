//
// Created by MorbidArk on 2023/2/25.
//

#include "vm.hpp"
#include "utils.hpp"

vm::vm()
{
    this->allocatedBytes = 0;
    this->curParser = nullptr;
}

void vm::initVM(VM *vm)
{
    vm->allocatedBytes = 0;
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
