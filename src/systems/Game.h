/* Sets up game values like state points and lives /src/systems/Game.h*/

#ifndef GAME_H
#define GAME_H

struct Game {
    int score;
    int lives;
    void Reset();
    void AddPoint();
    void LoseLife();
};

#endif