#pragma once

#include <execinfo.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#if defined(BUFSIZ)
#   define EXC_BUFSIZ BUFSIZ
#else
#   define EXC_BUFSIZ 256
#endif

struct BacktraceInformation {
    uint64_t level;
    char filename[EXC_BUFSIZ], symbol_name[EXC_BUFSIZ];
    void *address;
    uint64_t offset;
};

int parse_backtrace(size_t backtrace_count,  void *backtrace[static backtrace_count], struct BacktraceInformation dst[static backtrace_count]);

static int getbacktrace(size_t length, struct BacktraceInformation ifo[static length])
{
    void *bt[length];
    int i = backtrace(bt, length);
    void **addr = bt + 1;
    i--;

    return parse_backtrace(i, addr, ifo);
}


