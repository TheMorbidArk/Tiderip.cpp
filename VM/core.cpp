//
// Created by MorbidArk on 2023/3/2.
//

#include <cstring>
#include "core.hpp"
#include "Compiler.hpp"

#include "core.script.include"

#define CORE_MODULE VT_TO_VALUE(ValueType::VT_NULL)

//!object: object取反,结果为false
static bool primObjectNot(VM *vm UNUSED, Value *args)
{
    RET_VALUE(VT_TO_VALUE(ValueType::VT_FALSE))
}

//args[0] == args[1]: 返回object是否相等
static bool primObjectEqual(VM *vm UNUSED, Value *args)
{
    Value boolValue = BOOL_TO_VALUE(valueIsEqual(args[0], args[1]));
    RET_VALUE(boolValue)
}

//args[0] != args[1]: 返回object是否不等
static bool primObjectNotEqual(VM *vm UNUSED, Value *args)
{
    Value boolValue = BOOL_TO_VALUE(!valueIsEqual(args[0], args[1]));
    RET_VALUE(boolValue)
}

//args[0] is args[1]:类args[0]是否为类args[1]的子类
static bool primObjectIs(VM *vm, Value *args)
{
    //args[1]必须是class
    if (!VALUE_IS_CLASS(args[1]))
    {
        RUN_ERROR("argument must be class!");
    }
    
    Class *thisClass = Class::getClassOfObj(vm, args[0]);
    Class *baseClass = (Class *)(args[1].objHeader);
    
    //有可能是多级继承,因此自下而上遍历基类链
    while (baseClass != nullptr)
    {
        
        //在某一级基类找到匹配就设置返回值为VT_TRUE并返回
        if (thisClass == baseClass)
        {
            RET_VALUE(VT_TO_VALUE(ValueType::VT_TRUE))
        }
        baseClass = baseClass->superClass;
    }
    
    //若未找到基类,说明不具备is_a关系
    RET_VALUE(VT_TO_VALUE(ValueType::VT_FALSE))
}

//args[0].tostring: 返回args[0]所属class的名字
static bool primObjectToString(VM *vm UNUSED, Value *args)
{
    Class *tarClass = args[0].objHeader->thisClass;
    Value nameValue = OBJ_TO_VALUE(tarClass->name);
    RET_VALUE(nameValue)
}

//args[0].type:返回对象args[0]的类
static bool primObjectType(VM *vm, Value *args)
{
    Class *tarClass = Class::getClassOfObj(vm, args[0]);
    RET_OBJ(tarClass)
}

//args[0].name: 返回类名
static bool primClassName(VM *vm UNUSED, Value *args)
{
    RET_OBJ(VALUE_TO_CLASS(args[0])->name)
}

//args[0].supertype: 返回args[0]的基类
static bool primClassSupertype(VM *vm UNUSED, Value *args)
{
    Class *tarClass = VALUE_TO_CLASS(args[0]);
    if (tarClass->superClass != nullptr)
    {
        RET_OBJ(tarClass->superClass)
    }
    RET_VALUE(VT_TO_VALUE(ValueType::VT_NULL))
}

//args[0].toString: 返回类名
static bool primClassToString(VM *vm UNUSED, Value *args)
{
    RET_OBJ(VALUE_TO_CLASS(args[0])->name)
}

//args[0].same(args[1], args[2]): 返回args[1]和args[2]是否相等
static bool primObjectmetaSame(VM *vm UNUSED, Value *args)
{
    Value boolValue = BOOL_TO_VALUE(valueIsEqual(args[1], args[2]));
    RET_VALUE(boolValue)
}

//从modules中获取名为moduleName的模块
static ObjModule *getModule(VM *vm, Value moduleName)
{
    Value value = ObjMap::mapGet(vm->allModules, moduleName);
    if (value.type == ValueType::VT_UNDEFINED)
    {
        return nullptr;
    }
    
    return VALUE_TO_OBJMODULE(value);
}

//载入模块moduleName并编译
static ObjThread *loadModule(VM *vm, Value moduleName, const char *moduleCode)
{
    //确保模块已经载入到 vm->allModules
    //先查看是否已经导入了该模块,避免重新导入
    ObjModule *module = getModule(vm, moduleName);
    
    //若该模块未加载先将其载入,并继承核心模块中的变量
    if (module == nullptr)
    {
        //创建模块并添加到vm->allModules
        ObjString *modName = VALUE_TO_OBJSTR(moduleName);
        ASSERT(modName->value.start[modName->value.length] == '\0', "string.value.start is not terminated!");
        
        module = new ObjModule(vm, modName->value.start);
        ObjMap::mapSet(vm, vm->allModules, moduleName, OBJ_TO_VALUE(module));
        
        //继承核心模块中的变量
        ObjModule *coreModule = getModule(vm, CORE_MODULE);
        uint32_t idx = 0;
        while (idx < coreModule->moduleVarName.count)
        {
            defineModuleVar(vm, module,
                coreModule->moduleVarName.datas[idx].str,
                strlen(coreModule->moduleVarName.datas[idx].str),
                coreModule->moduleVarValue.datas[idx]);
            idx++;
        }
    }
    
    ObjFn *fn = compileModule(vm, module, moduleCode);
    ObjClosure *objClosure = new ObjClosure(vm, fn);
    ObjThread *moduleThread = new ObjThread(vm, objClosure);
    
    return moduleThread;
}

//执行模块,目前为空,桩函数
VMResult executeModule(VM *vm, Value moduleName, const char *moduleCode)
{
    ObjThread *objThread = loadModule(vm, moduleName, moduleCode);
    return VM_RESULT_ERROR;
}

//table中查找符号symbol 找到后返回索引,否则返回-1
int getIndexFromSymbolTable(SymbolTable *table, const char *symbol, uint32_t length)
{
    ASSERT(length != 0, "length of symbol is 0!");
    uint32_t index = 0;
    while (index < table->count)
    {
        if (length == table->datas[index].length &&
            memcmp(table->datas[index].str, symbol, length) == 0)
        {
            return (int)index;
        }
        index++;
    }
    return -1;
}

//往table中添加符号symbol,返回其索引
int addSymbol(VM *vm, SymbolTable *table, const char *symbol, uint32_t length)
{
    ASSERT(length != 0, "length of symbol is 0!");
    String string;
    string.str = ALLOCATE_ARRAY(vm, char, length + 1);
    memcpy(string.str, symbol, length);
    string.str[length] = '\0';
    string.length = length;
    Buffer<String>::BufferAdd(vm, table, string);
    return (int)(table->count - 1);
}

//定义类
static Class *defineClass(VM *vm, ObjModule *objModule, const char *name)
{
    //1先创建类
    Class *tarClass = new Class(vm, name, 0);
    
    //2把类做为普通变量在模块中定义
    defineModuleVar(vm, objModule, name, strlen(name), OBJ_TO_VALUE(tarClass));
    return tarClass;
}

//使class->methods[index]=method
void bindMethod(VM *vm, Class *tarClass, uint32_t index, Method method)
{
    if (index >= tarClass->methods.count)
    {
        Method emptyPad = { MethodType::MT_NONE, { 0 }};
        Buffer<Method>::BufferFillWrite(vm, &
            tarClass->methods, emptyPad, index -
            tarClass->methods.count + 1);
    }
    tarClass->methods.datas[index] = method;
}

//绑定基类
void bindSuperClass(VM *vm, Class *subClass, Class *superClass)
{
    subClass->superClass = superClass;
    
    //继承基类属性数
    subClass->fieldNum += superClass->fieldNum;
    
    //继承基类方法
    uint32_t idx = 0;
    while (idx < superClass->methods.count)
    {
        bindMethod(vm, subClass, idx, superClass->methods.datas[idx]);
        idx++;
    }
}

//编译核心模块
void buildCore(VM *vm)
{
    //核心模块不需要名字,模块也允许名字为空
    ObjModule *coreModule = new ObjModule(vm, nullptr);
    
    //创建核心模块,录入到vm->allModules
    ObjMap::mapSet(vm, vm->allModules, CORE_MODULE, OBJ_TO_VALUE(coreModule));
    
    //创建object类并绑定方法
    vm->objectClass = defineClass(vm, coreModule, "object");
    PRIM_METHOD_BIND(vm->objectClass, "!", primObjectNot)
    PRIM_METHOD_BIND(vm->objectClass, "==(_)", primObjectEqual)
    PRIM_METHOD_BIND(vm->objectClass, "!=(_)", primObjectNotEqual)
    PRIM_METHOD_BIND(vm->objectClass, "is(_)", primObjectIs)
    PRIM_METHOD_BIND(vm->objectClass, "toString", primObjectToString)
    PRIM_METHOD_BIND(vm->objectClass, "type", primObjectType)
    
    //定义classOfClass类,它是所有meta类的meta类和基类
    vm->classOfClass = defineClass(vm, coreModule, "class");
    
    //objectClass是任何类的基类
    bindSuperClass(vm, vm->classOfClass, vm->objectClass);
    
    PRIM_METHOD_BIND(vm->classOfClass, "name", primClassName)
    PRIM_METHOD_BIND(vm->classOfClass, "supertype", primClassSupertype)
    PRIM_METHOD_BIND(vm->classOfClass, "toString", primClassToString)
    
    //定义object类的元信息类objectMetaclass,它无须挂载到vm
    Class *objectMetaclass = defineClass(vm, coreModule, "objectMeta");
    
    //classOfClass类是所有meta类的meta类和基类
    bindSuperClass(vm, objectMetaclass, vm->classOfClass);
    
    //类型比较
    PRIM_METHOD_BIND(objectMetaclass, "same(_,_)", primObjectmetaSame)
    
    //绑定各自的meta类
    vm->objectClass->objHeader->
        thisClass = objectMetaclass;
    objectMetaclass->objHeader->
        thisClass = vm->classOfClass;
    vm->classOfClass->objHeader->
        thisClass = vm->classOfClass; //元信息类回路,meta类终点
    
    //执行核心模块
    executeModule(vm, CORE_MODULE, coreModuleCode);
    
}