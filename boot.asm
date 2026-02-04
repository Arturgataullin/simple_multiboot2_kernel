; определяем multiboot заголовок
; подготавлиеваем окружение перед передачей управления C коду

extern kernel_main

section .multiboot_header
header_start:
    dd 0xE85250D6       ; multiboot2 header magic
    dd 0        ; архитектура (0 = i386 формат заголовка)
    dd header_end - header_start        ; длина заголовка в байтах
    dd -(0xE85250D6 + 0 + (header_end - header_start))      ; контрольная сумма (в сумме должно дать 0 mod 2^32)

    ; Request: Framebuffer mode
    ; Tag type = 5 (Framebuffer)
    align 8
    dw 5        ; type
    dw 1        ; flags: 1 = обязательный
    dd 20       ; size (байт)
    dd 1024     ; width
    dd 768      ; height
    dd 32       ; bpp

    ; тег типа 11/12 с адресом EFI System Table GRUB добавляет сам при загрузке под UEFI

    align 8
    dw 7
    dw 1                                                
    dd 8

    ; EFI amd64 entry address
    ; Tag type = 9
    align 8
    dw 9
    dw 1
    dd 12
    dd _start       ; entry address (низкие 32 бита адреса)

    ; End tag
    align 8
    dw 0
    dw 0
    dd 8
header_end:

section .text
global _start

_start:
    mov rsp, stack_top      ; настраиваем стек

    ; по соглашению Multiboot2 при входе
    ; rax = magic (0x36D76289) для подтверждения что загрузчик multiboot2 совместим
    ; rbx = address of multiboot info structure (эту инфу о состоянии системе ядру передает GRUB)
    
    mov rdi, rax
    mov rsi, rbx

    call kernel_main

.hang:
    hlt     ; останавливаем проц, без этого после выхода из kernel_main он будет выполнять мусор (случайное поведение)
    jmp .hang


section .bss
align 16
stack_bottom:
    resb 16384
stack_top:
