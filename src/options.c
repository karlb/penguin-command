#include "main.h"
#include <errno.h>
#include <string.h>

char Player[10][200];
int Hiscore[10];

FILE *OpenOptionsFile( char *mode )
{
    char filename[200];
    FILE *file;
    
    if ((char *)getenv("HOME") != NULL)
		snprintf( filename, sizeof(filename), "%s/.penguin-command", getenv("HOME"));
    else
		sprintf( filename, "penguin-command.dat");

    file = fopen(filename, mode);
    
    if (file==NULL) {
	fprintf(stderr, "\nWarning: I could not open the options file ");
	if (strcmp(mode, "r") == 0)
	    fprintf(stderr, "for read:");
	else if (strcmp(mode, "w") == 0)
	    fprintf(stderr, "for write:");

	fprintf(stderr, "\n%s\n"
	    "The error that occured was:\n"
	    "%s\n\n", filename, strerror(errno));
    }
    
    return file;
}

void WriteOptions()
{
    FILE *file;
    int i;

    file=OpenOptionsFile("w");
    if (file==NULL) exit(1);
    
    fprintf(file, "# Penguin Command Options File\n\n");
    for (i=0;i<10;i++)
	fprintf(file, "%s\n", Player[i]);
    for (i=0;i<10;i++)
	fprintf(file, "%d\n", Hiscore[i]);
    fprintf(file, "%d\n",LowDetail);
    fprintf(file, "%d\n",MediumDetail);
    fprintf(file, "%d\n",HighDetail);
    fprintf(file, "%d\n",MusicVol);
    fprintf(file, "%d\n",SoundVol);

    fclose(file);
}

void ReadOptions()
{
    FILE *file;
    int i;
        
    file=OpenOptionsFile("r");
    
    if (file==NULL) {
	file=OpenOptionsFile("w");
	if (file==NULL) exit(1);
	
	fprintf(file, "# Penguin Command Options File\n\n");
	fprintf(file, "Sam\nLion\nTux\nThue\nTesmako\nChristian\nAkawaka\nDust Puppy\nMichael\nAndreas\n");
	for (i=0;i<10;i++)
	    fprintf(file, "%d\n", 5000-i*500);
	
	fclose(file);
	file=OpenOptionsFile("r");
    }

    fscanf(file, "%*s%*s%*s%*s%*s\n");
    for (i=0;i<10;i++)
	fscanf(file, "%[^\n]\n",Player[i]);
    for (i=0;i<10;i++)
	fscanf(file, "%d\n",&Hiscore[i]);
    if   ((fscanf(file, "%d\n",&LowDetail)==EOF)
	||(fscanf(file, "%d\n",&MediumDetail)==EOF)
	||(fscanf(file, "%d\n",&HighDetail)==EOF)
	||(fscanf(file, "%d\n",&MusicVol)==EOF)
	||(fscanf(file, "%d\n",&SoundVol)==EOF))
	WriteOptions();
//    for (i=0;i<10;i++)
//	printf("%s %d\n",Player[i],Hiscore[i]);
    
    fclose(file);
}

void FinalScore()
{
    int i;

    if (Score>Hiscore[9]) {

    char Name[200]="",text[200];
    int a;

    Blit(0,0,BackPic);
    PutString(Screen,400-SFont_TextWidth("Final Score")/2,200,"Final Score");
    sprintf(text,"%d", Score);
    PutString(Screen,400-SFont_TextWidth(text)/2,250,text);
    PutString(Screen,400-SFont_TextWidth("Enter Your Name:")/2,300,"Enter Your Name:");
    Update();
    SFont_Input(Screen, 260, 350, 280, Name);
    if (strcmp(Name,"")==0) strcpy(Name,"Anonym. Coward");
    for (i=0;i<10;i++) {
	if (Score>Hiscore[i]) {
	    // Move other players down on the list
	    for (a=9;a>=i;a--) {
		strcpy(Player[a],Player[a-1]);
		Hiscore[a]=Hiscore[a-1];		
	    }
	    // add the player
	    strcpy(Player[i],Name);
	    Hiscore[i]=Score;
	    break;
	}
    }
//    for (i=0;i<10;i++)
//	printf("%s %d\n",Player[i],Hiscore[i]);
    
    WriteOptions();
}}

void ShowHiscore()
{
    char text[200];
    int i;
    SDL_Event WaitEvent;
    
    BlitToBB(0,0,BackPic);
    PutString(BackBuffer,400-SFont_TextWidth("<Penguin Command Hiscores>")/2,70,"<Penguin Command Hiscores>");
    for (i=0;i<10;i++) {
	sprintf(text,"%s", Player[i]);
	PutString(BackBuffer,200,150+i*30,text);
	sprintf(text,"%d", Hiscore[i]);
	PutString(BackBuffer,600-SFont_TextWidth(text),150+i*30,text);
    }
    FadeScreen(1);
    SDL_WaitEvent(&WaitEvent);
    while ((WaitEvent.type!=SDL_KEYDOWN)&&(WaitEvent.type!=SDL_MOUSEBUTTONDOWN)) {
	SDL_PollEvent(&WaitEvent);
	UndrawMouse();
	DrawMouse();
	Update();
	SDL_WaitEvent(&WaitEvent);
    }
    Now=SDL_GetTicks();                          
}
