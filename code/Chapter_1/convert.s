	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 11, 1	sdk_version 11, 1
	.globl	_main                   ; -- Begin function main
	.p2align	2
_main:                                  ; @main
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #48             ; =48
	stp	x29, x30, [sp, #32]     ; 16-byte Folded Spill
	add	x29, sp, #32            ; =32
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	mov	w8, #0
	stur	wzr, [x29, #-4]
	adrp	x0, l_.str@PAGE
	add	x0, x0, l_.str@PAGEOFF
	stur	w8, [x29, #-12]         ; 4-byte Folded Spill
	bl	_printf
	adrp	x9, l_.str.1@PAGE
	add	x9, x9, l_.str.1@PAGEOFF
	mov	x0, x9
	mov	x9, sp
	sub	x10, x29, #8            ; =8
	str	x10, [x9]
	bl	_scanf
	ldur	w8, [x29, #-8]
                                        ; implicit-def: $x1
	mov	x1, x8
	adrp	x9, l_.str.2@PAGE
	add	x9, x9, l_.str.2@PAGEOFF
	mov	x0, x9
	mov	x9, sp
	str	x1, [x9]
	bl	_printf
	ldur	w8, [x29, #-8]
                                        ; implicit-def: $x1
	mov	x1, x8
	adrp	x9, l_.str.3@PAGE
	add	x9, x9, l_.str.3@PAGEOFF
	mov	x0, x9
	mov	x9, sp
	str	x1, [x9]
	bl	_printf
	ldur	w8, [x29, #-8]
                                        ; implicit-def: $x1
	mov	x1, x8
	adrp	x9, l_.str.4@PAGE
	add	x9, x9, l_.str.4@PAGEOFF
	mov	x0, x9
	mov	x9, sp
	str	x1, [x9]
	bl	_printf
	ldur	w8, [x29, #-12]         ; 4-byte Folded Reload
	mov	x0, x8
	ldp	x29, x30, [sp, #32]     ; 16-byte Folded Reload
	add	sp, sp, #48             ; =48
	ret
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__cstring,cstring_literals
l_.str:                                 ; @.str
	.asciz	"Enter an integer: "

l_.str.1:                               ; @.str.1
	.asciz	"%d"

l_.str.2:                               ; @.str.2
	.asciz	"decimal:    %d\n"

l_.str.3:                               ; @.str.3
	.asciz	"octal:    %o\n"

l_.str.4:                               ; @.str.4
	.asciz	"hexadecimal:    %x\n"

.subsections_via_symbols
