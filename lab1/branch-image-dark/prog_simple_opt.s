	.file	"prog.c"
	.text
	.p2align 4
	.globl	wtime
	.type	wtime, @function
wtime:
.LFB39:
	.cfi_startproc
	endbr64
	subq	$40, %rsp
	.cfi_def_cfa_offset 48
	movl	$1, %edi
	movq	%fs:40, %rax
	movq	%rax, 24(%rsp)
	xorl	%eax, %eax
	movq	%rsp, %rsi
	call	clock_gettime@PLT
	pxor	%xmm0, %xmm0
	pxor	%xmm1, %xmm1
	cvtsi2sdq	8(%rsp), %xmm0
	mulsd	.LC0(%rip), %xmm0
	cvtsi2sdq	(%rsp), %xmm1
	addsd	%xmm1, %xmm0
	movq	24(%rsp), %rax
	subq	%fs:40, %rax
	jne	.L5
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L5:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE39:
	.size	wtime, .-wtime
	.p2align 4
	.globl	image_is_dark
	.type	image_is_dark, @function
image_is_dark:
.LFB40:
	.cfi_startproc
	endbr64
	pushq	%rbx
	.cfi_def_cfa_offset 16
	.cfi_offset 3, -16
	testl	%edx, %edx
	jle	.L12
	movq	%rdi, %r11
	leal	-1(%rsi), %eax
	xorl	%r10d, %r10d
	xorl	%r9d, %r9d
	xorl	%edi, %edi
	leaq	1(%r11,%rax), %rbx
	.p2align 4,,10
	.p2align 3
.L8:
	movslq	%r10d, %r8
	leaq	(%r11,%r8), %rcx
	addq	%rbx, %r8
	testl	%esi, %esi
	jle	.L11
	.p2align 4,,10
	.p2align 3
.L9:
	movzbl	(%rcx), %eax
	addq	$1, %rcx
	shrb	$7, %al
	movzbl	%al, %eax
	addl	%eax, %edi
	cmpq	%rcx, %r8
	jne	.L9
.L11:
	addl	$1, %r9d
	addl	%esi, %r10d
	cmpl	%r9d, %edx
	jne	.L8
.L7:
	imull	%esi, %edx
	popq	%rbx
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	movl	%edx, %eax
	shrl	$31, %eax
	addl	%edx, %eax
	sarl	%eax
	cmpl	%edi, %eax
	setg	%al
	movzbl	%al, %eax
	ret
.L12:
	.cfi_restore_state
	xorl	%edi, %edi
	jmp	.L7
	.cfi_endproc
.LFE40:
	.size	image_is_dark, .-image_is_dark
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC1:
	.string	"Time %.6f, dark %d\n"
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB41:
	.cfi_startproc
	endbr64
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	movl	$132710400, %edi
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	pushq	%rbx
	.cfi_def_cfa_offset 40
	.cfi_offset 3, -40
	subq	$24, %rsp
	.cfi_def_cfa_offset 64
	call	malloc@PLT
	xorl	%edi, %edi
	movq	%rax, %r12
	leaq	15360(%r12), %rbx
	leaq	132725760(%r12), %rbp
	call	srand@PLT
.L16:
	leaq	-15360(%rbx), %r13
	.p2align 4,,10
	.p2align 3
.L17:
	call	rand@PLT
	addq	$1, %r13
	cltd
	shrl	$24, %edx
	addl	%edx, %eax
	movzbl	%al, %eax
	subl	%edx, %eax
	movb	%al, -1(%r13)
	cmpq	%rbx, %r13
	jne	.L17
	leaq	15360(%r13), %rbx
	cmpq	%rbp, %rbx
	jne	.L16
	xorl	%eax, %eax
	call	wtime
	movq	%r12, %rdi
	movl	$8640, %edx
	movl	$15360, %esi
	movsd	%xmm0, 8(%rsp)
	call	image_is_dark
	movl	%eax, %r13d
	xorl	%eax, %eax
	call	wtime
	subsd	8(%rsp), %xmm0
	movl	%r13d, %edx
	leaq	.LC1(%rip), %rsi
	movl	$1, %edi
	movl	$1, %eax
	call	__printf_chk@PLT
	movq	%r12, %rdi
	call	free@PLT
	addq	$24, %rsp
	.cfi_def_cfa_offset 40
	xorl	%eax, %eax
	popq	%rbx
	.cfi_def_cfa_offset 32
	popq	%rbp
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE41:
	.size	main, .-main
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC0:
	.long	-400107883
	.long	1041313291
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
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
