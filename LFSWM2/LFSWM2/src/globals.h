#ifndef _GLOBALS_
#define _GLOBALS_

#include "config.h"

#ifdef __DEBUG__
	#define MOVEKEYS (Mod4Mask|ControlMask|ShiftMask)
	#define SHOWXERRORS  errorFile=__FILE__;errorLine=__LINE__;
	#define HIDEXERRORS errorFile=NULL;errorLine=-1;
#else
	#define MOVEKEYS (Mod4Mask)
	#define SHOWXERRORS
	#define HIDEXERRORS
#endif

extern const char	*errorFile;
extern int			errorLine;

int xError(Display *display,XErrorEvent *e);


#endif