/*
 *
 * ©K. D. Hedger. Fri 29 Jul 13:49:05 BST 2022 keithdhedger@gmail.com

 * This file (windowsClass.h) is part of LFSWM2.

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

#ifndef _WINDOWSCLASS_
#define _WINDOWSCLASS_

#include <X11/Xutil.h>
#include <string.h>
#include <iostream>

class LFSWM2_Class;
#include "lfswm2Class.h"
#include "clientClass.h"

enum {NORMALWINDOW=0,DESKTOPWINDOW,DOCKWINDOW,MENUWINDOW,DIALOGWINDOW,TOOLWINDOW,UNKNOWNTYPE};

struct fontColour
{
	XftColor			color;
	XftDraw			*draw;
	Visual			*visual;
	Colormap			colormap;
	unsigned int		pixel;
};

struct themeStruct
{
	std::string						pathToTheme;
	int								titleBarHeight;
	int								leftWidth;
	int								rightWidth;
	int								bottomHeight;
	int								buttonOffset;
	int								buttonYOffset;
	int								buttonXSpacing;
	int								closeButtonSize;
	int								middleOffset;
	int								middleAdjust;
	int								titleAlignment;
	bool								fullWidthTitle;
	std::map<unsigned long,bool>		gotPart;
	std::map<unsigned long,int>		partsWidth;
	std::map<unsigned long,int>		partsHeight;
	std::map<unsigned long,Pixmap>	pixmaps;
	std::map<unsigned long,Pixmap>	masks;
};

static const char *themePartNames[]=
{
	"top-left-active",
	"top-left-inactive",
	"top-right-active",
	"top-right-inactive",
	"title-1-active",
	"title-1-inactive",
	"title-2-active",
	"title-2-inactive",
	"title-3-active",
	"title-3-inactive",
	"title-4-active",
	"title-4-inactive",
	"title-5-active",
	"title-5-inactive",
	"left-active",
	"left-inactive",
	"right-active",
	"right-inactive",
	"bottom-left-active",
	"bottom-left-inactive",
	"bottom-active",
	"bottom-inactive",
	"bottom-right-active",
	"bottom-right-inactive",
	"close-active",
	"close-inactive",
	"close-prelight",
	"close-pressed",
	"maximize-active",
	"maximize-inactive",
	"maximize-prelight",
	"maximize-pressed",
	"hide-active",
	"hide-inactive",
	"hide-prelight",
	"hide-pressed",
	"shade-active",
	"shade-inactive",
	"shade-prelight",
	"shade-pressed",
	"menu-active",
	"menu-inactive",
	"menu-prelight",
	"menu-pressed",
	NULL
};

class LFSWM2_windowClass
{
	public:
		LFSWM2_windowClass(LFSWM2_Class		*mainclass);
		~LFSWM2_windowClass(void);
		void									init(void);

//properties
		void									*LFSWM2_getProp(Window w,Atom prop,Atom type,long unsigned int *nitems_return);
		void									LFSWM2_setProp(Window w,Atom prop,Atom type,int fmt,void *ptr,int nelem);
		void									*LFSWM2_getFullProp(Window w,Atom prop,Atom type,int fmt,unsigned long *rcountp);
		void									LFSWM2_removeProp(Window w,Atom state);
		char									*LFSWM2_decodeTextProp(XTextProperty *p);

//windows
		int									LFSWM2_getWindowType(Window id);
		void									LFSWM2_addState(Window w,Atom state);
		void									LFSWM2_setWindowState(Window w,long state);
		void									LFSWM2_changeState(Window id,int how,Atom state);
		bool									LFSWM2_hasState(Window w,Atom state);

		hintsDataStruct						LFSWM2_getWindowHints(Window wid,bool movewindow=false);
		rectStruct							LFSWM2_getWindowRect(Window id,Window parent,bool dotranslate=true);
		Window								LFSWM2_getParentWindow(Window id);
		void									LFSWM2_reloadWindowState(Window id);

		void									LFSWM2_resizeWindow(Window id,int w,int h);
		void									LFSWM2_moveWindow(Window id,int x,int y);
		void									LFSWM2_moveResizeWindow(Window id,int x,int y,int w,int h);

		void									LFSWM2_setVisibilityForDesk(unsigned long desk);

//clients
		void									LFSWM2_buildClientList(void);
		void									LFSWM2_createClient(Window id);
		void									LFSWM2_destroyClient(Window id);
		LFSWM2_clientClass					*LFSWM2_getClientClass(Window id);
		void									LFSWM2_deleteClientEntry(Window id);
		void									LFSWM2_setClientList(Window id,bool addwindow);

//graphics
		struct fontColour					*LFSWM2_xftLoadColour(const char *name,const char *fallback);
//theme
		themeStruct							theme;
		void									LFSWM2_loadTheme(std::string);
		void									LFSWM2_refreshFrame(LFSWM2_clientClass *cc,XExposeEvent *e=NULL);
		void 								LFSWM2_refreshThemeFrame(LFSWM2_clientClass *cc);

//		std::string							leftTop;
//		std::string							middleTop;
//		std::string							rightTop;
//		std::string							leftSide;
//		std::string							rightSide;
//		std::string							leftBottom;
//		std::string							middleBottom;
//		std::string							rightBottom;

//vars
		LFSWM2_Class							*mainClass;
//clients
		std::map<Window,LFSWM2_clientClass*>	clientList;
//windows
		std::vector<Window>					windowIDList;
};

#endif
