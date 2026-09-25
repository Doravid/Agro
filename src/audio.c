#include <raylib.h>
#include "audio.h"

Sound shoot, hit, dash;
Music music;
Music bossMusic;

static float soundVolumes = 1.0;
static float musicVolume = 1.0;

void initSounds() {
    InitAudioDevice();

    shoot = LoadSound("resources/shoot.wav");
    dash = LoadSound("resources/dash.wav");
    hit = LoadSound("resources/hit.mp3");
    music = LoadMusicStream("resources/song.wav");
    bossMusic = LoadMusicStream("resources/bossMusic.wav");

    PlayMusicStream(music);
}
void playBossMusic() {
    PauseMusicStream(music);
    PlayMusicStream(bossMusic);
}
void updateSound() {
    UpdateMusicStream(music);
    UpdateMusicStream(bossMusic);
}
float getSoundVolumes() { return soundVolumes; }
float getMusicVolume() { return musicVolume; }

void setMyMusicVolume(float volume) {
    SetMusicVolume(music, volume);
    SetMusicVolume(bossMusic, volume);
    musicVolume = volume;
}

void setSoundVolumes(float volume) {
    SetSoundVolume(shoot, volume);
    SetSoundVolume(hit, volume);
    SetSoundVolume(dash, volume);
    soundVolumes = volume;
}
