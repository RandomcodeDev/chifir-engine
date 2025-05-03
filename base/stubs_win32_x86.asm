; \file IA-32 Windows function stubs
; \copyright 2025 Randomcode Developers

.386
.MODEL FLAT

TITLE IA-32 Windows function stubs

OPTION PROLOGUE:NONE

; I get that code had to be small back then, but fuck stdcall
MAKE_STUB MACRO name, paramSize
	.DATA
	PUBLIC @CatStr(__imp__, name, paramSize)
	@CatStr(__imp__, name, paramSize) LABEL DWORD
	PUBLIC @CatStr(_STUB_, name)
	@CatStr(_STUB_, name) DD 0
	.CODE
	PUBLIC @CatStr(_, name, paramSize)
	@CatStr(_, name, paramSize) PROC
		jmp [DWORD PTR @CatStr(_STUB_, name)]
	@CatStr(_, name, paramSize) ENDP
	PUBLIC @CatStr(_, name, _Available)
	@CatStr(_, name, _Available) PROC
		push ebp
		mov ebp, esp
		mov eax, @CatStr(_STUB_, name)
		cmp eax, 0
		jne Avail
		mov eax, 0
		pop ebp
		ret
Avail:
		mov eax, 1
		pop ebp
		ret
	@CatStr(_, name, _Available) ENDP
ENDM

MAKE_STUB_LOCAL MACRO name, paramSize
	MAKE_STUB name, paramSize
	.CODE
	@CatStr(_, name, paramSize) PROC
		jmp [DWORD PTR @CatStr(_STUB_, name)]
	@CatStr(_, name, paramSize) ENDP
ENDM

INCLUDE <stubs_win32_x86.inc>

END
