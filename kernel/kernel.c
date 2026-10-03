/* ==========================================
   DeepikaOS Kernel
   ========================================== */


/* ==========================================
   IDT Structures
   ========================================== */

struct idt_entry
{
    unsigned short base_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char flags;
    unsigned short base_high;
} __attribute__((packed));


struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));


/* ==========================================
   IDT
   ========================================== */

struct idt_entry idt[256];
struct idt_ptr idtp;


void idt_set_gate(
    unsigned char num,
    unsigned long base,
    unsigned short selector,
    unsigned char flags
)
{
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;

    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].flags = flags;
}


extern void load_idt(unsigned int);


void idt_init()
{
    idtp.limit = sizeof(idt) - 1;
    idtp.base = (unsigned int)&idt;

    for (int i = 0; i < 256; i++)
    {
        idt_set_gate(i, 0, 0, 0);
    }

    extern void keyboard_interrupt();

    /* Keyboard IRQ1 -> Interrupt 33 */

    idt_set_gate(
        33,
        (unsigned int)keyboard_interrupt,
        0x08,
        0x8E
    );

    load_idt((unsigned int)&idtp);
}


/* ==========================================
   Keyboard and VGA
   ========================================== */

#define VIDEO_MEMORY 0xB8000
#define KEYBOARD_DATA 0x60

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25


static const char keyboard_map[] =
{
    0, 0,

    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',

    '-', '=',
    '\b',
    '\t',

    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',

    '[', ']',
    '\n',
    0,

    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',

    ';', '\'',
    '`',
    0,
    '\\',

    'z', 'x', 'c', 'v', 'b', 'n', 'm',

    ',', '.', '/',
    0,
    '*',
    0,
    ' '
};


char *video_memory = (char *)VIDEO_MEMORY;


/* ==========================================
   Cursor and Command
   ========================================== */

int cursor_position = 0;
int command_start = 0;

char command[80];


/* ==========================================
   Print String
   ========================================== */

void print_string(const char *text)
{
    int i = 0;

    while (text[i] != '\0')
    {
        video_memory[cursor_position * 2] = text[i];
        video_memory[cursor_position * 2 + 1] = 0x07;

        cursor_position++;
        i++;
    }
}


/* ==========================================
   Clear Screen
   ========================================== */

void clear_screen()
{
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = 0x07;
    }

    cursor_position = 0;
}


/* ==========================================
   Compare Strings
   ========================================== */

int string_equal(const char *a, const char *b)
{
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i])
        {
            return 0;
        }

        i++;
    }

    return a[i] == '\0' && b[i] == '\0';
}


/* ==========================================
   New Line
   ========================================== */

void new_line()
{
    int current_row = cursor_position / SCREEN_WIDTH;

    current_row++;

    cursor_position = current_row * SCREEN_WIDTH;
}


/* ==========================================
   Show Shell Prompt
   ========================================== */

void show_prompt()
{
    print_string("deepika> ");

    command_start = cursor_position;
}


/* ==========================================
   Reboot
   ========================================== */

void reboot()
{
    unsigned char good;

    do
    {
        __asm__ volatile (
            "inb %1, %0"
            : "=a"(good)
            : "Nd"((unsigned short)0x64)
        );
    }
    while (good & 0x02);

    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"((unsigned char)0xFE),
          "Nd"((unsigned short)0x64)
    );
}


/* ==========================================
   Shutdown
   ========================================== */

void shutdown()
{
    __asm__ volatile (
        "outw %0, %1"
        :
        : "a"((unsigned short)0x2000),
          "Nd"((unsigned short)0x604)
    );

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}


/* ==========================================
   Execute Command
   ========================================== */

void execute_command()
{
    if (string_equal(command, "help"))
    {
        print_string("Available commands:");
        new_line();

        print_string("help  - Show commands");
        new_line();

        print_string("about - About DeepikaOS");
        new_line();

        print_string("clear - Clear screen");
        new_line();

        print_string("echo  - Display text");
        new_line();
    }

    else if (string_equal(command, "about"))
    {
        print_string("DeepikaOS");
        new_line();

        print_string("My First Operating System");
        new_line();

        print_string("Created by Deepika");
        new_line();
    }

    else if (string_equal(command, "version"))
    {
        print_string("DeepikaOS version 1.0");
        new_line();

        print_string("32-bit Kernel");
        new_line();
    }

    else if (string_equal(command, "reboot"))
    {
        print_string("Restarting DeepikaOS...");
        new_line();

        reboot();
    }

    else if (string_equal(command, "shutdown"))
    {
        print_string("Shutting down DeepikaOS...");
        new_line();

        shutdown();
    }

    else if (string_equal(command, "clear"))
    {
        clear_screen();
    }

    else if (command[0] == 'e' &&
             command[1] == 'c' &&
             command[2] == 'h' &&
             command[3] == 'o' &&
             command[4] == ' ')
    {
        print_string(command + 5);

        new_line();
    }

    else if (command[0] != '\0')
    {
        print_string("Unknown command");
        new_line();

        print_string("Type 'help' for available commands.");
        new_line();
    }

    show_prompt();
}


/* ==========================================
   Keyboard Interrupt Handler
   ========================================== */

void keyboard_handler()
{
    unsigned char scan_code;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(scan_code)
        : "Nd"((unsigned short)KEYBOARD_DATA)
    );


    /* Ignore key-release codes */

    if (!(scan_code & 0x80))
    {
        if (scan_code < sizeof(keyboard_map))
        {
            char key = keyboard_map[scan_code];


            /* ------------------------------
               Backspace
               ------------------------------ */

            if (key == '\b')
            {
                if (cursor_position > command_start)
                {
                    cursor_position--;

                    video_memory[cursor_position * 2] = ' ';
                    video_memory[cursor_position * 2 + 1] = 0x07;
                }
            }


            /* ------------------------------
               Enter
               ------------------------------ */

            else if (key == '\n')
            {
                int command_length =
                    cursor_position - command_start;

                command[command_length] = '\0';


                /* Move to next line */

                new_line();


                /* Execute command */

                execute_command();
            }


            /* ------------------------------
               Normal key
               ------------------------------ */

            else if (key != 0)
            {
                if (cursor_position - command_start < 79)
                {
                    command[cursor_position - command_start] = key;

                    video_memory[cursor_position * 2] = key;
                    video_memory[cursor_position * 2 + 1] = 0x07;

                    cursor_position++;
                }
            }
        }
    }


    /* ======================================
       Send End Of Interrupt
       ====================================== */

    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"((unsigned char)0x20),
          "Nd"((unsigned short)0x20)
    );
}


/* ==========================================
   PIC Remapping
   ========================================== */

void pic_remap()
{
    unsigned char a1, a2;
    unsigned char wait;

    __asm__ volatile (
        "outb %%al, $0x80"
        :
        : "a"((unsigned char)0)
    );

    /* Save current masks */

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(a1)
        : "Nd"((unsigned short)0x21)
    );

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(a2)
        : "Nd"((unsigned short)0xA1)
    );


    /* Start PIC initialization */

    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"((unsigned char)0x11),
          "Nd"((unsigned short)0x20)
    );

    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"((unsigned char)0x11),
          "Nd"((unsigned short)0xA0)
    );


    /* Master PIC vector offset = 32 */

    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"((unsigned char)0x20),
          "Nd"((unsigned short)0x21)
    );


    /* Slave PIC vector offset = 40 */

    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"((unsigned char)0x28),
          "Nd"((unsigned short)0xA1)
    );


    /* Tell master about slave at IRQ2 */

    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"((unsigned char)0x04),
          "Nd"((unsigned short)0x21)
    );


    /* Tell slave its cascade identity */

    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"((unsigned char)0x02),
          "Nd"((unsigned short)0xA1)
    );


    /* 8086 mode */

    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"((unsigned char)0x01),
          "Nd"((unsigned short)0x21)
    );

    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"((unsigned char)0x01),
          "Nd"((unsigned short)0xA1)
    );


    /* ======================================
       Interrupt Masks

       0xFD = 11111101
                  ^
                  IRQ1 keyboard enabled

       0xFF = all slave interrupts disabled
       ====================================== */

    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"((unsigned char)0xFD),
          "Nd"((unsigned short)0x21)
    );

    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"((unsigned char)0xFF),
          "Nd"((unsigned short)0xA1)
    );
}


/* ==========================================
   Kernel Main
   ========================================== */

void kernel_main(unsigned int multiboot_magic, unsigned int multiboot_info)
{
   
    /* Configure PIC */

    pic_remap();


    /* Configure IDT */

    idt_init();


    /* Clear screen */

    clear_screen();


    /* Welcome screen */

    print_string("================================");
    new_line();

    print_string("       Welcome to DeepikaOS!");
    new_line();

    print_string("================================");
    new_line();

    new_line();

    print_string("Keyboard Interrupt Test");
    new_line();

    print_string("Type something:");
    new_line();

    new_line();


    /* Show shell prompt */

    show_prompt();


    /* Enable CPU interrupts */

    __asm__ volatile ("sti");


    /* Wait for interrupts */

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}