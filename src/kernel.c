#include "kernel.h"
#include "kernel/init.h"
#include "kernel/mainloop.h"
#include "gdt/gdt.h"
#include "task/tss.h"
#include "memory/paging/paging.h"
#include "memory/memory.h"
#include "config.h"
#include "audio/audio.h"
#include "debug/simple_serial.h"
#include "task/process.h"
#include "graphics/vix_kernel.h"
#include "terminal/terminal.h"

struct paging_4gb_chunk *kernel_chunk = 0;

void kernel_page()
{
    kernel_registers();

    if (kernel_chunk)
        paging_switch(kernel_chunk);
}

// GDT and TSS
struct tss tss;
struct gdt gdt_real[VIOS_TOTAL_GDT_SEGMENTS];
struct gdt_structured gdt_structured[VIOS_TOTAL_GDT_SEGMENTS] = {
    {.base = 0x00, .limit = 0x00, .type = 0x00},
    {.base = 0x00, .limit = 0xffffffff, .type = 0x9a},
    {.base = 0x00, .limit = 0xffffffff, .type = 0x92},
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf8},
    {.base = 0x00, .limit = 0xffffffff, .type = 0xf2},
    {.base = (uint32_t)(uintptr_t)&tss, .limit = sizeof(tss) - 1, .type = 0x89}};








void kernel_main()
{
    simple_serial_init();
    simple_serial_puts("Serial Debug Initialized\n");

    simple_serial_puts("Initializing GDT and TSS...\n");
    kernel_init_gdt_and_tss();
    simple_serial_puts("GDT and TSS initialized\n");

    simple_serial_puts("Initializing devices...\n");
    kernel_init_devices();
    simple_serial_puts("Devices initialized\n");

    simple_serial_puts("Initializing paging...\n");
    kernel_init_paging();
    simple_serial_puts("Paging enabled\n");

    simple_serial_puts("Initializing graphics system...\n");
    kernel_init_graphics();
    simple_serial_puts("Graphics system initialized\n");

    // Display boot progress messages on screen after graphics are ready
    vix_kernel_clear_screen(VIX_RGB(0, 0, 0)); // Black background
    vix_kernel_draw_text("ViOS Kernel Boot Sequence", 10, 10, VIX_COLOR_WHITE);
    vix_kernel_draw_text("[OK] Serial Debug Initialized", 10, 30, VIX_COLOR_GREEN);
    vix_kernel_draw_text("[OK] GDT and TSS Initialized", 10, 50, VIX_COLOR_GREEN);
    vix_kernel_draw_text("[OK] Devices Initialized", 10, 70, VIX_COLOR_GREEN);
    vix_kernel_draw_text("[OK] Paging Enabled", 10, 90, VIX_COLOR_GREEN);
    vix_kernel_draw_text("[OK] Graphics System Ready", 10, 110, VIX_COLOR_GREEN);
    vix_kernel_present_frame();
    
    kernel_display_boot_message();
    simple_serial_puts("Boot message drawn\n");

    virtual_audio_control(VIRTUAL_AUDIO_BEEP);
    simple_serial_puts("Audio beep triggered\n");

    simple_serial_puts("Skipping timer IRQ unmasking for now...\n");
    // kernel_unmask_timer_irq(); // Disabled as it causes system restart
    simple_serial_puts("Timer IRQ unmasking skipped\n");

    simple_serial_puts("Skipping terminal load for VIX frontend test...\n");
    
// Keep the boot message visible for a moment
    simple_serial_puts("Boot sequence complete - displaying for 3 seconds\n");
    
    // Wait 3 seconds to see the boot message
    for (volatile int delay = 0; delay < 30000000; delay++);
    
    simple_serial_puts("Starting kernel main loop (VIX demo)...\n");
    kernel_run_main_loop(0); // This should run the VIX graphics demo, no mouse for now
}
