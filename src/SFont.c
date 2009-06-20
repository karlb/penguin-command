#include <SDL.h>
#include "SFont.h"
#include <string.h>
#include <stdlib.h>

SFont_FontInfo InternalFont;

Uint32 GetPixel(SDL_Surface *Surface, Sint32 X, Sint32 Y)
{

   Uint8  *bits;
   Uint32 Bpp;

   if (X<0) puts("SFONT ERROR: x too small in GetPixel. Report this to <karlb@gmx.net>");
   if (X>=Surface->w) puts("SFONT ERROR: x too big in GetPixel. Report this to <karlb@gmx.net>");
   
   Bpp = Surface->format->BytesPerPixel;

   bits = ((Uint8 *)Surface->pixels)+Y*Surface->pitch+X*Bpp;

   // Get the pixel
   switch(Bpp) {
      case 1:
         return *((Uint8 *)Surface->pixels + Y * Surface->pitch + X);
         break;
      case 2:
         return *((Uint16 *)Surface->pixels + Y * Surface->pitch/2 + X);
         break;
      case 3: { // Format/endian independent 
         Uint8 r, g, b;
         r = *((bits)+Surface->format->Rshift/8);
         g = *((bits)+Surface->format->Gshift/8);
         b = *((bits)+Surface->format->Bshift/8);
         return SDL_MapRGB(Surface->format, r, g, b);
         }
         break;
      case 4:
         return *((Uint32 *)Surface->pixels + Y * Surface->pitch/4 + X);
         break;
   }

    return -1;
}

void InitFont2(SFont_FontInfo *Font)
{
    int x = 0, i = 0;

    if ( Font->Surface==NULL ) {
		printf("The font has not been loaded!\n");
		exit(1);
    }

    if (SDL_MUSTLOCK(Font->Surface)) SDL_LockSurface(Font->Surface);

    while ( x < Font->Surface->w ) {
	if(GetPixel(Font->Surface,x,0)==SDL_MapRGB(Font->Surface->format,255,0,255)) { 
    	    Font->CharPos[i++]=x;
    	    while (( x < Font->Surface->w-1) && (GetPixel(Font->Surface,x,0)==SDL_MapRGB(Font->Surface->format,255,0,255)))
		x++;
	    Font->CharPos[i++]=x;
	}
	x++;
    }
    if (SDL_MUSTLOCK(Font->Surface)) SDL_UnlockSurface(Font->Surface);

    Font->h=Font->Surface->h;
    SDL_SetColorKey(Font->Surface, SDL_SRCCOLORKEY, GetPixel(Font->Surface, 0, Font->Surface->h-1));
}

void InitFont(SDL_Surface *Font)
{
    InternalFont.Surface=Font;
    InitFont2(&InternalFont);
}

void PutString2(SDL_Surface *Surface, SFont_FontInfo *Font, int x, int y, char *text)
{
    int ofs;
    int i=0;
    SDL_Rect srcrect,dstrect; 

    while (text[i]!='\0') {
        if (text[i]==' ') {
            x+=Font->CharPos[2]-Font->CharPos[1];
            i++;
	}
	else {
//	    printf("-%c- %c - %u\n",228,text[i],text[i]);
	    ofs=(text[i]-33)*2+1;
//	    printf("printing %c %d\n",text[i],ofs);
            srcrect.w = dstrect.w = (Font->CharPos[ofs+2]+Font->CharPos[ofs+1])/2-(Font->CharPos[ofs]+Font->CharPos[ofs-1])/2;
            srcrect.h = dstrect.h = Font->Surface->h-1;
            srcrect.x = (Font->CharPos[ofs]+Font->CharPos[ofs-1])/2;
            srcrect.y = 1;
    	    dstrect.x = x-(float)(Font->CharPos[ofs]-Font->CharPos[ofs-1])/2;
	    dstrect.y = y;

	    SDL_BlitSurface( Font->Surface, &srcrect, Surface, &dstrect); 

            x+=Font->CharPos[ofs+1]-Font->CharPos[ofs];
            i++;
        }
    }
}

void PutString(SDL_Surface *Surface, int x, int y, char *text)
{
    PutString2(Surface, &InternalFont, x, y, text);
}

int SFont_TextWidth2(SFont_FontInfo *Font, char *text)
{
    int ofs=0;
    int i=0,x=0;

    while (text[i]!='\0') {
        if (text[i]==' ') {
            x+=Font->CharPos[2]-Font->CharPos[1];
            i++;
	}
	else {
	    ofs=(text[i]-33)*2+1;
            x+=Font->CharPos[ofs+1]-Font->CharPos[ofs];
            i++;
        }
    }
//    printf ("--%d\n",x);
    return x;
}

int SFont_TextWidth(char *text)
{
    return SFont_TextWidth2(&InternalFont, text);
}

void XCenteredString2(SDL_Surface *Surface, SFont_FontInfo *Font, int y, char *text)
{
    PutString2(Surface, Font, Surface->w/2-SFont_TextWidth2(Font,text)/2, y, text);
}

void XCenteredString(SDL_Surface *Surface, int y, char *text)
{
    XCenteredString2(Surface, &InternalFont, y, text);
}

void SFont_InternalInput( SDL_Surface *Dest, SFont_FontInfo *Font, int x, int y, int PixelWidth, char *text)
{
    SDL_Event event;
    int ch=-1,blink=0;
    long blinktimer=0;
    SDL_Surface *Back;
    SDL_Rect rect;
//    int ofs=(text[0]-33)*2+1;
//    int leftshift=(Font->CharPos[ofs]-Font->CharPos[ofs-1])/2;
    
    Back = SDL_AllocSurface(Dest->flags,
    			    Dest->w,
    			    Font->h,
    			    Dest->format->BitsPerPixel,
    			    Dest->format->Rmask,
    			    Dest->format->Gmask,
			    Dest->format->Bmask, 0);
    rect.x=0;
    rect.y=y;
    rect.w=Dest->w;
    rect.h=Font->Surface->h;
    SDL_BlitSurface(Dest, &rect, Back, NULL);
    PutString2(Dest,Font,x,y,text);
    SDL_UpdateRects(Dest, 1, &rect);
        
    // start input
    SDL_EnableUNICODE(1);
    blinktimer=SDL_GetTicks();
    while (ch!=SDLK_RETURN) {
	if (event.type==SDL_KEYDOWN) {
	    ch=event.key.keysym.unicode;
	    if (((ch>31)||(ch=='\b')) && (ch<128)) {
		if ((ch=='\b')&&(strlen(text)>0))
		    text[strlen(text)-1]='\0';
		else if (ch!='\b')
		    sprintf(text,"%s%c",text,ch);
	        if (SFont_TextWidth2(Font,text)>PixelWidth) text[strlen(text)-1]='\0';
		SDL_BlitSurface( Back, NULL, Dest, &rect);
		PutString2(Dest, Font, x, y, text);
		SDL_UpdateRects(Dest, 1, &rect);
//		printf("%s ## %d\n",text,strlen(text));
		SDL_WaitEvent(&event);
	    }
	}
	if (SDL_GetTicks()>blinktimer) {
	    blink=1-blink;
	    blinktimer=SDL_GetTicks()+500;
	    if (blink) {
		PutString2(Dest, Font, x+SFont_TextWidth2(Font,text), y, "|");
		SDL_UpdateRects(Dest, 1, &rect);
//		SDL_UpdateRect(Dest, x+SFont_TextWidth2(Font,text), y, SFont_TextWidth2(Font,"|"), Font->Surface->h);
	    } else {
		SDL_BlitSurface( Back, NULL, Dest, &rect);
		PutString2(Dest, Font, x, y, text);
		SDL_UpdateRects(Dest, 1, &rect);
//		SDL_UpdateRect(Dest, x-(Font->CharPos[ofs]-Font->CharPos[ofs-1])/2, y, PixelWidth, Font->Surface->h);
	    }
	}
	SDL_Delay(1);
	SDL_PollEvent(&event);
    }
    text[strlen(text)]='\0';
    SDL_FreeSurface(Back);
}

void SFont_Input2( SDL_Surface *Dest, SFont_FontInfo *Font, int x, int y, int PixelWidth, char *text)
{
    SFont_InternalInput( Dest, Font, x, y, PixelWidth,  text);
}
void SFont_Input( SDL_Surface *Dest, int x, int y, int PixelWidth, char *text)
{
    SFont_Input2( Dest, &InternalFont, x, y, PixelWidth, text);
}
