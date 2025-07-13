#include "mainloop.h"
#include "../graphics/graphics.h"
#include "../graphics/renderer.h"
#include "../graphics/vix_kernel.h"
#include "../mouse/mouse.h"
#include "../task/process.h"
#include "../keyboard/keyboard.h"
#include "../string/string.h"

void kernel_run_main_loop(struct mouse *mouse)
{
    // Get screen info using VIX
    struct vix_screen_info screen_info;
    vix_kernel_get_screen_info(&screen_info);
    
    int animation_counter = 0;
    
    while (1)
    {
        // Clear screen to dark blue using VIX
        vix_kernel_clear_screen(VIX_RGB(20, 20, 60));
        
        // Draw VIX title
        vix_kernel_draw_text_scaled("VIX Graphics System Demo", 50, 20, VIX_COLOR_WHITE, 2);
        vix_kernel_draw_text("Kernel-Level VIX Graphics API in Action", 50, 50, VIX_COLOR_CYAN);
        
        // Draw screen info
        char info_buffer[128];
        // Simple string building since we don't have sprintf
        info_buffer[0] = 'S';
        info_buffer[1] = 'c';
        info_buffer[2] = 'r';
        info_buffer[3] = 'e';
        info_buffer[4] = 'e';
        info_buffer[5] = 'n';
        info_buffer[6] = ':';
        info_buffer[7] = ' ';
        info_buffer[8] = '1';
        info_buffer[9] = '0';
        info_buffer[10] = '2';
        info_buffer[11] = '4';
        info_buffer[12] = 'x';
        info_buffer[13] = '7';
        info_buffer[14] = '6';
        info_buffer[15] = '8';
        info_buffer[16] = '\0';
        vix_kernel_draw_text(info_buffer, 50, 80, VIX_COLOR_YELLOW);
        
        // Draw animated rectangles using VIX
        int rect_x = 50 + (animation_counter % 200);
        vix_kernel_fill_rect(rect_x, 120, 60, 40, VIX_COLOR_RED);
        vix_kernel_draw_text("Red", rect_x + 20, 135, VIX_COLOR_WHITE);
        
        int rect_size = 20 + (animation_counter % 40);
        vix_kernel_fill_rect(300, 120, rect_size, rect_size, VIX_COLOR_GREEN);
        vix_kernel_draw_text("Green", 300, 175, VIX_COLOR_WHITE);
        
        // Draw animated circles using VIX
        int circle_radius = 15 + (animation_counter % 20);
        vix_kernel_fill_circle(100, 250, circle_radius, VIX_COLOR_YELLOW);
        vix_kernel_draw_text("Yellow Circle", 60, 290, VIX_COLOR_WHITE);
        
        vix_kernel_fill_circle(250, 250, 25, VIX_COLOR_MAGENTA);
        vix_kernel_draw_text("Magenta Circle", 210, 290, VIX_COLOR_WHITE);
        
        // Draw lines using VIX
        vix_kernel_draw_line(50, 350, 400, 350, VIX_COLOR_CYAN);
        vix_kernel_draw_line(225, 320, 225, 380, VIX_COLOR_CYAN);
        vix_kernel_draw_text("VIX Lines", 260, 360, VIX_COLOR_WHITE);
        
        // Draw outline rectangles using VIX
        vix_kernel_draw_rect(450, 120, 120, 80, VIX_COLOR_WHITE);
        vix_kernel_draw_text("Outline", 490, 150, VIX_COLOR_WHITE);
        vix_kernel_draw_text("Rectangle", 480, 165, VIX_COLOR_WHITE);
        
        // Draw scaling demonstration
        vix_kernel_draw_text("Text Scaling:", 450, 220, VIX_COLOR_YELLOW);
        vix_kernel_draw_text("Scale 1", 450, 240, VIX_COLOR_WHITE);
        vix_kernel_draw_text_scaled("Scale 2", 450, 260, VIX_COLOR_GREEN, 2);
        vix_kernel_draw_text_scaled("Scale 3", 450, 290, VIX_COLOR_CYAN, 3);
        
        // Draw pixel art section using VIX
        vix_kernel_draw_text("Pixel Art:", 450, 350, VIX_COLOR_YELLOW);
        for (int i = 0; i < 30; i++) {
            for (int j = 0; j < 30; j++) {
                uint32_t color = VIX_RGB((i * 8) % 256, (j * 8) % 256, ((i + j) * 5) % 256);
                vix_kernel_draw_pixel(450 + i, 370 + j, color);
            }
        }
        
        // Draw mouse cursor using VIX
        vix_kernel_fill_rect(mouse->x, mouse->y, 12, 12, VIX_COLOR_WHITE);
        vix_kernel_fill_rect(mouse->x + 2, mouse->y + 2, 8, 8, VIX_COLOR_BLACK);
        vix_kernel_draw_text("Mouse", mouse->x + 15, mouse->y, VIX_COLOR_WHITE);
        
        // Draw status info
        vix_kernel_draw_text("ViOS VIX Graphics System - Kernel Level Demo", 50, screen_info.height - 40, VIX_COLOR_YELLOW);
        vix_kernel_draw_text("All graphics rendered through VIX API", 50, screen_info.height - 20, VIX_RGB(150, 150, 150));
        
        // Present the frame using VIX
        vix_kernel_present_frame();
        
        animation_counter++;
        
        // Simple frame delay
        for (volatile int i = 0; i < 50000; i++);
    }
}
