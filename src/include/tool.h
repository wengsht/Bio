//
//  tool.h
//  SpeechRecongnitionSystem
//
//  Created by Admin on 9/7/14.
//  Copyright (c) 2014 Admin. All rights reserved.
//

#ifndef BIO_tool_h
#define BIO_tool_h

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <ctype.h>

///
/// Debug Tool
///

#define TESTING (true)

void Tip(const char * msg,...);
void Warn(const char * msg,...);
void Log(const char * msg,...);
void ErrorLog(const char *msg,...);
void WarnLog(const char *msg,...);

///
/// some tool, which is nothing to do with the algorithm logic
///
#define NONE         "\033[m"
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"

#define RED_BACK "\033[7;31m"
#define GREEN_BACK "\033[7;32m"
#define BLUE_BACK "\033[7;34m"

#define NONE_BACK "\033[0m"

#define INF 1e10
#define eps 1e-10
bool tooClose(double a, double b);


#endif


