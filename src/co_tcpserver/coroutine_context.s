	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 12, 3	sdk_version 12, 3
	.globl	__ZN4cweb9tcpserver9coroutine16CoroutineContextC2Ev ## -- Begin function _ZN4cweb9tcpserver9coroutine16CoroutineContextC2Ev
	.p2align	4, 0x90
__ZN4cweb9tcpserver9coroutine16CoroutineContextC2Ev: ## @_ZN4cweb9tcpserver9coroutine16CoroutineContextC2Ev
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -24(%rbp)                 ## 8-byte Spill
	movq	$0, 72(%rax)
	callq	__ZNSt3__1L15__get_nullptr_tEv
	movq	%rax, -16(%rbp)
	leaq	-16(%rbp), %rdi
	callq	__ZNKSt3__19nullptr_tcvPT_IcEEv
	movq	%rax, %rcx
	movq	-24(%rbp), %rax                 ## 8-byte Reload
	movq	%rcx, 80(%rax)
	addq	$32, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90                         ## -- Begin function _ZNSt3__1L15__get_nullptr_tEv
__ZNSt3__1L15__get_nullptr_tEv:         ## @_ZNSt3__1L15__get_nullptr_tEv
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	leaq	-8(%rbp), %rdi
	movq	$-1, %rsi
	callq	__ZNSt3__19nullptr_tC1EMNS0_5__natEi
	movq	-8(%rbp), %rax
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90                         ## -- Begin function _ZNKSt3__19nullptr_tcvPT_IcEEv
__ZNKSt3__19nullptr_tcvPT_IcEEv:        ## @_ZNKSt3__19nullptr_tcvPT_IcEEv
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	xorl	%eax, %eax
                                        ## kill: def $rax killed $eax
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN4cweb9tcpserver9coroutine16CoroutineContextC1Ev ## -- Begin function _ZN4cweb9tcpserver9coroutine16CoroutineContextC1Ev
	.p2align	4, 0x90
__ZN4cweb9tcpserver9coroutine16CoroutineContextC1Ev: ## @_ZN4cweb9tcpserver9coroutine16CoroutineContextC1Ev
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	callq	__ZN4cweb9tcpserver9coroutine16CoroutineContextC2Ev
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN4cweb9tcpserver9coroutine16CoroutineContextC2EmPFvPvEPKv ## -- Begin function _ZN4cweb9tcpserver9coroutine16CoroutineContextC2EmPFvPvEPKv
	.p2align	4, 0x90
__ZN4cweb9tcpserver9coroutine16CoroutineContextC2EmPFvPvEPKv: ## @_ZN4cweb9tcpserver9coroutine16CoroutineContextC2EmPFvPvEPKv
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -48(%rbp)                 ## 8-byte Spill
	movq	-16(%rbp), %rcx
	movq	%rcx, 72(%rax)
	callq	__ZNSt3__1L15__get_nullptr_tEv
	movq	%rax, -40(%rbp)
	leaq	-40(%rbp), %rdi
	callq	__ZNKSt3__19nullptr_tcvPT_IcEEv
	movq	-48(%rbp), %rdi                 ## 8-byte Reload
	movq	%rax, 80(%rdi)
	movq	-16(%rbp), %rsi
	movq	-24(%rbp), %rdx
	movq	-32(%rbp), %rcx
	callq	__ZN4cweb9tcpserver9coroutine16CoroutineContext4InitEmPFvPvEPKv
	addq	$48, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN4cweb9tcpserver9coroutine16CoroutineContext4InitEmPFvPvEPKv ## -- Begin function _ZN4cweb9tcpserver9coroutine16CoroutineContext4InitEmPFvPvEPKv
	.p2align	4, 0x90
__ZN4cweb9tcpserver9coroutine16CoroutineContext4InitEmPFvPvEPKv: ## @_ZN4cweb9tcpserver9coroutine16CoroutineContext4InitEmPFvPvEPKv
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, -48(%rbp)                 ## 8-byte Spill
	movq	-16(%rbp), %rdi
	callq	_malloc
	movq	-48(%rbp), %rdi                 ## 8-byte Reload
	movq	%rax, 80(%rdi)
	movq	80(%rdi), %rax
	addq	72(%rdi), %rax
	addq	$-8, %rax
	movq	%rax, -40(%rbp)
	movq	-40(%rbp), %rax
	andq	$-16, %rax
	movq	%rax, -40(%rbp)
	xorl	%esi, %esi
	movl	$72, %edx
	callq	_memset
	movq	-48(%rbp), %rax                 ## 8-byte Reload
	movq	-24(%rbp), %rdx
	movq	-40(%rbp), %rcx
	movq	%rdx, (%rcx)
	movq	-40(%rbp), %rcx
	addq	$8, %rcx
	movq	%rcx, 32(%rax)
	movq	-40(%rbp), %rcx
	movq	%rcx, 56(%rax)
	movq	-32(%rbp), %rcx
	movq	%rcx, 64(%rax)
	movq	-24(%rbp), %rcx
	movq	%rcx, 40(%rax)
	addq	$48, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN4cweb9tcpserver9coroutine16CoroutineContextC1EmPFvPvEPKv ## -- Begin function _ZN4cweb9tcpserver9coroutine16CoroutineContextC1EmPFvPvEPKv
	.p2align	4, 0x90
__ZN4cweb9tcpserver9coroutine16CoroutineContextC1EmPFvPvEPKv: ## @_ZN4cweb9tcpserver9coroutine16CoroutineContextC1EmPFvPvEPKv
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	movq	-24(%rbp), %rdx
	movq	-32(%rbp), %rcx
	callq	__ZN4cweb9tcpserver9coroutine16CoroutineContextC2EmPFvPvEPKv
	addq	$32, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN4cweb9tcpserver9coroutine16CoroutineContextD2Ev ## -- Begin function _ZN4cweb9tcpserver9coroutine16CoroutineContextD2Ev
	.p2align	4, 0x90
__ZN4cweb9tcpserver9coroutine16CoroutineContextD2Ev: ## @_ZN4cweb9tcpserver9coroutine16CoroutineContextD2Ev
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	80(%rax), %rdi
	callq	_free
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN4cweb9tcpserver9coroutine16CoroutineContextD1Ev ## -- Begin function _ZN4cweb9tcpserver9coroutine16CoroutineContextD1Ev
	.p2align	4, 0x90
__ZN4cweb9tcpserver9coroutine16CoroutineContextD1Ev: ## @_ZN4cweb9tcpserver9coroutine16CoroutineContextD1Ev
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rdi
	callq	__ZN4cweb9tcpserver9coroutine16CoroutineContextD2Ev
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	__ZN4cweb9tcpserver9coroutine16CoroutineContext11ContextSwapEPS2_S3_ ## -- Begin function _ZN4cweb9tcpserver9coroutine16CoroutineContext11ContextSwapEPS2_S3_
	.p2align	4, 0x90
__ZN4cweb9tcpserver9coroutine16CoroutineContext11ContextSwapEPS2_S3_: ## @_ZN4cweb9tcpserver9coroutine16CoroutineContext11ContextSwapEPS2_S3_
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	callq	__ZN4cweb9tcpserver9coroutine12context_swapEPNS1_16CoroutineContextES3_
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90                         ## -- Begin function _ZNSt3__19nullptr_tC1EMNS0_5__natEi
__ZNSt3__19nullptr_tC1EMNS0_5__natEi:   ## @_ZNSt3__19nullptr_tC1EMNS0_5__natEi
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rdi
	movq	-16(%rbp), %rsi
	callq	__ZNSt3__19nullptr_tC2EMNS0_5__natEi
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.p2align	4, 0x90                         ## -- Begin function _ZNSt3__19nullptr_tC2EMNS0_5__natEi
__ZNSt3__19nullptr_tC2EMNS0_5__natEi:   ## @_ZNSt3__19nullptr_tC2EMNS0_5__natEi
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	$0, (%rax)
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
.subsections_via_symbols
