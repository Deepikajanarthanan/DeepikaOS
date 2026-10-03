bits 32

global keyboard_interrupt
global load_idt

extern keyboard_handler

keyboard_interrupt:
    pusha
    call keyboard_handler
    popa
    iretd

load_idt:
    mov eax, [esp + 4]
    lidt [eax]
    ret