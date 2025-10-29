/*For playing SFX and eventually BGM /src/util/Audio.cpp*/

#include "Audio.h"
#include <iostream>

Audio::Audio(const char* soundPath) {
    std::cerr << "Audio loaded: " << soundPath << std::endl;
}

Audio::~Audio() {
    std::cerr << "Audio unloaded" << std::endl;
}

void Audio::Play() {
    std::cerr << "Playing sound!" << std::endl;
}