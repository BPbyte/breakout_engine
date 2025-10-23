/* for initialization and gameplay loop /src/main.cpp */

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include "util/Texture.h"
#include "systems/SpriteRenderer.h"
#include "objects/Ball.h"
#include "objects/Paddle.h"
#include "systems/Game.h"
#include "objects/Brick.h"
#include "objects/Powerup.h"

// Function to generate bricks
void GenerateBricks(std::vector<Brick>& bricks, std::mt19937& gen) {
    bricks.clear(); // Clear existing bricks

    // Procedural Brick Generation
    std::uniform_int_distribution<> durabilityDistribution(1, 3);
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 6; col++) {
            bool isGap = std::uniform_real_distribution<>(0, 1)(gen) < 0.2;
            if (!isGap) {
            int durability = durabilityDistribution(gen);
            int scoreValue = durability * 10;
            bricks.emplace_back(100 + col*120, 100 + row*40, 80, 20, durability, scoreValue);}
        }
    }
}

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
    Texture brickTexture1("/home/breki/Projects/MyEngine/assets/sprites/brick1.png"); 
    Texture brickTexture2("/home/breki/Projects/MyEngine/assets/sprites/brick2.png"); 
    Texture brickTexture3("/home/breki/Projects/MyEngine/assets/sprites/brick3.png"); 
    Texture powerupTexture("/home/breki/Projects/MyEngine/assets/sprites/powerup.png");
    SpriteRenderer renderer;

    // Game Objects
    Paddle paddle = {400, 500, 64, 20, 0, 64}; 
    Ball ball = {400, 300, 200, -200, 8}; 
    Game game = {0, 3};
    std::vector<Brick> bricks;
    std::vector<Powerup> powerups;

    // Procedural Brick Generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dropChance(0, 1);
    GenerateBricks(bricks, gen);

    // Game Loop
    bool running = true;
    bool gameOver = false;
    float deltaTime = 0.016f; 
    while (running) {
        // Handle Events
        SDL_Event e;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r && gameOver) {
                game.Reset();
                ball = {400, 300, 200, -200, 8};
                paddle = {400, 500, 64, 20, 0, 64};
                bricks.clear();
                powerups.clear();
                GenerateBricks(bricks, gen);
                gameOver = false;
            }
        }
        
        // Update Physics (if not game over)
        if (!gameOver) {
            paddle.Update(deltaTime);
            ball.Update(deltaTime, paddle.x, paddle.y, paddle.w, paddle.h, game, gameOver);
            for (auto& brick : bricks) {
                if (brick.active && brick.CheckCollision(ball.x, ball.y, ball.radius)) {
                    ball.vy = -ball.vy;
                    game.score += brick.scoreValue;
                    std::cerr << "Score: " << game.score << std::endl;
                    // 5% chance to drop powerup
                    if (dropChance(gen) < 0.05) {
                        Powerup::Type type = (dropChance(gen) < 0.74) ? Powerup::PADDLE_SIZE : Powerup::EXTRA_LIFE;
                        powerups.emplace_back(brick.x, brick.y, 16, 16, type);
                    }
                }
            }
            // Update powerups
            for (auto& powerup : powerups) {
                if (powerup.active) {
                    powerup.Update(deltaTime, paddle.x, paddle.y, paddle.w, paddle.h);
                    // Apply powerup effect if collected
                    if (!powerup.active && powerup.collected) {
                        if (powerup.type == Powerup::PADDLE_SIZE) {
                            paddle.SetWidth(paddle.w * 1.5); // Increase paddle width by 50%
                            paddle.SetDuration(375); 
                        } else if (powerup.type == Powerup::EXTRA_LIFE) {
                            game.lives += 1;
                            std::cerr << "Extra life gained! Lives: " << game.lives << std::endl;
                        }
                    }
                }
            }
            // Remove inactive powerups
            powerups.erase(
                std::remove_if(powerups.begin(), powerups.end(), [](const Powerup& p) { return !p.active; }),
                powerups.end()
            );
        }
        
        // Rendering
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        renderer.DrawSprite(paddleTexture, paddle.x, paddle.y, paddle.w, paddle.h);

        renderer.DrawSprite(ballTexture, ball.x, ball.y, 16, 16);
        for (auto& brick : bricks) {
            if (brick.active) {
                // Select texture based on durability
                Texture& brickTexture = (brick.durability == 3) ? brickTexture3 :
                                       (brick.durability == 2) ? brickTexture2 : brickTexture1;
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