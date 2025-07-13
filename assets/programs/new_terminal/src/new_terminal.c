#include "vios.h"
#include "stdio.h"
#include "string.h"

#define MAX_INPUT 256

// String functions are provided by stdlib

void get_input(char *buffer, int max_len) {
    int pos = 0;
    int y_pos = 70;

    while (pos < max_len - 1) {
        char ch;
        int result = via_keyboard_read(&ch, 1, 1); 

        if (result > 0) {
            if (ch == '\n' || ch == '\r') {
                buffer[pos] = '\0';
                break;
            } else if (ch == '\b' || ch == 127) {
                if (pos > 0) {
                    pos--;
                    vix_draw_text(" ", 10 + (pos * 8), y_pos, VIX_COLOR_BLACK);
                }
            } else if (ch >= 32 && ch <= 126) {
                buffer[pos] = ch;
                char temp[2] = {ch, '\0'};
                vix_draw_text(temp, 10 + (pos * 8), y_pos, VIX_COLOR_WHITE);
                vix_present_frame();

                pos++;
            }
        }
    }

    buffer[pos] = '\0';
}

void draw_prompt() {
    vix_clear_screen(VIX_RGB(20, 20, 20));
    vix_draw_text("ViOS Terminal - VIA & VIX APIs", 10, 10, VIX_COLOR_WHITE);
    vix_draw_text("Type 'help' for commands.", 10, 30, VIX_COLOR_CYAN);
    vix_present_frame();
}

void terminal_loop() {
    char input[MAX_INPUT];
    int line_y = 70;

    draw_prompt();

    while (1) {
        vix_fill_rect(0, 50, 1024, 200, VIX_RGB(20, 20, 20));

        vix_draw_text("vios$ ", 10, 50, VIX_COLOR_GREEN);
        vix_present_frame();

        get_input(input, MAX_INPUT);

        if (strcmp(input, "help") == 0) {
            vix_draw_text("Available commands:", 10, line_y, VIX_COLOR_YELLOW);
            vix_draw_text("  help  - Show this help", 10, line_y + 20, VIX_COLOR_WHITE);
            vix_draw_text("  demo  - Run VIX graphics demo", 10, line_y + 40, VIX_COLOR_WHITE);
            vix_draw_text("  pong  - Run Pong game", 10, line_y + 60, VIX_COLOR_WHITE);
            vix_draw_text("  sound - Test VIA sound API", 10, line_y + 80, VIX_COLOR_WHITE);
            vix_draw_text("  exit  - Exit terminal", 10, line_y + 100, VIX_COLOR_WHITE);
        } else if (strcmp(input, "demo") == 0) {
            vix_draw_text("Loading VIX demo...", 10, line_y, VIX_COLOR_CYAN);
            vix_present_frame();
            vios_process_load_start("0:/bin/vix_demo.elf");
        } else if (strcmp(input, "pong") == 0) {
            vix_draw_text("Loading Pong game...", 10, line_y, VIX_COLOR_CYAN);
            vix_present_frame();
            vios_process_load_start("0:/bin/vix_pong.elf");
        } else if (strcmp(input, "sound") == 0) {
            vix_draw_text("Testing VIA sound API...", 10, line_y, VIX_COLOR_CYAN);
            vix_present_frame();
            via_sound_play(440, 1000); 
            vix_draw_text("Sound test complete!", 10, line_y + 20, VIX_COLOR_GREEN);
        } else if (strcmp(input, "exit") == 0) {
            vix_draw_text("Exiting terminal...", 10, line_y, VIX_COLOR_WHITE);
            vix_present_frame();
            break;
        } else if (input[0] != '\0') {
            vix_draw_text("Unknown command. Type 'help' for available commands.", 10, line_y, VIX_COLOR_RED);
        }

        vix_present_frame();

        for (volatile int i = 0; i < 1000000; i++);
    }
}

int main() {
    terminal_loop();
    return 0;
}
