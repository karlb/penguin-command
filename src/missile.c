#include "main.h"
#include "SDL_gfxPrimitives.h"

#define MissileColor SDL_MapRGB(Screen->format,0,200,0)
#define MissileGreen 200
#define MISSILE_MAX_NUM 80
#define FLYER_MAX_NUM 8
#define MISSILE_LENGTH 200
#define TAIL 80

typedef struct
{
    float x,y;
    float SourceX,SourceY;
    float xspeed,yspeed;
    coord Pixels[MISSILE_LENGTH];
    int CurrentPixel;
    int Proceeded;
    float Erase;
    int Target;
    int TargetX;
    int Split;
} MissileType;

typedef struct
{
    float x,y;
    float xspeed;
    float AnimPhase;
    Sint32 Release;
} FlyerType;

MissileType Missile[MISSILE_MAX_NUM];
FlyerType Flyer[FLYER_MAX_NUM];
int MissileCount=0,FlyerCount=0,UpdateMissiles=1;

void CreateMissile(int x, int y)
{
    int i;
    
    if (MissileCount<MISSILE_MAX_NUM) {

        MissileType *This;
	float Vec;

        This=&Missile[MissileCount];
	This->SourceX=This->x=x;
        This->SourceY=This->y=y;
	This->Target=abrand(0,8);
	if (This->Target<3)
	    This->TargetX = 120+This->Target*100;
	else if (This->Target<6)
	    This->TargetX = 480+(This->Target-3)*100;
	else 
	    This->TargetX = 40+(This->Target-6)*360;
	Vec=100*hypot(This->TargetX-This->x,510-This->y);
	This->xspeed=Wave[WaveNum].MissileSpeed*(This->TargetX-This->x)/Vec;
        This->yspeed=Wave[WaveNum].MissileSpeed*(510-This->y)/Vec;
        This->CurrentPixel=0;
        This->Erase=0;
        This->Split=0;
	for (i=0;i<MISSILE_LENGTH;i++) 
	    {This->Pixels[i].x=0;This->Pixels[i].y=0;}
	MissileCount++;
    }
    else printf("Too many Missiles\n");
}

void UndrawThisMissile(int i) {
    SDL_Rect rect;
    float f=0;

    SDL_SetAlpha(BackBuffer,0,0);
    while ( f<1 ) {
        rect.x = Missile[i].x*f + Missile[i].SourceX*(1-f)-2;
        rect.y = Missile[i].y*f + Missile[i].SourceY*(1-f)-2;
        rect.w = rect.h = 5;
        f += 4/hypot(Missile[i].x-Missile[i].SourceX,Missile[i].y-Missile[i].SourceY);
        SDL_BlitSurface( BackBuffer, &rect, Screen, &rect);
    }
}

void RemoveMissile(int i)
{
    Missile[i].Erase+=Speed/(float)3;
    if (Missile[i].Erase>255) {
	UndrawThisMissile(i);
	Missile[i]=Missile[MissileCount-1];
	MissileCount--;
    }
}

void DestroyTarget(int TargetNum, int TargetX)
{
    SDL_Rect rect;

    if (TargetNum<6) {
	rect.x=TargetX-50;
	rect.y=490;
	rect.w=100;
	rect.h=CityPic->h;
	SDL_BlitSurface(BackPic,&rect,BackBuffer,&rect);
	BlitToBB(rect.x,rect.y,CityHitPic);
	BlitPart(rect.x,rect.y,BackBuffer,rect);
	if ( ! CityDestroyed[TargetNum] ) {
	    CityDestroyed[TargetNum]=1;
	    CityNum--;
	}
    } else if (TargetNum<9) {
	rect.x=TargetX-CannonHitPic->w/2;
	rect.y=510;
	rect.w=CannonHitPic->w;
	rect.h=CannonHitPic->h;
	SDL_BlitSurface(BackPic,&rect,BackBuffer,&rect);
	BlitToBB(rect.x,rect.y,CannonHitPic);
	SDL_SetAlpha(BackBuffer,0,0);
	BlitPart(rect.x,rect.y,BackBuffer,rect);
	CannonDestroyed[TargetNum-6]=1;
	ShotNum[TargetNum-6]=-1;
	DrawShotNum(TargetNum-6);
    } else printf("WARNING: Target was %d!\n",TargetNum);
}

void HandleMissile()
{
    int i;
    
    for (i=0;i<MissileCount;i++)
    {
	// Hit!
	if ((Missile[i].y>=510)&&(Missile[i].Erase==0)) {
	    CreateExplosion(Missile[i].x,Missile[i].y);
	    DestroyTarget(Missile[i].Target,Missile[i].TargetX);
	    PlaySound(MissileExplosionSound);
	}
	// Delete Missile
	if  (
		((Missile[i].y>=510)
		||((int)Missile[i].x<0)
		||((int)Missile[i].x>=800))
		||(Missile[i].Erase>0)
	    )
	    RemoveMissile(i);
	else {
	    // Move Missile
	    Missile[i].x+=Missile[i].xspeed*Speed;
    	    Missile[i].y+=Missile[i].yspeed*Speed;
	    // Split Missile (Mirv)
	    if (Missile[i].Split) {
	        CreateMissile(Missile[i].x,Missile[i].y);
		Missile[i].Split=0;
	    }
	}
    }
}

void DrawMissile()
{
    int i;
    
    lock();
    for (i=0;i<MissileCount;i++)
    	aalineRGBA(Screen,(int)Missile[i].x,(int)Missile[i].y,(int)Missile[i].SourceX,(int)Missile[i].SourceY,0,MissileGreen,0,255-(int)Missile[i].Erase);
    unlock();
}

void UndrawMissile()
{
    int i;

    for (i=0;i<MissileCount;i++)
    	UndrawThisMissile(i);
}

int Split(int MissileNum)
{
    if (Missile[MissileNum].y>400) return 1;
    Missile[MissileNum].Split=1; return 0;
}

void CreateFlyer()
{
    if (FlyerCount==FLYER_MAX_NUM) {
	puts("Too many flyers");
	return;
    }

    Flyer[FlyerCount].y=abrand(0,350);
    Flyer[FlyerCount].AnimPhase=0;
    Flyer[FlyerCount].Release=5000+abrand(0,5000);
//    if (abrand(0,1)==0) {
	Flyer[FlyerCount].x=-FlyerPic[0]->w+20;
	Flyer[FlyerCount].xspeed=0.01*abrand(1,6);
//    } else {
//	Flyer[FlyerCount].x=800;
//	Flyer[FlyerCount].xspeed=-0.01*abrand(1,6);	
//    }
    FlyerCount++;
}

void DrawFlyer()
{
    int i;

    for (i=0;i<FlyerCount;i++) {
	if ((int)Flyer[i].AnimPhase > 15) {printf("Error in DrawFlyer!\n");return;}
	if (Flyer[i].AnimPhase < 0) {printf("Stupid (!) error in DrawFlyer!\n");return;}
	Blit(Flyer[i].x,Flyer[i].y,FlyerPic[(int)Flyer[i].AnimPhase]);
    }
}

void UndrawFlyer()
{
    int i;
    SDL_Rect rect;

    SDL_SetAlpha(BackBuffer,0,0);
    rect.w=FlyerPic[0]->w;
    rect.h=FlyerPic[0]->h;
    for (i=0;i<FlyerCount;i++) {
	rect.x=Flyer[i].x;
	rect.y=Flyer[i].y;
	SDL_BlitSurface(BackBuffer, &rect, Screen, &rect);
	AddThisRect(rect);
    }
}

void RemoveFlyer(int i)
{
    Flyer[i]=Flyer[FlyerCount-1];
    FlyerCount--;
}

void HandleFlyer()
{
    int i,num;

    for (i=0;i<FlyerCount;i++) {
	Flyer[i].x+=Flyer[i].xspeed*Speed;
	Flyer[i].AnimPhase+=0.02*Speed;
	if ((int)Flyer[i].AnimPhase>15) Flyer[i].AnimPhase-=15;
	if ( ((int)Flyer[i].x>=800) || (Flyer[i].x<-FlyerPic[0]->w) )
	    RemoveFlyer(i);
	Flyer[i].Release-=Speed;
	if (Flyer[i].Release<=0) {
	    Flyer[i].Release=4000+abrand(0,8000);
	    for (num=0;num<abrand(1,6);num++) {
		CreateMissile(Flyer[i].x+FlyerPic[0]->w/2,Flyer[i].y+FlyerPic[0]->h/2);
	    }
	}
    }
}

void MissileCollision(int x, int y, int rad)
{
    int i,wings;

//    printf("ads %d - %d - %d\n",x,y,rad);
    for (i=0;i<MissileCount;i++)
    {
	if (Missile[i].Erase==0)
	if (hypot(Missile[i].x-x,Missile[i].y-y)<rad) {
	    CreateExplosion(Missile[i].x,Missile[i].y);
	    RemoveMissile(i);
	    AddScore(20);
	    PlaySound(MissileExplosionSound);
	}
    }
    for (i=0;i<FlyerCount;i++) {
	if (Flyer[i].AnimPhase<=8)
	    wings=8-Flyer[i].AnimPhase;
	else
	    wings=Flyer[i].AnimPhase-8;
//	printf("%d\n",wings);
    
	if ( ( (x+rad>Flyer[i].x) && (x-rad<Flyer[i].x+FlyerPic[0]->w)
	    && (y+rad>Flyer[i].y+FlyerPic[0]->h/2-4) && (y-rad<Flyer[i].y+FlyerPic[0]->h/2+4) )
	    ||
	    (  (x+rad>Flyer[i].x) && (x-rad<Flyer[i].x+FlyerPic[0]->w)
	    && (y+rad>Flyer[i].y+FlyerPic[0]->h/2-wings*4) && (y-rad<Flyer[i].y+FlyerPic[0]->h/2+wings*4) ) )
	{
	    CreateExplosion(Flyer[i].x+abrand(0,FlyerPic[0]->w),Flyer[i].y+FlyerPic[0]->h/2+abrand(-wings*4,+wings*4));
	    CreateExplosion(Flyer[i].x+abrand(0,FlyerPic[0]->w),Flyer[i].y+FlyerPic[0]->h/2+abrand(-wings*4,+wings*4));
	    CreateExplosion(Flyer[i].x+abrand(0,FlyerPic[0]->w),Flyer[i].y+FlyerPic[0]->h/2+abrand(-wings*4,+wings*4));
	    RemoveFlyer(i);
	    AddScore(40);
	    PlaySound(MissileExplosionSound);
	    
	}
    }
}
