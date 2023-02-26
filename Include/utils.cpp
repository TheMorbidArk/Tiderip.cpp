//
// Created by MorbidArk on 2023/2/19.
//

#include <cstdlib>
#include <cstdarg>
#include "utils.hpp"
#include "common.hpp"
#include "vm.hpp"

//内存管理三种功能:
//   1 申请内存
//   2 修改空间大小
//   3 释放内存
void *memManager(VM *vm, void *ptr, uint32_t oldSize, uint32_t newSize)
{
    //累计系统分配的总内存
    vm->allocatedBytes += newSize - oldSize;
    
    //避免realloc(NULL, 0)定义的新地址,此地址不能被释放
    if (newSize == 0)
    {
        free(ptr);
        return nullptr;
    }
    
    return realloc(ptr, newSize);
}

uint32_t ceilToPowerOf2(uint32_t v)
{
    v += (v == 0);
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

void errorReport(void *parser, ErrorType errorType, const char *fmt, ...)
{
    char buffer[DEFAULT_BUfFER_SIZE] = { '\0' };
    va_list ap;
        va_start(ap, fmt);
    vsnprintf(buffer, DEFAULT_BUfFER_SIZE, fmt, ap);
        va_end(ap);
    
    switch (errorType)
    {
    case ERROR_IO:
    case ERROR_MEM:
        fprintf(stderr, "%s:%d In function %s():%s\n",
            __FILE__, __LINE__, __func__, buffer);
        break;
    case ERROR_LEX:
    case ERROR_COMPILE:
        ASSERT(parser != NULL, "parser is null!");
        /* TODO 实现 Parser */
//            fprintf(stderr, "%s:%d \"%s\"\n", ((Parser *) parser)->file,
//                    ((Parser *) parser)->preToken.lineNo, buffer);
        break;
    case ERROR_RUNTIME:
        fprintf(stderr, "%s\n", buffer);
        break;
    default:
        NOT_REACHED()
    }
    exit(1);
}
