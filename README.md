# DeepikaOS

### A 32-bit Educational Operating System Built from Scratch

DeepikaOS is a simple educational operating system developed from scratch to understand the fundamentals of **operating systems, x86 architecture, boot processes, kernel development, interrupts, keyboard input, and low-level hardware interaction**.

The project combines **x86 Assembly** and **C** to create a small 32-bit kernel that can boot through GRUB, initialize the CPU environment, handle keyboard interrupts, display text using VGA memory, and provide a basic command-line interface.

---

## 🖥️ Project Overview

The main purpose of DeepikaOS is to understand what happens between:

    Computer Startup
          ↓
      Bootloader
          ↓
        Kernel
          ↓
    Hardware Initialization
          ↓
    Interrupt Handling
          ↓
      Keyboard Input
          ↓
    Command-Line Interface

This project focuses on learning the fundamental building blocks of an operating system rather than creating a full-featured commercial OS.

---

## ✨ Features

- **32-bit x86 Kernel**
- **GRUB Bootloader**
- **Multiboot-Compatible Kernel**
- **x86 Protected Mode**
- **Global Descriptor Table (GDT)**
- **Interrupt Descriptor Table (IDT)**
- **Programmable Interrupt Controller (PIC)**
- **Hardware Keyboard Interrupts**
- **Keyboard Input Handling**
- **VGA Text-Mode Output**
- **Kernel Stack**
- **Basic Command-Line Shell**
- **Custom Linker Script**
- **QEMU-Based Testing**

---

## 🏗️ System Architecture

    ┌─────────────────────┐
    │    Computer / QEMU  │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │        GRUB         │
    │      Bootloader     │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │      boot.asm       │
    │  CPU + GDT + Stack  │
    └──────────┬──────────┘
               │
               ▼
    ┌─────────────────────┐
    │      kernel.c       │
    │     Kernel Main     │
    └──────────┬──────────┘
               │
       ┌───────┴────────┐
       │                │
       ▼                ▼
    ┌──────────────┐  ┌──────────────────┐
    │  Interrupt   │  │   VGA Text Mode  │
    │ System IDT   │  │      0xB8000     │
    │    + PIC     │  │                  │
    └──────┬───────┘  └──────────────────┘
           │
           ▼
    ┌──────────────────┐
    │   Keyboard IRQ1  │
    └────────┬─────────┘
             │
             ▼
    ┌──────────────────┐
    │ Keyboard Handler │
    └────────┬─────────┘
             │
             ▼
    ┌──────────────────┐
    │    Shell / CLI   │
    └──────────────────┘

---

## 🔧 Technologies Used

| Technology | Purpose |
|------------|---------|
| **C** | Kernel and shell logic |
| **x86 Assembly** | Boot code and interrupt handling |
| **NASM** | Assembly compilation |
| **GCC** | C compilation |
| **GNU Make** | Build automation |
| **GNU LD** | Kernel linking |
| **GRUB** | Bootloader |
| **QEMU** | OS emulation and testing |
| **VGA Text Mode** | Screen output |
| **WSL2** | Development environment |

---

## 📁 Project Structure

    DeepikaOS/
    ├── boot/
    │   ├── boot.asm
    │   ├── interrupts.asm
    │   └── grub/
    │       └── grub.cfg
    │
    ├── kernel/
    │   └── kernel.c
    │
    ├── iso/
    │   └── boot/
    │       ├── deepikaos.bin
    │       └── grub/
    │           └── grub.cfg
    │
    ├── Makefile
    ├── linker.ld
    ├── deepikaos.bin
    ├── deepikaos.iso
    ├── boot.o
    ├── interrupts.o
    ├── kernel.o
    └── qemu.log

---

## 🚀 Boot Process

When DeepikaOS starts, the execution flow is:

    Power On / QEMU
          ↓
    BIOS / Virtual Machine
          ↓
         GRUB
          ↓
    Multiboot Kernel
          ↓
       boot.asm
          ↓
       Load GDT
          ↓
    Set Data Segments
          ↓
    Initialize Kernel Stack
          ↓
     kernel_main()
          ↓
    Initialize Interrupt System
          ↓
    Initialize Keyboard
          ↓
    Enable Interrupts
          ↓
      Start Shell

---

## 🧠 Multiboot

DeepikaOS contains a Multiboot header so that GRUB can identify and load the kernel.

The Multiboot magic value used by the project is:

    0x1BADB002

The kernel can be checked using:

    grub-file --is-x86-multiboot deepikaos.bin

A successful verification returns:

    0

---

## ⚙️ Global Descriptor Table (GDT)

The **Global Descriptor Table (GDT)** is used by the x86 processor to define memory segments in protected mode.

DeepikaOS contains:

    GDT
    ├── Null Descriptor
    ├── Code Segment
    └── Data Segment

The project uses:

    Code Segment → 0x08
    Data Segment → 0x10

The GDT is loaded during the early boot process.

---

## ⚡ Interrupt Handling

Interrupts allow hardware devices to notify the CPU when an event occurs.

For example, when a key is pressed:

    Keyboard
        ↓
       IRQ1
        ↓
       PIC
        ↓
    Interrupt 33
        ↓
      IDT[33]
        ↓
    keyboard_interrupt()
        ↓
    keyboard_handler()
        ↓
      Process Key
        ↓
    Display Character

---

## ⌨️ Keyboard Interrupt

The keyboard traditionally uses:

    IRQ1

The PIC is remapped so that:

    IRQ0 → Interrupt 32
    IRQ1 → Interrupt 33

Therefore, the keyboard is handled through:

    Interrupt 33

The CPU uses entry `33` in the Interrupt Descriptor Table to locate the keyboard handler.

### Keyboard Interrupt Handler

The low-level interrupt entry point is written in Assembly:

    keyboard_interrupt:
        pusha
        call keyboard_handler
        popa
        iretd

### Explanation

**`pusha`**

Saves the CPU's general-purpose registers.

**`call keyboard_handler`**

Calls the C function responsible for processing keyboard input.

**`popa`**

Restores the previously saved registers.

**`iretd`**

Returns from the interrupt and resumes the previous CPU execution.

---

## 🖥️ VGA Text Mode

DeepikaOS uses VGA text-mode memory at:

    0xB8000

The kernel writes characters directly to this memory to display text.

A VGA text-mode character generally contains:

    Character + Color Attribute

This allows the kernel to display text without relying on a normal operating-system graphics library.

---

## 💻 Command-Line Interface

DeepikaOS provides a basic command-line shell.

Available commands include:

    help
    about
    version
    echo
    clear
    reboot
    shutdown

Example:

    deepika> help

The `help` command displays the available commands.

You can also use:

    deepika> about

to display information about DeepikaOS.

---

## 🔨 Build Process

The operating system is built in several stages:

    boot.asm
        ↓
      boot.o

    interrupts.asm
        ↓
    interrupts.o

    kernel.c
        ↓
      kernel.o

    boot.o + interrupts.o + kernel.o
                    ↓
                  Linker
                    ↓
              deepikaos.bin
                    ↓
                 GRUB ISO
                    ↓
              deepikaos.iso
                    ↓
                  QEMU
                    ↓
               DeepikaOS

---

## 💻 Requirements

The project was developed and tested using:

- **Windows 11**
- **WSL2**
- **Ubuntu**
- **NASM**
- **GCC**
- **GNU Make**
- **GNU Binutils**
- **GRUB Tools**
- **xorriso**
- **QEMU**

---

## ▶️ How to Build and Run

### 1. Clone the Repository

    git clone https://github.com/Deepikajanarthanan/DeepikaOS.git

Enter the project:

    cd DeepikaOS

### 2. Build the Kernel

    make

This assembles the Assembly files, compiles the C kernel, and links them into:

    deepikaos.bin

### 3. Copy the Kernel to the ISO Directory

    cp deepikaos.bin iso/boot/deepikaos.bin

### 4. Create the Bootable ISO

    grub-mkrescue -o deepikaos.iso iso

### 5. Run DeepikaOS

    qemu-system-i386 -cdrom deepikaos.iso -boot d

GRUB will start and load DeepikaOS.

---

## 🔍 Verification

Verify the kernel as a Multiboot kernel:

    grub-file --is-x86-multiboot deepikaos.bin

Check the kernel file:

    file deepikaos.bin

Expected type:

    ELF 32-bit LSB executable, Intel i386

---

## 🧹 Clean Build

To remove generated compilation files:

    make clean

Then rebuild:

    make

---

## 📊 Project Status

| Component | Status |
|-----------|--------|
| Bootloader | ✅ Completed |
| Multiboot Kernel | ✅ Completed |
| 32-bit Protected Mode | ✅ Completed |
| GDT | ✅ Completed |
| Kernel Stack | ✅ Completed |
| VGA Text Output | ✅ Completed |
| IDT | ✅ Completed |
| PIC | ✅ Completed |
| Keyboard Interrupt | ✅ Completed |
| Keyboard Handler | ✅ Completed |
| Command Shell | ✅ Completed |
| QEMU Testing | ✅ Completed |

---

## 🎯 Learning Objectives

This project was created to understand:

- How a computer starts an operating system
- How a bootloader loads a kernel
- How Assembly and C work together
- How x86 protected mode works
- How the GDT is used
- How the kernel establishes its stack
- How interrupts work
- How hardware communicates with the CPU
- How keyboard input reaches the kernel
- How VGA memory is used for text output
- How a basic command-line shell works
- How source code becomes a bootable operating system

---

---

## 🔮 Future Improvements

Possible future improvements include:

- Memory management
- Heap allocation
- Timer interrupts
- Multitasking
- Process management
- File system support
- Disk drivers
- Mouse input
- Improved shell
- User/kernel privilege separation
- Virtual memory
- System calls
- Basic application support

---

## 👩‍💻 Author

### Deepika Janarthanan

---



