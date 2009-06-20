
#define TOTAL_WAVE_NUM 100

typedef struct {
    int MissilesToDestroy;
    int MirvChance;
    int BombChance;
    int TimeBetweenShots;
    int FlyerChance;
    float MissileSpeed;
} WaveType;

extern WaveType Wave[TOTAL_WAVE_NUM];

void InitWaves();
