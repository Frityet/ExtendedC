#include "exceptions.h"
#include "backtrace.h"

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdbool.h>
#include <sys/signal.h>
#include <unistd.h>
#include <signal.h>
#include <stdarg.h>

static struct {
    jmp_buf stack;
    void (^handler)(Exception_t ex);
    Exception_t info;
} current_exception;

static struct {
    size_t length;
    int data[EXC_BUFSIZ];
} signals_to_catch;

static bool registered_handlers = false;
static void signal_handler(int rawsig)
{
    Exception_t ex = current_exception.info;

    if (signals_to_catch.length == 0 && current_exception.handler == NULL) {
        void *bt[EXC_BUFSIZ] = {0};
        int i = backtrace(bt, EXC_BUFSIZ);
        void **addr = bt + 1;
        i--;

        ex.what = strsignal(rawsig);
        ex.signal = rawsig;
        ex.trace.call_count = i;
        ex.trace.calls = calloc(i, sizeof(struct BacktraceInformation));
        parse_backtrace(i, addr, ex.trace.calls);

        current_exception.handler = ^(Exception_t ex) {
            fprintf(stderr, "%s\n", ex.what);

            fprintf(stderr, "Backtrace:\n");
            size_t bt_c = ex.trace.calls->level + 1;
            for (size_t i = 0; i < bt_c; i++)
                fprintf(stderr, "- [%lld] = %s (%s)\n", ex.trace.calls[i].level, ex.trace.calls[i].symbol_name, ex.trace.calls[i].filename);

            exit(ex.signal);
        };
    }

    int sig = -1;
    if (ex.signal == 0) {
        ex.signal = sig = rawsig;
    } else {
        for (size_t i = 0; i < signals_to_catch.length; i++) {
            if (signals_to_catch.data[i] == rawsig) { sig = rawsig; break; }
        }
    }
    if (sig == -1) return;


    current_exception.handler(ex);
    free(ex.trace.calls);
    // memset(&current_exception, 0, sizeof(current_exception));
    current_exception = (typeof(current_exception)){0};
    longjmp(current_exception.stack, sig);
}

void throw_exception(const Exception_t *ex)
{
    current_exception.info = *ex;
    kill(getpid(), ex->signal);
}

__attribute__((sentinel(0, 1)))
void set_exception_signal_interrupts(int t1, ...)
{
    va_list l;
    va_start(l, t1);

    int sig = t1;
    do {
        signals_to_catch.data[signals_to_catch.length++] = sig;
    } while ((sig = va_arg(l, int)) != 0);

    va_end(l);
}

void register_exception_signal(int sig)
{
    signal(sig, signal_handler);
}

int set_exception_jmp(void (^handler)(Exception_t ex))
{
    current_exception.handler = handler;

    if (setjmp(current_exception.stack) == 0) return true;
    else return false;
}
