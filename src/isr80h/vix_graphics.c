#include "vix_graphics.h"
#include "isr80h.h"
#include "task/task.h"
#include "task/process.h"
#include "graphics/graphics.h"
#include "kernel.h"
#include "idt/idt.h"
#include <stdint.h>

// VIX Graphics API Implementation

void *isr80h_command11_vix_draw_pixel(struct interrupt_frame *frame)
{
    // Parameters: EBX = x, ECX = y, EDX = color (RGB packed)
    int x = (int)frame->ebx;
    int y = (int)frame->ecx;
    uint32_t rgb = (uint32_t)frame->edx;
    
    // Extract RGB components
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    
    // Convert to graphics color and draw
    Color color = graphics_rgb_to_color(r, g, b);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        graphics_set_pixel(ctx->back_buffer, x, y, color);
    }
    
    return 0;
}

void *isr80h_command12_vix_draw_rect(struct interrupt_frame *frame)
{
    // Parameters: EBX = x, ECX = y, EDX = width, ESI = height, EDI = color
    int x = (int)frame->ebx;
    int y = (int)frame->ecx;
    int width = (int)frame->edx;
    int height = (int)frame->esi;
    uint32_t rgb = (uint32_t)frame->edi;
    
    // Extract RGB components
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    
    // Convert to graphics color and draw
    Color color = graphics_rgb_to_color(r, g, b);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Rectangle rect = {x, y, width, height};
        graphics_draw_rect(ctx->back_buffer, rect, color);
    }
    
    return 0;
}

void *isr80h_command13_vix_fill_rect(struct interrupt_frame *frame)
{
    // Parameters: EBX = x, ECX = y, EDX = width, ESI = height, EDI = color
    int x = (int)frame->ebx;
    int y = (int)frame->ecx;
    int width = (int)frame->edx;
    int height = (int)frame->esi;
    uint32_t rgb = (uint32_t)frame->edi;
    
    // Extract RGB components
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    
    // Convert to graphics color and fill
    Color color = graphics_rgb_to_color(r, g, b);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Rectangle rect = {x, y, width, height};
        graphics_fill_rect(ctx->back_buffer, rect, color);
    }
    
    return 0;
}

void *isr80h_command14_vix_clear_screen(struct interrupt_frame *frame)
{
    // Parameters: EBX = color (RGB packed)
    uint32_t rgb = (uint32_t)frame->ebx;
    
    // Extract RGB components
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    
    // Convert to graphics color and clear
    Color color = graphics_rgb_to_color(r, g, b);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        graphics_clear_surface(ctx->back_buffer, color);
    }
    
    return 0;
}

void *isr80h_command15_vix_present_frame(struct interrupt_frame *frame)
{
    // No parameters - just present the current frame
    graphics_present();
    return 0;
}

void *isr80h_command16_vix_get_screen_info(struct interrupt_frame *frame)
{
    // Parameters: EBX = pointer to screen_info structure
    struct vix_screen_info *info = (struct vix_screen_info *)frame->ebx;
    
    // Copy screen info to user space
    struct task *current = task_current();
    if (current) {
        GraphicsContext *ctx = graphics_get_context();
        if (ctx) {
            struct vix_screen_info kernel_info = {
                .width = ctx->current_mode.width,
                .height = ctx->current_mode.height,
                .bpp = ctx->current_mode.bpp,
                .refresh_rate = ctx->current_mode.refresh_rate
            };
            
            // Copy to user space (simplified - in real implementation would need proper user space copying)
            *info = kernel_info;
        }
    }
    
    return 0;
}

void *isr80h_command17_vix_draw_line(struct interrupt_frame *frame)
{
    // Parameters: EBX = x1, ECX = y1, EDX = x2, ESI = y2, EDI = color
    int x1 = (int)frame->ebx;
    int y1 = (int)frame->ecx;
    int x2 = (int)frame->edx;
    int y2 = (int)frame->esi;
    uint32_t rgb = (uint32_t)frame->edi;
    
    // Extract RGB components
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    
    // Convert to graphics color and draw
    Color color = graphics_rgb_to_color(r, g, b);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Point start = {x1, y1};
        Point end = {x2, y2};
        graphics_draw_line(ctx->back_buffer, start, end, color);
    }
    
    return 0;
}

void *isr80h_command18_vix_draw_circle(struct interrupt_frame *frame)
{
    // Parameters: EBX = x, ECX = y, EDX = radius, ESI = color
    int x = (int)frame->ebx;
    int y = (int)frame->ecx;
    int radius = (int)frame->edx;
    uint32_t rgb = (uint32_t)frame->esi;
    
    // Extract RGB components
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    
    // Convert to graphics color and draw
    Color color = graphics_rgb_to_color(r, g, b);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Point center = {x, y};
        graphics_draw_circle(ctx->back_buffer, center, radius, color);
    }
    
    return 0;
}

void *isr80h_command19_vix_fill_circle(struct interrupt_frame *frame)
{
    // Parameters: EBX = x, ECX = y, EDX = radius, ESI = color
    int x = (int)frame->ebx;
    int y = (int)frame->ecx;
    int radius = (int)frame->edx;
    uint32_t rgb = (uint32_t)frame->esi;
    
    // Extract RGB components
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    
    // Convert to graphics color and fill
    Color color = graphics_rgb_to_color(r, g, b);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Point center = {x, y};
        graphics_fill_circle(ctx->back_buffer, center, radius, color);
    }
    
    return 0;
}

void *isr80h_command20_vix_draw_text(struct interrupt_frame *frame)
{
    // Parameters: EBX = text (char*), ECX = x, EDX = y, ESI = color
    char *text = (char *)frame->ebx;
    int x = (int)frame->ecx;
    int y = (int)frame->edx;
    uint32_t rgb = (uint32_t)frame->esi;
    
    // Validate text pointer
    if (!text) {
        return (void *)-1;
    }
    
    // Extract RGB components
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    
    // Convert to graphics color and draw text
    Color color = graphics_rgb_to_color(r, g, b);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Point position = {x, y};
        graphics_draw_text(ctx->back_buffer, text, position, color);
    }
    
    return 0;
}

void *isr80h_command21_vix_draw_text_scaled(struct interrupt_frame *frame)
{
    // Parameters: EBX = text (char*), ECX = x, EDX = y, ESI = color, EDI = scale
    char *text = (char *)frame->ebx;
    int x = (int)frame->ecx;
    int y = (int)frame->edx;
    uint32_t rgb = (uint32_t)frame->esi;
    int scale = (int)frame->edi;
    
    // Validate text pointer and scale
    if (!text || scale <= 0) {
        return (void *)-1;
    }
    
    // Extract RGB components
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;
    
    // Convert to graphics color and draw scaled text
    Color color = graphics_rgb_to_color(r, g, b);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Point position = {x, y};
        graphics_draw_text_scaled(ctx->back_buffer, text, position, color, scale);
    }
    
    return 0;
}

void *isr80h_command22_vix_text_width(struct interrupt_frame *frame)
{
    // Parameters: EBX = text (char*), ECX = scale
    char *text = (char *)frame->ebx;
    int scale = (int)frame->ecx;
    
    // Validate text pointer and scale
    if (!text || scale <= 0) {
        return (void *)-1;
    }
    
    // Calculate text width
    int width = graphics_text_width(text, scale);
    return (void *)width;
}

void *isr80h_command23_vix_text_height(struct interrupt_frame *frame)
{
    // Parameters: EBX = scale
    int scale = (int)frame->ebx;
    
    // Validate scale
    if (scale <= 0) {
        return (void *)-1;
    }
    
    // Calculate text height
    int height = graphics_text_height(scale);
    return (void *)height;
}
