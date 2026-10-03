bits 32

section .multiboot
align 4

    dd 0x1BADB002
    dd 0
    dd -(0x1BADB002 + 0)

section .bss
align 16

stack_bottom:
    resb 16384

stack_top:


section .text

global start
extern kernel_main


start:
    cli

    mov byte [0xB8000], 'X'
    mov byte [0xB8001], 0x07

    ; ------------------------------------------
    ; Load our own Global Descriptor Table
    ; ------------------------------------------

    lgdt [gdt_descriptor]

    ; Reload code segment

    jmp 0x08:protected_mode


protected_mode:

    ; ------------------------------------------
    ; Load data segments
    ; ------------------------------------------

    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax


    ; ------------------------------------------
    ; Set kernel stack
    ; ------------------------------------------

    mov esp, stack_top


    ; ------------------------------------------
    ; Start C kernel
    ; ------------------------------------------

push ebx
push eax

call kernel_main

add esp, 8


hang:
    hlt
    jmp hang


; ==========================================
; Global Descriptor Table
; ==========================================

gdt_start:


gdt_null:
    dq 0x0000000000000000


gdt_code:
    dq 0x00CF9A000000FFFF


gdt_data:
    dq 0x00CF92000000FFFF


gdt_end:


gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start