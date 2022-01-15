#ifndef __MACRO_H__
#define __MACRO_H__

#include <string.h>
#include <assert.h>
#include "util.h"

#define __ASSERT(x) \
    if(!(x)) { \
        __LOG_ERROR(__LOG_ROOT) << "ASSERTION: " #x \
                                << "\nBacktrace:\n" \
                                << sylar::BacktraceToString(100, 2, "    ");\
        assert(x); \
    }

#define __ASSERT2(x, w) \
    if(!(x)) { \
        __LOG_ERROR(__LOG_ROOT) << "ASSERTION: " #x \
                                << "\n" << w \
                                << "\nBacktrace:\n" \
                                << sylar::BacktraceToString(100, 2, "    ");\
        assert(x); \
    }

#endif