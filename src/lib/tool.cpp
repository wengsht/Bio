#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>
#include "tool.h"
#include <math.h>

static int getch() {
    struct termios oldt,
    newt;
    int ch;
    tcgetattr( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
    return ch;
}

static int getche() {
    int ch = getch();
    printf("%c",ch);
    return ch;
}

void Warn(const char *msg,...){
    //if(! TESTING) return;
	char Buffer[128];
	va_list ArgList;
	va_start (ArgList, msg);
	vsprintf (Buffer, msg, ArgList);
	va_end (ArgList);
    toupper(Buffer[0]);
	printf("" LIGHT_RED"WARNNING: ");
	printf ("%s", Buffer);
	printf(NONE "\n");
}

void Tip(const char * msg,...){
    char Buffer[128];
	va_list ArgList;
	va_start (ArgList, msg);
	vsprintf (Buffer, msg, ArgList);
	va_end (ArgList);
//	printf("Tip: ");
    toupper(Buffer[0]);
	printf ("%s", Buffer);
	printf(NONE "\n");
}

void Log(const char * msg,...){
    if(! TESTING) return;
	char Buffer[128];
	va_list ArgList;
	va_start (ArgList, msg);
	vsprintf (Buffer, msg, ArgList);
	va_end (ArgList);
    toupper(Buffer[0]);
    printf(BLUE);
	printf("Log: ");
	printf ("%s", Buffer);
	printf(NONE "\n");
}

void ErrorLog(const char *msg,...){
    //if(! TESTING) return;
	char Buffer[128];
	va_list ArgList;
	va_start (ArgList, msg);
	vsprintf (Buffer, msg, ArgList);
	va_end (ArgList);
    toupper(Buffer[0]);
	printf("\n" LIGHT_RED"### ERROR: ");
	printf ("%s", Buffer);
	printf(NONE "\n\n");
    fflush(stdin);
    getch();
}

void WarnLog(const char *msg,...){
    if(! TESTING) return;
	char Buffer[128];
	va_list ArgList;
	va_start (ArgList, msg);
	vsprintf (Buffer, msg, ArgList);
	va_end (ArgList);
    toupper(Buffer[0]);
	printf("\nWARNNING: ");
	printf ("%s", Buffer);
	printf("\n\n");
    fflush(stdin);
    getch();
}

bool tooClose(double a, double b) {
    if(fabs(a-b) <= eps)
        return true;
    return false;
}


