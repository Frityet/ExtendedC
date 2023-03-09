#include <signal.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <Block.h>
#include <unistd.h>
#include <stdbool.h>

#include "extended/backtrace.h"
#include "extended/exceptions.h"

#define func(...) __typeof(__typeof(__VA_ARGS__) ^)

typedef struct String String;
typedef struct String {
    func(String *(const String *)) concat;
    func(void()) free;
    func(bool(const String *)) equals;
    size_t length;
    const char data[];
} String;

String *strn(size_t slen, const char s[static slen])
{
    String *this = calloc(1, sizeof(*this) + slen + 1);

    this->length = slen;
    assert(strlcpy((char *)this->data, s, slen) == strlen(s));

    this->free = Block_copy(^ {
        Block_release(this->concat);
        Block_release(this->free);
        Block_release(this->equals);
        free(this);
    });

    this->concat = Block_copy(^(const String *other) {
        String *news = strn(this->length + other->length, "\0");
        strlcat((char *)news->data, this->data, news->length);
        strlcat((char *)news->data, other->data, news->length);
        return news;
    });

    this->equals = Block_copy(^(const String *other) {
        return (bool)(strcmp(this->data, other->data) == 0);
    });

    return this;
}

static inline String *str(const char *s)
{ return strn(strlen(s) + 1, s); }

#define MC_concat2(x) _mc_##x##_deferexpr
#define MC_concat1(x) MC_concat2(x)
#define defer __attribute__((cleanup(_mc_rundefer))) void (^MC_concat1(__LINE__))(void) = ^
static void _mc_rundefer(void (^*cb)(void)) { (*cb)(); }

#define function(called) auto called = ^
#define auto __auto_type


// auto main = ^(int argc, const char *argv[]) {
int main(int argc, const char *argv[])
{
    String *s1 = str("Hello");
    __builtin_dump_struct(s1, &printf);

    String *s2 = str(", ");
    String *s3 = str("World!");
    defer {
        try {
            s1->free();
            s2->free();
            s3->free();
            kill(getpid(), SIGSEGV);
        } catch(SIGSEGV)(Exception_t ex) {
            fprintf(stderr, "Could not free at %s:%d (in %s).\nTrace:\n", ex.trace.file, ex.trace.line, ex.trace.function);
            for (size_t i = 0; i < ex.trace.call_count; i++) {
                struct BacktraceInformation bt = ex.trace.calls[i];
                printf("[%lld] %p (%s) in %s", bt.level, bt.address, bt.symbol_name, bt.filename);
            }
        } execute();
    };



    puts(s1->concat(s2)->concat(s3)->data); //LEAK

    if (argc < 3) {
        puts("Done");
        return 1;
    }

    s1 = str(argv[1]), s2 = str(argv[2]);
    defer {
        try {
            s1->free();
            s2->free();
        } catch(SIGSEGV)(Exception_t ex) {
            fprintf(stderr, "Could not free at %s:%d (in %s).\nTrace:\n", ex.trace.file, ex.trace.line, ex.trace.function);
            for (size_t i = 0; i < ex.trace.call_count; i++) {
                struct BacktraceInformation bt = ex.trace.calls[i];
                printf("[%lld] %p (%s) in %s", bt.level, bt.address, bt.symbol_name, bt.filename);
            }
        } execute();
    };

    if (s1->equals(s2)) printf("Strings %p and %p equals!\n", s1, s2);
    else printf("Strings %p and %p do not equals!\n", s1, s2);

    return 0;
};
