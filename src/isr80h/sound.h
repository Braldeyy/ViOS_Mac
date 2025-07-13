#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

struct interrupt_frame;

// Sound system call handlers
void *isr80h_command26_sound_play(struct interrupt_frame *frame);
void *isr80h_command27_sound_stop(struct interrupt_frame *frame);

#endif // SOUND_H
