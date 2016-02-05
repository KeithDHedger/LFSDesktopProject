/*
 *
 * ©K. D. Hedger. Thu 23 Jul 16:15:33 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKwindow.h) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LFSTKWINDOW_
#define _LFSTKWINDOW_

#include <X11/Xlib.h>
#include <X11/Xutil.h>

//#include "LFSTKLib.h"
//#include "LFSTKGadget.h"
//#include "LFSTKGlobals.h"

struct	monitorStruct
{
	int		x;
	int		y;
	int		w;
	int		h;
};

//struct	gadgetList;
struct	gadgetList
{
//	gadgetList			*prev;
//	gadgetList			*next;
//	LFSTK_gadgetClass	*gadget;
};

/**
 *
 * \brief Window class for LFSToolKit.
 *
 * Basic window class for LFSToolKit.
 *
 * \note The main window class MUST be passed to child widgets.
 *
 */

class LFSTK_windowClass
{
	public:
		LFSTK_windowClass();
		~LFSTK_windowClass();
		LFSTK_windowClass(int x,int y,int w,int h,const char* name,bool override,bool loadvars=true);

		void LFSTK_showWindow(bool all=true);
		void LFSTK_hideWindow(void);

		void LFSTK_clearWindow(void);
		unsigned long LFSTK_setColour(const char *name);
		void LFSTK_resizeWindow(int w,int h,bool tellx=true);
		void LFSTK_moveWindow(int x,int y,bool tellx=true);

		void LFSTK_setListener(Window w,listener *l);
		listener *LFSTK_getListener(Window w);

		void LFSTK_setFontString(const char *s);
		void LFSTK_setFontColourName(int p,const char *colour);
		void LFSTK_setWindowColourName(int p,const char* colour);
		void LFSTK_setDecorated(bool isDecorated);
		void LFSTK_setKeepAbove(bool set);
		void LFSTK_setActive(bool set);
		bool LFSTK_getActive(void);
		void LFSTK_setSticky(bool set);
		bool LFSTK_getSticky(void);
		void LFSTK_setWindowType(const char *type);

		int LFSTK_getMonitorCount(void);
		const monitorStruct* LFSTK_getMonitorData(int monitor);
		const monitorStruct* LFSTK_getMonitors(void);
	
		void LFSTK_reloadGlobals(void);
		void LFSTK_setXProperty(Atom property,Atom type,int format,void *dataptr,int propcnt);
		void LFSTK_sendMessage(const char *msg,unsigned long data0,unsigned long data1,unsigned long data2,unsigned long data3,unsigned long data4);

		geometryStruct	*LFSTK_getGeom(void);
		int LFSTK_windowOnMonitor(void);

		void LFSTK_setTile(const char *path,int size);

		Display			*display;
		Window			window;
		GC				gc;
		int				screen;
		Visual			*visual;
		Window			rootWindow;		
		Colormap		cm;
		XContext		listeners;
		XftDraw 		*draw;

		char			*fontString;
		char			*fontColourNames[MAXCOLOURS];
		colourStruct	windowColourNames[MAXCOLOURS];
		bool			autoLabelColour;
		LFSTK_lib		*globalLib;

		fontStruct		*font;

		Window			parentWindow;
		int				x;
		int				y;
		Pixmap			tile[2];
		bool			useTile;

	private:
		void initWindow(bool loadvars);
		void loadGlobalColours(void);
		void loadMonitorData(void);
		int				w;
		int				h;
		char			*windowName;
		bool			isActive;
		bool			isSticky;
		int				monitorCount;
		monitorStruct	*monitors;
		
};

#endif
