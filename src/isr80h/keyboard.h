#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

struct interrupt_frame;

// Keyboard system call handlers
void *isr80h_command24_keyboard_read(struct interrupt_frame *frame);
void *isr80h_command25_keyboard_state(struct interrupt_frame *frame);

#endif // KEYBOARD_H
