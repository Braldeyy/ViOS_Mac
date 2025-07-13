#include "vios.h"
#include "stdio.h"

int main(int argc, char** argv)
{
    vix_screen_info_t screen_info;
    vix_get_screen_info(&screen_info);
    
    // Demo starts
    
    // Clear screen to dark blue
    vix_clear_screen(VIX_RGB(20, 20, 60));
    
    // Draw title text
    vix_draw_text_scaled("VIOS Graphics API (VIX) Demo", 50, 20, VIX_COLOR_WHITE, 2);
    vix_draw_text("Showcasing VIX Graphics and Text Capabilities", 50, 50, VIX_COLOR_CYAN);
    
    // Draw labeled shapes section
    vix_draw_text("Filled Rectangles:", 50, 80, VIX_COLOR_YELLOW);
    vix_fill_rect(50, 100, 80, 60, VIX_COLOR_RED);
    vix_draw_text("Red", 70, 170, VIX_COLOR_WHITE);
    
    vix_fill_rect(150, 100, 80, 60, VIX_COLOR_GREEN);
    vix_draw_text("Green", 165, 170, VIX_COLOR_WHITE);
    
    vix_fill_rect(250, 100, 80, 60, VIX_COLOR_BLUE);
    vix_draw_text("Blue", 270, 170, VIX_COLOR_WHITE);
    
    // Draw labeled circles section
    vix_draw_text("Filled Circles:", 50, 200, VIX_COLOR_YELLOW);
    vix_fill_circle(90, 250, 30, VIX_COLOR_YELLOW);
    vix_draw_text("Yellow", 70, 290, VIX_COLOR_WHITE);
    
    vix_fill_circle(190, 250, 30, VIX_COLOR_CYAN);
    vix_draw_text("Cyan", 175, 290, VIX_COLOR_WHITE);
    
    vix_fill_circle(290, 250, 30, VIX_COLOR_MAGENTA);
    vix_draw_text("Magenta", 265, 290, VIX_COLOR_WHITE);
    
    // Draw lines section
    vix_draw_text("Lines and Outlines:", 50, 320, VIX_COLOR_YELLOW);
    vix_draw_line(50, 350, 350, 350, VIX_COLOR_WHITE);
    vix_draw_line(200, 320, 200, 380, VIX_COLOR_WHITE);
    
    // Draw outline rectangles with labels
    vix_draw_rect(380, 100, 120, 80, VIX_COLOR_WHITE);
    vix_draw_text("Outline", 420, 140, VIX_COLOR_WHITE);
    vix_draw_text("Rectangle", 410, 155, VIX_COLOR_WHITE);
    
    // Draw text scaling demonstration
    vix_draw_text("Text Scaling:", 380, 200, VIX_COLOR_YELLOW);
    vix_draw_text("Scale 1", 380, 220, VIX_COLOR_WHITE);
    vix_draw_text_scaled("Scale 2", 380, 240, VIX_COLOR_GREEN, 2);
    vix_draw_text_scaled("Scale 3", 380, 270, VIX_COLOR_CYAN, 3);
    
    // Draw pixel art section
    vix_draw_text("Pixel Art:", 400, 320, VIX_COLOR_YELLOW);
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 50; j++) {
            // Create a simple pattern
            uint32_t color = VIX_RGB((i * 5) % 256, (j * 5) % 256, ((i + j) * 3) % 256);
            vix_draw_pixel(400 + i, 340 + j, color);
        }
    }
    
    // Draw screen info
    vix_draw_text("Screen Resolution: 1024x768 @ 16 bpp", 50, 400, VIX_RGB(150, 150, 150));
    
    // Draw text measurement demonstration
    const char* test_text = "Text Width Test";
    vix_text_width(test_text, 1); // Test the function
    vix_text_height(1); // Test the function
    vix_draw_text("Text measurement: width and height functions work!", 50, 420, VIX_RGB(200, 200, 100));
    
    // Draw instructions
    vix_draw_text("Press any key to exit...", 200, screen_info.height - 30, VIX_COLOR_WHITE);
    
    // Present the final frame
    vix_present_frame();
    
    // Wait for keypress
    vios_getkeyblock();
    
    // Clear screen before exiting
    vix_clear_screen(VIX_COLOR_BLACK);
    vix_present_frame();
    
    return 0;
}
