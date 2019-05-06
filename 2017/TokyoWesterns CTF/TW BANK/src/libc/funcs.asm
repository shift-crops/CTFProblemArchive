global _libc_args, _exit, _read, _write, _open, _close, _munmap, _mmap2

_libc_args:
	push ebp
	mov ebp, esp

	mov esi, [ebp+0x08]
	mov eax, [esi]
        lea ecx, [esi+0x04]
        mov edx, eax
        inc edx
        imul edx, 0x4
        add edx, ecx

	mov edi, [ebp+0x0c]
	mov [edi], ecx
	mov edi, [ebp+0x10]
	mov [edi], edx

	leave
	ret

_exit:
	push ebp
	mov ebp, esp
	mov eax, 0x01
	call __arg2reg
	mov eax, 0x01
	int 0x80
	pop ebp
	hlt

_read:
	push ebp
	mov ebp, esp
	push ebx
	mov eax, 0x03
	call __arg2reg
	mov eax, 0x03
	int 0x80
	pop ebx
	pop ebp
	ret

_write:
	push ebp
	mov ebp, esp
	push ebx
	mov eax, 0x03
	call __arg2reg
	mov eax, 0x04
	int 0x80
	pop ebx
	pop ebp
	ret

_open:
	push ebp
	mov ebp, esp
	push ebx
	mov eax, 0x03
	call __arg2reg
	mov eax, 0x05
	int 0x80
	pop ebx
	pop ebp
	ret

_close:
	push ebp
	mov ebp, esp
	push ebx
	mov eax, 0x01
	call __arg2reg
	mov eax, 0x06
	int 0x80
	pop ebx
	pop ebp
	ret

_munmap:
	push ebp
	mov ebp, esp
	push ebx
	mov eax, 0x02
	call __arg2reg
	mov eax, 0x5b
	int 0x80
	pop ebx
	pop ebp
	ret

_mmap2:
	push ebp
	mov ebp, esp
	push ebx
	push esi
	push edi

	; drop PROT_EXEC
	mov edx, 0x04
	not edx
	and [ebp+0x10], edx

	mov eax, 0x06
	call __arg2reg
	mov eax, 0xc0
	int 0x80
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

__arg2reg:
	mov ebx, [ebp+0x08]
	cmp eax, 1
	jle __ret
	mov ecx, [ebp+0x0c]
	cmp eax, 2
	jle __ret
	mov edx, [ebp+0x10]
	cmp eax, 3
	jle __ret
	mov esi, [ebp+0x14]
	cmp eax, 4
	jle __ret
	mov edi, [ebp+0x18]
	cmp eax, 5
	jle __ret
	mov ebp, [ebp+0x1c]
__ret:
	ret
