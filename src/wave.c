#include "wave.h"

extern int ArcadeMode;
WaveType Wave[TOTAL_WAVE_NUM];

void InitWaves()
{
    int i;
    
    if (!ArcadeMode) 
    for (i=0;i<TOTAL_WAVE_NUM;i++) {
	Wave[i].MissilesToDestroy = 10+i;
	Wave[i].MirvChance = 30+i*4;
	Wave[i].BombChance = i*2;
	Wave[i].FlyerChance = 5;
	Wave[i].TimeBetweenShots = 3000-i*200;
	Wave[i].MissileSpeed = 1.9+((float)i/4);
	if (Wave[i].TimeBetweenShots<800) Wave[i].TimeBetweenShots=800;
	if (Wave[i].MirvChance>65) Wave[i].MirvChance=65;
	if (Wave[i].BombChance>20) Wave[i].BombChance=20;
    } else
    for (i=0;i<TOTAL_WAVE_NUM;i++) {
	Wave[i].MissilesToDestroy = 30+i*2;
	Wave[i].MirvChance = 25+i*4;
	Wave[i].BombChance = i*2+3;
	Wave[i].FlyerChance = 8;
	Wave[i].TimeBetweenShots = 1900-i*300;
	Wave[i].MissileSpeed = 2.6+((float)i/3);
	if (Wave[i].TimeBetweenShots<800) Wave[i].TimeBetweenShots=800;
	if (Wave[i].MirvChance>65) Wave[i].MirvChance=65;
	if (Wave[i].BombChance>20) Wave[i].BombChance=20;
    }
}
