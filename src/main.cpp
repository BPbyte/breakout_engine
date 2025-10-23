/* for initalization and gameplay loop /src/main.cpp*/

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <random>
#include "util/Texture.h"
#include "systems/SpriteRenderer.h"
#include "objects/Ball.h"
#include "objects/Paddle.h"
#include "systems/Game.h"
#include "objects/Brick.h"
#include "objects/Powerup.h"

int main() {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("MyEngine", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);

    // Create Viewport
    SDL_GL_CreateContext(window);
    glViewport(0, 0, 800, 600);  
    glOrtho(0, 800, 600, 0, -1, 1); 
    glEnable(GL_TEXTURE_2D);
    
    // Load Textures
    Texture paddleTexture("/home/breki/Projects/MyEngine/assets/sprites/paddle.png");
    Texture ballTexture("/home/breki/Projects/MyEngine/assets/sprites/ball.png");
    Texture brickTexture("/home/breki/Projects/MyEngine/assets/sprites/brick.png");
    Texture powerupTexture("/home/breki/Projects/MyEngine/assets/sprites/powerup.png");
    SpriteRenderer renderer;

    // Game Objects --Memory allocated here
    Paddle paddle = {400, 500, 64, 32};  // Bottom, 64x32
    Ball ball = {400, 300, 200, -200, 8}; // Center, 16x16
    Game game = {0, 3};
    // memory consuimg items
    std::vector<Brick> bricks;
    std::vector<Powerup> powerups;

    // Procedural Brick Generation (5x3 grid)
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dropChance(0, 1);
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 5; col++) {
            int durability = (row == 0) ? 3 : (row == 1) ? 2 : 1; // Top row = 3 hits, middle = 2, bottom = 1
            int scoreValue = durability * 10; // 10, 20, or 30 points
            bricks.emplace_back(100 + col*120, 100 + row*40, 80, 20, durability, scoreValue);
        }
    }

    // Game Loop
    bool running = true;
    bool gameOver = false;
    float deltaTime = 0.016f;  // ~60 FPS
    while (running) {
        // Handle Events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r && gameOver) {
                game.Reset();
                ball = {400, 300, 200, -200, 8};
                paddle = {400, 500, 64, 32};
                bricks.clear();
                powerups.clear();
                for (int row = 0; row < 3; row++) {
                    for (int col = 0; col < 5; col++) {
                        int durability = (row == 0) ? 3 : (row == 1) ? 2 : 1;
                        int scoreValue = durability * 10;
                        // emplace_back allocated memory dynamically
                        bricks.emplace_back(100 + col*120, 100 + row*40, 80, 20, durability, scoreValue);
                    }
                }
                gameOver = false;
            }
        }
        
        // Update Physics (if not game over)
        if (!gameOver) {
            paddle.Update(deltaTime);
            ball.Update(deltaTime, paddle.x, paddle.y, paddle.w, paddle.h, game, gameOver);
            for (auto& brick : bricks) {
                if (brick.CheckCollision(ball.x, ball.y, ball.radius)) {
                    ball.vy = -ball.vy;
                    game.score += brick.scoreValue;
                    std::cerr << "Score: " << game.score << std::endl;
                    // 20% chance to drop powerup
                    if (dropChance(gen) < 0.2) {
                        Powerup::Type type = (dropChance(gen) < 0.5) ? Powerup::PADDLE_SIZE : Powerup::EXTRA_LIFE;
                        powerups.emplace_back(brick.x, brick.y, 16, 16, type);
                    }
                }
            }
        }
        
        // Rendering
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        renderer.DrawSprite(paddleTexture, paddle.x, paddle.y, paddle.w, paddle.h);
        renderer.DrawSprite(ballTexture, ball.x, ball.y, 16, 16);
        for (auto& brick : bricks) {
            if (brick.active) {
                renderer.DrawSprite(brickTexture, brick.x, brick.y, brick.w, brick.h);
            }
        }
        for (auto& powerup : powerups) {
            if (powerup.active) {
                renderer.DrawSprite(powerupTexture, powerup.x, powerup.y, powerup.w, powerup.h);
            }
        }
        
        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }
    
    // Destroy Session
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}