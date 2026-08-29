#include <raylib.h>
#include "audio.h"

Sound shoot, death, pickUp, dash;
Music music;

void initSounds()
{
    InitAudioDevice();
    shoot = LoadSound("resources/shoot.wav");
    dash = LoadSound("resources/dash.wav");
    music = LoadMusicStream("resources/song.wav");
    PlayMusicStream(music);
}

void updateSound()
{
    UpdateMusicStream(music);
}