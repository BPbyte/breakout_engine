/* Sets up menus /src/systems/Menu.cpp*/

#include "Menu.h"
#include <SDL2/SDL.h>
#include <iostream>

Menu::Menu() : state(MENU),
    startButton("/home/breki/Projects/MyEngine/assets/sprites/start.png"),
    gameOverText("/home/breki/Projects/MyEngine/assets/sprites/gameover.png") {
}

void Menu::Update(SDL_Event& e, bool& running) {
    if (e.type == SDL_KEYDOWN) {
        if (state == MENU && e.key.keysym.sym == SDLK_SPACE) {
            state = PLAYING;
            std::cerr << "Game started!" << std::endl;
        } else if (state == GAME_OVER && e.key.keysym.sym == SDLK_r) {
            state = MENU;
        } else if (e.type == SDL_QUIT) {
            running = false;
        }
    }
}

void Menu::Render(SpriteRenderer& renderer) {
    if (state == MENU) {
        renderer.DrawSprite(startButton, 400, 300, 200, 50);
    } else if (state == GAME_OVER) {
        renderer.DrawSprite(gameOverText, 400, 300, 200, 50);
    }
}