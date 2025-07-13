#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <stdbool.h>
#include "../graphics/vix_kernel.h"

// Terminal configuration
#define TERMINAL_WIDTH 80
#define TERMINAL_HEIGHT 25
#define TERMINAL_BUFFER_SIZE 1024
#define TERMINAL_HISTORY_SIZE 10

// Terminal colors
#define TERMINAL_BG_COLOR VIX_RGB(0, 0, 0)      // Black background
#define TERMINAL_TEXT_COLOR VIX_COLOR_WHITE      // White text
#define TERMINAL_CURSOR_COLOR VIX_COLOR_CYAN     // Cyan cursor
#define TERMINAL_PROMPT_COLOR VIX_COLOR_GREEN    // Green prompt

// Terminal character structure
typedef struct {
    char ch;
    uint32_t color;
    bool dirty;  // Needs redraw
} terminal_char_t;

// Terminal state structure
typedef struct {
    terminal_char_t screen[TERMINAL_HEIGHT][TERMINAL_WIDTH];
    char input_buffer[TERMINAL_BUFFER_SIZE];
    char history[TERMINAL_HISTORY_SIZE][TERMINAL_BUFFER_SIZE];
    int cursor_x, cursor_y;
    int input_pos;
    int history_pos;
    int history_count;
    bool cursor_visible;
    uint32_t cursor_blink_counter;
    uint32_t bg_color;
    bool initialized;
} terminal_state_t;

// Terminal functions
void terminal_init(void);
void terminal_clear(void);
void terminal_putchar(char ch, uint32_t color);
void terminal_puts(const char* str, uint32_t color);
void terminal_printf(uint32_t color, const char* format, ...);
void terminal_draw_cursor(void);
void terminal_update_display(void);
void terminal_handle_input(char ch);
void terminal_execute_command(const char* command);
void terminal_scroll_up(void);
void terminal_newline(void);
void terminal_backspace(void);
void terminal_add_to_history(const char* command);
void terminal_show_prompt(void);
void terminal_main_loop(void);

// Built-in commands
void terminal_cmd_help(void);
void terminal_cmd_clear(void);
void terminal_cmd_info(void);
void terminal_cmd_echo(const char* args);
void terminal_cmd_bgcolor(const char* color);
void terminal_cmd_colors(void);
void terminal_cmd_history(void);
void terminal_cmd_vix(void);
void terminal_cmd_mem(void);

#endif // TERMINAL_H
