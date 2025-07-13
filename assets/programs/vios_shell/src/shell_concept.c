// ViOS General Shell Concept
// This would replace individual .elf files with a dynamic loader

#include "vios.h"
#include "shell.h"

typedef struct {
    char name[64];
    char path[256];
    char description[128];
} program_entry_t;

// Built-in programs registry
program_entry_t programs[] = {
    {"pong", "/programs/pong.elf", "Classic Pong game"},
    {"vix_demo", "/programs/vix_demo.elf", "VIX Graphics demo"},
    {"test", "/programs/test.elf", "System test program"},
    {"ls", "built-in", "List files"},
    {"help", "built-in", "Show available commands"},
    {"clear", "built-in", "Clear screen"},
    {"exit", "built-in", "Exit shell"},
    {0} // Sentinel
};

void shell_main() {
    char command[256];
    char args[512];
    
    vix_clear_screen();
    vios_print("ViOS Shell v1.0", 10, 10, 255, 255, 255, 1);
    vios_print("Type 'help' for available commands", 10, 30, 200, 200, 200, 1);
    
    while (1) {
        // Display prompt
        vios_print("vios> ", 10, current_line * 20, 0, 255, 0, 1);
        
        // Get user input
        shell_get_input(command, sizeof(command));
        
        // Parse command and arguments
        shell_parse_command(command, args);
        
        // Handle built-in commands
        if (strcmp(command, "help") == 0) {
            shell_show_help();
        } else if (strcmp(command, "ls") == 0) {
            shell_list_files();
        } else if (strcmp(command, "clear") == 0) {
            vix_clear_screen();
            current_line = 0;
        } else if (strcmp(command, "exit") == 0) {
            break;
        } else {
            // Try to load and execute external program
            shell_execute_program(command, args);
        }
        
        current_line++;
    }
}

void shell_execute_program(const char* program_name, const char* args) {
    // Find program in registry
    program_entry_t* prog = shell_find_program(program_name);
    
    if (!prog) {
        vios_print("Command not found: ", 10, current_line * 20, 255, 0, 0, 1);
        vios_print(program_name, 200, current_line * 20, 255, 0, 0, 1);
        return;
    }
    
    if (strcmp(prog->path, "built-in") == 0) {
        // Already handled above
        return;
    }
    
    // Load and execute the .elf file
    // This would use your existing ELF loader
    int result = vios_exec(prog->path, args);
    
    if (result != 0) {
        vios_print("Failed to execute: ", 10, current_line * 20, 255, 0, 0, 1);
        vios_print(program_name, 200, current_line * 20, 255, 0, 0, 1);
    }
}

void shell_show_help() {
    vios_print("Available commands:", 10, current_line * 20, 255, 255, 0, 1);
    current_line++;
    
    for (int i = 0; programs[i].name[0]; i++) {
        char line[512];
        sprintf(line, "  %-12s - %s", programs[i].name, programs[i].description);
        vios_print(line, 20, current_line * 20, 200, 200, 200, 1);
        current_line++;
    }
}

// This would be your main shell program
int main(int argc, char** argv) {
    shell_main();
    return 0;
}
