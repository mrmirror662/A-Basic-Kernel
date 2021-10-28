global start
global multiboot_info_ptr

%define text_addr 0xb8000
WHITE_ON_BLACK equ 0x0f 


section .text
[bits 32]
start:
    ;init stack
    mov [multiboot_info_ptr],ebx
    mov esp ,stack_top
    mov ebx , hello
    call print
    call Check_CPU_ID
    call Check_Long_Mode
    call init_pageing

    lgdt [GDT64.Pointer]    ; Load the 64-bit global descriptor table.

    jmp GDT64.Code:Realm64       ; Set the code segment and enter 64-bit long mode.
    hlt

print:
    mov edx , text_addr

    loop:    
        mov al,[ebx]
        mov ah , byte WHITE_ON_BLACK
        mov [edx],ax
        add edx,2
        add ebx,1
        cmp al , 0
        je end
        jmp loop
        end:
        ret

Check_CPU_ID:
    pushfd
    pop eax
 
    ; Copy to ECX as well for comparing later on
    mov ecx, eax
 
    ; Flip the ID bit
    xor eax, 1 << 21
 
    ; Copy EAX to FLAGS via the stack
    push eax
    popfd
 
    ; Copy FLAGS back to EAX (with the flipped bit if CPUID is supported)
    pushfd
    pop eax
 
    ; Restore FLAGS from the old version stored in ECX (i.e. flipping the ID bit
    ; back if it was ever flipped).
    push ecx
    popfd
 
    ; Compare EAX and ECX. If they are equal then that means the bit wasn't
    ; flipped, and CPUID isn't supported.
    xor eax, ecx
    jz .NoCPUID
    ret
.NoCPUID:
    mov ebx , nocpuidmsg
    call print
    hlt
Check_Long_Mode:
    mov eax, 0x80000000    ; Set the A-register to 0x80000000.
    cpuid                  ; CPU identification.
    cmp eax, 0x80000001    ; Compare the A-register with 0x80000001.
    jb No_Long_Mode
    mov eax, 0x80000001    ; Set the A-register to 0x80000001.
    cpuid                  ; CPU identification.
    test edx, 1 << 29      ; Test if the LM-bit, which is bit 29, is set in the D-register.
    jz No_Long_Mode      
    ret
No_Long_Mode:
    mov ebx , nolongmode
    hlt   


init_pageing:
    mov eax, cr0                                   ; Set the A-register to control register 0.
    and eax, 01111111111111111111111111111111b     ; Clear the PG-bit, which is bit 31.
    mov cr0, eax
    
    mov edi , 0x1000
    mov cr3 , edi
    xor eax , eax
    mov ecx , 4096
    rep stosd
    mov edi , cr3

    mov DWORD [edi] , 0x2003
    add edi , 0x1000
    mov DWORD [edi] , 0x3003
    add edi , 0x1000
    mov DWORD [edi] , 0x4003
    add edi , 0x1000

    mov ebx , 0x00000003
    mov ecx ,512

    .loop:
        mov DWORD [edi],ebx
        add ebx , 0x1000
        add edi , 8
        loop .loop;
    mov eax,cr4 
    or eax , 1<<5        
    mov cr4 , eax
    
    mov ecx, 0xC0000080          ; Set the C-register to 0xC0000080, which is the EFER MSR.
    rdmsr                        ; Read from the model-specific register.
    or eax, 1 << 8               ; Set the LM-bit which is the 9th bit (bit 8).
    wrmsr  

    mov eax, cr0                 ; Set the A-register to control register 0.
    or eax, 1 << 31 | 1 << 0     ; Set the PG-bit, which is the 31nd bit, and the PM-bit, which is the 0th bit.
    mov cr0, eax    

    ret     

Switch_From_PM:

section .data

nolongmode:
db "no long mode!!"
db 0

nocpuidmsg:
db "no cpuid!!"
db 0


hello:
db "Hello world!!"
db 0

multiboot_info_ptr:
resd 0
; Access bits
PRESENT        equ 1 << 7
NOT_SYS        equ 1 << 4
EXEC           equ 1 << 3
DC             equ 1 << 2
RW             equ 1 << 1
ACCESSED       equ 1 << 0
 
; Flags bits
GRAN_4K       equ 1 << 7
SZ_32         equ 1 << 6
LONG_MODE     equ 1 << 5
 
GDT64:
    .Null: equ $ - GDT64
        dq 0
    .Code: equ $ - GDT64
        dd 0xFFFF                                   ; Limit & Base (low)
        db 0                                        ; Base (mid)
        db PRESENT | NOT_SYS | EXEC | RW | 0xF      ; Access
        db GRAN_4K | LONG_MODE                      ; Flags
        db 0                                        ; Base (high)
    .Data: equ $ - GDT64
        dd 0xFFFF                                   ; Limit & Base (low)
        db 0                                        ; Base (mid)
        db PRESENT | NOT_SYS | RW | 0xF             ; Access
        db GRAN_4K | LONG_MODE                      ; Flags
        db 0                                        ; Base (high)
    .TSS: equ $ - GDT64
        dd 0x00000068
        dd 0x00CF8900
    .Pointer:
        dw $ - GDT64 - 1
        dq GDT64


[bits 64] 
[extern cpp_kstart]
Realm64:
    mov edi , 0xb8000
    mov rax, 0x1F201F201F201F20 
    mov ecx, 500      
    rep stosq ; Clear the screen.
    call cpp_kstart
    hlt   

print64:
   mov edx , text_addr
    loop64:    
        mov al,[ebx]
        mov ah , byte WHITE_ON_BLACK
        mov [edx],ax
        add edx,2
        add ebx,1
        cmp al , 0
        je end64
        jmp loop64
        end64:
        ret  
section .bss 
stack_bottom:
resb 4096*4
stack_top:




