#include "sound.h"
#include "../task/task.h"
#include "../task/process.h"
#include "../audio/audio.h"
#include "../debug/simple_serial.h"

void *isr80h_command26_sound_play(struct interrupt_frame *frame)
{
    struct task *task = task_current();
    if (!task) {
        simple_serial_puts("Sound play: No current task\n");
        return (void *)-1;
    }

    struct process *process = task->process;
    if (!process) {
        simple_serial_puts("Sound play: No process for task\n");
        return (void *)-1;
    }

    // Parameters: frequency (Hz), duration (ms)
    uint32_t frequency = (uint32_t)task_get_stack_item(task, 0);
    uint32_t duration = (uint32_t)task_get_stack_item(task, 1);
    
    simple_serial_puts("Sound play: Playing tone\n");
    
    // Use the existing audio system to play a tone
    // TODO: Use frequency and duration parameters
    (void)frequency; // Suppress unused variable warning
    (void)duration;  // Suppress unused variable warning
    virtual_audio_control(VIRTUAL_AUDIO_BEEP);
    
    return (void *)0;
}

void *isr80h_command27_sound_stop(struct interrupt_frame *frame)
{
    struct task *task = task_current();
    if (!task) {
        simple_serial_puts("Sound stop: No current task\n");
        return (void *)-1;
    }

    struct process *process = task->process;
    if (!process) {
        simple_serial_puts("Sound stop: No process for task\n");
        return (void *)-1;
    }

    simple_serial_puts("Sound stop: Stopping audio\n");
    
    // Stop any currently playing audio
    virtual_audio_control(VIRTUAL_AUDIO_STOP);
    
    return (void *)0;
}
