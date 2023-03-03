//
// Created by MorbidArk on 2023/3/2.
//

#pragma once

#include "vm.hpp"

VMResult executeModule(VM *vm, Value moduleName, const char *moduleCode);
int getIndexFromSymbolTable(SymbolTable *table, const char *symbol, uint32_t length);
int addSymbol(VM *vm, SymbolTable *table, const char *symbol, uint32_t length);
void buildCore(VM *vm);
void bindMethod(VM *vm, Class *tarClass, uint32_t index, Method method);
void bindSuperClass(VM *vm, Class *subClass, Class *superClass);
