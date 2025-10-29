/* Sets up game values like state points and lives /src/systems/Game.h*/

#ifndef GAME_H
#define GAME_H
#include <vector>
#include "objects/Bullet.h"  // New include
struct Game {
    int score;
    int lives;
    // Powerup states
    bool meteorActive = false;
    float meteorDuration = 0.0f;
    bool bigBallActive = false;
    float bigBallDuration = 0.0f;
    bool blasterActive = false;
    float blasterDuration = 0.0f;
    int bigPaddleRemainingHits = 0;  // >0 = active, counts down on paddle hits
    std::vector<Bullet> bullets;
    void Reset();
    void AddPoint();
    void LoseLife();
};
#endif