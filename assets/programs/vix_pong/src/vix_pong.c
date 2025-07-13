#include "vios.h"
#include "stdio.h"

// Pong game constants
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define PADDLE_WIDTH 15
#define PADDLE_HEIGHT 80
#define BALL_SIZE 12
#define PADDLE_SPEED 4
#define BALL_SPEED_X 3
#define BALL_SPEED_Y 2
#define SCORE_TARGET 10

// Game state
typedef struct {
    // Ball
    int ball_x, ball_y;
    int ball_dx, ball_dy;
    
    // Paddles
    int paddle1_y, paddle2_y;
    
    // Scores
    int score1, score2;
    
    // Game timing
    int frame_count;
    int game_over;
    int winner;
} PongGame;

static PongGame game;

void init_pong_game(void) {
    // Initialize ball in center
    game.ball_x = SCREEN_WIDTH / 2;
    game.ball_y = SCREEN_HEIGHT / 2;
    game.ball_dx = BALL_SPEED_X;
    game.ball_dy = BALL_SPEED_Y;
    
    // Initialize paddles in center
    game.paddle1_y = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;
    game.paddle2_y = (SCREEN_HEIGHT - PADDLE_HEIGHT) / 2;
    
    // Initialize scores
    game.score1 = 0;
    game.score2 = 0;
    game.frame_count = 0;
    game.game_over = 0;
    game.winner = 0;
}

void update_ai_paddles(void) {
    // Simple AI for both paddles
    int paddle1_center = game.paddle1_y + PADDLE_HEIGHT / 2;
    int paddle2_center = game.paddle2_y + PADDLE_HEIGHT / 2;
    
    // Paddle 1 (left) AI - follows ball with some delay
    if (game.ball_y < paddle1_center - 10) {
        game.paddle1_y -= PADDLE_SPEED;
    } else if (game.ball_y > paddle1_center + 10) {
        game.paddle1_y += PADDLE_SPEED;
    }
    
    // Paddle 2 (right) AI - slightly less perfect
    if (game.frame_count % 2 == 0) { // Move every other frame (slower)
        if (game.ball_y < paddle2_center - 15) {
            game.paddle2_y -= PADDLE_SPEED;
        } else if (game.ball_y > paddle2_center + 15) {
            game.paddle2_y += PADDLE_SPEED;
        }
    }
    
    // Keep paddles on screen
    if (game.paddle1_y < 0) game.paddle1_y = 0;
    if (game.paddle1_y > SCREEN_HEIGHT - PADDLE_HEIGHT) 
        game.paddle1_y = SCREEN_HEIGHT - PADDLE_HEIGHT;
    
    if (game.paddle2_y < 0) game.paddle2_y = 0;
    if (game.paddle2_y > SCREEN_HEIGHT - PADDLE_HEIGHT) 
        game.paddle2_y = SCREEN_HEIGHT - PADDLE_HEIGHT;
}

void update_ball(void) {
    // Move ball
    game.ball_x += game.ball_dx;
    game.ball_y += game.ball_dy;
    
    // Ball collision with top/bottom walls
    if (game.ball_y <= 0 || game.ball_y >= SCREEN_HEIGHT - BALL_SIZE) {
        game.ball_dy = -game.ball_dy;
    }
    
    // Ball collision with left paddle
    if (game.ball_x <= 30 + PADDLE_WIDTH && 
        game.ball_x >= 30 &&
        game.ball_y >= game.paddle1_y && 
        game.ball_y <= game.paddle1_y + PADDLE_HEIGHT) {
        game.ball_dx = -game.ball_dx;
        // Add some angle based on where it hits the paddle
        int hit_pos = (game.ball_y - game.paddle1_y) - PADDLE_HEIGHT/2;
        game.ball_dy = hit_pos / 8;
    }
    
    // Ball collision with right paddle
    if (game.ball_x >= SCREEN_WIDTH - 30 - PADDLE_WIDTH - BALL_SIZE && 
        game.ball_x <= SCREEN_WIDTH - 30 &&
        game.ball_y >= game.paddle2_y && 
        game.ball_y <= game.paddle2_y + PADDLE_HEIGHT) {
        game.ball_dx = -game.ball_dx;
        // Add some angle
        int hit_pos = (game.ball_y - game.paddle2_y) - PADDLE_HEIGHT/2;
        game.ball_dy = hit_pos / 8;
    }
    
    // Ball goes off left side - Player 2 scores
    if (game.ball_x < 0) {
        game.score2++;
        game.ball_x = SCREEN_WIDTH / 2;
        game.ball_y = SCREEN_HEIGHT / 2;
        game.ball_dx = BALL_SPEED_X;
        game.ball_dy = (game.frame_count % 2 == 0) ? BALL_SPEED_Y : -BALL_SPEED_Y;
    }
    
    // Ball goes off right side - Player 1 scores
    if (game.ball_x > SCREEN_WIDTH) {
        game.score1++;
        game.ball_x = SCREEN_WIDTH / 2;
        game.ball_y = SCREEN_HEIGHT / 2;
        game.ball_dx = -BALL_SPEED_X;
        game.ball_dy = (game.frame_count % 2 == 0) ? BALL_SPEED_Y : -BALL_SPEED_Y;
    }
    
    // Check for game over
    if (game.score1 >= SCORE_TARGET) {
        game.game_over = 1;
        game.winner = 1;
    } else if (game.score2 >= SCORE_TARGET) {
        game.game_over = 1;
        game.winner = 2;
    }
}

void draw_dashed_line(void) {
    // Draw center dashed line
    for (int y = 0; y < SCREEN_HEIGHT; y += 20) {
        vix_fill_rect(SCREEN_WIDTH/2 - 2, y, 4, 10, VIX_COLOR_WHITE);
    }
}

void draw_pong_game(void) {
    // Clear screen
    vix_clear_screen(VIX_COLOR_BLACK);
    
    // Draw center line
    draw_dashed_line();
    
    // Draw paddles
    vix_fill_rect(30, game.paddle1_y, PADDLE_WIDTH, PADDLE_HEIGHT, VIX_COLOR_WHITE);
    vix_fill_rect(SCREEN_WIDTH - 30 - PADDLE_WIDTH, game.paddle2_y, PADDLE_WIDTH, PADDLE_HEIGHT, VIX_COLOR_WHITE);
    
    // Draw ball
    vix_fill_rect(game.ball_x, game.ball_y, BALL_SIZE, BALL_SIZE, VIX_COLOR_WHITE);
    
    // Draw scores
    char score_text[32];
    // Player 1 score
    if (game.score1 < 10) {
        score_text[0] = '0' + game.score1;
        score_text[1] = '\0';
    } else {
        score_text[0] = '1';
        score_text[1] = '0' + (game.score1 - 10);
        score_text[2] = '\0';
    }
    vix_draw_text_scaled(score_text, SCREEN_WIDTH/2 - 100, 50, VIX_COLOR_WHITE, 4);
    
    // Player 2 score
    if (game.score2 < 10) {
        score_text[0] = '0' + game.score2;
        score_text[1] = '\0';
    } else {
        score_text[0] = '1';
        score_text[1] = '0' + (game.score2 - 10);
        score_text[2] = '\0';
    }
    vix_draw_text_scaled(score_text, SCREEN_WIDTH/2 + 50, 50, VIX_COLOR_WHITE, 4);
    
    // Draw title
    vix_draw_text_scaled("VIX PONG DEMO", SCREEN_WIDTH/2 - 160, 10, VIX_COLOR_CYAN, 2);
    
    // Draw game over message
    if (game.game_over) {
        if (game.winner == 1) {
            vix_draw_text_scaled("LEFT PLAYER WINS!", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT/2, VIX_COLOR_GREEN, 3);
        } else {
            vix_draw_text_scaled("RIGHT PLAYER WINS!", SCREEN_WIDTH/2 - 210, SCREEN_HEIGHT/2, VIX_COLOR_GREEN, 3);
        }
        vix_draw_text_scaled("Game will restart in 3 seconds...", SCREEN_WIDTH/2 - 300, SCREEN_HEIGHT/2 + 60, VIX_COLOR_YELLOW, 2);
    }
    
    // Draw instructions
    vix_draw_text("Auto-playing AI vs AI Pong - First to 10 wins!", 50, SCREEN_HEIGHT - 40, VIX_COLOR_YELLOW);
    vix_draw_text("VIX Graphics System Demo", 50, SCREEN_HEIGHT - 20, VIX_RGB(150, 150, 150));
}

int main(int argc, char** argv) {
    vix_screen_info_t screen_info;
    vix_get_screen_info(&screen_info);
    
    init_pong_game();
    
    int restart_timer = 0;
    
    while (1) {
        // Update game logic
        if (!game.game_over) {
            update_ai_paddles();
            update_ball();
        } else {
            // Handle game over and restart
            restart_timer++;
            if (restart_timer > 180) { // About 3 seconds at 60 FPS
                init_pong_game();
                restart_timer = 0;
            }
        }
        
        // Draw everything
        draw_pong_game();
        
        // Present frame
        vix_present_frame();
        
        // Simple frame delay (rough 60 FPS)
        for (volatile int i = 0; i < 100000; i++);
        
        game.frame_count++;
        
        // Reset after playing for a very long time to prevent overflow
        if (game.frame_count > 100000) {
            init_pong_game();
        }
    }
    
    return 0;
}
