#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_timer.h>
#include <math.h>
#include "gfx.h"
#include "game.h"
#include "missile.h"
#include "shot.h"
#include "explosion.h"
#include "SFont.h"
#include "wave.h"
#include "sound.h"
#include "bomb.h"
#include "options.h"
#include "mouse.h"

extern int fullscreen,CityDestroyed[6],CannonDestroyed[3],Score,Quit,HWCursor,NoAlpha,StartLevel,ArcadeMode;
extern int SoundVol,MusicVol;
extern SDL_Surface *background,*ShotPic,*CityPic,*BackPic,*CityHitPic,
		   *Cannon[21],*Font,*BombPic[10],*CannonHitPic,
		   *ExplosionPic[21],*CursorPic[10],*FlyerPic[16];
extern int LowDetail, MediumDetail, HighDetail;
extern SDL_Event event;

extern int Wait( int time_in_ms );

//#define JOYSTICK 1
