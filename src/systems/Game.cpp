#include "Game.h"
#include <iostream>

void Game::Reset() {
    score = 0;
    lives = 3;
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