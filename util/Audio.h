/*For playing SFX and eventually BGM /src/util/Audio.h*/

#ifndef AUDIO_H
#define AUDIO_H

class Audio {
public:
    Audio(const char* soundPath);
    ~Audio();
    void Play();
};

#endif