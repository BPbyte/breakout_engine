/* Sets up menus /src/systems/Menu.h*/

#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>
#include "util/Texture.h"
#include "systems/SpriteRenderer.h"

enum GameState { MENU, PLAYING, GAME_OVER };

class Menu {
public:
    GameState state;
    Texture startButton;
    Texture gameOverText;

    Menu();
    void Update(SDL_Event& e, bool& running);
    void Render(SpriteRenderer& renderer);
};

#endif