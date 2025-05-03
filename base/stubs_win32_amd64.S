# \file AMD64 Windows function stubs
# \copyright 2025 Randomcode Developers

.intel_syntax noprefix

.macro MAKE_STUB name
	.data
	.globl __imp_\name
	__imp_\name:

	.globl STUB_\name
	STUB_\name: .quad 0

	.text
	.globl \name\()_Forwarder
	\name\()_Forwarder:
		jmp QWORD PTR STUB_\name

	.globl \name\()_Available
	\name\()_Available:
		push rbp
		mov rbp, rsp
		mov rax, STUB_\name
		cmp rax, 0
		jne 1f
		mov eax, 0
		pop rbp
		ret
	1:
		mov eax, 1
		pop rbp
		ret
.endm

.macro MAKE_STUB_LOCAL name
	MAKE_STUB \name
	.text
	.globl \name
	\name:
		jmp QWORD PTR STUB_\name
.endm

#include "stubs_win32_amd64.inc"

