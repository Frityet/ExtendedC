EXTERN _main_caller

[SECTION .text]
[GLOBAL _start]
_start:
    XOR     RBP, RBP            ;Clear RBP

    MOV     RDI, [RSP]          ;Load argc into RDI
    LEA     RSI, [RSP + 0x8]    ;Load argv into RSI

    CALL    _main_caller

    MOV     RDI, RAX            ;Load return value into RDI
    MOV     RAX, 0x2000001
    SYSCALL
    RET                         ;Exit

