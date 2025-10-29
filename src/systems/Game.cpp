/* Sets up game values like state points and lives /src/systems/Game.cpp*/

#include "Game.h"
#include <iostream>

void Game::Reset() {
    score = 0;
    lives = 3;
    meteorActive = false;
    meteorDuration = 0.0f;
    bigBallActive = false;
    bigBallDuration = 0.0f;
    blasterActive = false;
    blasterDuration = 0.0f;
    bigPaddleRemainingHits = 0;
    bullets.clear();
    std::cerr << "Game reset: Score=" << score << ", Lives=" << lives << std::endl;
}

void Game::AddPoint() {
    score++;
    std::cerr << "Score: " << score << std::endl;
}

void Game::LoseLife() {
    lives--;
    std::cerr << "Life lost! Lives=" << lives << std::endl;
}