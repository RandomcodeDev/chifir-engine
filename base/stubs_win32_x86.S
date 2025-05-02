# \file IA-32 Windows function stubs (GNU syntax)
# \copyright Randomcode Developers

.intel_syntax noprefix

.macro MAKE_STUB name paramSize
    .data
    .globl __imp__\name\paramSize
	__imp__\name\paramSize:

    .globl _STUB_\name
_STUB_\name:
    .long 0

    .text
	.globl _\name\paramSize
_\name\paramSize:
    jmp DWORD PTR _STUB_\name

    .globl _\name\()_Available
_\name\()_Available:
    push ebp
    mov ebp, esp
    mov eax, _STUB_\name
    cmp eax, 0
    jne 1f
    mov eax, 0
    pop ebp
    ret
1:
    mov eax, 1
    pop ebp
    ret
.endm

#include "stubs_win32_x86.inc"
