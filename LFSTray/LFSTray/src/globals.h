/*
 *
 * ©K. D. Hedger. Tue 21 Jan 13:03:10 GMT 2025 keithdhedger@gmail.com

 * This file (globals.h) is part of LFSTray.

 * LFSTray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSTray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSTray.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _GLOBALS_
#define _GLOBALS_

//#define __DEBUG__
#include <X11/Xatom.h>

#include <lfstk/LFSTKGlobals.h>
#include <Imlib2.h>
#include <X11/extensions/shape.h>

#include "config.h"

template <typename t> void move(std::vector<t>& v,size_t oldIndex,size_t newIndex)
{
	if(oldIndex>newIndex)
		std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
	else        
		std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
}

struct sysIcons
{
	Window		iconWindow;
	Window		parentWindow;
	std::string	windowName;
	int			x;
	int			y;
	int			w;
	int			h;
	bool			embedded;
	Pixmap		background;
	bool			done=false;
};

class LFSTray_embedClass;

#include "trayClass.h"
#include "embedClass.h"

#include "callbacks.h"

// XEMBED messages
#define XEMBED_EMBEDDED_NOTIFY 0
// Flags for _XEMBED_INFO
#define XEMBED_MAPPED (1 << 0)

/* defined in the systray spec */
#define SYSTEM_TRAY_REQUEST_DOCK 0
#define SYSTEM_TRAY_BEGIN_MESSAGE 1
#define SYSTEM_TRAY_CANCEL_MESSAGE 2

extern bool					xerror;
extern int					errorcode;
extern std::string			doingwhat;

int windowErrorHandler(Display *d,XErrorEvent *e);

#ifdef __DEBUG__
void DEBUG_printEventData(XEvent *e,bool verbose);
void DEBUG_printAtom(Atom a);
void DEBUG_showIcons(void);
#endif

#endif
