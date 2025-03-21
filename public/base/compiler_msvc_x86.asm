.386
.MODEL FLAT

TITLE MSVC x86 support

OPTION PROLOGUE:NONE

.DATA
cookieMsg db "Security cookie has wrong value"

EXTERN ___security_cookie : DWORD

PUBLIC __fltused
__fltused DD 9875h

.CODE

PUBLIC @_RTC_CheckStackVars@8
@_RTC_CheckStackVars@8 PROC
@_RTC_CheckStackVars@8 ENDP

Base_AbortSafe TEXTEQU <?Base_AbortSafe@@YAXHPBD@Z>
EXTERN Base_AbortSafe : PROC

EXTERN _chkstk_Forwarder : PROC
PUBLIC __chkstk
__chkstk PROC
	jmp _chkstk_Forwarder
__chkstk ENDP

PUBLIC @__security_check_cookie@4
@__security_check_cookie@4 PROC
	cmp ecx, [___security_cookie]
	jnz Fail

	; Instructions that would go here don't work with the security cookie value I choose to use

	ret

Fail:
	lea ecx, [cookieMsg]
	push ecx
	push 0C0000409h
	call Base_AbortSafe
@__security_check_cookie@4 ENDP

;PUBLIC _memset
;_memset PROC
;	jmp Base_MemSet
;_memset ENDP

;PUBLIC _memcpy
;_memcpy PROC
;	jmp Base_MemCopy
;_memcpy ENDP

PUBLIC __GSHandlerCheck
__GSHandlerCheck PROC
	mov eax, 1
	ret
__GSHandlerCheck ENDP

; all functions below this point are copied from SDL
PUBLIC __ftol
__ftol PROC
        push        ebp
        mov         ebp,esp
        sub         esp,20h
        and         esp,0FFFFFFF0h
        fld         st(0)
        fst         dword ptr [esp+18h]
        fistp       qword ptr [esp+10h]
        fild        qword ptr [esp+10h]
        mov         edx,dword ptr [esp+18h]
        mov         eax,dword ptr [esp+10h]
        test        eax,eax
        je          integer_QnaN_or_zero
arg_is_not_integer_QnaN:
        fsubp       st(1),st
        test        edx,edx
        jns         positive
        fstp        dword ptr [esp]
        mov         ecx,dword ptr [esp]
        xor         ecx,80000000h
        add         ecx,7FFFFFFFh
        adc         eax,0
        mov         edx,dword ptr [esp+14h]
        adc         edx,0
        jmp         localexit
positive:
        fstp        dword ptr [esp]
        mov         ecx,dword ptr [esp]
        add         ecx,7FFFFFFFh
        sbb         eax,0
        mov         edx,dword ptr [esp+14h]
        sbb         edx,0
        jmp         localexit
integer_QnaN_or_zero:
        mov         edx,dword ptr [esp+14h]
        test        edx,7FFFFFFFh
        jne         arg_is_not_integer_QnaN
        fstp        dword ptr [esp+18h]
        fstp        dword ptr [esp+18h]
localexit:
        leave
        ret
__ftol ENDP

PUBLIC __ftol2
__ftol2 PROC
	jmp __ftol
__ftol2 ENDP

PUBLIC __ftol2_sse
__ftol2_sse PROC
    jmp __ftol
__ftol2_sse ENDP

PUBLIC __allmul
__allmul PROC
        mov         eax, dword ptr[esp+8]
        mov         ecx, dword ptr[esp+10h]
        or          ecx, eax
        mov         ecx, dword ptr[esp+0Ch]
        jne         hard
        mov         eax, dword ptr[esp+4]
        mul         ecx
        ret         10h
hard:
        push        ebx
        mul         ecx
        mov         ebx, eax
        mov         eax, dword ptr[esp+8]
        mul         dword ptr[esp+14h]
        add         ebx, eax
        mov         eax, dword ptr[esp+8]
        mul         ecx
        add         edx, ebx
        pop         ebx
        ret         10h
__allmul ENDP

PUBLIC __alldiv
__alldiv PROC
       push        edi
        push        esi
        push        ebx
        xor         edi,edi
        mov         eax,dword ptr [esp+14h]
        or          eax,eax
        jge         L1
        inc         edi
        mov         edx,dword ptr [esp+10h]
        neg         eax
        neg         edx
        sbb         eax,0
        mov         dword ptr [esp+14h],eax
        mov         dword ptr [esp+10h],edx
L1:
        mov         eax,dword ptr [esp+1Ch]
        or          eax,eax
        jge         L2
        inc         edi
        mov         edx,dword ptr [esp+18h]
        neg         eax
        neg         edx
        sbb         eax,0
        mov         dword ptr [esp+1Ch],eax
        mov         dword ptr [esp+18h],edx
L2:
        or          eax,eax
        jne         L3
        mov         ecx,dword ptr [esp+18h]
        mov         eax,dword ptr [esp+14h]
        xor         edx,edx
        div         ecx
        mov         ebx,eax
        mov         eax,dword ptr [esp+10h]
        div         ecx
        mov         edx,ebx
        jmp         L4
L3:
        mov         ebx,eax
        mov         ecx,dword ptr [esp+18h]
        mov         edx,dword ptr [esp+14h]
        mov         eax,dword ptr [esp+10h]
L5:
        shr         ebx,1
        rcr         ecx,1
        shr         edx,1
        rcr         eax,1
        or          ebx,ebx
        jne         L5
        div         ecx
        mov         esi,eax
        mul         dword ptr [esp+1Ch]
        mov         ecx,eax
        mov         eax,dword ptr [esp+18h]
        mul         esi
        add         edx,ecx
        jb          L6
        cmp         edx,dword ptr [esp+14h]
        ja          L6
        jb          L7
        cmp         eax,dword ptr [esp+10h]
        jbe         L7
L6:
        dec         esi
L7:
        xor         edx,edx
        mov         eax,esi
L4:
        dec         edi
        jne         L8
        neg         edx
        neg         eax
        sbb         edx,0
L8:
        pop         ebx
        pop         esi
        pop         edi
        ret         10h
__alldiv ENDP

PUBLIC __aulldiv
__aulldiv PROC
        push        ebx
        push        esi
        mov         eax,dword ptr [esp+18h]
        or          eax,eax
        jne         L1
        mov         ecx,dword ptr [esp+14h]
        mov         eax,dword ptr [esp+10h]
        xor         edx,edx
        div         ecx
        mov         ebx,eax
        mov         eax,dword ptr [esp+0Ch]
        div         ecx
        mov         edx,ebx
        jmp         L2
L1:
        mov         ecx,eax
        mov         ebx,dword ptr [esp+14h]
        mov         edx,dword ptr [esp+10h]
        mov         eax,dword ptr [esp+0Ch]
L3:
        shr         ecx,1
        rcr         ebx,1
        shr         edx,1
        rcr         eax,1
        or          ecx,ecx
        jne         L3
        div         ebx
        mov         esi,eax
        mul         dword ptr [esp+18h]
        mov         ecx,eax
        mov         eax,dword ptr [esp+14h]
        mul         esi
        add         edx,ecx
        jb          L4
        cmp         edx,dword ptr [esp+10h]
        ja          L4
        jb          L5
        cmp         eax,dword ptr [esp+0Ch]
        jbe         L5
L4:
        dec         esi
L5:
        xor         edx,edx
        mov         eax,esi
L2:
        pop         esi
        pop         ebx
        ret         10h
__aulldiv ENDP

PUBLIC __allrem
__allrem PROC
        push        ebx
        push        edi
        xor         edi,edi
        mov         eax,dword ptr [esp+10h]
        or          eax,eax
        jge         L1
        inc         edi
        mov         edx,dword ptr [esp+0Ch]
        neg         eax
        neg         edx
        sbb         eax,0
        mov         dword ptr [esp+10h],eax
        mov         dword ptr [esp+0Ch],edx
L1:
        mov         eax,dword ptr [esp+18h]
        or          eax,eax
        jge         L2
        mov         edx,dword ptr [esp+14h]
        neg         eax
        neg         edx
        sbb         eax,0
        mov         dword ptr [esp+18h],eax
        mov         dword ptr [esp+14h],edx
L2:
        or          eax,eax
        jne         L3
        mov         ecx,dword ptr [esp+14h]
        mov         eax,dword ptr [esp+10h]
        xor         edx,edx
        div         ecx
        mov         eax,dword ptr [esp+0Ch]
        div         ecx
        mov         eax,edx
        xor         edx,edx
        dec         edi
        jns         L4
        jmp         L8
L3:
        mov         ebx,eax
        mov         ecx,dword ptr [esp+14h]
        mov         edx,dword ptr [esp+10h]
        mov         eax,dword ptr [esp+0Ch]
L5:
        shr         ebx,1
        rcr         ecx,1
        shr         edx,1
        rcr         eax,1
        or          ebx,ebx
        jne         L5
        div         ecx
        mov         ecx,eax
        mul         dword ptr [esp+18h]
        xchg        eax,ecx
        mul         dword ptr [esp+14h]
        add         edx,ecx
        jb          L6
        cmp         edx,dword ptr [esp+10h]
        ja          L6
        jb          L7
        cmp         eax,dword ptr [esp+0Ch]
        jbe         L7
L6:
        sub         eax,dword ptr [esp+14h]
        sbb         edx,dword ptr [esp+18h]
L7:
        sub         eax,dword ptr [esp+0Ch]
        sbb         edx,dword ptr [esp+10h]
        dec         edi
        jns         L8
L4:
        neg         edx
        neg         eax
        sbb         edx,0
L8:
        pop         edi
        pop         ebx
        ret         10h
__allrem ENDP

PUBLIC __allshl
__allshl PROC

        cmp         cl,40h
        jae         RETZERO
        cmp         cl,20h
        jae         MORE32
        shld        edx,eax,cl
        shl         eax,cl
        ret
MORE32:
        mov         edx,eax
        xor         eax,eax
        and         cl,1Fh
        shl         edx,cl
        ret
RETZERO:
        xor         eax,eax
        xor         edx,edx
        ret
__allshl ENDP

PUBLIC __allshr
__allshr PROC
        cmp         cl,40h
        jae         RETZERO
        cmp         cl,20h
        jae         MORE32
        shrd        eax,edx,cl
        sar         edx,cl
        ret
MORE32:
        mov         eax,edx
        xor         edx,edx
        and         cl,1Fh
        sar         eax,cl
        ret
RETZERO:
        xor         eax,eax
        xor         edx,edx
        ret
__allshr ENDP

PUBLIC __aullshr
__aullshr PROC
        cmp         cl,40h
        jae         RETZERO
        cmp         cl,20h
        jae         MORE32
        shrd        eax,edx,cl
        shr         edx,cl
        ret
MORE32:
        mov         eax,edx
        xor         edx,edx
        and         cl,1Fh
        shr         eax,cl
        ret
RETZERO:
        xor         eax,eax
        xor         edx,edx
        ret
__aullshr ENDP

PUBLIC __aullrem
__aullrem PROC
        push        ebx
        mov         eax,dword ptr [esp+14h]
        or          eax,eax
        jne         L1
        mov         ecx,dword ptr [esp+10h]
        mov         eax,dword ptr [esp+0Ch]
        xor         edx,edx
        div         ecx
        mov         eax,dword ptr [esp+8]
        div         ecx
        mov         eax,edx
        xor         edx,edx
        jmp         L2
L1:
        mov         ecx,eax
        mov         ebx,dword ptr [esp+10h]
        mov         edx,dword ptr [esp+0Ch]
        mov         eax,dword ptr [esp+8]
L3:
        shr         ecx,1
        rcr         ebx,1
        shr         edx,1
        rcr         eax,1
        or          ecx,ecx
        jne         L3
        div         ebx
        mov         ecx,eax
        mul         dword ptr [esp+14h]
        xchg        eax,ecx
        mul         dword ptr [esp+10h]
        add         edx,ecx
        jb          L4
        cmp         edx,dword ptr [esp+0Ch]
        ja          L4
        jb          L5
        cmp         eax,dword ptr [esp+8]
        jbe         L5
L4:
        sub         eax,dword ptr [esp+10h]
        sbb         edx,dword ptr [esp+14h]
L5:
        sub         eax,dword ptr [esp+8]
        sbb         edx,dword ptr [esp+0Ch]
        neg         edx
        neg         eax
        sbb         edx,0
L2:
        pop         ebx
        ret         10h
__aullrem ENDP

END
