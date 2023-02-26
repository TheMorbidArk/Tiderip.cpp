//
// Created by MorbidArk on 2023/2/19.
//

#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstdint>

typedef class vm VM;
typedef class parser Parser;
typedef class Class Class;

#define UNUSED [[maybe_unused]]

#ifdef DEBUG
#define ASSERT(condition, errMsg) \
      do {\
     if (!(condition)) {\
        fprintf(stderr, "ASSERT failed! %s:%d In function %s(): %s\n", \
           __FILE__, __LINE__, __func__, errMsg); \
        abort();\
     }\
      } while (0);
#else
#define ASSERT(condition, errMsg) ((void)0)
#endif

#define NOT_REACHED()\
   do {\
      fprintf(stderr, "NOT_REACHED: %s:%d In function %s()\n", \
     __FILE__, __LINE__, __func__);\
      while (1);\
   } while (0);
