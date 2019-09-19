	.file	"386.c"
	.text
	.p2align 4,,15
	.globl	startclock
	.type	startclock, @function
startclock:
.LFB8:
	.cfi_startproc
	leaq	start(%rip), %rdi
	subq	$8, %rsp
	.cfi_def_cfa_offset 16
	movl	$100, hz(%rip)
	call	times@PLT
	addq	$8, %rsp
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE8:
	.size	startclock, .-startclock
	.p2align 4,,15
	.globl	stopclock
	.type	stopclock, @function
stopclock:
.LFB9:
	.cfi_startproc
	subq	$56, %rsp
	.cfi_def_cfa_offset 64
	movq	%rsp, %rdi
	movq	%fs:40, %rax
	movq	%rax, 40(%rsp)
	xorl	%eax, %eax
	call	times@PLT
	movq	(%rsp), %rax
	subl	start(%rip), %eax
	movq	40(%rsp), %rdx
	xorq	%fs:40, %rdx
	jne	.L8
	addq	$56, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 8
	ret
.L8:
	.cfi_restore_state
	call	__stack_chk_fail@PLT
	.cfi_endproc
.LFE9:
	.size	stopclock, .-stopclock
	.local	start
	.comm	start,32,32
	.globl	tcmp
	.data
	.align 8
	.type	tcmp, @object
	.size	tcmp, 8
tcmp:
	.long	171798692
	.long	1075028951
	.comm	hz,4,4
	.ident	"GCC: (Ubuntu 7.4.0-1ubuntu1~18.04.1) 7.4.0"
	.section	.note.GNU-stack,"",@progbits
