#include "init.h"
#include "../kernel.h"
#include "../gdt/gdt.h"
#include "../memory/heap/kheap.h"
#include "../memory/memory.h"
#include "../fs/file.h"
#include "../disk/disk.h"
#include "../idt/idt.h"
#include "../task/tss.h"
#include "../task/process.h"
#include "../task/task.h"
#include "../keyboard/keyboard.h"
#include "../mouse/mouse.h"
#include "../memory/paging/paging.h"
#include "../isr80h/isr80h.h"
#include "../panic/panic.h"
#include "../graphics/graphics.h"
#include "../graphics/vix_kernel.h"
#include "../audio/audio.h"
#include "../mouse/ps2_mouse.h"
#include "../io/io.h"
#include "../config.h"
#include "../string/string.h"
#include "../debug/simple_serial.h"

// External declarations
extern struct tss tss;
extern struct gdt gdt_real[VIOS_TOTAL_GDT_SEGMENTS];
extern struct gdt_structured gdt_structured[VIOS_TOTAL_GDT_SEGMENTS];
extern struct paging_4gb_chunk *kernel_chunk;

void kernel_init_gdt_and_tss(void)
{
    simple_serial_puts("Converting GDT structured entries to GDT entries...\n");
    gdt_structured_to_gdt(gdt_real, gdt_structured, VIOS_TOTAL_GDT_SEGMENTS);

    simple_serial_puts("Loading GDT...\n");
    gdt_load(gdt_real, sizeof(gdt_real) - 1);

    simple_serial_puts("Initializing TSS...\n");
    memset(&tss, 0, sizeof(tss));
    
    // Use a safer stack pointer that points to a known good memory area
    // The heap starts at 0x01000000, so we'll use a region just before it
    tss.esp0 = 0x00900000;  // Use 9MB mark as kernel stack
    tss.ss0 = KERNEL_DATA_SELECTOR;
    
    // Set up basic TSS fields to prevent faults
    tss.cs = KERNEL_CODE_SELECTOR;
    tss.ss = KERNEL_DATA_SELECTOR;
    tss.ds = KERNEL_DATA_SELECTOR;
    tss.es = KERNEL_DATA_SELECTOR;
    tss.fs = KERNEL_DATA_SELECTOR;
    tss.gs = KERNEL_DATA_SELECTOR;
    
    simple_serial_puts("Skipping TSS loading for now...\n");
    // tss_load(0x28); // Disabled until we get basic functionality working

    simple_serial_puts("GDT and TSS initialized successfully.\n");
}

void kernel_init_paging(void)
{
    kernel_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    if (!kernel_chunk) {
        panic("Failed to create kernel page directory");
    }
    paging_switch(kernel_chunk);
    enable_paging();
}

void kernel_init_devices(void)
{
    simple_serial_puts("  Initializing heap...\n");
    kheap_init();
    simple_serial_puts("  Heap initialized\n");
    
    simple_serial_puts("  Initializing filesystem...\n");
    fs_init();
    simple_serial_puts("  Filesystem initialized\n");
    
    simple_serial_puts("  Searching for disks...\n");
    // Temporarily disable disk search for debugging
    // disk_search_and_init();
    simple_serial_puts("  Disk search completed (disabled for debugging)\n");
    
    simple_serial_puts("  Initializing IDT...\n");
    idt_init();
    simple_serial_puts("  IDT initialized\n");
    
    simple_serial_puts("  Initializing keyboard...\n");
    keyboard_init();
    simple_serial_puts("  Keyboard initialized\n");
    
    simple_serial_puts("  Initializing mouse...\n");
    mouse_init();
    simple_serial_puts("  Mouse initialized\n");
    
    simple_serial_puts("  Initializing audio...\n");
    audio_init();
    simple_serial_puts("  Audio initialized\n");
    
    simple_serial_puts("  Registering ISR80h commands...\n");
    isr80h_register_commands();
    simple_serial_puts("  ISR80h commands registered\n");
}

struct mouse *kernel_init_graphics(void)
{
    struct mouse *mouse = ps2_mouse_init();
    if (!mouse) {
        panic("Failed to initialize PS/2 mouse");
    }

    VBEInfoBlock *VBE = (VBEInfoBlock *)VBEInfoAddress;
    mouse->x = VBE->x_resolution / 2;
    mouse->y = VBE->y_resolution / 2;

    if (!graphics_initialize()) {
        panic("Failed to initialize graphics system");
    }

    return mouse;
}

void kernel_display_boot_message(void)
{
    vix_kernel_clear_screen(VIX_RGB(11, 25, 69));
    vix_kernel_draw_text("ViOS VIX Graphics System Initialized", 10, 10, VIX_COLOR_WHITE);
    vix_kernel_draw_text("VIX API Ready for User Programs", 10, 30, VIX_COLOR_CYAN);
    vix_kernel_present_frame();
}

void kernel_unmask_timer_irq(void)
{
    uint8_t mask = inb(0x21);
    mask &= ~0x01; // Unmask IRQ0 (timer)
    outb(0x21, mask);
}
