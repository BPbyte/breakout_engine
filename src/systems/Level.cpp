/* Sets up levels or maps /src/systems/Level.cpp*/

#include "Level.h"
#include <random>
#include <iostream>

void Level::LoadLevel(int levelId) {
    bricks.clear();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> chance(0, 1);

    int rows = (levelId == 1) ? 3 : 4; // More rows for higher levels
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < 5; col++) {
            if (chance(gen) < 0.8) { // 80% chance for brick
                int durability = (row == 0) ? 3 : (row == 1) ? 2 : 1;
                int scoreValue = durability * 10;
                bricks.emplace_back(100 + col*120, 100 + row*40, 80, 20, durability, scoreValue);
            }
        }
    }
    std::cerr << "Loaded level " << levelId << " with " << bricks.size() << " bricks" << std::endl;
}