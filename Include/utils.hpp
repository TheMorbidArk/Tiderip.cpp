
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "common.hpp"

/* TODO 使用类模板重构ValueBuffer结构 */

void *memManager(VM *vm, void *ptr, uint32_t oldSize, uint32_t newSize);

#define ALLOCATE(vmPtr, type) \
   (type*)memManager(vmPtr, NULL, 0, sizeof(type))

#define ALLOCATE_EXTRA(vmPtr, mainType, extraSize) \
   (mainType*)memManager(vmPtr, NULL, 0, sizeof(mainType) + extraSize)

#define ALLOCATE_ARRAY(vmPtr, type, count) \
   (type*)memManager(vmPtr, NULL, 0, sizeof(type) * count)

#define DEALLOCATE_ARRAY(vmPtr, arrayPtr, count) \
   memManager(vmPtr, arrayPtr, sizeof(arrayPtr[0]) * count, 0)

#define DEALLOCATE(vmPtr, memPtr) memManager(vmPtr, memPtr, 0, 0)

/**
 * 找出大于等于v最近的2次幂
 * @param v 底数V
 * @return 大于等于最接近V的2次幂
 */
uint32_t ceilToPowerOf2(uint32_t v);

typedef struct
{
    char *str;
    uint32_t length;
} String;

typedef struct
{
    uint32_t length; // 除结束'\0'之外的字符个数
    char start[];  // 类似c99中的柔性数组
} CharValue;  // 字符串缓冲区

template<class Type>
class Buffer
{
  public:
    Type *datas;             // 数据缓冲区
    uint32_t count;     // 缓冲区中已使用的元素个数
    uint32_t capacity;  // 缓冲区容量
    
    Buffer();
    static void BufferInit(Buffer *buf);
    static void BufferFillWrite(VM *vm, Buffer *buf, Type data, uint32_t fillCount);
    static void BufferAdd(VM *vm, Buffer *buf, Type data);
    static void BufferClear(VM *vm, Buffer *buf);
    
};

template<class Type>
Buffer<Type>::Buffer()
{
    this->datas = nullptr;
    this->count = 0;
    this->capacity = 0;
}

template<class Type>
void Buffer<Type>::BufferInit(Buffer *buf)
{
    buf->datas = nullptr;
    buf->count = buf->capacity = 0;
}

template<class Type>
void Buffer<Type>::BufferFillWrite(VM *vm, Buffer *buf, Type data, uint32_t fillCount)
{
    uint32_t newCounts = buf->count + fillCount;
    if (newCounts > buf->capacity)
    {
        size_t oldSize = buf->capacity * sizeof(Type);
        buf->capacity = ceilToPowerOf2(newCounts);
        size_t newSize = buf->capacity * sizeof(Type);
        ASSERT(newSize > oldSize, "faint...memory allocate!");
        buf->datas = (Type *)memManager(vm, buf->datas, oldSize, newSize);
    }
    uint32_t cnt = 0;
    while (cnt < fillCount)
    {
        buf->datas[buf->count++] = data;
        cnt++;
    }
}

template<class Type>
void Buffer<Type>::BufferAdd(VM *vm, Buffer *buf, Type data)
{
    BufferFillWrite(vm, buf, data, 1);
}

template<class Type>
void Buffer<Type>::BufferClear(VM *vm, Buffer *buf)
{
    size_t oldSize = buf->capacity * sizeof(buf->datas[0]);
    memManager(vm, buf->datas, oldSize, 0);
    BufferInit(buf);
}

typedef uint8_t Byte;
typedef char Char;
typedef int Int;

#define SymbolTable Buffer<String>
void symbolTableClear(VM*, SymbolTable* buffer);
typedef enum
{
    ERROR_IO,
    ERROR_MEM,
    ERROR_LEX,
    ERROR_COMPILE,
    ERROR_RUNTIME
} ErrorType;

void errorReport(void *parser, ErrorType errorType, const char *fmt, ...);

#define IO_ERROR(...)\
   errorReport(NULL, ERROR_IO, __VA_ARGS__)

#define MEM_ERROR(...)\
   errorReport(NULL, ERROR_MEM, __VA_ARGS__)

#define LEX_ERROR(parser, ...)\
   errorReport(parser, ERROR_LEX, __VA_ARGS__)

#define COMPILE_ERROR(parser, ...)\
   errorReport(parser, ERROR_COMPILE, __VA_ARGS__)

#define RUN_ERROR(...)\
   errorReport(NULL, ERROR_RUNTIME, __VA_ARGS__)

#define DEFAULT_BUfFER_SIZE 512
