/* for initialization and gameplay loop /src/main.cpp */

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <algorithm>
#include <stb_image.h>  
#include <utility>  // For std::pair
#include "util/Texture.h"
#include "systems/SpriteRenderer.h"
#include "objects/Ball.h"
#include "objects/Paddle.h"
#include "systems/Game.h"
#include "objects/Brick.h"
#include "objects/Powerup.h"
#include "objects/Bullet.h"
#include "systems/Menu.h"


void GenerateBricks(std::vector<Brick>& bricks, int wave, const std::string& mode = "random", uint32_t seed = 0) {
    bricks.clear();

    // Seed the local generator (random if seed==0, else use provided for reproducibility)
    uint32_t usedSeed = (seed == 0) ? static_cast<uint32_t>(std::random_device{}()) : seed;
    std::mt19937 gen(usedSeed);

    // Weighted random mode selection if mode == "random"
    std::string selectedMode = mode;
    if (mode == "random") {
        std::vector<std::string> modes = {"default", "pyramid", "cluster", "spiral", "labyrinth"};
        std::vector<double> weights = {0.4, 0.15, 0.15, 0.15, 0.15};  // Default is most common
        std::discrete_distribution<> modeDist(weights.begin(), weights.end());
        int idx = modeDist(gen);
        selectedMode = modes[idx];
        std::cerr << "Randomly selected mode: " << selectedMode << std::endl;
    }

    // Distributions
    std::uniform_int_distribution<> durDist(1 + (wave / 3), 3 + (wave / 5));
    std::uniform_real_distribution<> realDist(0.0, 1.0);

    // Grid setup (rows top-to-bottom, cols left-to-right)
    const int cols = 8;  // Increased to 8 columns
    float screenHeight = 600.0f;  // Hardcode; later pull from Game/SDL
    float topOffset = 100.0f;
    float rowSpacing = 40.0f;
    float bottomBuffer = 100.0f;  // Space for paddle/ball
    int maxRows = static_cast<int>((screenHeight - topOffset - bottomBuffer) / rowSpacing);  // ~10 rows max (100 + 9*40 = 460 < 500)
    int rows = std::min(8 + (wave / 3), maxRows);  // Increased base to 8 rows for wave 1; scales slower to max 10
    std::vector<std::vector<int>> layout(rows, std::vector<int>(cols, 0));  // 0=empty, >0=durability

    // Mode-based generation
    if (selectedMode == "default") {
        // Grid with random gaps (enhanced: dynamic gap chance, spans varied patterns per wave)
        float gapChance = 0.15f - (wave * 0.01f);  // Reduced base gap to 15% for more bricks (~85% density)
        if (gapChance < 0.05f) gapChance = 0.05f;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (realDist(gen) >= gapChance) {
                    layout[r][c] = durDist(gen);
                }
            }
        }
    } else if (selectedMode == "pyramid") {
        // Pyramid: narrow top (row 0: 1 brick), wider bottom; fill extras randomly
        int pyramidRows = std::min(rows, cols);  // Up to cols for wider base
        for (int r = 0; r < pyramidRows; ++r) {
            int width = std::min(r + 1, cols);  // 1 at top, up to cols at bottom
            int startCol = (cols - width) / 2;
            for (int c = 0; c < width; ++c) {
                int col = startCol + c;
                if (col < cols) {
                    layout[r][col] = durDist(gen);
                }
            }
        }
        // Fill remaining rows randomly (higher density: 70%)
        for (int r = pyramidRows; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (std::bernoulli_distribution(0.7)(gen)) {
                    layout[r][c] = durDist(gen);
                }
            }
        }
    } else if (selectedMode == "cluster") {
        // Clusters: 3-5 random dense groups (radius ~2 cells, 70-95% fill) + overall 20% random fill for more bricks
        std::uniform_int_distribution<> numClustersDist(3, 5);
        int numClusters = numClustersDist(gen);
        std::uniform_real_distribution<> clusterProb(0.7, 0.95);  // Increased density
        for (int cl = 0; cl < numClusters; ++cl) {
            int centerR = std::uniform_int_distribution<>(0, rows - 1)(gen);
            int centerC = std::uniform_int_distribution<>(0, cols - 1)(gen);
            for (int dr = -2; dr <= 2; ++dr) {
                for (int dc = -2; dc <= 2; ++dc) {
                    int nr = centerR + dr;
                    int nc = centerC + dc;
                    if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && layout[nr][nc] == 0) {
                        if (std::bernoulli_distribution(clusterProb(gen))(gen)) {
                            layout[nr][nc] = durDist(gen);
                        }
                    }
                }
            }
        }
        // Add some random bricks outside clusters for density
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (layout[r][c] == 0 && std::bernoulli_distribution(0.2)(gen)) {
                    layout[r][c] = durDist(gen);
                }
            }
        }
    } else if (selectedMode == "spiral") {
        // Fixed spiral: layer-by-layer inward spiral, place with 50-80% density
        std::vector<std::pair<int, int>> spiralOrder;
        int layer = 0;
        while (layer * 2 < std::min(rows, cols)) {
            // Top row of layer
            for (int c = layer; c < cols - layer; ++c) {
                spiralOrder.emplace_back(layer, c);
            }
            // Right column of layer (skip top corner)
            for (int r = layer + 1; r < rows - layer; ++r) {
                spiralOrder.emplace_back(r, cols - layer - 1);
            }
            // Bottom row of layer (if not same as top)
            if (rows - layer - 1 > layer) {
                for (int c = cols - layer - 2; c >= layer; --c) {
                    spiralOrder.emplace_back(rows - layer - 1, c);
                }
            }
            // Left column of layer (skip bottom corner)
            if (cols - layer - 1 > layer) {
                for (int r = rows - layer - 2; r > layer; --r) {
                    spiralOrder.emplace_back(r, layer);
                }
            }
            ++layer;
        }
        // Center if odd dimensions (simplified, add if needed)
        if (rows % 2 == 1 && cols % 2 == 1 && layer * 2 == std::min(rows, cols) - 1) {
            spiralOrder.emplace_back(rows / 2, cols / 2);
        }
        std::uniform_real_distribution<> spiralProb(0.5, 0.8);  // Increased density
        for (const auto& [pr, pc] : spiralOrder) {
            if (pr < rows && pc < cols && spiralProb(gen) > 0.5) {
                layout[pr][pc] = durDist(gen);
            }
        }
    } else if (selectedMode == "labyrinth") {
        // Labyrinth: dense "walls" in even cols (with 20% gaps), sparse "paths" in odd cols but with 20% bricks
        float wallGapChance = 0.2f;  // Reduced gaps for more bricks
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (c % 2 == 0) {  // Walls
                    if (realDist(gen) >= wallGapChance) {
                        layout[r][c] = durDist(gen);
                    }
                } else {  // Paths (increased to 20% bricks)
                    if (std::bernoulli_distribution(0.2)(gen)) {
                        layout[r][c] = durDist(gen);
                    }
                }
            }
        }
    } else {
        std::cerr << "Unknown mode '" << selectedMode << "'; falling back to default." << std::endl;
        // Use default logic
        float gapChance = 0.15f - (wave * 0.01f);
        if (gapChance < 0.05f) gapChance = 0.05f;
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (realDist(gen) >= gapChance) {
                    layout[r][c] = durDist(gen);
                }
            }
        }
        selectedMode = "default";
    }

    // Place bricks from layout
    float startX = 50.0f;  // Adjusted left margin for 8 cols
    float startY = 100.0f;
    float brickW = 80.0f;
    float brickH = 20.0f;
    float colSpacing = 90.0f;  // Fits 8 cols: 50 + 7*90 = 680 < 800
    // Optional: Add jitter for randomization
    std::uniform_real_distribution<float> xJitter(-5.0f, 5.0f);  // Small random offset per brick x
    // rowSpacing already declared above
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int dur = layout[r][c];
            if (dur > 0) {
                float x = startX + c * colSpacing + xJitter(gen);
                float y = startY + r * rowSpacing;
                int scoreValue = dur * 10 * (1 + (wave / 10));
                bricks.emplace_back(x, y, brickW, brickH, dur, scoreValue);
            }
        }
    }

    std::cerr << "Generated '" << selectedMode << "' pattern for wave " << wave << " with " << bricks.size()
              << " bricks (seed: " << usedSeed << "). Copy seed to replay!" << std::endl;
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
    // Ball starts on paddle, unlaunched
    float paddleTop = paddle.y - paddle.h / 2;
    Ball ball = {paddle.x, paddleTop - 8 - 1, 0, 0, 8}; 
    ball.launched = false;  // Add this line; requires update to Ball struct
    Game game = {0, 3};  // Lives back to 3
    std::vector<Brick> bricks;
    std::vector<Powerup> powerups;

    // Procedural Brick Generation
    std::random_device rd;
    std::mt19937 gen(rd());
    int wave = 1;
    std::uniform_real_distribution<> dropChance(0, 1);
    GenerateBricks(bricks, wave);  // Uses "random" by default
    std::vector<uint32_t> levelSeeds;

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
                    paddleTop = paddle.y - paddle.h / 2;
                    ball = {paddle.x, paddleTop - 8 - 1, 0, 0, 8}; 
                    ball.launched = false;
                    bricks.clear();
                    powerups.clear();
                    wave = 1;
                    GenerateBricks(bricks, wave);  // Uses "random" by default
                } else if (menu.state == PLAYING && e.key.keysym.sym == SDLK_SPACE) {
                    if (!ball.launched) {
                        // Launch ball
                        ball.vx = 200;
                        ball.vy = -200;
                        ball.launched = true;
                        std::cerr << "Ball launched!" << std::endl;
                    } else if (game.blasterActive) {
                        // Fire blaster
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
        }
        
        // Update Physics (only if playing)
        if (menu.state == PLAYING) {
            paddle.Update(deltaTime);
            ball.Update(deltaTime, paddle, game);
            // Follow paddle if not launched
            if (!ball.launched) {
                paddleTop = paddle.y - paddle.h / 2;
                ball.x = paddle.x;
                ball.y = paddleTop - ball.radius - 1.0f;
                ball.vx = 0;
                ball.vy = 0;
            }
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
                        // std::cerr << "Brick destroyed! Score: " << game.score << std::endl;
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
                GenerateBricks(bricks, wave);  // Uses "random" by default
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