	.file	"prog.c"
	.text
	.globl	wtime
	.type	wtime, @function
wtime:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	leaq	-32(%rbp), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	clock_gettime@PLT
	movq	-32(%rbp), %rax
	pxor	%xmm1, %xmm1
	cvtsi2sdq	%rax, %xmm1
	movq	-24(%rbp), %rax
	pxor	%xmm2, %xmm2
	cvtsi2sdq	%rax, %xmm2
	movsd	.LC0(%rip), %xmm0
	mulsd	%xmm2, %xmm0
	addsd	%xmm1, %xmm0
	movq	%xmm0, %rax
	movq	-8(%rbp), %rdx
	subq	%fs:40, %rdx
	je	.L3
	call	__stack_chk_fail@PLT
.L3:
	movq	%rax, %xmm0
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	wtime, .-wtime
	.globl	image_is_dark
	.type	image_is_dark, @function
image_is_dark:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movl	%esi, -28(%rbp)
	movl	%edx, -32(%rbp)
	movl	$0, -12(%rbp)
	movl	$0, -8(%rbp)
	jmp	.L5
.L8:
	movl	$0, -4(%rbp)
	jmp	.L6
.L7:
	movl	-8(%rbp), %eax
	imull	-28(%rbp), %eax
	movl	%eax, %edx
	movl	-4(%rbp), %eax
	addl	%edx, %eax
	movslq	%eax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	shrb	$7, %al
	movzbl	%al, %eax
	addl	%eax, -12(%rbp)
	addl	$1, -4(%rbp)
.L6:
	movl	-4(%rbp), %eax
	cmpl	-28(%rbp), %eax
	jl	.L7
	addl	$1, -8(%rbp)
.L5:
	movl	-8(%rbp), %eax
	cmpl	-32(%rbp), %eax
	jl	.L8
	movl	-28(%rbp), %eax
	imull	-32(%rbp), %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	cmpl	%eax, -12(%rbp)
	setl	%al
	movzbl	%al, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	image_is_dark, .-image_is_dark
	.section	.rodata
.LC1:
	.string	"Time %.6f, dark %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	$132710400, %edi
	call	malloc@PLT
	movq	%rax, -16(%rbp)
	movl	$0, %edi
	call	srand@PLT
	movl	$0, -28(%rbp)
	jmp	.L11
.L14:
	movl	$0, -24(%rbp)
	jmp	.L12
.L13:
	call	rand@PLT
	cltd
	shrl	$24, %edx
	addl	%edx, %eax
	movzbl	%al, %eax
	subl	%edx, %eax
	movl	%eax, %ecx
	movl	-28(%rbp), %eax
	imull	$15360, %eax, %edx
	movl	-24(%rbp), %eax
	addl	%edx, %eax
	movslq	%eax, %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movl	%ecx, %edx
	movb	%dl, (%rax)
	addl	$1, -24(%rbp)
.L12:
	cmpl	$15359, -24(%rbp)
	jle	.L13
	addl	$1, -28(%rbp)
.L11:
	cmpl	$8639, -28(%rbp)
	jle	.L14
	movl	$0, %eax
	call	wtime
	movq	%xmm0, %rax
	movq	%rax, -8(%rbp)
	movq	-16(%rbp), %rax
	movl	$8640, %edx
	movl	$15360, %esi
	movq	%rax, %rdi
	call	image_is_dark
	movl	%eax, -20(%rbp)
	movl	$0, %eax
	call	wtime
	movq	%xmm0, %rax
	movq	%rax, %xmm0
	subsd	-8(%rbp), %xmm0
	movsd	%xmm0, -8(%rbp)
	movl	-20(%rbp), %edx
	movq	-8(%rbp), %rax
	movl	%edx, %esi
	movq	%rax, %xmm0
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$1, %eax
	call	printf@PLT
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	free@PLT
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	main, .-main
	.section	.rodata
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
