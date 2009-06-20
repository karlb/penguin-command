#include "main.h"

#define ShotColor SDL_MapRGB(Screen->format,200,0,0)
#define SPEED 32
#define RELOAD 10000

ShotType Shot[MAX_SHOT_NUM];
int ShotCount=0, LeftHandedMouse=0;
int ShotNum[3]={10,10,10};
float ShotReload[3];

void CreateShot(int x, int y, int button)
{
    int SourceX=100,SourceY=515,i;

    if (y > 510) y = 510;

    if (button>3) {
	//printf("Wrong mouse button\n");
	return;
    }
    if (LeftHandedMouse) {
	button=4-button;
    }

    SourceX=-300+button*350;
    if ((ShotCount==0)||(x!=Shot[ShotCount-1].DestX)||(y!=Shot[ShotCount-1].DestY))
    if (ShotNum[button-1]>0) {
	if (ShotCount<MAX_SHOT_NUM) {

    	    ShotType *This;
	    float Vec;

	    Vec=100*hypot((x-SourceX),(y-SourceY));
    	    This=&Shot[ShotCount];
	    This->x=SourceX;
    	    This->y=SourceY;
    	    This->xspeed=SPEED*(x-SourceX)/Vec;
	    This->yspeed=SPEED*(y-SourceY)/Vec;
    	    This->CurrentPixel=0;
    	    This->Erase=0;
	    This->SourceX=SourceX;
	    This->SourceY=SourceY;
	    This->DestX=x;
	    This->DestY=y;
	    for (i=0;i<LINE_LENGTH;i++) 
		{This->Pixels[i].x=0;This->Pixels[i].y=0;}
	    ShotCount++;
	    if (ShotNum[button-1]==10) 
		ShotReload[button-1]=RELOAD;
	    ShotNum[button-1]--;
    	    DrawShotNum(button-1);
	    ActivateCursor();
	} else {
	    printf("Too many Shots\n");
	}
    }
    
//    Line(SourceX,SourceY,x,y,ShotColor);
//    Blit(x,y,ShotPic);
}

void Reload()
{
    int i;

//    printf ("%d\n",CityNum);
    for (i=0;i<3;i++) {
	if (!CannonDestroyed[i]) {
	    ShotReload[i]-=Speed*CityNum*2;
	    if ((ShotReload[i]<=0)&&(ShotNum[i]<10)) {
		ShotReload[i]=RELOAD;
		ShotNum[i]++;
		DrawShotNum(i);
	    }
	}
    }
}

void RemoveShotPix(int i, int a)
{
    SDL_Rect rect;
    int DelPix;

    rect.w=1;
    rect.h=1;
    DelPix=(Shot[i].CurrentPixel-a) % LINE_LENGTH;
    rect.x=Shot[i].Pixels[DelPix].x;
    rect.y=Shot[i].Pixels[DelPix].y;

    if (rect.x>=800) return;

    PutPixel(Screen,rect.x,rect.y,ShotColor);
    SDL_BlitSurface(BackBuffer, &rect, Screen, &rect);
    AddRect(rect.x,rect.y,rect.x+1,rect.y+1);
}

void HandleShot()
{
    int i,a;
    
    for (i=0;i<ShotCount;i++)
    {
	if (Shot[i].Erase==0)
	{
    	    // Move Shot
	    Shot[i].x+=Shot[i].xspeed*Speed;
    	    Shot[i].y+=Shot[i].yspeed*Speed;
	    if (Shot[i].x>=800) {
		printf("WARNING: shot too far right!\n");
	    }
	}
	// Delete Shot
	if  (Shot[i].y-Shot[i].DestY<=0)
	{
	    Shot[i].y=Shot[i].DestY;
	    Shot[i].x=Shot[i].DestX;
    	    if (Shot[i].Erase==0) CreateExplosion(Shot[i].x,Shot[i].y);
	    Shot[i].Erase+=Speed/(float)7;
	    if (Shot[i].Erase>255) {
    		SDL_SetAlpha(BackBuffer,0,0);
 		for (a=Shot[i].CurrentPixel;a>=0;a--) RemoveShotPix(i,a);
		Shot[i]=Shot[ShotCount-1];
		ShotCount--;
	    }
	    else {
		SDL_SetAlpha(BackBuffer,SDL_SRCALPHA,Shot[i].Erase);
		for (a=Shot[i].CurrentPixel;a>=0;a--) RemoveShotPix(i,a);
 	    }
	}
    }
    if (ArcadeMode) Reload();
}

void DrawShot()
{
    int i;
    
    lock();
    for (i=0;i<ShotCount;i++) {
	if (Shot[i].Erase==0) {
	    if (
		    ((int)Shot[i].x!=Shot[i].Pixels[Shot[i].CurrentPixel].x)
		    ||((int)Shot[i].y!=Shot[i].Pixels[Shot[i].CurrentPixel].y)	    
		)
	    {
		if (++Shot[i].CurrentPixel>=LINE_LENGTH)
		    printf("Line too long in function DrawShot\n");
		PutPixel(Screen,Shot[i].x,Shot[i].y,ShotColor);
		Shot[i].Pixels[Shot[i].CurrentPixel].x=Shot[i].x;
		Shot[i].Pixels[Shot[i].CurrentPixel].y=Shot[i].y;
		Shot[i].Proceeded=1;
		if (Shot[i].CurrentPixel>1) {
		    if (++Shot[i].CurrentPixel>=LINE_LENGTH)
			printf("Line too long in function DrawShot\n");
		    Shot[i].Pixels[Shot[i].CurrentPixel].x=(Shot[i].x+Shot[i].Pixels[Shot[i].CurrentPixel-1].x)/2;
		    Shot[i].Pixels[Shot[i].CurrentPixel].y=(Shot[i].y+Shot[i].Pixels[Shot[i].CurrentPixel-1].y)/2;
		    Shot[i].Proceeded=1;
		    PutPixel(Screen,Shot[i].Pixels[Shot[i].CurrentPixel].x,
		    Shot[i].Pixels[Shot[i].CurrentPixel].y,ShotColor);
		}
	    } else {
		Shot[i].Proceeded=0;
	    }
	}
    }
    unlock();
}

void UndrawShot()
{

}

void DrawShotNum(int Where)
{
    int x,y,shots;
    SDL_Rect rect;

    rect.x=8+Where*360;
    rect.y=550;
    rect.w=60;
    rect.h=45;
    BlitPart(rect.x,rect.y,BackPic,rect);
    y=0; shots=0;
    while (shots<ShotNum[Where]) {
	x=0;
	while ((shots<ShotNum[Where])&&(x<=y)) {
	    Blit((38+Where*360)+(x-y/(float)2)*10,550+y*7,ShotPic);
	    shots++;
	    x++;
	}
	y++;
    }
    SDL_BlitSurface(Screen, &rect, BackBuffer, &rect);
}
