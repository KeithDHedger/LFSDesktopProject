/*
 *
 * ©K. D. Hedger. Tue  9 Aug 20:42:29 BST 2022 keithdhedger@gmail.com

 * This file (clientClass.h) is part of LFSWM2.

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

#ifndef _CLIENTCLASS_
#define _CLIENTCLASS_

#include <X11/Xlib.h>

class LFSWM2_Class;
#include "lfswm2Class.h"

class LFSWM2_clientClass
{
	public:
		LFSWM2_clientClass(LFSWM2_Class *mainclass,Window id);
		~LFSWM2_clientClass(void);

		void				LFSWM2_setWindowName(void);

		bool				LFSWM2_handleControls(XEvent *e);

//vars
		LFSWM2_Class		*mainClass;

		Window			contentWindow=0;
		Window			frameWindow=0;
		std::string		name="";
		int				windowType=-1;
		bool				visible=false;
		Desk				onDesk=0;
		bool				visibleOnAllDesks=false;
		bool				onTop=false;
		bool				onBottom=false;
		bool				isMaximized=false;
		bool				isMinimized=false;
		rectStruct		clientWindowRect;
		rectStruct		frameWindowRect;
		rectStruct		clientPreMaxRect;
		rectStruct		framePreMaxRect;
		bool				nameIsUTF=false;
		bool				moveToTop=false;
	
		Window			closeButton;
		Window			maximizeButton;
		Window			minimizeButton;
		Window			shadeButton;

		void				LFSWM2_showWindow(void);
		void				LFSWM2_hideWindow(void);
		void				LFSWM2_minWindow(void);
		void				LFSWM2_maxWindow(void);
		void				LFSWM2_sendCloseWindow(void);
		void				drawMouseLeave(Window id,Pixmap pm,struct controlData data);
	private:
		void				drawMouseEnter(Window id,controlData data,Pixmap pm);
};

#endif
