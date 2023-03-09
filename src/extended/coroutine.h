/**
 * Copyright (C) 2023 Amrit Bhogal
 *
 * This file is part of ExtendedC.
 *
 * ExtendedC is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ExtendedC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ExtendedC.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <i386/types.h>
#include <stddef.h>
#include <setjmp.h>


struct CoroutineContext {
    union {
        struct {
            register_t rax, rbx, rcx, rdx, rsi, rdi, rbp, rsp, r8, r9, r10, r11, r12, r13, r14, r15;
            register_t rip, rflags;
        };

        jmp_buf buffer;
    } registers;

    struct {
        void *base;
        size_t size;
    } stack;
};
