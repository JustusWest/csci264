.file	"stats.c"
	.text
	.globl	min_max
	.type	min_max, @function
min_max:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	$0, %r8			# temp_min = 0
	movq	$0, %r9			# temp_max = 0
	movq	$1, %rax		# i = 1
	jmp	.L2
.L5:
    movq	(%rdi,%rax,8), %r10	# %r10 = arr[i]
	cmpq	(%rdi,%r8,8), %r10	# compare arr[i] with arr[temp_min]
	jge	.L3
	movq	%rax, %r8		# temp_min = i
.L3:
    #TODO: Update similar to L5
	movq	-8(%rbp), %rax
	leaq	0(,%rax,8), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rdx
	movq	-48(%rbp), %rax
	movq	(%rax), %rax
	leaq	0(,%rax,8), %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	movq	(%rax), %rax
	cmpq	%rax, %rdx
	jle	.L4
	movq	-48(%rbp), %rax
	movq	-8(%rbp), %rdx
	movq	%rdx, (%rax)
.L4:
	addq	$1, %rax #i++
.L2:
    cmpq	%rsi, %rax		# compare i with size
	jl	.L5
    # HINT: the function has arguements *min and *max, which are pointers
    # r8 and r9 are storing temp values for min and max. You need to assign
    # them by following the pointer before returning.
	popq	%rbp
	ret
	.size	min_max, .-min_max
	.globl	swap
	.type	swap, @function
swap:
	pushq	%rbp
	movq	%rsp, %rbp
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	-24(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -8(%rbp)
	movq	-32(%rbp), %rax
	movq	(%rax), %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, (%rax)
	movq	-32(%rbp), %rax
	movq	-8(%rbp), %rdx
	movq	%rdx, (%rax)
	nop
	popq	%rbp
	ret
	.size	swap, .-swap
	.section	.rodata
.LC0:
	.string	"%ld"
.LC1:
	.string	"malloc failed"
.LC2:
	.string	"%ld "
	.text
	.globl	main
	.type	main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$64, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	-56(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc99_scanf@PLT
	movq	-56(%rbp), %rax
	salq	$3, %rax
	movq	%rax, %rdi
	call	malloc@PLT
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jne	.L8
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	$1, %edi
	call	exit@PLT
.L8:
	movq	$0, -32(%rbp)
	jmp	.L9
.L10:
	movq	-32(%rbp), %rax
	leaq	0(,%rax,8), %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, %rsi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	__isoc99_scanf@PLT
	addq	$1, -32(%rbp)
.L9:
	movq	-56(%rbp), %rax
	cmpq	%rax, -32(%rbp)
	jl	.L10
	movq	-56(%rbp), %rsi
	leaq	-40(%rbp), %rcx
	leaq	-48(%rbp), %rdx
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	min_max
	movq	-40(%rbp), %rax
	leaq	0(,%rax,8), %rdx
	movq	-16(%rbp), %rax
	addq	%rax, %rdx
	movq	-48(%rbp), %rax
	leaq	0(,%rax,8), %rcx
	movq	-16(%rbp), %rax
	addq	%rcx, %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	swap
	movq	$0, -24(%rbp)
	jmp	.L11
.L12:
	movq	-24(%rbp), %rax
	leaq	0(,%rax,8), %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, %rsi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	addq	$1, -24(%rbp)
.L11:
	movq	-56(%rbp), %rax
	cmpq	%rax, -24(%rbp)
	jl	.L12
	movl	$10, %edi
	call	putchar@PLT
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	free@PLT
	movl	$0, %eax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L14
	call	__stack_chk_fail@PLT
.L14:
	leave
	ret
	.size	main, .-main
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4: