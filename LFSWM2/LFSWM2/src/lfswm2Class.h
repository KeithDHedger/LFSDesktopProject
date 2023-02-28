/*
 *
 * ©K. D. Hedger. Fri 29 Jul 13:40:42 BST 2022 keithdhedger@gmail.com

 * This file (lfswm2Class.h) is part of LFSWM2.

 * LFSWM2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSWM2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWM2.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LFSWM2CLASS_
#define _LFSWM2CLASS_

#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <X11/Xatom.h>
#include <X11/Xft/Xft.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xinerama.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <getopt.h>

#include <lfstk/LFSTKGlobals.h>

#include "config.h"

//#define __DEBUG__

template <typename t> void move(std::vector<t>& v,size_t oldIndex,size_t newIndex)
{
	if(oldIndex>newIndex)
		std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
	else        
		std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
}

class LFSWM2_windowClass;
class LFSWM2_eventsClass;
class LFSWM2_clientClass;
class LFSWM2_messageClass;

#include "eventsClass.h"
#include "windowsClass.h"
#include "clientClass.h"
#include "messageClass.h"

#define BORDER_WIDTH 0
#define CONTROL_GAP 24

#define deleteWindowSize 8
__attribute__((unused)) static unsigned char deleteWindowBits[]=
{
	0x81,0x42,0x24,0x18,0x18,0x24,0x42,0x81
};

#define shadeWindowSize 8
__attribute__((unused)) static unsigned char shadeWindowBits[]=
{
   0xff, 0x00, 0xff, 0x7e, 0x3c, 0x18, 0x00, 0x00
};

#define minimizeWindowSize 8
__attribute__((unused)) static unsigned char minimizeWindowBits[]=
{
   0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00
};

#define maximizeWindowSize 8
__attribute__((unused)) static unsigned char maximizeWindowBits[]=
{
   0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18
};

#define NET_WM_STATE_REMOVE 0
#define NET_WM_STATE_ADD 1
#define NET_WM_STATE_TOGGLE 2

//source (data[0]) client messgae:0=no source  1=application 2=pager
enum MESSAGESOURCE {NOSRC=0,APPLICATIONSRC,PAGERSRC};
enum MESSAGETYPE {REFRESHTHEME,QUITLFSWM,RESTARTLFSWM,NOMSG};
enum TRISTATE {INVALID=-1,LOGIC0=0,LOGIC1=1};

struct controlData
{
	rectStruct	boundingBox;
	rectStruct	pixmapBox;
	int			startX;
};

__attribute__((unused)) static LFSWM2_Class	*theMainClass=NULL;

class LFSWM2_Class
{
	public:
		LFSWM2_Class(int argc,char **argv);
		~LFSWM2_Class(void);

		void					LFSWM2_initRootWindow(void);
		void					LFSWM2_setDeskCount(unsigned long val);
		unsigned long		LFSWM2_getDesktopCount(void);
		void					LFSWM2_setCurrentDesktop(unsigned long i,bool force=false,bool dovis=true);

		static int			LFSWM2_wmDetected(Display *display,XErrorEvent *e);
		static int			LFSWM2_xError(Display *display,XErrorEvent *e);
		static int			LFSWM2_xWarnings(Display *display,XErrorEvent *e);

		void					LFSWM2_pushXErrorHandler(void);
		void					LFSWM2_popXErrorHandler(void);

//vars
		unsigned int			numberOfDesktops=6;
		unsigned long		currentDesktop=0;
		Display				*display;
		Window				rootWindow;
		Visual				*defaultVisual;
		Colormap				defaultColourmap;
		int					blackColour;
		int					whiteColour;
		GC					mainGC;
		unsigned int			screen;
		int					depth;
		int					numberOfMonitors;
		std::vector<rectStruct>	monitors;

		int					displayWidth;
		int					displayHeight;
		Window				supporting;
		std::map<const char*,Atom>	atoms;

//theme
//colours
		struct fontColour	*frameBG;
		struct fontColour	*frameFG;
		struct fontColour	*frameText;
		XftFont				*frameFont;
		int					titleBarSize=20;
		int					sideBarSize=2;
		int					bottomBarSize=8;
		int					titlePosition=2;

		struct controlData	closeControlStruct;
		struct controlData	maximizeControlStruct;
		struct controlData	minimizeControlStruct;
		struct controlData	shadeControlStruct;
		Pixmap				closeBitMap;
		Pixmap				maximizeBitMap;
		Pixmap				minimizeBitMap;
		Pixmap				shadeBitMap;

		Cursor				topLeftCursor;
		Cursor				topRightCursor;
		Cursor				rightCursor;
		Cursor				bottomRightCursor;
		Cursor				bottomCursor;
		Cursor				bottomLeftCursor;
		Cursor				leftCursor;
		Cursor				rootCursor;
		int					restackCnt=-1;
		bool					doingMove=false;

		LFSWM2_eventsClass	*mainEventClass;
		LFSWM2_windowClass	*mainWindowClass;
		LFSWM2_messageClass	*messages;
		LFSTK_lib			*lfstkLib;

#ifdef __DEBUG__
		void					DEBUG_printAtom(Atom a);
		void					DEBUG_printEventData(XEvent *e,bool verbose);
		void					DEBUG_printRect(rectStruct r);
#endif

		bool		tb=false;
		int					msgQueueKey=999;
	private:
		void					cliOptions(int argc,char **argv);
		void					printHelp(void);

//vars
		XErrorHandler		lastXErrorHandler;
};

#endif
