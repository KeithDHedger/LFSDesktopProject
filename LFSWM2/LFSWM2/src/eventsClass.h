/*
 *
 * ©K. D. Hedger. Fri 29 Jul 13:51:57 BST 2022 keithdhedger@gmail.com

 * This file (eventsClass.h) is part of LFSWM2.

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

#ifndef _EVENTSCLASS_
#define _EVENTSCLASS_

#include <X11/Xlib.h>

class LFSWM2_Class;
#include "lfswm2Class.h"
#include "messageClass.h"

class LFSWM2_eventsClass
{
	public:
		LFSWM2_eventsClass(LFSWM2_Class *mainclass);
		~LFSWM2_eventsClass(void);

		void			LFSWM2_mainEventLoop(void);
		void			LFSWM2_sendConfigureEvent(Window wid,struct rectStruct r);
		void			LFSWM2_doClientMsg(Window id,XClientMessageEvent *e);
		void			LFSWM2_moveToTop(Window id);
		void			LFSWM2_moveToBottom(Window id);
		void			LFSWM2_restack(void);


//vars
		LFSWM2_Class	*mainClass;
	private:
};

#endif
