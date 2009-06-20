#include "main.h"
#include <math.h>
#include "../data/gfx/cursor.xbm"
#include "../data/gfx/cursor_mask.xbm"
#include "../data/gfx/cursor_mask1.xbm"
#include "../data/gfx/cursor_mask2.xbm"
#include "../data/gfx/cursor_mask3.xbm"
#include "../data/gfx/cursor_mask4.xbm"
#include "../data/gfx/cursor_mask5.xbm"
#include "../data/gfx/cursor_mask6.xbm"

#define SIZE 128
#define FRAMENUM 7

Uint8 CursorBits[SIZE],CursorMaskBits[FRAMENUM][SIZE];
float CursorAnim=0;
SDL_Cursor *CursorData,*OldCursorData;
SDL_Rect OldMouseRect;

Uint8 convert(Uint8 b)
{
    Uint8 temp;
    
    temp = (((b & 0x01) << 7) |
	    ((b & 0x02) << 5) |
	    ((b & 0x04) << 3) |
	    ((b & 0x08) << 1) |
	    ((b & 0x10) >> 1) |
	    ((b & 0x20) >> 3) |
	    ((b & 0x40) >> 5) |
	    ((b & 0x80) >> 7));
	    
    return temp;
}

void LoadCursor()
{
    int i;

    for (i = 0; i < SIZE; i++) {
        CursorBits[i]=convert(cursor_bits[i]);
	CursorMaskBits[0][i]=convert(cursor_mask_bits[i]);
	CursorMaskBits[1][i]=convert(cursor_mask1_bits[i]);
	CursorMaskBits[2][i]=convert(cursor_mask2_bits[i]);
	CursorMaskBits[3][i]=convert(cursor_mask3_bits[i]);
	CursorMaskBits[4][i]=convert(cursor_mask4_bits[i]);
	CursorMaskBits[5][i]=convert(cursor_mask5_bits[i]);
	CursorMaskBits[6][i]=convert(cursor_mask6_bits[i]);
    }
    SDL_SetCursor(SDL_CreateCursor(CursorBits, CursorMaskBits[0], 32, 32, 15, 15));
}

void ActivateCursor()
{
    if (HWCursor) {
	CursorAnim=FRAMENUM;
    } else
    {
	CursorAnim=9;
    }
}

void HandleCursor()
{
    if (HWCursor) {
	CursorAnim-=Speed/100;
	if ((int)CursorAnim<0)
	    CursorAnim=0;
	OldCursorData=CursorData;
	SDL_SetCursor(CursorData=SDL_CreateCursor(CursorBits, CursorMaskBits[(int)CursorAnim], 32, 32, 15, 15));
	SDL_FreeCursor(OldCursorData);
    } else {
	if (CursorAnim>0)
	    CursorAnim-=Speed/30;
	if (CursorAnim<0)
	    CursorAnim=0;
    }
}

void UndrawMouse()
{
    if (!HWCursor) {
	SDL_SetAlpha(BackBuffer,0,0);
	SDL_BlitSurface(BackBuffer, &OldMouseRect, Screen, &OldMouseRect);
	AddThisRect(OldMouseRect);
    }
}

void DrawMouse()
{
    int MouseX,MouseY;
    
    if (!HWCursor) {
	SDL_GetMouseState(&MouseX, &MouseY);
	Blit(MouseX-17,MouseY-17,CursorPic[(int)CursorAnim]);
	OldMouseRect=blitrect;
    }
}

void WaitWithMouse( int time_in_ms )
{
    if (HWCursor)
	SDL_Delay( time_in_ms );
    else {
        SDL_BlitSurface(BackBuffer,NULL,FadeBuffer,NULL);
        SDL_BlitSurface(Screen,NULL,BackBuffer,NULL);
        while ( Wait(time_in_ms) ) {
    	    SDL_Delay(10);	
	    SDL_PollEvent(&event);
	    DrawMouse();
	    Update();
	    UndrawMouse();
	}
	SDL_BlitSurface(FadeBuffer,NULL,BackBuffer,NULL);
    }
}
