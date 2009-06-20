#define BOMB_MAX_NUM 20
#define BOMBSPEEDY 40
#define BOMBSPEEDX 80

#include "main.h"

typedef struct {
    float x,y;
    float Anim;
    int Target;
    int TargetX;
} BombType;

BombType Bomb[BOMB_MAX_NUM];
int BombCount=0;

void CreateBomb()
{
    if (BombCount<BOMB_MAX_NUM) {
	Bomb[BombCount].x=abrand(100,700);
	Bomb[BombCount].y=0;
	Bomb[BombCount].Anim=0;
	Bomb[BombCount].Target=-1;
	Bomb[BombCount].TargetX=-1;
	BombCount++;
    } else printf("Too many Smartbombs!\n");
}

void DrawBomb()
{
    SDL_Rect rect;
    int i;
    
    for (i=0;i<BombCount;i++) {
	rect.x = Bomb[i].x-BombPic[0]->w/2;
	rect.y = Bomb[i].y-BombPic[0]->w/2;
	rect.w = BombPic[0]->w;
	rect.h = BombPic[0]->h;
	Blit(rect.x,rect.y,BombPic[(int)Bomb[i].Anim]);
	AddThisRect(blitrect);
    }
}

void HandleBomb()
{
    int i;
    
    for (i=0;i<BombCount;i++) {
	// Move and animate bomb
	Bomb[i].y+=(float)BOMBSPEEDY/1000*Speed;
	Bomb[i].x+=(float)BOMBSPEEDX/1000*Speed*Menace(Bomb[i].x,Bomb[i].y);
	if (Bomb[i].TargetX!=-1) {
	    if (Bomb[i].TargetX>Bomb[i].x)
		Bomb[i].x+=(float)BOMBSPEEDX/500*Speed;		
	    else if (Bomb[i].TargetX!=(int)Bomb[i].x)
		Bomb[i].x-=(float)BOMBSPEEDX/500*Speed;		
	}
	Bomb[i].Anim+=Speed/100;
	if ((int)Bomb[i].Anim>=10)
	    Bomb[i].Anim-=10;
	// Give a target to the bomb
	if ((Bomb[i].y>470)&&(Bomb[i].TargetX==-1)) {
	    if (Bomb[i].x<70) Bomb[i].Target=6; else       // cannon
	    if (Bomb[i].x<170) Bomb[i].Target=0; else
	    if (Bomb[i].x<270) Bomb[i].Target=1; else
	    if (Bomb[i].x<370) Bomb[i].Target=2; else
	    if (Bomb[i].x<430) Bomb[i].Target=7; else	// cannon
	    if (Bomb[i].x<530) Bomb[i].Target=3; else
	    if (Bomb[i].x<630) Bomb[i].Target=4; else
	    if (Bomb[i].x<730) Bomb[i].Target=5; else
		Bomb[i].Target=8;
	    if (Bomb[i].Target<3)
		Bomb[i].TargetX = 120+Bomb[i].Target*100;
	    else if (Bomb[i].Target<6)
		Bomb[i].TargetX = 480+(Bomb[i].Target-3)*100;
	    else
		Bomb[i].TargetX = 40+(Bomb[i].Target-6)*360;
	}
	// Hit !
	if (Bomb[i].y>510-BombPic[0]->h/2) {
	    PlaySound(BombExplosionSound);
	    CreateExplosion(Bomb[i].x,Bomb[i].y);
	    DestroyTarget(Bomb[i].Target,Bomb[i].TargetX);
	    Bomb[i]=Bomb[BombCount-1];
	    BombCount--;
	}
	// Out of screen
	if ( (Bomb[i].x-BombPic[0]->w/2>=800)||(Bomb[i].x<1-BombPic[0]->w/2) ) {
	    Bomb[i]=Bomb[BombCount-1];
	    BombCount--;
	}
    }
}

void UndrawBomb()
{
    SDL_Rect rect;
    int i;
    
    for (i=0;i<BombCount;i++) {
	rect.x = Bomb[i].x-BombPic[0]->w/2;
	rect.y = Bomb[i].y-BombPic[0]->w/2;
	rect.w = BombPic[0]->w;
	rect.h = BombPic[0]->h;
	SDL_BlitSurface( BackBuffer, &rect, Screen, &rect);
	AddThisRect(rect);
    }    
}

void BombCollision(int x, int y, int rad)
{
    int i;
    
    for (i=0;i<BombCount;i++) {
	if (hypot(Bomb[i].x-x,Bomb[i].y-y)<rad+BombPic[0]->h-8) {
	    PlaySound(BombExplosionSound);
	    CreateExplosion(Bomb[i].x,Bomb[i].y);
	    Bomb[i]=Bomb[BombCount-1];
	    BombCount--;
	    AddScore(30);
	}
    }
}
