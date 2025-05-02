; \file AMD64 Windows function stubs
; \copyright Randomcode Developers

TITLE AMD64 Windows function stubs

OPTION PROLOGUE:NONE

MAKE_STUB MACRO name
	.DATA
	PUBLIC @CatStr(__imp_, name)
	@CatStr(__imp_, name) LABEL QWORD
	PUBLIC @CatStr(STUB_, name)
	@CatStr(STUB_, name) DQ 0
	.CODE
	@CatStr(name, _Forwarder) PROC
		jmp [QWORD PTR @CatStr(STUB_, name)]
	@CatStr(name, _Forwarder) ENDP
	PUBLIC @CatStr(name, _Available)
	@CatStr(name, _Available) PROC
		push rbp
		mov rbp, rsp
		mov rax, @CatStr(STUB_, name)
		cmp rax, 0
		jne Avail
		mov eax, 0
		pop rbp
		ret
Avail:
		mov eax, 1
		pop rbp
		ret
	@CatStr(name, _Available) ENDP
ENDM

MAKE_STUB_LOCAL MACRO name
	MAKE_STUB name
	.CODE
	name PROC
		jmp [QWORD PTR @CatStr(STUB_, name)]
	name ENDP
ENDM

INCLUDE <stubs_win32_amd64.inc>

END
