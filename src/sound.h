#include "SDL_mixer.h"

#define MUSICNUM 3

void InitSound();
void PlayMusic();
void SetVol();
void PlaySound(Mix_Chunk *Sample);

extern int sound;
#ifndef USE_SOUND
  extern void *MissileExplosionSound,*BombExplosionSound,*GameoverSound,*CitySound,*ClickSound;
  extern void *Music[MUSICNUM];
#else
  extern Mix_Chunk *MissileExplosionSound,*BombExplosionSound,*GameoverSound,*CitySound,*ClickSound;
  extern Mix_Music *Music[MUSICNUM];
#endif
