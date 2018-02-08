/******************************************************
*
*     ©keithhedger Sun  3 May 19:46:41 BST 2015
*     kdhedger68713@gmail.com
*
*     prefs.h
* 
******************************************************/

#ifndef _PREFS_
#define _PREFS_

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/Xdbe.h>

#include "wind.h"

extern args				wmPrefs[];
extern args				themeRC[];

void saveVarsToFile(const char* filepath,args* dataptr,const char *fs);
bool loadVarsFromFile(char* filepath,args* dataptr,const char *fs);

#endif
