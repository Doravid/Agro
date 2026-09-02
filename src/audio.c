#include <raylib.h>
#include "audio.h"

Sound shoot, hit, dash;
Music music;
Music bossMusic;

void initSounds()
{
    InitAudioDevice();

    shoot = LoadSound("resources/shoot.wav");
    dash = LoadSound("resources/dash.wav");
    hit = LoadSound("resources/hit.mp3");
    music = LoadMusicStream("resources/song.wav");
    bossMusic = LoadMusicStream("resources/bossMusic.wav");

    PlayMusicStream(music);
}
void playBossMusic()
{
    PauseMusicStream(music);
    PlayMusicStream(bossMusic);
}
void updateSound()
{
    UpdateMusicStream(music);
    UpdateMusicStream(bossMusic);
}