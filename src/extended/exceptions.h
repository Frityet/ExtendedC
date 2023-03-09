#pragma once

#include "extended/backtrace.h"

typedef struct {
    const char *what;
    int signal;

    struct {
        const char *file, *function;
        unsigned int line;

        size_t call_count;
        struct BacktraceInformation *calls;
    } trace;
} Exception_t;

__attribute__((sentinel(0, 1)))
void    set_exception_signal_interrupts(int i, ...);

void    throw_exception(const Exception_t *ex);
void    register_exception_signal(int sig);
int     set_exception_jmp(void (^handler)(Exception_t ex));

#define try void (^_try_exec_##__func__)() = ^
#define catch(...) ;set_exception_signal_interrupts(__VA_ARGS__, NULL); void (^_catch_exec_##__func__)(Exception_t) = ^
#define execute()  ;if (set_exception_jmp(_catch_exec_##__func__)) {\
                        puts("EXEPTION CAUGHT");\
                        _try_exec_##__func__();\
                        puts("EXEPTION END");\
                    }
#define ref __block

#define throw(x) _Generic(x,    char *: throw_exception(&(Exception_t){ .signal = SIGUSR1, .what = (x) }),\
                                Exception_t *: throw_exception(x))

__attribute__((constructor(1)))
static void register_exception_signals()
{
    register_exception_signal(SIGSEGV);
    register_exception_signal(SIGBUS);
    register_exception_signal(SIGUSR1);
}
