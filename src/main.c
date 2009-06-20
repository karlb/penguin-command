/*  Penguin Command: a clone of the famous Missile Command game
    Copyright (C) 2000 Karl Bartel

    This program is free software; you can redistribute it and/or modify        
    it under the terms of the GNU General Public License as published by        
    the Free Software Foundation; either version 2 of the License, or           
    (at your option) any later version.                                         
                                                                                
    This program is distributed in the hope that it will be useful,       
    but WITHOUT ANY WARRANTY; without even the implied warranty of              
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               
    GNU General Public License for more details.                
                                                                               
    You should have received a copy of the GNU General Public License           
    along with this program; if not, write to the Free Software                 
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA   
                                                                                
    Karl Bartel                                                         
    Cecilienstr. 14                                                    
    12307 Berlin                                                  
    GERMANY
    karlb@gmx.net                                                      
*/                                                                            
#include <main.h>
#include <time.h> /* For seeding rand() */
#include <string.h>
#ifdef JOYSTICK
#include "joystick.h"
#endif
#ifndef VERSION
#define VERSION "1.6.?"
#endif


    int fullscreen=1, CityDestroyed[6]={0,0,0,0,0,0},
	CannonDestroyed[3]={0,0,0}, Score=0, HWCursor=0, NoAlpha=0, ArcadeMode=0, MusicVol=10, SoundVol=10;
//images
    SDL_Surface *ShotPic,*CityPic,*BackPic,*CityHitPic,*Cannon[21],*Font,
		*BombPic[10],*CannonHitPic,*TitlePic,*ExplosionPic[21],
		*CursorPic[10],*FlyerPic[16];
//event handling  
    SDL_Event event;
//other
    int Quit=0, Loaded=0, StartLevel=0;
    Uint32 WaitStart=0;
    int LowDetail=0, MediumDetail=0, HighDetail=1;

int Wait( int time_in_ms )
{
    if (WaitStart==0)
	WaitStart=SDL_GetTicks();
    if (WaitStart+time_in_ms <= SDL_GetTicks()) {
	WaitStart=0;
	return 0;
    }
    return 1;
}

void LoadImages()
{
    int i;
    char text[200];

    BackPic = LoadImage("back.jpg",0);
    SDL_FreeSurface(ShotPic);
    ShotPic = LoadImage("shot.png",3);
    CityPic = LoadImage("city.png",4);
    CityHitPic = LoadImage("cityhit.png",4);
    CannonHitPic = LoadImage("cannonhit.png",4);
    for (i=0;i<21;i++) {
	sprintf(text,"cannon%d.png",i+1);
	SDL_FreeSurface(Cannon[i]);
	Cannon[i] = LoadImage(text,3);
    }
    for (i=0;i<10;i++) {
	sprintf(text,"bomb%d.png",i+1);
	SDL_FreeSurface(BombPic[i]);
	BombPic[i] = LoadImage(text,3);
    }
    for (i=0;i<20;i++) {
	sprintf(text,"%dexplo.png",i+1);
	SDL_FreeSurface(ExplosionPic[i]);
	ExplosionPic[i] = LoadImage(text,3);
    }
    for (i=0;i<9;i++) {
	sprintf(text,"%dcursor.png",i+1);
	SDL_FreeSurface(CursorPic[i]);
	CursorPic[i] = LoadImage(text,3);
    }
    CursorPic[9] = CursorPic[0];
    for (i=0;i<16;i++) {
	sprintf(text,"%dflyer.png",i+1);
	SDL_FreeSurface(FlyerPic[i]);
	FlyerPic[i] = LoadImage(text,3);
    }
}

int Button(int x, int y, char *String, int prev, SDL_Surface *Background)
{
    SDL_Rect rect;
    int MouseX,MouseY;
    
    rect.x=x;
    rect.y=y;
    rect.w=SFont_TextWidth(String);
    rect.h=Font->h;
    
    SDL_GetMouseState(&MouseX, &MouseY);
    if ((MouseX>x)&&(MouseY>y)
	&&(MouseX<x+SFont_TextWidth(String))
	&&(MouseY<y+Font->h))
    {
	if (prev!=1) {
	    // clear
	    SDL_SetAlpha(Background,0,0);
	    SDL_BlitSurface(Background, &rect, Screen, &rect);
	    // draw
	    PutString(Screen, x, y, String);
	    AddThisRect(rect);
	    SDL_BlitSurface(Screen, &rect, BackBuffer, &rect);
	    PlaySound(ClickSound);
	}
	return 1;
    } else if (prev!=0) {
	// clear
	SDL_SetAlpha(Background,0,0);
	SDL_BlitSurface(Background, &rect, Screen, &rect);
	// draw
	PutString(Screen, x, y, String);
	AddThisRect(rect);
	// darken
	SDL_SetAlpha(Background,SDL_SRCALPHA,100);
	SDL_BlitSurface(Background, &rect, Screen, &rect);
        SDL_SetAlpha(Background,0,0);
	SDL_BlitSurface(Screen, &rect, BackBuffer, &rect);
    }
    
    return 0;
}

void SetDetail()
{
    if (LowDetail) {
	HWCursor=1;
	NoAlpha=1;
    }
    if (MediumDetail) {
	HWCursor=0;
	NoAlpha=1;
    }
    if (HighDetail) {
	HWCursor=0;
	NoAlpha=0;
    }
}

void ShowHelp()
{
    float anim=0;
    SDL_Rect rect;
    SDL_Event WaitEvent;
    char DetailString[100],LevelString[100],text[100];
    int Level=3,SoundV=3,MusicV=3;
	
    Blit(0,0,BackPic);
    ExplosionCount=0;
    XCenteredString(Screen,10,"< Penguin Command Help & Options >");
    PutString(Screen,20,60,"Objective: Defend cities!");
    PutString(Screen,20,100,"Controls: Mouse, or mouse + a,s,d keys");
    PutString(Screen,20,140,"Score:");
    PutString(Screen,150,185,"10    Remaining Shot");
    PutString(Screen,150,220,"20    Destroyed Missile");
    PutString(Screen,150,255,"30    Destroyed Smartbomb");
    PutString(Screen,150,290,"80    Remaining City");
    Blit(80-ShotPic->w/2,200-ShotPic->h/2,ShotPic);
    Blit(80-CityPic->w/2,310-CityPic->h/2,CityPic);
    XCenteredString(Screen,380,"< Settings >");
    SDL_BlitSurface(Screen,NULL,BackBuffer,NULL);
    SDL_BlitSurface(Screen,NULL,FadeBuffer,NULL);
    Update();
    Now=SDL_GetTicks();
    while (Level || SoundV || MusicV) {
		SDL_PollEvent(&WaitEvent);
		while (   (SDL_GetMouseState(NULL,NULL)==SDL_BUTTON(1))
				  ||(SDL_GetMouseState(NULL,NULL)==SDL_BUTTON(2))
				  ||(SDL_GetMouseState(NULL,NULL)==SDL_BUTTON(3)) )
	    {SDL_PollEvent(&WaitEvent);}
        while (WaitEvent.type!=SDL_KEYDOWN && WaitEvent.type!=SDL_MOUSEBUTTONDOWN) {
    	    SDL_PollEvent(&WaitEvent);
			UndrawMouse();
			// Explosion
			UndrawExplosion();
			if (ExplosionCount<1) CreateExplosion(80,235);
			HandleExplosion(0);
			DrawExplosion();
			// Bomb
			anim+=Speed/100;
			if ((int)anim>=10) anim-=10;
			rect.x=80-BombPic[0]->w/2; rect.y=270-BombPic[0]->h/2;
			rect.w=BombPic[0]->w; rect.h=BombPic[0]->h;
			SDL_BlitSurface( BackBuffer, &rect, Screen, &rect);
			Blit(rect.x,rect.y,BombPic[(int)anim]);
			AddThisRect(blitrect);
			// detail button
			if (LowDetail) strcpy(DetailString,"Low Detail");
			if (MediumDetail) strcpy(DetailString,"Medium Detail");
			if (HighDetail) strcpy(DetailString,"High Detail");
			//	    Detail=Button(400-SFont_TextWidth(DetailString)/2,430,DetailString,Detail,FadeBuffer);
			// level start button
			sprintf(LevelString,"Starting at Wave %d", StartLevel+1);
			Level=Button(400-SFont_TextWidth(LevelString)/2,430,LevelString,Level,FadeBuffer);
			// sound buttons
			sprintf(text,"Music Volume: %d", MusicVol);
			MusicV=Button(390-SFont_TextWidth(text),490,text,MusicV,FadeBuffer);
			sprintf(text,"Sound Volume: %d", SoundVol);
			SoundV=Button(410,490,text,SoundV,FadeBuffer);
			// display
			GetSpeed();
			DrawMouse();
			Update();
		}
		if (SoundV) {
			if (SDL_GetMouseState(NULL,NULL)==SDL_BUTTON(1))
				if (SoundVol<15) SoundVol++;
			if (SDL_GetMouseState(NULL,NULL)==SDL_BUTTON(3))
				if (SoundVol>0) SoundVol--;
			SoundV=3;
			rect.x=400;
			rect.y=490;
			rect.w=400;
			rect.h=Font->h;
			AddThisRect(rect);
            SDL_SetAlpha( FadeBuffer, 0, 0);
			SDL_BlitSurface( FadeBuffer, &rect, BackBuffer, &rect);
			SDL_BlitSurface( FadeBuffer, &rect, Screen, &rect);
			SetVol();
		}
		if (MusicV) {
			if (SDL_GetMouseState(NULL,NULL)==SDL_BUTTON(1))
				if (MusicVol<15) MusicVol++;
			if (SDL_GetMouseState(NULL,NULL)==SDL_BUTTON(3))
				if (MusicVol>0) MusicVol--;
			MusicV=3;
			rect.x=0;
			rect.y=490;
			rect.w=400;
			rect.h=Font->h;
			AddThisRect(rect);
            SDL_SetAlpha( FadeBuffer, 0, 0);
			SDL_BlitSurface( FadeBuffer, &rect, BackBuffer, &rect);
			SDL_BlitSurface( FadeBuffer, &rect, Screen, &rect);
#ifdef USE_SOUND
			Mix_VolumeMusic(MusicVol*7);
#endif
		}
		if (Level) {
			if (SDL_GetMouseState(NULL,NULL)==SDL_BUTTON(1))
				if (StartLevel<30) StartLevel+=5;
			if (SDL_GetMouseState(NULL,NULL)==SDL_BUTTON(3))
				if (StartLevel>0) StartLevel-=5;
			Level=3;
			rect.x=100;
			rect.y=430;
			rect.w=600;
			rect.h=Font->h;
            SDL_SetAlpha( FadeBuffer, 0, 0);
			SDL_BlitSurface( FadeBuffer, &rect, BackBuffer, &rect);
			SDL_BlitSurface( FadeBuffer, &rect, Screen, &rect);
			AddThisRect(rect);
		}
		SDL_Delay(10);
		while (SDL_GetMouseState(NULL,NULL)==SDL_BUTTON(1)) {SDL_PollEvent(&WaitEvent);SDL_Delay(10);}
    }
	//    SetDetail();
	//    LoadImages();
	//    SDL_ShowCursor(HWCursor);
}

void ShowCredits()
{
    float MusicDown=110;

    BlitToBB(0,0,BackPic);
    XCenteredString(BackBuffer,10,"<Penguin Command Credits>");
    PutString(BackBuffer,20,60 ,"Code:");
    PutString(BackBuffer,20,90,"- Karl Bartel <karlb@gmx.net>");
    PutString(BackBuffer,20,140,"Graphics:");
    PutString(BackBuffer,20,170,"- Karl Bartel");
    PutString(BackBuffer,20,200,"- Shane Simmons");
    PutString(BackBuffer,20,140+MusicDown,"Music:");
    PutString(BackBuffer,20,170+MusicDown,"- Skaven of the Future Crew");
    PutString(BackBuffer,100,200+MusicDown,"Data Jack");
    PutString(BackBuffer,100,230+MusicDown,"Ice Frontier");
    PutString(BackBuffer,100,260+MusicDown,"Rama Gardens");
    PutString(BackBuffer,20,310+MusicDown,"- Seth Peelle");
    PutString(BackBuffer,100,340+MusicDown,"Penguin Night");
    XCenteredString(BackBuffer,560,"http://www.linux-games.com/");
    FadeScreen(1);
    Now=SDL_GetTicks();
    while (SDL_GetMouseState(NULL,NULL)==SDL_BUTTON(1)) {SDL_PollEvent(&event);SDL_Delay(10);}
    while ((event.type!=SDL_KEYDOWN)&&(event.type!=SDL_MOUSEBUTTONDOWN)) {
	SDL_PollEvent(&event);
	UndrawMouse();
	DrawMouse();
        GetSpeed();
	Update();
    }
}

void ShowTitle()
{
    int start=3,help=3,hiscore=3,quit=3,delay=0,credits=3,arcade=3;
    float NewExplosion=0;

    // show Title
    printf ("** Blit Background **\n");
    Blit(0,0,TitlePic);
    printf ("** Background image drawn **\n");
    if (!Loaded) {
	printf("** Loading Font **\n");
	Font = LoadImage("abc.png",4);
	printf("** Init Font **\n");
	InitFont( Font );
	//SDL_EventState(SDL_KEYUP, SDL_IGNORE);
	printf("** Loading Images **\n");
	// load...
	LoadImages();
	Loaded=1;
	MenuEvents();
    }
	SDL_SetAlpha(Font, SDL_SRCALPHA, 0);
    ExplosionCount=0;CursorAnim=0;
    SDL_BlitSurface(TitlePic,NULL,BackBuffer,NULL);
    // menu
    while (!Quit) {
	if (delay) {
	    Blit(0,0,TitlePic);
	    SDL_BlitSurface(TitlePic,NULL,BackBuffer,NULL);
	    while (event.type==SDL_MOUSEBUTTONDOWN)
		SDL_PollEvent(&event);
	    Now=SDL_GetTicks();
	    start=3;help=3;hiscore=3;quit=3;credits=3;arcade=3;
	}
	UndrawMouse();
	hiscore=Button(50,300,"Hiscores",hiscore,TitlePic);
	start=Button(400-SFont_TextWidth("Normal Game")/2,300,"Normal Game",start,TitlePic);
	arcade=Button(400-SFont_TextWidth("Arcade Game")/2,340,"Arcade Game",arcade,TitlePic);
	help=Button(750-SFont_TextWidth("Help&Conf"),300,"Help&Conf",help,TitlePic);
	credits=Button(750-SFont_TextWidth("Credits"),450,"Credits",credits,TitlePic);
	quit=Button(50,450,"Quit",quit,TitlePic);
	// Fancy explosions
	UndrawExplosion();
	if ((ExplosionCount<10)&&(NewExplosion<0)) {
	    CreateExplosion(abrand(0,799),abrand(0,280));
	    NewExplosion+=abrand(0,600)+50;
	}
	NewExplosion-=Speed;
	HandleExplosion(0);
	DrawExplosion();
	GetSpeed();
	// Explosions end
	DrawMouse();
	Update();
	delay=0;
	SDL_PollEvent(&event);
	if (event.type==SDL_MOUSEBUTTONDOWN) {
	    if (hiscore) {
		ShowHiscore();
		SDL_WaitEvent(&event);
	    }
	    if (start) {
		ArcadeMode=0;
		break;
	    }
	    if (arcade) {
		ArcadeMode=1;
		break;
	    }
	    if (help) ShowHelp();
	    if (credits) ShowCredits();
	    if (quit) Quit=1;
	    delay=1;
	}
    }
    ExplosionCount=0;
//    SDL_WaitEvent(&event);
}

void TextHelp(char *argv[])
{
    printf("\nPenguin Command Version %s\n",VERSION);
    puts("The newest version can be obtained at http://www.linux-games.com/\n");
    printf("Usage: %s [options]\n",argv[0]);
    puts("  [-f, --fullscreen]         start in fullscreen mode (default)");
    puts("  [-w, --windowed]           start in windowed mode");
    puts("  [-s, --nosound]            start without sound");
    puts("  [-l, --lefthandedmouse]    swaps left and right mouse buttons");
    puts("  [-h, --help]               this text\n");

    exit(0);
}

void ReadCommandLine(char *argv[])
{
    int i;
    for ( i=1;argv[i];i++ ) {
	if ((strcmp(argv[i],"--fullscreen")==0)||(strcmp(argv[i],"-f")==0)) fullscreen=1; else
	if ((strcmp(argv[i],"--windowed")==0)||(strcmp(argv[i],"-w")==0)) fullscreen=0; else
	if ((strcmp(argv[i],"--nosound")==0)||(strcmp(argv[i],"-s")==0)) sound=0; else
	if ((strcmp(argv[i],"--lefthandedmouse")==0)||(strcmp(argv[i],"-l")==0)) LeftHandedMouse=1; else
	if ((strcmp(argv[i],"--help")==0)||(strcmp(argv[i],"-h")==0)) TextHelp(argv);
	else {
	    printf("\nUnknown parameter: \"%s\" \n", argv[i]);
	    TextHelp(argv);
	}
    }
}

int main(int argc, char *argv[])
{
// intialisation
//    if ( SDL_Init(SDL_INIT_AUDIO ) < 0 ) ComplainAndExit();
    ReadCommandLine(argv);
    printf("** Starting SDL init **\n");
    init_SDL();
    SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
    SDL_WM_SetCaption("Penguin Command","Penguin Command");
    srand((unsigned)time(NULL));
#ifdef JOYSTICK
/* joystick */
    printf("** Finding Joysticks **\n");
    JoystickInit();
#endif
// load data
    printf("** Loading Title Screen **\n");
    TitlePic = LoadImage("title.jpg",0);
    InitSound();
// set volume
    ReadOptions();
// show title screen
    printf("** Playing Music **\n");
    SetVol();
    SDL_EnableUNICODE(1);
//    LoadCursor();
//    SetDetail();
    SDL_ShowCursor(HWCursor);
    printf("** Ready to enter Menu **\n");
// start game
	PlayMusic();
    while (!Quit) {
#ifdef USE_SOUND
		if (sound) {
			printf("** Fade in Music **\n");
			//Mix_FadeInMusic(Music[abrand(0,MUSICNUM-1)],0,5000);
		}
#endif
		printf("** Showing Title Screen **\n");
		ShowTitle();
		if (Quit) break;
		StartGame();
		FinalScore();
		ShowHiscore();
    }
// user wants to quit
#ifdef USE_SOUND
    Mix_FadeOutMusic(1000);
#endif
    {SDL_Rect rect; rect.x=0;rect.y=0;rect.w=800;rect.h=600; SDL_FillRect(BackBuffer,&rect,0);}
    FadeScreen(0.9);
    WriteOptions();
//    printf("Awaiting SDL_Quit()\n");
    SDL_Quit();
//    printf("SDL_Quit() finished.\n");
    return 0;
}
