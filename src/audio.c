#include <raylib.h>
#include "audio.h"

Sound shoot, hit, dash;
Music music;

void initSounds()
{
    InitAudioDevice();
    shoot = LoadSound("resources/shoot.wav");
    dash = LoadSound("resources/dash.wav");
    hit = LoadSound("resources/hit.mp3");
    music = LoadMusicStream("resources/song.wav");
    PlayMusicStream(music);
}

void updateSound()
{
    UpdateMusicStream(music);
}