#ifndef __MACRO_H__
#define __MACRO_H__

#include <string.h>
#include <assert.h>
#include "util.h"

#if defined __GNUC__ || defined __llvm__
#   define __LICKLY(x) __builtin_expect(!!(x), 1)
#   define __UNLICKLY(x) __builtin_expect(!!(x),0)
#else
#   define __LICKLY(x) (x)
#   define __UNLICKLY(x) (x)
#endif

#define __ASSERT(x) \
    if(__UNLICKLY(!(x))) { \
        __LOG_ERROR(__LOG_ROOT) << "ASSERTION: " #x \
                                << "\nBacktrace:\n" \
                                << sylar::BacktraceToString(100, 2, "    ");\
        assert(x); \
    }

#define __ASSERT2(x, w) \
    if(__UNLICKLY(!(x))) { \
        __LOG_ERROR(__LOG_ROOT) << "ASSERTION: " #x \
                                << "\n" << w \
                                << "\nBacktrace:\n" \
                                << sylar::BacktraceToString(100, 2, "    ");\
        assert(x); \
    }

#endif