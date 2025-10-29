/* Sets up levels or maps /src/systems/Level.h*/

#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include "objects/Brick.h"

class Level {
public:
    std::vector<Brick> bricks;
    void LoadLevel(int levelId);
};

#endif