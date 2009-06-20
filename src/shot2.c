#include "main.h"
#include "SDL_gfxPrimitives.h"

#define ShotRed 200

void UndrawThisShot(int i) {
    SDL_Rect rect;
    float f=0;

    SDL_SetAlpha(BackBuffer,0,0);
    while ( f <= 1 ) {
        rect.x = Shot[i].x*f + Shot[i].SourceX*(1-f) -5;
        rect.y = Shot[i].y*f + Shot[i].SourceY*(1-f) -5;
        rect.w = rect.h = 10;
        SDL_BlitSurface( BackBuffer, &rect, Screen, &rect);
	if ( f == 1)
	    break;
        f += 8/hypot(Shot[i].x-Shot[i].SourceX,Shot[i].y-Shot[i].SourceY);
	if ( f > 1 ) f=1;
    }
}

void HandleShot2()
{
    int i;
    
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
//		UndrawLine(Shot[i].x,Shot[i].y,Shot[i].SourceX,Shot[i].SourceY,ShotColor);
		UndrawThisShot(i);
		Shot[i]=Shot[ShotCount-1];
		ShotCount--;
	    }
	    else {
/*		SDL_Rect rect;
		
		rect.x = Shot[i].DestX*Shot[i].Erase + Shot[i].SourceX*(1-Shot[i].Erase);
		rect.y = Shot[i].DestY*Shot[i].Erase + Shot[i].SourceY*(1-Shot[i].Erase);
		rect.w = rect.h = Speed;
		SDL_BlitSurface( BackBuffer, &rect, Screen, &rect);
		
		Line(Shot[i].x,Shot[i].y,Shot[i].SourceX,Shot[i].SourceY,ShotColor);
		SDL_SetAlpha(BackBuffer,SDL_SRCALPHA,Shot[i].Erase);
		UndrawLine(Shot[i].x,Shot[i].y,Shot[i].SourceX,Shot[i].SourceY,ShotColor);*/
 	    }
	}
    }
    if (ArcadeMode) Reload();
}

void DrawShot2()
{
    int i;
    
    lock();
    for (i=0;i<ShotCount;i++)
	aalineRGBA(Screen,(int)Shot[i].x,(int)Shot[i].y,Shot[i].SourceX,Shot[i].SourceY,ShotRed,0,0,255-(int)Shot[i].Erase);
    unlock();
}

void UndrawShot2()
{
    int i;
		
    for (i=0;i<ShotCount;i++)
	UndrawThisShot( i);
}
