#include "../task/task.h"
#include "../task/process.h"
#include "keyboard/keyboard.h"
#include "../debug/simple_serial.h"

void *isr80h_command24_keyboard_read(struct interrupt_frame *frame)
{
    struct task *task = task_current();
    if (!task) {
        simple_serial_puts("Keyboard read: No current task\n");
        return (void *)-1;
    }

    struct process *process = task->process;
    if (!process) {
        simple_serial_puts("Keyboard read: No process for task\n");
        return (void *)-1;
    }

    // Parameters: buffer pointer, buffer size, blocking flag
    char *buffer = (char *)task_get_stack_item(task, 0);
    int buffer_size = (int)task_get_stack_item(task, 1);
    int blocking = (int)task_get_stack_item(task, 2);
    
    simple_serial_puts("Keyboard read: Reading keyboard input\n");
    
    if (blocking) {
        // Blocking read - wait for input
        char key = keyboard_pop();
        while (key == 0) {
            key = keyboard_pop();
        }
        if (buffer_size > 0) {
            buffer[0] = key;
            return (void *)1; // Return number of characters read
        }
    } else {
        // Non-blocking read - return immediately
        char key = keyboard_pop();
        if (key != 0 && buffer_size > 0) {
            buffer[0] = key;
            return (void *)1; // Return number of characters read
        }
    }
    
    return (void *)0; // No characters read
}

void *isr80h_command25_keyboard_state(struct interrupt_frame *frame)
{
    struct task *task = task_current();
    if (!task) {
        simple_serial_puts("Keyboard state: No current task\n");
        return (void *)-1;
    }

    struct process *process = task->process;
    if (!process) {
        simple_serial_puts("Keyboard state: No process for task\n");
        return (void *)-1;
    }

    simple_serial_puts("Keyboard state: Getting keyboard state\n");
    
    // Return keyboard state (modifier keys, etc.)
    // For now, just return 0 (no special state)
    return (void *)0;
}
