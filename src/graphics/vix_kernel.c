#include "vix_kernel.h"
#include "graphics.h" // Existing graphics functions

void vix_kernel_draw_pixel(int x, int y, uint32_t color) {
    Color c = graphics_rgb_to_color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        graphics_set_pixel(ctx->back_buffer, x, y, c);
    }
}

void vix_kernel_draw_rect(int x, int y, int width, int height, uint32_t color) {
    Color c = graphics_rgb_to_color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Rectangle rect = {x, y, width, height};
        graphics_draw_rect(ctx->back_buffer, rect, c);
    }
}

void vix_kernel_fill_rect(int x, int y, int width, int height, uint32_t color) {
    Color c = graphics_rgb_to_color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Rectangle rect = {x, y, width, height};
        graphics_fill_rect(ctx->back_buffer, rect, c);
    }
}

void vix_kernel_clear_screen(uint32_t color) {
    Color c = graphics_rgb_to_color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        graphics_clear_surface(ctx->back_buffer, c);
    }
}

void vix_kernel_present_frame(void) {
    graphics_present();
}

void vix_kernel_get_screen_info(struct vix_screen_info *info) {
    GraphicsContext *ctx = graphics_get_context();
    if (ctx) {
        info->width = ctx->current_mode.width;
        info->height = ctx->current_mode.height;
        info->bpp = ctx->current_mode.bpp;
        info->refresh_rate = ctx->current_mode.refresh_rate;
    }
}

void vix_kernel_draw_line(int x1, int y1, int x2, int y2, uint32_t color) {
    Color c = graphics_rgb_to_color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Point start = {x1, y1};
        Point end = {x2, y2};
        graphics_draw_line(ctx->back_buffer, start, end, c);
    }
}

void vix_kernel_draw_circle(int x, int y, int radius, uint32_t color) {
    Color c = graphics_rgb_to_color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Point center = {x, y};
        graphics_draw_circle(ctx->back_buffer, center, radius, c);
    }
}

void vix_kernel_fill_circle(int x, int y, int radius, uint32_t color) {
    Color c = graphics_rgb_to_color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Point center = {x, y};
        graphics_fill_circle(ctx->back_buffer, center, radius, c);
    }
}

void vix_kernel_draw_text(const char *text, int x, int y, uint32_t color) {
    Color c = graphics_rgb_to_color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Point position = {x, y};
        graphics_draw_text(ctx->back_buffer, text, position, c);
    }
}

void vix_kernel_draw_text_scaled(const char *text, int x, int y, uint32_t color, int scale) {
    Color c = graphics_rgb_to_color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    GraphicsContext *ctx = graphics_get_context();
    if (ctx && ctx->back_buffer) {
        Point position = {x, y};
        graphics_draw_text_scaled(ctx->back_buffer, text, position, c, scale);
    }
}

int vix_kernel_text_width(const char *text, int scale) {
    return graphics_text_width(text, scale);
}

int vix_kernel_text_height(int scale) {
    return graphics_text_height(scale);
}
