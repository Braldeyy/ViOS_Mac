#ifndef VIX_KERNEL_H
#define VIX_KERNEL_H

#include <stdint.h>

// VIX color utilities
#define VIX_RGB(r, g, b) (((uint32_t)(r) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(b))
#define VIX_COLOR_BLACK VIX_RGB(0, 0, 0)
#define VIX_COLOR_WHITE VIX_RGB(255, 255, 255)
#define VIX_COLOR_RED VIX_RGB(255, 0, 0)
#define VIX_COLOR_GREEN VIX_RGB(0, 255, 0)
#define VIX_COLOR_BLUE VIX_RGB(0, 0, 255)
#define VIX_COLOR_YELLOW VIX_RGB(255, 255, 0)
#define VIX_COLOR_CYAN VIX_RGB(0, 255, 255)
#define VIX_COLOR_MAGENTA VIX_RGB(255, 0, 255)

// Screen info structure
struct vix_screen_info {
    int width;
    int height;
    int bpp;
    int refresh_rate;
};

// Kernel-level VIX functions (direct calls, no system calls)
void vix_kernel_draw_pixel(int x, int y, uint32_t color);
void vix_kernel_draw_rect(int x, int y, int width, int height, uint32_t color);
void vix_kernel_fill_rect(int x, int y, int width, int height, uint32_t color);
void vix_kernel_clear_screen(uint32_t color);
void vix_kernel_present_frame(void);
void vix_kernel_get_screen_info(struct vix_screen_info *info);
void vix_kernel_draw_line(int x1, int y1, int x2, int y2, uint32_t color);
void vix_kernel_draw_circle(int x, int y, int radius, uint32_t color);
void vix_kernel_fill_circle(int x, int y, int radius, uint32_t color);
void vix_kernel_draw_text(const char *text, int x, int y, uint32_t color);
void vix_kernel_draw_text_scaled(const char *text, int x, int y, uint32_t color, int scale);
int vix_kernel_text_width(const char *text, int scale);
int vix_kernel_text_height(int scale);

#endif // VIX_KERNEL_H
