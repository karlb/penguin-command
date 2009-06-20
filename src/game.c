#include "main.h"
#include "joystick.h"

int endgame=0,MouseX,MouseY,MissilesCreated=0,NewMissile=10000,WaveNum=0,
    NewCity=0,CityNum,pause=0;
Uint32 Now,TimeStart,FrameCount=0;
double Speed,FramesPerSecond;
char text[200];

void AddScore(int ScoreBonus)
{
    SDL_Rect rect;
    
    Score+=ScoreBonus*(WaveNum/2+1);
    sprintf(text, "Score: %d", Score);
    rect.x=0;
    rect.y=0;
    rect.w=SFont_TextWidth(text)+30;
    rect.h=Font->h;
    SDL_BlitSurface( BackPic, &rect, BackBuffer, &rect);
    PutString(BackBuffer,0,0,text);
    SDL_SetAlpha(BackBuffer,(SDL_SRCALPHA), 255);
    SDL_BlitSurface( BackBuffer, &rect, Screen, &rect);
    AddThisRect(rect);
}

void GetSpeed()
{
    FrameCount++;
    FramesPerSecond=FrameCount*(float)1000/(SDL_GetTicks()-TimeStart);
    Speed=SDL_GetTicks()-Now;
    // limit speed to 10ms per frame
    while (Speed<10) {
	SDL_Delay(1);
        Speed=(SDL_GetTicks()-Now);
    }
    Now=SDL_GetTicks();
}


void Pause(const SDL_Event *event)
{
    pause=1;    

    // clean input queue
    while ( event->type == SDL_KEYDOWN )
	SDL_PollEvent((SDL_Event *)event);
	
    // write message to screen
    SDL_BlitSurface(Screen,NULL,FadeBuffer,NULL);
    XCenteredString(Screen,250,"Game Paused");
    SDL_UpdateRect(Screen,0,0,0,0);

    // wait
    while ( event->type != SDL_KEYDOWN )
	SDL_PollEvent((SDL_Event *)event);

    // Clean pic
    SDL_BlitSurface(FadeBuffer,NULL,Screen,NULL);
    SDL_UpdateRect(Screen,0,0,0,0);
    Now=SDL_GetTicks();
    pause=0;
}

int NormalEvents(const SDL_Event *event)
{
    switch (event->type) {
	case SDL_QUIT:
	    printf("Quit Requested\n");
	    Quit=1;
	break;
	case SDL_KEYDOWN:
	    if ( event->key.keysym.sym == SDLK_ESCAPE )
		Quit=1;
	    if ( (event->key.keysym.sym == SDLK_RETURN) &&
		(event->key.keysym.mod & KMOD_ALT) )
		    SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
	break;
    }
    
    return 1;
}

void MenuEvents()
{
    SDL_SetEventFilter(NormalEvents);
}

int GameEvents(const SDL_Event *event)
{
    int button=-1;
    Uint8* keys;

    switch (event->type) {
	case SDL_QUIT:
	    printf("Quit Requested\n");
	    Quit=1;
	break;
#ifdef JOYSTICK
        /* JOYAXISMOTION is handled elsewhere. */
        case SDL_JOYBUTTONDOWN:
                /* TODO: Let user configure which button does what. */
                if (event->jbutton.state == SDL_PRESSED) {
                        SDL_GetMouseState(&MouseX, &MouseY);
                        /* Joystick buttons are numbered from 0, mouse
                          buttons are numbered from 1 */
                        CreateShot(MouseX,MouseY,event->button.button+1);
                }
                break;
#endif /* JOYSTICK */	
	case SDL_MOUSEBUTTONDOWN:
	    SDL_GetMouseState(&MouseX, &MouseY);
    	    CreateShot(MouseX,MouseY,event->button.button);
	break;    
	case SDL_KEYDOWN:
	    keys = SDL_GetKeyState(NULL);
    	    if ( event->key.keysym.sym == SDLK_a ) button=1;
    	    if ( event->key.keysym.sym == SDLK_s ) button=2;
	    if ( event->key.keysym.sym == SDLK_d ) button=3;
	    if ( button > 0 ) {
		CreateShot(MouseX,MouseY,button);
	    }
	    if ( event->key.keysym.sym == SDLK_ESCAPE ) {
		printf("Quit Requested\n");
		endgame=1;
	    }
            // Fullscreen toggle
	    if ( (event->key.keysym.sym == SDLK_RETURN) &&
		(event->key.keysym.mod & KMOD_ALT) )
		    SDL_WM_ToggleFullScreen(SDL_GetVideoSurface());
	    // Pause
	    if (( ( event->key.keysym.sym == SDLK_PAUSE ) ||
	        ( event->key.keysym.sym == SDLK_p ) ) &&
		( !pause) )
		    Pause(event);
	break;
    }    
    return 1;

}

void UndrawCannon(int x, int y)
{
    SDL_Rect rect;
    
    rect.x=x-(int)(Cannon[0]->w/2);
    rect.y=y;
    rect.w=Cannon[0]->w;
    rect.h=Cannon[0]->h;
    SDL_SetAlpha(BackBuffer,(SDL_SRCALPHA),255);
    BlitPart(rect.x,rect.y,BackBuffer,rect);
}

void DrawCannon(int x, int y)
{
    int add=0;
    float deg;

    SDL_GetMouseState(&MouseX, &MouseY);
    deg=atan((float)(MouseY-(y+20))/(float)(MouseX-x));
    if (MouseX<x) {
	add+=20;
    }
    if (MouseY>y+20)
    	Blit(x-(int)(Cannon[0]->w/2),y,Cannon[0+add]);
    else    
	Blit(x-(int)(Cannon[0]->w/2),y,Cannon[add-(int)(deg*0.6366*10)]);
}

void DrawCenteredText(int y, char *text)
{
    SDL_Rect rect;

    rect.x=400-SFont_TextWidth(text)/2;
    rect.y=y;
    rect.w=SFont_TextWidth(text);
    rect.h=Font->h;
    SDL_SetAlpha(BackBuffer,(SDL_SRCALPHA), 255);
    SDL_BlitSurface( BackBuffer, &rect, Screen, &rect);
    PutString(Screen,rect.x,rect.y,text);
    AddThisRect(rect);
}

void StartWave()
{
    int i,dummy=0;

    NewMissile=0;
    MissilesCreated=0;
    BlitToBB(0,0,BackPic);
    // New City?
    for (i=0;i<6;i++)
        if (!CityDestroyed[i]) dummy++;
    i=abrand(0,5);
    if (dummy<6) {
        if ((!NewCity)||(ArcadeMode)) {
    	    while (!CityDestroyed[i])
		i=abrand(0,5);
    	    CityDestroyed[i]=0;
	    if (i<3)
		Blit(70+i*100,490,CityPic);
	    else
		Blit(130+i*100,490,CityPic);
	    PlaySound(CitySound);
	    WaitWithMouse(100);
	    Update();
	    WaitWithMouse(200);
	    NewCity=1;
	}
	else NewCity=0;
    }
    CityNum=0;
    for (i=0;i<6;i++)
        if (!CityDestroyed[i]) CityNum++;
    // Draw Cities
    dummy=0;
    for (i=0;i<3;i++)
	if (CityDestroyed[i])
	    BlitToBB(70+i*100,490,CityHitPic);
	else
	    BlitToBB(70+i*100,490,CityPic);
    dummy=0;
    for (i=0;i<3;i++)
	if (CityDestroyed[i+3])
	    BlitToBB(430+i*100,490,CityHitPic);
	else
	    BlitToBB(430+i*100,490,CityPic);
    // Printf "Staring Wave" message
    AddScore(0);
    Blit(0,0,BackBuffer);
    sprintf(text,"Starting Wave #%d",WaveNum+1);
    PutString(Screen, 400-SFont_TextWidth(text)/2, 170, text);
    sprintf(text,"Score %d x",WaveNum/2+1);
    PutString(Screen, 400-SFont_TextWidth(text)/2, 220, text);
    DrawCenteredText(370, "Defend Cities!");
    for (i=0;i<3;i++) {
	if ( CannonDestroyed[i] ) {
	    ShotNum[i]=3;
	    CannonDestroyed[i]=0;
	} else
	    ShotNum[i]=10;
	DrawShotNum(i);
    }
    Update();
    // Wait 2 seconds
    WaitWithMouse(2000-1000*ArcadeMode);
    Blit(0,0,BackBuffer);
    for (i=0;i<3;i++) DrawShotNum(i);
    SDL_SetEventFilter(GameEvents);
}

void WaveScoring()
{
    int x,i,OldScore;

    UndrawMouse();
    SDL_SetEventFilter(NormalEvents);
    DrawCenteredText(140,"Wave Scoring");
    x=400-(ShotNum[0]+ShotNum[1]+ShotNum[2])*4;
//    printf("1: %d\t2: %d\t3: %d\tE: %d\t\n",ShotNum[0],ShotNum[1],ShotNum[2],x);
    OldScore=Score;
    if (!ArcadeMode)
    for (i=0;i<3;i++) {
        while(ShotNum[i]>0) {
	    Blit(x,200,ShotPic);
	    x+=8;
	    AddScore(10);
	    DrawShotNum(i);
	    sprintf(text,"  %d  ", Score-OldScore);
	    DrawCenteredText(235, text);
	    Update();
	    WaitWithMouse(70);
	    ShotNum[i]--;
	}
	DrawShotNum(i);
    }
    x=400;
    OldScore=Score;
    for (i=0;i<6;i++)
	if (!CityDestroyed[i]) x-=50;
    for (i=0;i<6;i++)
	if (!CityDestroyed[i]) {
	    Blit(x,285-ArcadeMode*100,CityPic);
	    x+=100;
	    AddScore(80);
	    sprintf(text,"  %d  ", Score-OldScore);
	    DrawCenteredText(380-ArcadeMode*100, text);
	    Update();
	    WaitWithMouse(400-ArcadeMode*100);
	}
    WaitWithMouse(3000-ArcadeMode*1500);
    Now=SDL_GetTicks();
//    sprintf(text,"Starting Wave #%d",WaveNum+1);
//    PutString(Screen, 400-SFont_TextWidth(text)/2, 300, text);
}

void ProcessEvents()
{
    int i,Rand,b;

    SDL_PumpEvents();
    if (HighDetail) UndrawShot2(); else UndrawShot();
    UndrawFlyer();
    UndrawMissile();
    UndrawExplosion();
    UndrawBomb();
    if (!CannonDestroyed[0]) UndrawCannon(40,510);
    if (!CannonDestroyed[1]) UndrawCannon(400,510);
    if (!CannonDestroyed[2]) UndrawCannon(760,510);
    UndrawMouse();
    HandleMissile();
    if (HighDetail) HandleShot2(); else HandleShot();
    HandleFlyer();
    HandleExplosion(1);
    HandleBomb();
    HandleCursor();
    DrawMissile();
    if (HighDetail) DrawShot2(); else DrawShot();
    DrawBomb();
    DrawFlyer();
    DrawExplosion();
    if (!CannonDestroyed[0]) DrawCannon(40,510);
    if (!CannonDestroyed[1]) DrawCannon(400,510);
    if (!CannonDestroyed[2]) DrawCannon(760,510);
#ifdef JOYSTICK
    JoystickMove();
#endif
    DrawMouse();
    // Are all cities destroyed?
    if (!endgame) {
        endgame=1;
	for (i=0;i<6;i++)
	    if (CityDestroyed[i]==0)
		endgame=0;
    }
    // New Missiles
    if ((NewMissile<0)&&(Wave[WaveNum].MissilesToDestroy>MissilesCreated)) {
	Rand=abrand(0,99);
	if ((Rand<Wave[WaveNum].MirvChance)&&(MissileCount>0)) {
	    i=abrand(0,MissileCount-1);
	    b=0;
	    while ((Split(i))&&(b<100)) {
		i=abrand(0,MissileCount-1);
		b++;
	    }
	} else if (Rand>99-Wave[WaveNum].FlyerChance) {
	    CreateFlyer();
	} else if (Rand>99-Wave[WaveNum].BombChance-Wave[WaveNum].FlyerChance) {
	    CreateBomb();
	} else
	    CreateMissile(abrand(100,700),0);
	NewMissile+=Wave[WaveNum].TimeBetweenShots;
	MissilesCreated++;
    }
    NewMissile-=Speed;
    // New Wave
    if (
	(MissilesCreated==Wave[WaveNum].MissilesToDestroy)
	&&(MissileCount==0)
	&&(ExplosionCount==0)
	&&(BombCount==0)
	&&(FlyerCount==0)
	&&(ShotCount==0)
	) {
        WaveScoring();
	WaveNum++;
	StartWave();
    }    
}

void ShowRules()
{
    int i;

    Blit(0,0,BackPic);
    for (i=0;i<6;i++)
	if (i<3)
	    Blit(70+i*100,490,CityPic);
	else
	    Blit(130+i*100,490,CityPic);
    
    XCenteredString(Screen,50,"< Arcade Mode >");
    PutString(Screen,10,160,"-No extra points for remaining shots.");
    PutString(Screen,10,200,"-More cities result in faster shot regeneration.");
    PutString(Screen,10,240,"-One city is regenerated per wave.");
    PutString(Screen,10,280,"-The game is more difficult.");
    XCenteredString(Screen,400,"click to continue");
    SDL_BlitSurface(Screen, NULL, BackBuffer, NULL);
    Update();
    while (SDL_GetMouseState(NULL,NULL)==SDL_BUTTON(1)) {
	SDL_PollEvent(&event);
	SDL_Delay(10);
    }
    while ((event.type!=SDL_KEYDOWN)&&(event.type!=SDL_MOUSEBUTTONDOWN)) {
	SDL_PollEvent(&event);
	UndrawMouse();
	DrawMouse();
	GetSpeed();
	Update();
    }
}

void StartGame()
{
    int i;
    
    InitWaves();
    for (i=0;i<6;i++) CityDestroyed[i]=0;
    for (i=0;i<3;i++) CannonDestroyed[i]=0;
    ShotCount=0;
    MissileCount=0;
    ExplosionCount=0;
    BombCount=0;
    FlyerCount=0;
    WaveNum=StartLevel;
    Score=0;
    NewCity=0;
    MissilesCreated=0;

    if (ArcadeMode) ShowRules();
    SDL_SetEventFilter(GameEvents);
    StartWave();
    Now=SDL_GetTicks();
    Flicker=1;endgame=0;
    while ((!endgame)&&(!Quit)) {
        FullUpdate();
        GetSpeed();
	SDL_PollEvent(&event);
	ProcessEvents();
        //SDL_Delay(60);
    }
#ifdef USE_SOUND
//    Mix_FadeOutMusic(4000);
#endif
    PlaySound(GameoverSound);
    i=0;
    if (!Quit) 
	while (i<2000) {
    	    FullUpdate();
    	    GetSpeed();
	    ProcessEvents();
	    i+=Speed;
	}
    Flicker=0;
    printf("FPS %f\n",FramesPerSecond);
    SDL_SetEventFilter(NormalEvents);
}

