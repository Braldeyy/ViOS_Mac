#include "vios.h"
#include "stdio.h"
#include "string.h"

// Using standard library string functions

#define MAX_INPUT 256
#define MAX_PROGRAMS 32
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16

typedef struct {
    char name[64];
    char description[128];
    char filename[64];
} program_info_t;

// Registry of available programs
program_info_t program_registry[] = {
    {"vix_demo", "VIX Graphics API demonstration", "vix_demo.elf"},
    {"vix_text_demo", "VIX Text rendering demonstration", "vix_text_demo.elf"},
    {"help", "Show available commands", "built-in"},
    {"clear", "Clear the screen", "built-in"},
    {"list", "List available programs", "built-in"},
    {"exit", "Exit the terminal", "built-in"},
    {"info", "Show system information", "built-in"},
    {"", "", ""} // Sentinel
};

// Terminal state
int cursor_x = 0;
int cursor_y = 0;
int line_height = 1;

void terminal_init() {
    // Clear screen using vios_print by filling with spaces
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            vios_print(" ", x, y, 0, 0, 0, 1);
        }
    }
    
    // Draw header
    vios_print("ViOS Terminal v1.0", 2, 1, 255, 255, 255, 2);
    vios_print("Dynamic Program Loader & Shell", 2, 3, 0, 255, 255, 1);
    vios_print("Type 'help' for available commands", 2, 4, 200, 200, 200, 1);
    
    // Draw separator line
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        vios_print("-", x, 5, 100, 100, 100, 1);
    }
    
    cursor_x = 0;
    cursor_y = 7;
}

void terminal_print(const char* text, int r, int g, int b) {
    vios_print(text, cursor_x, cursor_y, r, g, b, 1);
    cursor_y += line_height;
    
    // Scroll if we're near the bottom
    if (cursor_y > SCREEN_HEIGHT - 3) {
        // Simple scroll - reinitialize
        terminal_init();
    }
}

void terminal_newline() {
    cursor_y += line_height;
    cursor_x = 0;
}

void terminal_prompt() {
    vios_print("vios> ", cursor_x, cursor_y, 0, 255, 0, 1);
    cursor_x += 6; // "vios> " is 6 characters
}

void show_help() {
    terminal_print("Available commands:", 0, 255, 255);
    terminal_newline();
    
    for (int i = 0; program_registry[i].name[0]; i++) {
        char line[256];
        // Simple string formatting (no sprintf available)
        strcpy(line, "  ");
        strcat(line, program_registry[i].name);
        strcat(line, " - ");
        strcat(line, program_registry[i].description);
        
        terminal_print(line, 255, 255, 255);
    }
    terminal_newline();
}

void show_system_info() {
    terminal_print("System Information:", 0, 255, 255);
    terminal_newline();
    
    terminal_print("  OS: ViOS (VIOS Operating System)", 255, 255, 255);
    terminal_print("  Graphics: VIX (VIOS Graphics API)", 255, 255, 255);
    terminal_print("  Screen: 1024x768 @ 16bpp", 255, 255, 255);
    terminal_print("  Fonts: AtariST8x16 Bitmap Font", 255, 255, 255);
    terminal_print("  Terminal: Dynamic ELF Loader", 255, 255, 255);
    terminal_newline();
}

void list_programs() {
    terminal_print("Available Programs:", 0, 255, 255);
    terminal_newline();
    
    for (int i = 0; program_registry[i].name[0]; i++) {
        if (strcmp(program_registry[i].filename, "built-in") != 0) {
            char line[256];
            strcpy(line, "  ");
            strcat(line, program_registry[i].name);
            strcat(line, " (");
            strcat(line, program_registry[i].filename);
            strcat(line, ")");
            
            terminal_print(line, 255, 255, 255);
        }
    }
    terminal_newline();
}

int execute_program(const char* program_name) {
    // Find program in registry
    for (int i = 0; program_registry[i].name[0]; i++) {
        if (strcmp(program_registry[i].name, program_name) == 0) {
            if (strcmp(program_registry[i].filename, "built-in") == 0) {
                return 0; // Built-in command, handled elsewhere
            }
            
            // Try to load and execute the program
            terminal_print("Loading program...", 0, 255, 255);
            
            // Use the existing process loading system
            vios_process_load_start(program_registry[i].filename);
            
            // After program finishes, reinitialize terminal
            terminal_init();
            terminal_print("Program finished. Back to terminal.", 0, 255, 255);
            return 1;
        }
    }
    
    // Program not found
    char error_msg[256];
    strcpy(error_msg, "Command not found: ");
    strcat(error_msg, program_name);
    terminal_print(error_msg, 255, 0, 0);
    return 0;
}

void get_input(char* buffer, int max_len) {
    int pos = 0;
    int input_x = cursor_x;
    
    while (pos < max_len - 1) {
        int key = vios_getkeyblock();
        
        if (key == '\n' || key == '\r') {
            buffer[pos] = '\0';
            break;
        } else if (key == '\b' || key == 127) { // Backspace
            if (pos > 0) {
                pos--;
                input_x -= 1;
                // Clear the character
                vios_print(" ", input_x, cursor_y, 0, 0, 0, 1);
            }
        } else if (key >= 32 && key <= 126) { // Printable characters
            buffer[pos] = key;
            
            // Display the character
            char temp[2] = {key, '\0'};
            vios_print(temp, input_x, cursor_y, 255, 255, 255, 1);
            
            pos++;
            input_x += 1;
        }
    }
    
    buffer[pos] = '\0';
}

int main(int argc, char** argv) {
    char input[MAX_INPUT];
    
    terminal_init();
    
    while (1) {
        terminal_prompt();
        
        get_input(input, MAX_INPUT);
        
        terminal_newline();
        
        // Process command
        if (strlen(input) == 0) {
            continue;
        }
        
        // Built-in commands
        if (strcmp(input, "help") == 0) {
            show_help();
        } else if (strcmp(input, "clear") == 0) {
            terminal_init();
        } else if (strcmp(input, "list") == 0) {
            list_programs();
        } else if (strcmp(input, "info") == 0) {
            show_system_info();
        } else if (strcmp(input, "exit") == 0) {
            terminal_print("Goodbye!", 0, 255, 255);
            vios_sleep(1);
            break;
        } else {
            // Try to execute as program
            if (!execute_program(input)) {
                terminal_print("Type 'help' for available commands", 150, 150, 150);
            }
        }
    }
    
    // Clear screen before exit
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            vios_print(" ", x, y, 0, 0, 0, 1);
        }
    }
    
    return 0;
}
