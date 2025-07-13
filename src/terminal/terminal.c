#include "terminal.h"
#include "../string/string.h"
#include "../memory/memory.h"
#include "../debug/simple_serial.h"
#include "../keyboard/keyboard.h"

terminal_state_t terminal;

void terminal_init(void) {
    memset(&terminal, 0, sizeof(terminal_state_t));
    terminal_clear();
    terminal.cursor_visible = true;
    terminal.initialized = true;
    terminal_show_prompt();
}

void terminal_clear(void) {
    for (int y = 0; y < TERMINAL_HEIGHT; y++) {
        for (int x = 0; x < TERMINAL_WIDTH; x++) {
            terminal.screen[y][x].ch = ' ';
            terminal.screen[y][x].color = TERMINAL_TEXT_COLOR;
            terminal.screen[y][x].dirty = true;
        }
    }
    terminal.cursor_x = 0;
    terminal.cursor_y = 0;
    terminal_update_display();
}

void terminal_putchar(char ch, uint32_t color) {
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
    for (int y = 0; y < TERMINAL_HEIGHT; y++) {
        for (int x = 0; x < TERMINAL_WIDTH; x++) {
            if (terminal.screen[y][x].dirty) {
                vix_kernel_draw_text(&terminal.screen[y][x].ch, x * 8, y * 16, terminal.screen[y][x].color);
                terminal.screen[y][x].dirty = false;
            }
        }
    }
    vix_kernel_present_frame();
}

void terminal_handle_input(char ch) {
    if (ch == '\n') {
        terminal_newline();
    } else if (ch == 8) {
        terminal_backspace();
    } else {
        terminal_putchar(ch, TERMINAL_TEXT_COLOR);
    }
}

void terminal_execute_command(const char* command) {
    terminal_puts("\n", TERMINAL_TEXT_COLOR);
    if (strcmp(command, "help") == 0) {
        terminal_cmd_help();
    } else if (strcmp(command, "clear") == 0) {
        terminal_cmd_clear();
    } else if (strcmp(command, "info") == 0) {
        terminal_cmd_info();
    } else if (strncmp(command, "echo ", 5) == 0) {
        terminal_cmd_echo(command + 5);
    } else {
        terminal_printf(TERMINAL_TEXT_COLOR, "Unknown command: %s\n", command);
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
    terminal_puts("\n# ", TERMINAL_PROMPT_COLOR);
}

void terminal_main_loop(void) {
    terminal_init();
    char ch;
    while (1) {
        // Get keyboard input
        ch = keyboard_pop();
        if (ch != 0) {
            terminal_handle_input(ch);
        }
        // Simple delay to prevent busy waiting
        for (volatile int i = 0; i < 10000; i++);
    }
}

// Example of built-in command function
void terminal_cmd_help(void) {
    terminal_puts("Available commands:\n", TERMINAL_TEXT_COLOR);
    terminal_puts("help - Show this help message\n", TERMINAL_TEXT_COLOR);
    terminal_puts("clear - Clear the screen\n", TERMINAL_TEXT_COLOR);
    terminal_puts("info - Show system information\n", TERMINAL_TEXT_COLOR);
    terminal_puts("echo <text> - Echo the input text\n", TERMINAL_TEXT_COLOR);
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

