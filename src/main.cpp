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
#include "objects/Bullet.h"
#include "systems/Menu.h"


// Function to generate bricks
void GenerateBricks(std::vector<Brick>& bricks, std::mt19937& gen, int wave) {
    bricks.clear();
    std::uniform_int_distribution<> durabilityDistribution(1 + (wave / 3), 3 + (wave / 5));  // e.g., wave 1: 1-3, wave 6: 2-4
    int rows = std::min(8 + (wave / 2), 12);  // Start at 8, +1 every 2 waves, cap 12
    float gapChance = 0.2f - (wave * 0.01f);  // Fewer gaps as waves increase (min 0.1)
    if (gapChance < 0.1f) gapChance = 0.1f;
    std::uniform_real_distribution<> gapDist(0, 1);
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < 6; col++) {
            bool isGap = gapDist(gen) < gapChance;
            if (!isGap) {
                int durability = durabilityDistribution(gen);
                int scoreValue = durability * 10 * (1 + (wave / 10));  // Slight score scaling
                bricks.emplace_back(100 + col*120, 100 + row*40, 80, 20, durability, scoreValue);
            }
        }
    }
    std::cerr << "Wave " << wave << " generated: " << rows << " rows, " << bricks.size() << " bricks" << std::endl;
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
    Texture BlasterTexture("/home/breki/Projects/MyEngine/assets/sprites/blaster.png");
    Texture MeteorTexture("/home/breki/Projects/MyEngine/assets/sprites/meteor.png");

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
    Game game = {0, 6};
    std::vector<Brick> bricks;
    std::vector<Powerup> powerups;

    // Procedural Brick Generation
    std::random_device rd;
    std::mt19937 gen(rd());
    int wave = 1;
    std::uniform_real_distribution<> dropChance(0, 1);
    GenerateBricks(bricks, gen, wave);

    // Menu
    Menu menu;

    // Game Loop
    bool running = true;
    float deltaTime = 0.016f; 
    static Uint32 lastFireTime = 0;
    while (running) {
        // Handle Events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            menu.Update(e, running);
            if (e.type == SDL_KEYDOWN) {
                if (menu.state == PLAYING && e.key.keysym.sym == SDLK_r) {
                    game.Reset();
                    ball = {400, 300, 200, -200, 8};
                    paddle = {400, 500, 64, 20, 0, 64};
                    bricks.clear();
                    powerups.clear();
                    wave = 1;
                    GenerateBricks(bricks, gen, wave);
                } else if (menu.state == PLAYING && e.key.keysym.sym == SDLK_SPACE && game.blasterActive) {
                    Uint32 currentTime = SDL_GetTicks();
                    if (currentTime - lastFireTime > 200) {
                        float bulletY = paddle.y - paddle.h / 2 - 4.0f;
                        game.bullets.emplace_back(paddle.x, bulletY, 0.0f, -300.0f);
                        std::cerr << "Blaster fired!" << std::endl;
                        lastFireTime = currentTime;
                    }
                }
            }
        }
        
        // Update Physics (only if playing)
        if (menu.state == PLAYING) {
            paddle.Update(deltaTime);
            ball.Update(deltaTime, paddle, game);
            for (auto& powerup : powerups) {
                if (powerup.active) {
                    powerup.Update(deltaTime, paddle, ball, game);
                }
            }
            // Apply collected powerups
            for (size_t i = 0; i < powerups.size(); ++i) {
                auto& powerup = powerups[i];
                if (!powerup.active && powerup.collected) {
                    switch (powerup.type) {
                        case Powerup::PADDLE_SIZE:
                            if (game.bigPaddleRemainingHits == 0) {
                                paddle.originalWidth = paddle.w;
                                paddle.w *= 1.5f;
                                std::cerr << "Big paddle activated! 3 hits remaining." << std::endl;
                            }
                            game.bigPaddleRemainingHits = 3;
                            break;
                        case Powerup::EXTRA_LIFE:
                            game.lives += 1;
                            std::cerr << "Extra life gained! Lives: " << game.lives << std::endl;
                            break;
                        case Powerup::METEOR:
                            game.meteorActive = true;
                            game.meteorDuration = 300.0f;
                            std::cerr << "Meteor activated! Duration: 5s" << std::endl;
                            break;
                        case Powerup::BIG_BALL:
                            if (!game.bigBallActive) {
                                ball.radius *= 2.0f;
                                std::cerr << "Big ball activated! Radius doubled." << std::endl;
                            }
                            game.bigBallActive = true;
                            game.bigBallDuration = 300.0f;
                            break;
                        case Powerup::BLASTER:
                            game.blasterActive = true;
                            game.blasterDuration = 300.0f;
                            std::cerr << "Blaster activated! Duration: 5s" << std::endl;
                            break;
                    }
                    powerup.collected = false;
                }
            }
            powerups.erase(
                std::remove_if(powerups.begin(), powerups.end(), [](const Powerup& p) { return !p.active; }),
                powerups.end()
            );
            // Brick collisions
            float effectiveRadius = game.bigBallActive ? ball.radius * 2.0f : ball.radius;
            for (auto& brick : bricks) {
                if (brick.active && brick.IsColliding(ball.x, ball.y, effectiveRadius)) {
                    bool destroy = false;
                    bool bounce = true;
                    if (game.meteorActive) {
                        destroy = true;
                        bounce = false;
                    } else if (game.bigBallActive) {
                        destroy = true;
                    } else {
                        brick.durability--;
                        if (brick.durability <= 0) destroy = true;
                    }
                    if (destroy) {
                        brick.active = false;
                        game.score += brick.scoreValue;
                        std::cerr << "Brick destroyed! Score: " << game.score << std::endl;
                        if (dropChance(gen) < 0.05) { //powerup chance
                            float subtype = dropChance(gen);
                            Powerup::Type type;
                            if (subtype < 0.5f) type = Powerup::PADDLE_SIZE;
                            else if (subtype < 0.7f) type = Powerup::EXTRA_LIFE;
                            else if (subtype < 0.8f) type = Powerup::METEOR;
                            else if (subtype < 0.9f) type = Powerup::BIG_BALL;
                            else type = Powerup::BLASTER;
                            powerups.emplace_back(brick.x, brick.y, 16, 16, type);
                        }
                    }
                    if (bounce) ball.vy = -ball.vy;
                }
            }
            // Update bullets
            for (auto& bullet : game.bullets) {
                if (bullet.active) {
                    bullet.y += bullet.vy * deltaTime;
                    if (bullet.y > 600) {
                        bullet.active = false;
                    } else {
                        for (auto& brick : bricks) {
                            if (brick.active && bullet.IsColliding(brick)) {
                                brick.durability--;
                                if (brick.durability <= 0) {
                                    brick.active = false;
                                    game.score += brick.scoreValue;
                                    std::cerr << "Bullet destroyed brick! Score: " << game.score << std::endl;
                                    if (dropChance(gen) < 0.05) {
                                        float subtype = dropChance(gen);
                                        Powerup::Type type;
                                        if (subtype < 0.5f) type = Powerup::PADDLE_SIZE;
                                        else if (subtype < 0.7f) type = Powerup::EXTRA_LIFE;
                                        else if (subtype < 0.8f) type = Powerup::METEOR;
                                        else if (subtype < 0.9f) type = Powerup::BIG_BALL;
                                        else type = Powerup::BLASTER;
                                        powerups.emplace_back(brick.x, brick.y, 16, 16, type);
                                    }
                                }
                                bullet.active = false;
                                break;
                            }
                        }
                    }
                }
            }
            game.bullets.erase(
                std::remove_if(game.bullets.begin(), game.bullets.end(), [](const Bullet& b) { return !b.active; }),
                game.bullets.end()
            );
            // Tick powerup durations
            if (game.meteorDuration > 0) {
                game.meteorDuration -= 1.0f;
                if (game.meteorDuration <= 0) {
                    game.meteorActive = false;
                    std::cerr << "Meteor expired!" << std::endl;
                }
            }
            if (game.bigBallDuration > 0) {
                game.bigBallDuration -= 1.0f;
                if (game.bigBallDuration <= 0 && game.bigBallActive) {
                    ball.radius /= 2.0f;
                    game.bigBallActive = false;
                    std::cerr << "Big ball expired! Radius halved." << std::endl;
                }
            }
            if (game.blasterDuration > 0) {
                game.blasterDuration -= 1.0f;
                if (game.blasterDuration <= 0) {
                    game.blasterActive = false;
                    std::cerr << "Blaster expired!" << std::endl;
                }
            }
            // Check for wave clear
            int activeBricks = Brick::CountActive(bricks);
            if (activeBricks == 0) {
                game.score += 500;  // Clear bonus
                std::cerr << "Wave " << wave << " cleared! Bonus +500. Total score: " << game.score << std::endl;
                wave++;
                game.bullets.clear();
                GenerateBricks(bricks, gen, wave);
            }
            if (game.lives <= 0) {
                menu.state = GAME_OVER;
            }
        }
        
        // Rendering
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        if (menu.state == PLAYING) {
            Texture& activePaddleTex = game.blasterActive ? BlasterTexture : paddleTexture;
            renderer.DrawSprite(activePaddleTex, paddle.x, paddle.y, paddle.w, paddle.h);
            float ballR = game.bigBallActive ? ball.radius * 2.0f : ball.radius;
            Texture& activeBallTex = game.meteorActive ? MeteorTexture : ballTexture;
            renderer.DrawSprite(activeBallTex, ball.x, ball.y, ballR * 2.0f, ballR * 2.0f);
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
            for (const auto& bullet : game.bullets) {
                if (bullet.active) {
                    renderer.DrawSprite(powerupTexture, bullet.x, bullet.y, 8.0f, 8.0f);
                }
            }
            // Draw score and lives (scaled up)
            renderer.DrawNumber(digitTextures.data(), game.score, 10, 20, 24.0f, 24.0f);  
            renderer.DrawNumber(digitTextures.data(), game.lives, 740, 20, 24.0f, 24.0f);  
            renderer.DrawNumber(digitTextures.data(), wave, 370, 20, 24.0f, 24.0f);  // Wave display left of lives
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