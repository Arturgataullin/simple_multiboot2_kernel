// цель - получить доступ к EFI system table через multiboot2, вывести ее структуру через последовательный порт

#include <stdint.h>
#include <stddef.h>

#define MULTIBOOT_MAGIC 0x36D76289
#define MULTIBOOT_TAG_TYPE_FRAMEBUFFER 8

typedef struct {
    uint32_t type;
    uint32_t size;
} MULTIBOOT_TAG;

// struct multiboot_tag_efi64 {
//     uint32_t type;
//     uint32_t size;
//     uint64_t efi_system_table;
// };


// typedef uint64_t EFI_HANDLE;
// typedef uint64_t UINTN;
// typedef uint32_t UINT32;
// typedef uint16_t CHAR16;

// typedef struct {
//     uint64_t Signature;
//     uint32_t Revision;
//     uint32_t HeaderSize;
//     uint32_t CRC32;
//     uint32_t Reserved;
// } EFI_TABLE_HEADER;

// typedef struct {
//     EFI_TABLE_HEADER Hdr;
//     CHAR16 *FirmwareVendor;
//     UINT32 FirmwareRevision;
//     EFI_HANDLE ConsoleInHandle;
//     void *ConIn;
//     EFI_HANDLE ConsoleOutHandle;
//     void *ConOut;
//     EFI_HANDLE StandardErrorHandle;
//     void *StdErr;
//     void *RuntimeServices;
//     void *BootServices;
//     UINTN NumberOfTableEntries;
//     void *ConfigurationTable;
// } EFI_SYSTEM_TABLE;

typedef struct {
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint8_t reserved;
} MULTIBOOT_TAG_FRAMEBUFFER;


// заполенние буфера
void memset32(uint32_t* fdest, uint32_t color, size_t count) {
    for (size_t i = 0; i < count; i++) {
        fdest[i] = color;
    }
}

void draw_rect(uint32_t* fdest, uint32_t fwidth, uint32_t fheight, uint32_t x, uint32_t y, uint32_t width, uint32_t height, size_t color) {
    if ((x + width) > fwidth || (y + height) > fheight) return;
    for (uint32_t cy = y; cy < y + height; cy++) {
        memset32(fwidth * (y - 1) + x, color, width);
    }
}

// #define COM1 0x3F8

// // запись в порт
// static inline void outb(uint16_t port, uint8_t val) {
//     __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
// }

// // чтение из порта
// static inline uint8_t inb(uint16_t port) {
//     uint8_t ret;
//     __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
//     return ret;
// }

// static void serial_init(void) {
//     outb(COM1 + 1, 0x00);
//     outb(COM1 + 3, 0x80);
//     outb(COM1 + 0, 0x03);
//     outb(COM1 + 1, 0x00);
//     outb(COM1 + 3, 0x03);
//     outb(COM1 + 2, 0xC7);
//     outb(COM1 + 4, 0x0B);
// }

// static int serial_ready(void) {
//     return (inb(COM1 + 5) & 0x20) != 0;
// }

// static void serial_putc(char c) {
//     while (!serial_ready()) { }
//     outb(COM1, (uint8_t)c);
// }

// static void serial_print(const char *s) {
//     while (*s) {
//         char c = *s++;
//         if (c == '\n') serial_putc('\r');
//         serial_putc(c);
//     }
// }

// static void serial_print_hex(uint64_t v) {
//     static const char hex[] = "0123456789ABCDEF";
//     serial_print("0x");
//     for (int i = 60; i >= 0; i -= 4)
//         serial_putc(hex[(v >> i) & 0xF]);
// }


void kernel_main(uint32_t magic, uintptr_t mb_info_addr) {

    if (magic != MULTIBOOT_MAGIC)
        return;

    serial_init();
    serial_print("Kernel started\n");

    MULTIBOOT_TAG *tag = (MULTIBOOT_TAG*)(mb_info_addr + 8);
    MULTIBOOT_TAG_FRAMEBUFFER *fb = NULL;

    while (tag->type != 0) {
        if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
            fb = (MULTIBOOT_TAG_FRAMEBUFFER*)tag;
            break;
        }
        tag = (MULTIBOOT_TAG*)((uint8_t*)tag + ((tag->size + 7) & ~7));
    }

    if (!fb) {
        serial_print("Frambuffer not found\n");
        while (1);
    }

    uint32_t *fdest = (uint32_t*)fb->framebuffer_addr;
    uint32_t width = fb->framebuffer_width;
    uint32_t height = fb->framebuffer_height;

    draw_rect(fdest, width, height, width/2, height/2, 200, 200, 0x00FF00);

    // serial_print("Hdr            = "); serial_print_hex((uint64_t)(uintptr_t)&st->Hdr); serial_print("\n");
    // serial_print("FirmwareVendor = "); serial_print_hex((uint64_t)(uintptr_t)st->FirmwareVendor); serial_print("\n");
    // serial_print("FirmwareRev    = "); serial_print_hex((uint64_t)st->FirmwareRevision); serial_print("\n");
    // serial_print("ConsoleIn      = "); serial_print_hex((uint64_t)st->ConsoleInHandle); serial_print("\n");
    // serial_print("ConIn          = "); serial_print_hex((uint64_t)(uintptr_t)st->ConIn); serial_print("\n");
    // serial_print("ConsoleOut     = "); serial_print_hex((uint64_t)st->ConsoleOutHandle); serial_print("\n");
    // serial_print("ConOut         = "); serial_print_hex((uint64_t)(uintptr_t)st->ConOut); serial_print("\n");
    // serial_print("StdErrHandle   = "); serial_print_hex((uint64_t)st->StandardErrorHandle); serial_print("\n");
    // serial_print("StdErr         = "); serial_print_hex((uint64_t)(uintptr_t)st->StdErr); serial_print("\n");
    // serial_print("RuntimeSrv     = "); serial_print_hex((uint64_t)(uintptr_t)st->RuntimeServices); serial_print("\n");
    // serial_print("BootSrv        = "); serial_print_hex((uint64_t)(uintptr_t)st->BootServices); serial_print("\n");
    // serial_print("ConfigTables   = "); serial_print_hex((uint64_t)st->NumberOfTableEntries); serial_print("\n");
    // serial_print("ConfigTablePtr = "); serial_print_hex((uint64_t)(uintptr_t)st->ConfigurationTable); serial_print("\n");

    while (1);
}

