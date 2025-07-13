# Kernel-Level Terminal Implementation Plan

## Current Architecture Analysis

### Current System Structure

1. **Kernel Boot Process** (`src/kernel.c`):
   - Initializes system components (GDT, TSS, devices, paging, graphics)
   - Attempts to load `vios_terminal.elf` as a separate process
   - Falls back to graphics demo in main loop if terminal fails

2. **Process-Based Terminal** (`assets/programs/vios_terminal/src/vios_terminal.c`):
   - Separate ELF executable loaded as a user process
   - Uses system calls to interact with kernel
   - Limited by process boundaries and system call overhead

3. **Built-in Terminal State** (`src/kernel/mainloop.c`):
   - Already has a basic kernel-level terminal implementation
   - Terminal mode flag controls whether to show terminal or graphics demo
   - Uses direct graphics calls instead of VIX API

## Items Needed for Full Kernel-Level Terminal

### 1. Enhanced Terminal Core (Already Partially Implemented)

**Current Features in `mainloop.c`:**
- Terminal buffer (25x80 character grid)
- Color support per character
- Input handling with keyboard buffer
- Basic command processing
- Screen scrolling

**Needs Enhancement:**
- Command history
- Tab completion
- Better input editing (cursor movement, home/end keys)
- Variable screen size support
- Copy/paste functionality

### 2. Built-in Command Registry System

**Required Components:**
```c
// Command structure
typedef struct {
    char name[32];
    char description[128];
    int (*handler)(int argc, char **argv);
    int flags; // permissions, hidden, etc.
} kernel_command_t;

// Command registry
static kernel_command_t kernel_commands[] = {
    {"help", "Show available commands", cmd_help, 0},
    {"info", "Show system information", cmd_info, 0},
    {"mem", "Show memory information", cmd_mem, 0},
    {"proc", "Show process information", cmd_proc, 0},
    {"vix", "VIX graphics commands", cmd_vix, 0},
    {"test", "Run system tests", cmd_test, 0},
    // ... more commands
};
```

### 3. System Information Commands

**Memory Management Interface:**
- Memory usage display (`mem` command)
- Heap statistics
- Process memory information
- Page table viewer

**Process Management Interface:**
- Process list viewer (`ps` command)
- Process creation/termination
- Process switching controls

**Hardware Information:**
- CPU information
- Graphics mode information
- Disk/filesystem information

### 4. VIX Graphics Integration

**Built-in VIX Command Handler:**
```c
int cmd_vix(int argc, char **argv) {
    if (argc < 2) {
        terminal_print("Usage: vix <command> [args]", 0xFF0000);
        return -1;
    }
    
    if (strcmp(argv[1], "demo") == 0) {
        // Switch to graphics demo mode
        terminal_mode = 0;
    } else if (strcmp(argv[1], "clear") == 0) {
        // Clear screen with color
        uint32_t color = argc > 2 ? parse_color(argv[2]) : VIX_COLOR_BLACK;
        vix_clear_screen(color);
        vix_present_frame();
    }
    // ... more VIX commands
    
    return 0;
}
```

### 5. System Call Interface Improvements

**Enhanced System Call Handlers:**
- Direct kernel command execution
- Kernel state inspection
- Resource management commands

### 6. File System Integration

**File Operations:**
- Directory listing (`ls`)
- File viewing (`cat`)
- File operations (`cp`, `mv`, `rm`)
- Path navigation (`cd`, `pwd`)

### 7. Testing Framework Integration

**Built-in Test Commands:**
```c
int cmd_test(int argc, char **argv) {
    if (argc < 2) {
        list_available_tests();
        return 0;
    }
    
    if (strcmp(argv[1], "memory") == 0) {
        return run_memory_tests();
    } else if (strcmp(argv[1], "graphics") == 0) {
        return run_graphics_tests();
    } else if (strcmp(argv[1], "all") == 0) {
        return run_all_tests();
    }
    
    return -1;
}
```

## Implementation Strategy

### Phase 1: Enhanced Core Terminal

1. **Expand `mainloop.c` terminal functionality:**
   - Add command parsing with argc/argv
   - Implement command registry system
   - Add command history buffer
   - Improve input editing capabilities

2. **Restructure terminal code:**
   - Move terminal functions to `src/terminal/` directory
   - Create `terminal.h` and `terminal.c`
   - Keep mainloop.c focused on the main loop

### Phase 2: Command System

1. **Create command infrastructure:**
   - `src/terminal/commands.h` - Command structure definitions
   - `src/terminal/commands.c` - Command registry and dispatch
   - `src/terminal/builtins/` - Directory for built-in commands

2. **Implement core commands:**
   - System information commands
   - Memory management commands
   - Process management commands
   - VIX graphics commands

### Phase 3: Advanced Features

1. **File system integration:**
   - Directory navigation commands
   - File manipulation commands
   - Integration with existing VFS

2. **Testing integration:**
   - Built-in test runner
   - System diagnostics
   - Hardware testing

### Phase 4: Process Integration

1. **Hybrid approach:**
   - Keep kernel terminal as primary interface
   - Allow loading external programs when needed
   - Provide seamless switching between modes

## Benefits of Kernel-Level Terminal

### Performance
- No system call overhead for basic operations
- Direct access to kernel data structures
- Faster response times

### Capabilities
- Full access to kernel internals
- Real-time system monitoring
- Direct hardware control

### Reliability
- Cannot crash due to process failures
- Always available even if user processes fail
- Better debugging capabilities

### Integration
- Seamless VIX graphics integration
- Direct access to all system resources
- Unified system management interface

## Code Structure

```
src/
├── terminal/
│   ├── terminal.h          # Terminal interface
│   ├── terminal.c          # Core terminal implementation
│   ├── commands.h          # Command system
│   ├── commands.c          # Command dispatch
│   ├── input.c             # Input handling and editing
│   ├── history.c           # Command history
│   └── builtins/
│       ├── system.c        # System information commands
│       ├── memory.c        # Memory management commands
│       ├── process.c       # Process management commands
│       ├── vix.c           # VIX graphics commands
│       ├── filesystem.c    # File system commands
│       └── test.c          # Testing commands
├── kernel/
│   └── mainloop.c          # Updated to use terminal module
└── ...
```

## Migration Strategy

1. **Gradual Migration:**
   - Keep existing process-based terminal working
   - Add kernel terminal as alternative boot option
   - Allow runtime switching between modes

2. **Fallback Support:**
   - If kernel terminal has issues, fall back to process-based
   - Maintain compatibility with existing ELF programs

3. **Feature Parity:**
   - Ensure kernel terminal has all features of process-based version
   - Add new capabilities not possible in user space

## Implementation Priority

1. **High Priority:**
   - Command registry system
   - Basic built-in commands (help, info, mem, proc)
   - VIX graphics integration
   - File system commands

2. **Medium Priority:**
   - Advanced input editing
   - Command history
   - Testing framework integration
   - Process management commands

3. **Low Priority:**
   - Tab completion
   - Scripting support
   - Configuration system
   - Advanced debugging commands

This kernel-level terminal would provide a much more powerful and integrated system management interface while maintaining the ability to run external programs when needed.
