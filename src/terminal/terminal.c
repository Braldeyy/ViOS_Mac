#include "terminal.h"
#include "../string/string.h"
#include "../memory/memory.h"
#include "../debug/simple_serial.h"
#include "../keyboard/keyboard.h"
#include "../io/io.h"

terminal_state_t terminal;

// Simple kernel keyboard buffer (not dependent on process system)
#define KERNEL_KEYBOARD_BUFFER_SIZE 256
static char kernel_keyboard_buffer[KERNEL_KEYBOARD_BUFFER_SIZE];
static int kernel_keyboard_head = 0;
static int kernel_keyboard_tail = 0;

// Kernel keyboard buffer functions
void kernel_keyboard_push(char c) {
    if (c == 0) return;
    
    int next_tail = (kernel_keyboard_tail + 1) % KERNEL_KEYBOARD_BUFFER_SIZE;
    if (next_tail != kernel_keyboard_head) {
        kernel_keyboard_buffer[kernel_keyboard_tail] = c;
        kernel_keyboard_tail = next_tail;
    }
}

char kernel_keyboard_pop(void) {
    if (kernel_keyboard_head == kernel_keyboard_tail) {
        return 0; // Buffer empty
    }
    
    char c = kernel_keyboard_buffer[kernel_keyboard_head];
    kernel_keyboard_head = (kernel_keyboard_head + 1) % KERNEL_KEYBOARD_BUFFER_SIZE;
    return c;
}

// Direct keyboard input reading (bypasses process system)
char terminal_get_keyboard_input(void) {
    // Try kernel buffer first
    char c = kernel_keyboard_pop();
    if (c != 0) return c;
    
    // If kernel buffer empty, try direct hardware read
    if (insb(0x64) & 0x01) { // Check if keyboard data is available
        uint8_t scancode = insb(0x60);
        // This is a simplified version - in real implementation you'd need
        // to convert scancode to character properly
        if (scancode < 0x80) { // Key press (not release)
            // For now, just return simple characters for testing
            if (scancode == 0x1C) return '\n'; // Enter
            if (scancode == 0x39) return ' ';  // Space
            if (scancode >= 0x10 && scancode <= 0x19) {
                // Q-P row
                char qwerty[] = "qwertyuiop";
                return qwerty[scancode - 0x10];
            }
            if (scancode >= 0x1E && scancode <= 0x26) {
                // A-L row
                char asdf[] = "asdfghjkl";
                return asdf[scancode - 0x1E];
            }
            if (scancode >= 0x2C && scancode <= 0x32) {
                // Z-M row
                char zxcv[] = "zxcvbnm";
                return zxcv[scancode - 0x2C];
            }
        }
    }
    
    return 0;
}

void terminal_init(void) {
    simple_serial_puts("Terminal initializing...");
    memset(&terminal, 0, sizeof(terminal_state_t));
    terminal.bg_color = TERMINAL_BG_COLOR;
    simple_serial_puts("Terminal clearing screen...");
    terminal_clear();
    terminal.cursor_visible = true;
    terminal.initialized = true;
    simple_serial_puts("Terminal showing prompt...");
    terminal_show_prompt();
    simple_serial_puts("Terminal initialized successfully.");
}

void terminal_clear(void) {
    simple_serial_puts("Clearing terminal screen...");
    
    // Fill entire screen with background color using rectangles
    vix_kernel_fill_rect(0, 0, 800, 600, terminal.bg_color);
    
    for (int y = 0; y < TERMINAL_HEIGHT; y++) {
        for (int x = 0; x < TERMINAL_WIDTH; x++) {
            terminal.screen[y][x].ch = ' ';
            terminal.screen[y][x].color = TERMINAL_TEXT_COLOR;
            terminal.screen[y][x].dirty = true;
        }
    }
    terminal.cursor_x = 0;
    terminal.cursor_y = 0;
    simple_serial_puts("About to update display...");
    terminal_update_display();
    simple_serial_puts("Terminal screen cleared.");
}

void terminal_putchar(char ch, uint32_t color) {
    simple_serial_puts("Putting character...");
    if (terminal.cursor_x >= TERMINAL_WIDTH) {
        terminal_newline();
    }
    terminal.screen[terminal.cursor_y][terminal.cursor_x].ch = ch;
    terminal.screen[terminal.cursor_y][terminal.cursor_x].color = color;
    terminal.screen[terminal.cursor_y][terminal.cursor_x].dirty = true;
    terminal.cursor_x++;
    terminal_update_display();
}

void terminal_puts(const char* str, uint32_t color) {
    while (*str) {
        terminal_putchar(*str++, color);
    }
}

void terminal_printf(uint32_t color, const char* format, ...) {
    // Simple implementation without vsnprintf for now
    terminal_puts(format, color);
}

void terminal_draw_cursor(void) {
    if (terminal.cursor_visible) {
        terminal_putchar('_', TERMINAL_CURSOR_COLOR);
    }
}

void terminal_update_display(void) {
    simple_serial_puts("Updating display...");
    for (int y = 0; y < TERMINAL_HEIGHT; y++) {
        for (int x = 0; x < TERMINAL_WIDTH; x++) {
            if (terminal.screen[y][x].dirty) {
                char str[2];
                str[0] = terminal.screen[y][x].ch;
                str[1] = '\0';
                simple_serial_puts("Drawing character at position");
                vix_kernel_draw_text(str, x * 8, y * 16, terminal.screen[y][x].color);
                terminal.screen[y][x].dirty = false;
            }
        }
    }
    simple_serial_puts("Presenting frame...");
    vix_kernel_present_frame();
    simple_serial_puts("Display updated.");
}

void terminal_handle_input(char ch) {
    if (ch == '\n' || ch == '\r') {
        // Process current input buffer as command
        terminal.input_buffer[terminal.input_pos] = '\0';
        terminal_execute_command(terminal.input_buffer);
        terminal.input_pos = 0;
        memset(terminal.input_buffer, 0, TERMINAL_BUFFER_SIZE);
    } else if (ch == 8) {
        // Backspace
        if (terminal.input_pos > 0) {
            terminal.input_pos--;
            terminal.input_buffer[terminal.input_pos] = '\0';
            terminal_backspace();
        }
    } else if (ch >= 32 && ch <= 126) {
        // Printable characters
        if (terminal.input_pos < TERMINAL_BUFFER_SIZE - 1) {
            terminal.input_buffer[terminal.input_pos] = ch;
            terminal.input_pos++;
            terminal_putchar(ch, TERMINAL_TEXT_COLOR);
        }
    }
}

void terminal_execute_command(const char* command) {
    terminal_puts("\n", TERMINAL_TEXT_COLOR);
    
    // Skip empty commands
    if (strlen(command) == 0) {
        terminal_show_prompt();
        return;
    }
    
    // Add command to history
    terminal_add_to_history(command);
    
    if (strcmp(command, "help") == 0) {
        terminal_cmd_help();
    } else if (strcmp(command, "clear") == 0) {
        terminal_cmd_clear();
    } else if (strcmp(command, "info") == 0) {
        terminal_cmd_info();
    } else if (strncmp(command, "echo ", 5) == 0) {
        terminal_cmd_echo(command + 5);
    } else if (strncmp(command, "bgcolor ", 8) == 0) {
        terminal_cmd_bgcolor(command + 8);
    } else if (strcmp(command, "colors") == 0) {
        terminal_cmd_colors();
    } else if (strcmp(command, "history") == 0) {
        terminal_cmd_history();
    } else if (strcmp(command, "vix") == 0) {
        terminal_cmd_vix();
    } else if (strcmp(command, "mem") == 0) {
        terminal_cmd_mem();
    } else {
        terminal_puts("Unknown command: ", VIX_COLOR_RED);
        terminal_puts(command, VIX_COLOR_RED);
        terminal_puts("\nType 'help' for available commands.\n", TERMINAL_TEXT_COLOR);
    }
    terminal_show_prompt();
}

void terminal_scroll_up(void) {
    memcpy(terminal.screen, terminal.screen[1], (TERMINAL_HEIGHT - 1) * TERMINAL_WIDTH * sizeof(terminal_char_t));
    for (int x = 0; x < TERMINAL_WIDTH; x++) {
        terminal.screen[TERMINAL_HEIGHT - 1][x].ch = ' ';
        terminal.screen[TERMINAL_HEIGHT - 1][x].color = TERMINAL_TEXT_COLOR;
        terminal.screen[TERMINAL_HEIGHT - 1][x].dirty = true;
    }
}

void terminal_newline(void) {
    terminal.cursor_x = 0;
    if (++terminal.cursor_y >= TERMINAL_HEIGHT) {
        terminal_scroll_up();
        terminal.cursor_y = TERMINAL_HEIGHT - 1;
    }
}

void terminal_backspace(void) {
    if (terminal.cursor_x > 0) {
        terminal.cursor_x--;
        terminal.screen[terminal.cursor_y][terminal.cursor_x].ch = ' ';
        terminal.screen[terminal.cursor_y][terminal.cursor_x].color = TERMINAL_TEXT_COLOR;
        terminal.screen[terminal.cursor_y][terminal.cursor_x].dirty = true;
        terminal_update_display();
    }
}

void terminal_add_to_history(const char* command) {
    if (strlen(command) > 0) {
        // Manually move history entries
        for (int i = TERMINAL_HISTORY_SIZE - 1; i > 0; i--) {
            memcpy(terminal.history[i], terminal.history[i-1], TERMINAL_BUFFER_SIZE);
        }
        strncpy(terminal.history[0], command, TERMINAL_BUFFER_SIZE);
        terminal.history[0][TERMINAL_BUFFER_SIZE - 1] = '\0';
        if (terminal.history_count < TERMINAL_HISTORY_SIZE) {
            terminal.history_count++;
        }
    }
}

void terminal_show_prompt(void) {
    simple_serial_puts("Showing prompt...");
    terminal_puts("# ", TERMINAL_PROMPT_COLOR);
    simple_serial_puts("Prompt shown.");
}

void terminal_main_loop(void) {
    simple_serial_puts("TERMINAL: Step A - Function entry");
    
    // Manual initialization without calling complex functions
    simple_serial_puts("TERMINAL: Step B - About to memset");
    memset(&terminal, 0, sizeof(terminal_state_t));
    simple_serial_puts("TERMINAL: Step C - memset completed");
    
    terminal.bg_color = TERMINAL_BG_COLOR;
    terminal.cursor_visible = true;
    terminal.initialized = true;
    simple_serial_puts("TERMINAL: Step D - Basic init completed");
    
    // Clear screen manually
    simple_serial_puts("TERMINAL: Step E - About to clear screen");
    vix_kernel_clear_screen(VIX_RGB(0, 0, 0)); // Black background
    simple_serial_puts("TERMINAL: Step F - Screen cleared");
    
    // Initialize terminal screen buffer
    simple_serial_puts("TERMINAL: Step G - About to init screen buffer");
    for (int y = 0; y < TERMINAL_HEIGHT; y++) {
        for (int x = 0; x < TERMINAL_WIDTH; x++) {
            terminal.screen[y][x].ch = ' ';
            terminal.screen[y][x].color = TERMINAL_TEXT_COLOR;
            terminal.screen[y][x].dirty = false;
        }
    }
    simple_serial_puts("TERMINAL: Step H - Screen buffer initialized");
    
    terminal.cursor_x = 0;
    terminal.cursor_y = 0;
    
    // Show prompt directly
    simple_serial_puts("TERMINAL: Step I - About to setup prompt");
    terminal.screen[0][0].ch = '#';
    terminal.screen[0][0].color = TERMINAL_PROMPT_COLOR;
    terminal.screen[0][1].ch = ' ';
    terminal.screen[0][1].color = TERMINAL_PROMPT_COLOR;
    terminal.cursor_x = 2;
    simple_serial_puts("TERMINAL: Step J - Prompt setup in buffer");
    
    // Draw the prompt on screen
    simple_serial_puts("TERMINAL: Step K - About to draw prompt");
    vix_kernel_draw_text("# ", 0, 0, TERMINAL_PROMPT_COLOR);
    simple_serial_puts("TERMINAL: Step L - Prompt drawn, about to present");
    vix_kernel_present_frame();
    simple_serial_puts("TERMINAL: Step M - Frame presented");
    
    simple_serial_puts("TERMINAL: Step N - About to start input loop");
    
    // Test counter to limit iterations for debugging
    int debug_counter = 0;
    
    // Main input loop
    char ch;
    while (debug_counter < 1000) { // Limited for debugging
        debug_counter++;
        if (debug_counter % 100 == 0) {
            simple_serial_puts("TERMINAL: Still in loop");
        }
        
        // Get keyboard input using our kernel input method
        ch = terminal_get_keyboard_input();
        if (ch != 0) {
            simple_serial_puts("TERMINAL: Got input - exiting for now");
            break; // Exit on first input for debugging
        }
        
        // Small delay to prevent busy waiting
        for (volatile int i = 0; i < 10000; i++);
    }
    
    simple_serial_puts("TERMINAL: Step O - Exiting function");
    return;
}

// Example of built-in command function
void terminal_cmd_help(void) {
    terminal_puts("Available commands:\n", TERMINAL_TEXT_COLOR);
    terminal_puts("help - Show this help message\n", TERMINAL_TEXT_COLOR);
    terminal_puts("clear - Clear the screen\n", TERMINAL_TEXT_COLOR);
    terminal_puts("info - Show system information\n", TERMINAL_TEXT_COLOR);
    terminal_puts("echo <text> - Echo the input text\n", TERMINAL_TEXT_COLOR);
    terminal_puts("bgcolor <color> - Change background color\n", TERMINAL_TEXT_COLOR);
    terminal_puts("colors - List available colors\n", TERMINAL_TEXT_COLOR);
    terminal_puts("history - Show command history\n", TERMINAL_TEXT_COLOR);
    terminal_puts("vix - VIX graphics information\n", TERMINAL_TEXT_COLOR);
    terminal_puts("mem - Show memory information\n", TERMINAL_TEXT_COLOR);
}

void terminal_cmd_clear(void) {
    terminal_clear();
}

void terminal_cmd_info(void) {
    terminal_puts("ViOS Kernel Terminal\n", TERMINAL_TEXT_COLOR);
    terminal_printf(TERMINAL_TEXT_COLOR, "Terminal Size: %dx%d\n", TERMINAL_WIDTH, TERMINAL_HEIGHT);
}

void terminal_cmd_echo(const char* args) {
    terminal_printf(TERMINAL_TEXT_COLOR, "%s\n", args);
}

void terminal_cmd_bgcolor(const char* color) {
    if (strcmp(color, "red") == 0) {
        terminal.bg_color = VIX_COLOR_RED;
    } else if (strcmp(color, "green") == 0) {
        terminal.bg_color = VIX_COLOR_GREEN;
    } else if (strcmp(color, "blue") == 0) {
        terminal.bg_color = VIX_COLOR_BLUE;
    } else if (strcmp(color, "white") == 0) {
        terminal.bg_color = VIX_COLOR_WHITE;
    } else if (strcmp(color, "black") == 0) {
        terminal.bg_color = VIX_COLOR_BLACK;
    } else {
        terminal_puts("Unknown color. Available colors: red, green, blue, white, black.\n", VIX_COLOR_RED);
        return;
    }
    terminal_puts("Background color changed successfully.\n", TERMINAL_TEXT_COLOR);
    terminal_clear();
}

void terminal_cmd_colors(void) {
    terminal_puts("Available colors: red, green, blue, white, black.\n", TERMINAL_TEXT_COLOR);
}

void terminal_cmd_history(void) {
    for (int i = terminal.history_count - 1; i >= 0; i--) {
        terminal_printf(TERMINAL_TEXT_COLOR, "%d: %s\n", terminal.history_count - i, terminal.history[i]);
    }
}

void terminal_cmd_vix(void) {
    terminal_puts("VIX Framebuffer Graphics\n", TERMINAL_TEXT_COLOR);
    terminal_puts("Framebuffer size: 800x600\n", TERMINAL_TEXT_COLOR);
    terminal_puts("Use 'bgcolor' <color> to set background.\n", TERMINAL_TEXT_COLOR);
}

void terminal_cmd_mem(void) {
    terminal_puts("Memory info:\n", TERMINAL_TEXT_COLOR);
    terminal_puts("Total RAM: 128MB\n", TERMINAL_TEXT_COLOR);
}

