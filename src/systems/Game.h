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