bits 32

section .data
   rval dd 4

section .asm

global call, ret
extern push, pop

call:
	push	ebp
	mov	ebp, esp
	sub	esp, 0x4

	mov	eax, [ebp+0x4]
	mov	[esp], eax
	call	push
	mov	eax, [ebp]
	mov	[esp], eax
	call	push

	mov	eax, [ebp+0x8]
	mov	edx, ret_stub
	mov	[ebp+0x8], edx
	leave
	add	esp, 0x4
	jmp	eax

ret:
	mov	eax, [esp+0x4]
	mov	[rval], eax

	call	pop
	mov	[ebp], eax
	mov	eax, restore_eip
	mov	[ebp+0x4], eax
	ret

restore_eip:
	sub	esp, 0x4
	call	pop
	mov	[esp], eax
	mov	eax, [rval]
	jmp	[esp]

ret_stub:
	sub	esp, 0x8
	mov	ebp, esp
	push	eax
	call	ret
	leave
	ret

