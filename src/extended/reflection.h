#pragma once

#include <stdbool.h>
#include "extended/backtrace.h"

enum TypeClass {
    TypeClass_STRUCT,
    TypeClass_UNION,
    TypeClass_ENUM,
    TypeClass_PRIMITIVE,
};

enum GenericTypeID {
    GenericTypeID_BOOL,
    GenericTypeID_CHAR,
    GenericTypeID_SHORT,
    GenericTypeID_INT,
    GenericTypeID_LONG,
    GenericTypeID_FLOAT,
    GenericTypeID_DOUBLE,
};

struct GenericType {
    enum GenericTypeID id;
    bool is_short, is_long;
};

struct TypeInfo {
    enum TypeClass class;
    char name[EXC_BUFSIZ];

    union {
        struct {

        } struct_info;

        struct {

        } union_info;


    };
};


#define typeinfo(...) ({\
    char buf[EXC_BUFSIZ] = {0};\
    __builtin_dump_struct(__VA_ARGS__, &dump_to_string, EXC_BUFSIZ, buf);\
    struct TypeInfo t = {0};\
    parse_typeinfo(&t, buf);\
    t;\
})

int dump_to_string(size_t length, char buf[static length], const char *fmt, ...);

int parse_typeinfo(struct TypeInfo *t, const char buffer[static EXC_BUFSIZ]);
