#include "main.h"

#define MAX_EXPLOSION_NUM 20
#define EXPLOSION_SIZE 24
#define MENACE_RANGE 45

typedef struct {
    int x,y;
    float rad;
    int oldrad;
    SDL_Rect Restore;
} ExplosionType;

ExplosionType Explosion[MAX_EXPLOSION_NUM];
int ExplosionCount=0,Flicker=0;

void CreateExplosion(int x, int y)
{
//    if ((ExplosionCount==0)||(x!=Explosion[ExplosionCount-1].x)||(y!=Explosion[ExplosionCount-1].y))
    if (ExplosionCount<MAX_EXPLOSION_NUM){
	ExplosionType *This;
	
	This=&Explosion[ExplosionCount];
	This->x=x;
	This->y=y;
	This->rad=0;
	This->oldrad=0;
	ExplosionCount++;
    }
    else printf("Too many Explosions\n");
}

void HandleExplosion(int InGame)
{
    int i,rad;

    for (i=0;i<ExplosionCount;i++) {
	rad=Explosion[i].rad;
	Explosion[i].oldrad=Explosion[i].rad;
	Explosion[i].rad+=Speed/(float)20;
	if (rad>=EXPLOSION_SIZE) rad-=2*(rad-EXPLOSION_SIZE);
	if (InGame) {
	    MissileCollision(Explosion[i].x,Explosion[i].y,rad);
	    BombCollision(Explosion[i].x,Explosion[i].y,rad);
	}
	// Delete explosions
	if (Explosion[i].rad>=2*EXPLOSION_SIZE) {
	    AddThisRect(Explosion[i].Restore);
	    Explosion[i]=Explosion[ExplosionCount-1];    
	    i--;
	    ExplosionCount--;
	}
    }    
}

void DrawExplosion()
{
    Sint32 i;
    float rad;

    for (i=0;i<ExplosionCount;i++) {
//        if ((int)Explosion[i].rad!=Explosion[i].oldrad)
	{
	    rad=Explosion[i].rad;
	    if (rad>=EXPLOSION_SIZE)
		rad-=2*(rad-EXPLOSION_SIZE);
	    if (Flicker) rad+=abrand(-2,2);
	    rad=(rad*20)/EXPLOSION_SIZE;
	    if (rad<0) rad=0;
	    if (rad>19) rad=19;
	    Blit(Explosion[i].x-ExplosionPic[0]->w/2,Explosion[i].y-ExplosionPic[0]->h/2,ExplosionPic[(int)rad]);
	    Explosion[i].Restore=blitrect;
	}
    }	
}

void UndrawExplosion()
{
    Uint32 i;

    SDL_SetAlpha(BackBuffer,0,0);
    for (i=0;i<ExplosionCount;i++)
//    if ((int)Explosion[i].rad!=Explosion[i].oldrad) 
    {
	SDL_BlitSurface(BackBuffer, &Explosion[i].Restore, Screen, &Explosion[i].Restore);
    }
}

int Menace(int x, int y)
{
    int i,dir=0;
    
    for (i=0;i<ExplosionCount;i++) {
	if ((Explosion[i].y>y)&&(Explosion[i].y<y+MENACE_RANGE))
	    if ((Explosion[i].x+MENACE_RANGE>x)&&(Explosion[i].x-MENACE_RANGE<x)) {
		if (Explosion[i].x>x) dir-=1;
		else dir +=1;
	    }
    }
    if (dir<0) return (-1);
    if (dir>0) return (1);
    return (0);
}
