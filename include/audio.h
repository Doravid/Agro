#include <raylib.h>

extern Sound shoot, dash, hit;

void initSounds();
void updateSound();
void playBossMusic();
// Sound Getter / Setter
void setSoundVolumes(float volume);
float getSoundVolumes();
// Music getter / setter.
float getMusicVolume();
void setMyMusicVolume(float volume);
