#include "isr80h.h"
#include "io.h"
#include "idt/idt.h"
#include "heap.h"
#include "kernel.h"
#include "process.h"
#include "file.h"
#include "vix_graphics.h"
#include "sound.h"
#include "../debug/simple_serial.h"

// Include keyboard system call handlers
void *isr80h_command24_keyboard_read(struct interrupt_frame *frame);
void *isr80h_command25_keyboard_state(struct interrupt_frame *frame);

void isr80h_register_commands()
{
    simple_serial_puts("Registering command 0\n");
    isr80h_register_command(SYSTEM_COMMAND0_EXIT, isr80h_command0_exit);
    simple_serial_puts("Command 0 registered\n");

    simple_serial_puts("Registering command 1\n");
    isr80h_register_command(SYSTEM_COMMAND1_PRINT, isr80h_command1_print);
    simple_serial_puts("Command 1 registered\n");

    simple_serial_puts("Registering command 2\n");
    isr80h_register_command(SYSTEM_COMMAND2_GETKEY, isr80h_command2_getkey);
    simple_serial_puts("Command 2 registered\n");

    simple_serial_puts("Registering command 3\n");
    isr80h_register_command(SYSTEM_COMMAND3_PUTCHAR, isr80h_command3_putchar);
    simple_serial_puts("Command 3 registered\n");

    simple_serial_puts("Registering command 4\n");
    isr80h_register_command(SYSTEM_COMMAND4_MALLOC, isr80h_command4_malloc);
    simple_serial_puts("Command 4 registered\n");

    simple_serial_puts("Registering command 5\n");
    isr80h_register_command(SYSTEM_COMMAND5_FREE, isr80h_command5_free);
    simple_serial_puts("Command 5 registered\n");

    simple_serial_puts("Registering command 6\n");
    isr80h_register_command(SYSTEM_COMMAND6_PROCESS_LOAD_START, isr80h_command6_process_load_start);
    simple_serial_puts("Command 6 registered\n");

    simple_serial_puts("Registering command 7\n");
    isr80h_register_command(SYSTEM_COMMAND7_INVOKE_SYSTEM_COMMAND, isr80h_command7_invoke_system_command);
    simple_serial_puts("Command 7 registered\n");

    simple_serial_puts("Registering command 8\n");
    isr80h_register_command(SYSTEM_COMMAND8_GET_PROGRAM_ARGUMENTS, isr80h_command8_get_program_arguments);
    simple_serial_puts("Command 8 registered\n");

    simple_serial_puts("Registering command 9\n");
    isr80h_register_command(SYSTEM_COMMAND9_SLEEP, isr80h_command9_sleep);
    simple_serial_puts("Command 9 registered\n");

    simple_serial_puts("Registering command 10\n");
    isr80h_register_command(SYSTEM_COMMAND10_READ, isr80h_command10_read);
    simple_serial_puts("Command 10 registered\n");

    simple_serial_puts("Registering VIX graphics commands\n");
    
    isr80h_register_command(SYSTEM_COMMAND11_VIX_DRAW_PIXEL, isr80h_command11_vix_draw_pixel);
    isr80h_register_command(SYSTEM_COMMAND12_VIX_DRAW_RECT, isr80h_command12_vix_draw_rect);
    isr80h_register_command(SYSTEM_COMMAND13_VIX_FILL_RECT, isr80h_command13_vix_fill_rect);
    isr80h_register_command(SYSTEM_COMMAND14_VIX_CLEAR_SCREEN, isr80h_command14_vix_clear_screen);
    isr80h_register_command(SYSTEM_COMMAND15_VIX_PRESENT_FRAME, isr80h_command15_vix_present_frame);
    isr80h_register_command(SYSTEM_COMMAND16_VIX_GET_SCREEN_INFO, isr80h_command16_vix_get_screen_info);
    isr80h_register_command(SYSTEM_COMMAND17_VIX_DRAW_LINE, isr80h_command17_vix_draw_line);
    isr80h_register_command(SYSTEM_COMMAND18_VIX_DRAW_CIRCLE, isr80h_command18_vix_draw_circle);
    isr80h_register_command(SYSTEM_COMMAND19_VIX_FILL_CIRCLE, isr80h_command19_vix_fill_circle);
    isr80h_register_command(SYSTEM_COMMAND20_VIX_DRAW_TEXT, isr80h_command20_vix_draw_text);
    isr80h_register_command(SYSTEM_COMMAND21_VIX_DRAW_TEXT_SCALED, isr80h_command21_vix_draw_text_scaled);
    isr80h_register_command(SYSTEM_COMMAND22_VIX_TEXT_WIDTH, isr80h_command22_vix_text_width);
    isr80h_register_command(SYSTEM_COMMAND23_VIX_TEXT_HEIGHT, isr80h_command23_vix_text_height);
    
    simple_serial_puts("All VIX commands registered\n");
    
    // Temporarily disabled keyboard and sound system calls
    // isr80h_register_command(SYSTEM_COMMAND24_KEYBOARD_READ, isr80h_command24_keyboard_read);
    // isr80h_register_command(SYSTEM_COMMAND25_KEYBOARD_STATE, isr80h_command25_keyboard_state);
    // isr80h_register_command(SYSTEM_COMMAND26_SOUND_PLAY, isr80h_command26_sound_play);
    // isr80h_register_command(SYSTEM_COMMAND27_SOUND_STOP, isr80h_command27_sound_stop);
}
