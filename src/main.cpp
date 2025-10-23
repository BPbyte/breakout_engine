/* for initialization and gameplay loop /src/main.cpp */

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <string>
#include <stb_image.h>  
#include "util/Texture.h"
#include "systems/SpriteRenderer.h"
#include "objects/Ball.h"
#include "objects/Paddle.h"
#include "systems/Game.h"
#include "objects/Brick.h"
#include "objects/Powerup.h"
#include "systems/Menu.h"


// Function to generate bricks
void GenerateBricks(std::vector<Brick>& bricks, std::mt19937& gen) {
    bricks.clear();
    std::uniform_int_distribution<> durabilityDistribution(1, 3);
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 6; col++) {
            bool isGap = std::uniform_real_distribution<>(0, 1)(gen) < 0.2;
            if (!isGap) {
                int durability = durabilityDistribution(gen);
                int scoreValue = durability * 10;
                bricks.emplace_back(100 + col*120, 100 + row*40, 80, 20, durability, scoreValue);
            }
        }
    }
}

int main() {
    // Set stb_image flip globally before any loading
    stbi_set_flip_vertically_on_load(true);

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
    std::vector<Texture> digitTextures;
    for (int i = 0; i < 10; ++i) {
        std::string path = "/home/breki/Projects/MyEngine/assets/sprites/" + std::to_string(i) + ".png";
        digitTextures.emplace_back(path.c_str());
        // std::cerr << "Loaded digit texture " << i << ": " << path << std::endl; // Debug
    }
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

    // Menu
    Menu menu;

    // Game Loop
    bool running = true;
    float deltaTime = 0.016f; 
    while (running) {
        // Handle Events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            menu.Update(e, running);
            if (e.type == SDL_KEYDOWN && menu.state == PLAYING && e.key.keysym.sym == SDLK_r) {
                game.Reset();
                ball = {400, 300, 200, -200, 8};
                paddle = {400, 500, 64, 20, 0, 64};
                bricks.clear();
                powerups.clear();
                GenerateBricks(bricks, gen);
            }
        }
        
        // Update Physics (only if playing)
        if (menu.state == PLAYING) {
            paddle.Update(deltaTime);
            ball.Update(deltaTime, paddle.x, paddle.y, paddle.w, paddle.h, game);
            for (auto& brick : bricks) {
                if (brick.active && brick.CheckCollision(ball.x, ball.y, ball.radius)) {
                    ball.vy = -ball.vy;
                    game.score += brick.scoreValue;
                    std::cerr << "Score: " << game.score << std::endl;
                    if (dropChance(gen) < 0.05) {
                        Powerup::Type type = (dropChance(gen) < 0.74) ? Powerup::PADDLE_SIZE : Powerup::EXTRA_LIFE;
                        powerups.emplace_back(brick.x, brick.y, 16, 16, type);
                    }
                }
            }
            for (auto& powerup : powerups) {
                if (powerup.active) {
                    powerup.Update(deltaTime, paddle.x, paddle.y, paddle.w, paddle.h);
                    if (!powerup.active && powerup.collected) {
                        if (powerup.type == Powerup::PADDLE_SIZE) {
                            paddle.SetWidth(paddle.w * 1.5);
                            paddle.SetDuration(375); 
                        } else if (powerup.type == Powerup::EXTRA_LIFE) {
                            game.lives += 1;
                            std::cerr << "Extra life gained! Lives: " << game.lives << std::endl;
                        }
                    }
                }
            }
            powerups.erase(
                std::remove_if(powerups.begin(), powerups.end(), [](const Powerup& p) { return !p.active; }),
                powerups.end()
            );

            if (game.lives <= 0) {
                menu.state = GAME_OVER;
            }
        }
        
        // Rendering
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        if (menu.state == PLAYING) {
            renderer.DrawSprite(paddleTexture, paddle.x, paddle.y, paddle.w, paddle.h);
            renderer.DrawSprite(ballTexture, ball.x, ball.y, 16, 16);
            for (auto& brick : bricks) {
                if (brick.active) {
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
            // Draw score and lives (scaled up)
            renderer.DrawNumber(digitTextures.data(), game.score, 10, 20, 24.0f, 24.0f);  
            renderer.DrawNumber(digitTextures.data(), game.lives, 740, 20, 24.0f, 24.0f);  
        }
        
        menu.Render(renderer);  // Draws menu or game over screen
        
        SDL_GL_SwapWindow(window);
        SDL_Delay(16);
    }
    
    // Destroy Session
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}