#include "backtrace.h"

#include <stdio.h>

int parse_backtrace(size_t backtrace_count,  void *backtrace[static backtrace_count], struct BacktraceInformation dst[static backtrace_count])
{
    char **raw = backtrace_symbols(backtrace, backtrace_count);
    if (raw == NULL) return 1;

    for (size_t i = 0; i < backtrace_count; i++) {
        if (sscanf(raw[i], "%lld %s %p %s + %lld", &dst[i].level, dst[i].filename, &dst[i].address, dst[i].symbol_name, &dst[i].offset) != 5) {
            free(raw);
            return 2;
        }

        dst[i].address -= dst[i].offset;
    }

    free(raw);
    return 0;
}
