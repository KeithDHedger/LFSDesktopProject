/*
 *
 * ©K. D. Hedger. Tue 21 Jan 15:57:44 GMT 2025 keithdhedger@gmail.com

 * This file (trayClass.h) is part of LFSTray.

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

#ifndef _TRAYCLASS_
#define _TRAYCLASS_

#include "globals.h"

enum TrayPos{NW=1,NE,SE,SW,N,E,S,W};

class LFSTray_trayClass
{
	public:
		LFSTray_trayClass(LFSTK_applicationClass *app);
		~LFSTray_trayClass(void);

		LFSTK_applicationClass		*apc;
		LFSTray_embedClass			*embedClass;
		std::map<Window,sysIcons>	iconList;

		Atom			_NET_SYSTEM_TRAY_SCREEN;
		Atom			MANAGER;
		Atom			_NET_SYSTEM_TRAY_OPCODE;
		Atom			_NET_SYSTEM_TRAY_MESSAGE_DATA;
		Atom			_NET_WM_NAME;
		Atom			UTF8_STRING;
		Atom			_NET_WM_PID;
		Atom			_XEMBED;

		std::string	doingwhat;
		int			iconSize=128;
		bool			vertical=false;
		int			onMonitor=0;
		TrayPos		gravity=SE;
		int			stacking=ABOVEALL;

		int			firstrun=0;
		bool			isbelow=false;
		int			debugmsg=0;

		void			setTrayAtoms(void);
		void*		getProperty(Window w,Atom prop,Atom type,long unsigned int *nitems_return);
		std::string	getWindowName(Window win);
		void			resetWindow(void);
		void			setStacking(int stackit);
		std::string	imagePath="";
		Pixmap		externalPixmap=None;
		Pixmap		externalMaskPixmap=None;

};
#endif
