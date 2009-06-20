#define MAX_SHOT_NUM 20
#define LINE_LENGTH 4000

typedef struct
{
    int x,y;
} coord;

typedef struct
{
    float x,y;
    float xspeed,yspeed;
    coord Pixels[LINE_LENGTH];
    int Proceeded;
    float Erase;
    int CurrentPixel;
    int SourceX,SourceY;
    int DestX,DestY;    
} ShotType;

void CreateShot(int x, int y, int button);
void CreateShot2(int x, int y, int button);
void HandleShot();
void DrawShot();
void UndrawShot();
void HandleShot2();
void DrawShot2();
void UndrawShot2();
void DrawShotNum(int Where);
void Reload();

extern int ShotNum[3], ShotCount, LeftHandedMouse;
extern ShotType Shot[MAX_SHOT_NUM];
