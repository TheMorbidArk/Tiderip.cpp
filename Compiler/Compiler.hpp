//
// Created by MorbidArk on 2023/3/5.
//

#pragma once

#include "ObjFn.hpp"
#include "core.hpp"

#define MAX_LOCAL_VAR_NUM 128
#define MAX_UPVALUE_NUM 128
#define MAX_ID_LEN 128   //变量名最大长度

#define MAX_METHOD_NAME_LEN MAX_ID_LEN
#define MAX_ARG_NUM 16

//函数名长度+'('+n个参数+(n-1)个参数分隔符','+')'
#define MAX_SIGN_LEN MAX_METHOD_NAME_LEN + MAX_ARG_NUM * 2 + 1

#define MAX_FIELD_NUM 128

typedef enum
{
    SIGN_CONSTRUCT,  //构造函数
    SIGN_METHOD,  //普通方法
    SIGN_GETTER, //getter方法
    SIGN_SETTER, //setter方法
    SIGN_SUBSCRIPT, //getter形式的下标
    SIGN_SUBSCRIPT_SETTER   //setter形式的下标
} SignatureType;   //方法的签名

typedef enum
{
    BP_NONE,      //无绑定能力
    
    //从上往下,优先级越来越高
    BP_LOWEST,    //最低绑定能力
    BP_ASSIGN,    // =
    BP_CONDITION,   // ?:
    BP_LOGIC_OR,    // ||
    BP_LOGIC_AND,   // &&
    BP_EQUAL,      // == !=
    BP_IS,        // is
    BP_CMP,       // < > <= >=
    BP_BIT_OR,    // |
    BP_BIT_AND,   // &
    BP_BIT_SHIFT, // << >>
    BP_RANGE,       // ..
    BP_TERM,      // + -
    BP_FACTOR,      // * / %
    BP_UNARY,    // - ! ~
    BP_CALL,     // . () []
    BP_HIGHEST
} BindPower;   //定义了操作符的绑定权值,即优先级

typedef struct
{
    //如果此upvalue是直接外层函数的局部变量就置为true,
    //否则置为false
    bool isEnclosingLocalVar;
    
    //外层函数中局部变量的索引或者外层函数中upvalue的索引
    //这取决于isEnclosingLocalVar的值
    uint32_t index;
} Upvalue;  //upvalue结构

typedef struct
{
    const char *name;
    uint32_t length;
    int scopeDepth;  //局部变量作用域

//表示本函数中的局部变量是否是其内层函数所引用的upvalue,
//当其内层函数引用此变量时,由其内层函数来设置此项为true.
    bool isUpvalue;
} LocalVar;    //局部变量

typedef struct
{
    SignatureType type;  //签名类型
    const char *name;    //签名
    uint32_t length;    //签名长度
    uint32_t argNum;    //参数个数
} Signature;        //签名

typedef struct loop
{
    int condStartIndex;   //循环中条件的地址
    int bodyStartIndex;   //循环体起始地址
    int scopeDepth;  //循环中若有break,告诉它需要退出的作用域深度
    int exitIndex;   //循环条件不满足时跳出循环体的目标地址
    struct loop *enclosingLoop;   //外层循环
} Loop;   //loop结构

typedef struct
{
    ObjString *name;          //类名
    SymbolTable fields;          //类属性符号表
    bool inStatic;          //若当前编译静态方法就为真
    Buffer<Int> instantMethods;  //实例方法
    Buffer<Int> staticMethods;   //静态方法
    Signature *signature;      //当前正在编译的签名
} ClassBookKeep;    //用于记录类编译时的信息

class CompileUnit
{
  public:
    // 所编译的函数
    ObjFn *fn;
    
    //作用域中允许的局部变量的个量上限
    LocalVar localVars[MAX_LOCAL_VAR_NUM];
    
    //已分配的局部变量个数
    uint32_t localVarNum;
    
    //记录本层函数所引用的upvalue
    Upvalue upvalues[MAX_UPVALUE_NUM];
    
    //此项表示当前正在编译的代码所处的作用域,
    int scopeDepth;
    
    //当前使用的slot个数
    uint32_t stackSlotNum;
    
    //当前正在编译的循环层
    Loop *curLoop;
    
    //当前正编译的类的编译信息
    ClassBookKeep *enclosingClassBK;
    
    //包含此编译单元的编译单元,即直接外层
    CompileUnit *enclosingUnit;
    
    //当前parser
    Parser *curParser;
    
    CompileUnit(Parser *parser, CompileUnit *enclosingUnit, bool isMethod);
    static void initCompileUnit(Parser *parser, CompileUnit *cu, CompileUnit *enclosingUnit, bool isMethod);
};  //编译单元 ;

//指示符函数指针
typedef void (*DenotationFn)(CompileUnit *cu, bool canAssign);

//签名函数指针
typedef void (*methodSignatureFn)(CompileUnit *cu, Signature *signature);

typedef struct
{
    const char *id;          //符号
    
    //左绑定权值,不关注左边操作数的符号此值为0
    BindPower lbp;
    
    //字面量,变量,前缀运算符等不关注左操作数的Token调用的方法
    DenotationFn nud;
    
    //中缀运算符等关注左操作数的Token调用的方法
    DenotationFn led;
    
    //表示本符号在类中被视为一个方法,
    //为其生成一个方法签名.
    methodSignatureFn methodSign;
} SymbolBindRule;   //符号绑定规则

typedef enum
{
    VAR_SCOPE_INVALID,
    VAR_SCOPE_LOCAL,    //局部变量
    VAR_SCOPE_UPVALUE,  //upvalue
    VAR_SCOPE_MODULE    //模块变量
} VarScopeType;   //标识变量作用域

typedef struct
{
    VarScopeType scopeType;   //变量的作用域
    //根据scodeType的值,
    //此索引可能指向局部变量或upvalue或模块变量
    int index;
} Variable;

namespace opCode
{
    static int writeByte(CompileUnit *cu, int byte);
    static void writeOpCode(CompileUnit *cu, OpCode opCode);
    static int writeByteOperand(CompileUnit *cu, int operand);
    inline static void writeShortOperand(CompileUnit *cu, int operand);
    static int writeOpCodeByteOperand(CompileUnit *cu, OpCode opCode, int operand);
    static void writeOpCodeShortOperand(CompileUnit *cu, OpCode opCode, int operand);
}

uint32_t getBytesOfOperands(Byte *instrStream, Value *constants, int ip);
static void infixOperator(CompileUnit *cu, bool canAssign UNUSED);
static void unaryOperator(CompileUnit *cu, bool canAssign UNUSED);

int defineModuleVar(VM *vm, ObjModule *objModule, const char *name, uint32_t length, Value value);
ObjFn *compileModule(VM *vm, ObjModule *objModule, const char *moduleCode);
static uint32_t addConstant(CompileUnit *cu, Value constant);
static void expression(CompileUnit *cu, BindPower rbp);
static void compileStatment(CompileUnit *cu);
static void compileProgram(CompileUnit *cu);