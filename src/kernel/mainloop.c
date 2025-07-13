#include "mainloop.h"
#include "../graphics/graphics.h"
#include "../graphics/renderer.h"
#include "../graphics/vix_kernel.h"
#include "../mouse/mouse.h"
#include "../task/process.h"
#include "../keyboard/keyboard.h"
#include "../string/string.h"
#include "../debug/simple_serial.h"
#include "../terminal/terminal.h"
#include "../io/io.h"

// Simple kernel terminal state
static char terminal_buffer[80 * 25]; // 80 columns, 25 rows
static char input_buffer[256];
static int input_pos = 0;
static int cursor_x = 0;
static int cursor_y = 0;
static int terminal_bg_color = 0x000000; // Black background

void kernel_terminal_clear()
{
    for (int i = 0; i < 80 * 25; i++) {
        terminal_buffer[i] = ' ';
    }
    cursor_x = 0;
    cursor_y = 0;
}

void kernel_terminal_putchar(char c)
{
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y >= 25) {
            // Scroll up
            for (int i = 0; i < 80 * 24; i++) {
                terminal_buffer[i] = terminal_buffer[i + 80];
            }
            for (int i = 80 * 24; i < 80 * 25; i++) {
                terminal_buffer[i] = ' ';
            }
            cursor_y = 24;
        }
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            terminal_buffer[cursor_y * 80 + cursor_x] = ' ';
        }
    } else {
        terminal_buffer[cursor_y * 80 + cursor_x] = c;
        cursor_x++;
        if (cursor_x >= 80) {
            cursor_x = 0;
            cursor_y++;
            if (cursor_y >= 25) {
                cursor_y = 24;
                // Scroll logic here if needed
            }
        }
    }
}

void kernel_terminal_print(const char* str)
{
    while (*str) {
        kernel_terminal_putchar(*str);
        str++;
    }
}

void kernel_terminal_render()
{
    // Clear screen
    vix_kernel_clear_screen(terminal_bg_color);
    
    // Draw terminal content
    for (int y = 0; y < 25; y++) {
        for (int x = 0; x < 80; x++) {
            char c = terminal_buffer[y * 80 + x];
            if (c != ' ') {
                char str[2] = {c, '\0'};
                vix_kernel_draw_text(str, x * 8, y * 16, VIX_COLOR_WHITE);
            }
        }
    }
    
    // Draw cursor
    vix_kernel_fill_rect(cursor_x * 8, cursor_y * 16, 8, 16, VIX_RGB(128, 128, 128));
    
    vix_kernel_present_frame();
}

void kernel_terminal_execute_command(const char* cmd)
{
    if (strncmp(cmd, "help", 4) == 0) {
        kernel_terminal_print("Available commands:\n");
        kernel_terminal_print("  help - Show this help\n");
        kernel_terminal_print("  clear - Clear screen\n");
        kernel_terminal_print("  echo <text> - Echo text\n");
        kernel_terminal_print("  bgcolor <color> - Change background (red/green/blue/black)\n");
    } else if (strncmp(cmd, "clear", 5) == 0) {
        kernel_terminal_clear();
    } else if (strncmp(cmd, "echo ", 5) == 0) {
        kernel_terminal_print(cmd + 5);
        kernel_terminal_print("\n");
    } else if (strncmp(cmd, "bgcolor red", 11) == 0) {
        terminal_bg_color = VIX_RGB(64, 0, 0);
    } else if (strncmp(cmd, "bgcolor green", 13) == 0) {
        terminal_bg_color = VIX_RGB(0, 64, 0);
    } else if (strncmp(cmd, "bgcolor blue", 12) == 0) {
        terminal_bg_color = VIX_RGB(0, 0, 64);
    } else if (strncmp(cmd, "bgcolor black", 13) == 0) {
        terminal_bg_color = 0x000000;
    } else if (strlen(cmd) > 0) {
        kernel_terminal_print("Unknown command: ");
        kernel_terminal_print(cmd);
        kernel_terminal_print("\n");
    }
}

void kernel_run_main_loop(struct mouse *mouse)
{
    simple_serial_puts("Starting kernel terminal...\n");
    
    // Initialize terminal
    kernel_terminal_clear();
    kernel_terminal_print("ViOS Kernel Terminal\n");
    kernel_terminal_print("Type 'help' for available commands.\n");
    kernel_terminal_print("\n# ");
    
    input_pos = 0;
    
    while (1) {
        kernel_terminal_render();
        
        // Simple keyboard polling (direct port access)
        unsigned char scancode = 0;
        
        // Check if keyboard data is available
        if ((inb(0x64) & 0x01)) {
            scancode = inb(0x60);
            
            // Simple scancode to ASCII conversion (basic keys only)
            char ascii = 0;
            
            // Handle key releases (ignore)
            if (scancode & 0x80) {
                continue;
            }
            
            // Basic ASCII mapping for common keys
            static char scancode_to_ascii[] = {
                0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
                '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
                0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
                0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
                '*', 0, ' '
            };
            
            if (scancode < sizeof(scancode_to_ascii)) {
                ascii = scancode_to_ascii[scancode];
            }
            
            if (ascii) {
                if (ascii == '\n') {
                    // Execute command
                    input_buffer[input_pos] = '\0';
                    kernel_terminal_print("\n");
                    
                    kernel_terminal_execute_command(input_buffer);
                    
                    // Reset for next command
                    input_pos = 0;
                    kernel_terminal_print("# ");
                } else if (ascii == '\b') {
                    // Backspace
                    if (input_pos > 0) {
                        input_pos--;
                        kernel_terminal_putchar('\b');
                    }
                } else {
                    // Add character to input buffer
                    if (input_pos < 255) {
                        input_buffer[input_pos] = ascii;
                        input_pos++;
                        kernel_terminal_putchar(ascii);
                    }
                }
            }
        }
        
        // Small delay
        for (volatile int i = 0; i < 10000; i++);
    }
}
