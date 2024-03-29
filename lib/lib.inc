global _start

section .data
message: db 'aye sup dawg', 0
block: db 'xxxxxxxxxxxxxxxxxxxx', 0
msg: db 'aye sup dawg', 0
parseme: db '-123456', 0


section .text

exit:
	mov rax, 60			; 'exit' syscall num
	xor rdi, rdi		; zero rdi
	syscall	
	ret

string_length:
	xor rax, rax		; len in rax after proc
	cmp byte [rdi+rax], 0	;test for null byte outside of loop, fix bug on ''
	je .ret
 .loop:
	inc rax
	cmp byte [rdi+rax], 0
	; inc rax
	; dec rax
	jnz .loop
  .ret:
	ret

print_string:
	call string_length
	mov rdx, rax		; length arg moved for syscall		
	mov rsi, rdi		; string ptr moved for syscall
	mov rax, 1			; 'write' syscall numb
	mov rdi, 1			; stdout descriptor
	syscall
	ret				

print_char:
	push rdi			; place arg to print on stack
	mov rsi, rsp		; pointer to the value on stack
	mov rdx, 1			; length to write
	mov rdi, 1			; file descriptor where to write
	mov rax, 1			; syscall write code
	syscall
	pop rdi
	ret

print_newline:
mov rax, 1
mov rdi, 1
	mov rdx, 1
	push 10
	mov rsi, rsp
	syscall
	pop rdi
	ret

print_uint:
	cmp rdi, 0
	jz .handlezero
	mov rax, rdi		;place arg into diff register
	xor rbx, rbx		;count in rbx from zero
	mov rdi, 10			;need 10 in a register 
  .fillbuf:
	cmp rax, 0			;if rax is zero, 
	je .revprint			;jump to print
	xor rdx, rdx		;zero
	div rdi			; rax <- qoutient, rdx <- rem 
	add rdx, '0'		; transform digit to ASCII code
	push rdx			; ASCII digits stored in reverse
	inc rbx				
	jmp .fillbuf
  .revprint:
	cmp rbx, 0
	je .end
	dec rbx	
	mov rsi, rsp
	mov rdi, 1
	mov rdx, 1
	mov rax, 1
	syscall
	pop rax
	jmp .revprint

  .end:
	ret

  .handlezero:
	mov rsi, '0'
	push rsi
	mov rsi, rsp
	mov rdi, 1
	mov rdx, 1
	mov rax, 1
	syscall
	pop rax
	ret

print_int:
	cmp rdi, 0
	jge print_uint 
	push rdi
	mov rdi, '-'
	call print_char
	pop rdi	
	neg rdi
	call print_uint
	ret

read_char:
	push rdi
	; no args, returns char in rax
	push rax	;just make a spot in stack so i can read into the addr location
	mov rsi, rsp	;the ptr to the place on the stack is arg to syscall read
	mov rdi, 0	; stdin 
	mov rax, 0	; syscall num for read
	mov rdx, 1 	; length
	syscall
	pop rax
	pop rdi
	ret

 
read_word:	;takes buf addr in rdi, size in rsi  
  	push rdi	;going to use this reg but need bufbase to pop rax for ret
  	dec rsi	;length -1
  
  .leadingwhite:
	call read_char
	cmp rax, 9 
	je .leadingwhite
	cmp rax, 10
	je .leadingwhite
	cmp rax, 32
	je .leadingwhite

  .nonwhite:
	mov byte [rdi], al
	inc rdi 
	dec rsi	

	call read_char
	cmp rax, 9
	je .retbuf
	cmp rax, 10
	je .retbuf
	cmp rax, 32
	je .retbuf

	cmp rsi, 0
	jne .nonwhite

 
	xor rax, rax
	ret
  .retbuf:
	mov byte[rdi], 0 ;attach nullbyte
	pop rax
	ret		
	
string_equals:		
	mov al, byte[rsi]
	mov dl, byte[rdi]
	inc rsi
	inc rdi
	cmp al, dl
	jne .ret0
	cmp al, 0
	jne string_equals
  .ret1:
	mov rax, 1
	ret
  .ret0:
	mov rax, 0
	ret

string_copy:	;rdi <- ptr to str, rsi <- ptr to buf, rdx <- buf len
	push rsi	;going to ret this in rax if success
	inc rdx
  .loop:
	dec rdx
	cmp rdx, 0
	je .ovrflo

	mov al, byte[rdi]
	mov byte[rsi], al
	inc rdi
	inc rsi	
	cmp al, 0
	jne .loop
	pop rax
	ret

  .ovrflo:
	mov rax, 0
	ret		

parse_uint:	;ptr to str in rdi. rets number in rax, char count in rdx
	push rdi
	xor rax, rax
	mov rcx, 10

  .loop:
	mov bl, byte[rdi]
	cmp rbx, 0
	je .ret
	inc rdi	;ptr advanced
	sub rbx, 48	;convert atoi
	mul rcx	;accumulator shift, ie acc *=10
	add rax, rbx	;accumulator += next int
	jmp .loop
 
  .ret:
	pop rdx
	sub rdi, rdx
	mov rdx, rsi
	ret
	
parse_int:
	cmp byte[rdi], '-'
	jne parse_uint
	
	inc rdi
	call parse_uint
	neg rax
	ret
	
	














