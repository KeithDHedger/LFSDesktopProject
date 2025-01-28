/*
 *
 * ©K. D. Hedger. Tue 21 Jan 13:07:30 GMT 2025 keithdhedger@gmail.com

 * This file (callbacks.cpp) is part of LFSTray.

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

#include "globals.h"

bool eventCB(LFSTK_applicationClass *p,XEvent *event)
{
	switch(event->type)
		{
		case ClientMessage:
			//fprintf(stderr,">>>>>ClientMessage>>>\n");
			{
				Window win;
				unsigned long opcode=event->xclient.data.l[1];
				switch (opcode)
					{
					case SYSTEM_TRAY_REQUEST_DOCK:
						{
							win=event->xclient.data.l[2];
							trayClass->firstrun=100;
							bool retval=trayClass->embedClass->addIcon(win);
							trayClass->embedClass->embedWindow(win);
							trayClass->resetWindow();
							trayClass->firstrun=0;
						}
					break;

					case SYSTEM_TRAY_BEGIN_MESSAGE:
					case SYSTEM_TRAY_CANCEL_MESSAGE:
						fprintf(stderr,"N_MESSAGE\n");
						// we don't show baloons messages.
						break;

					default:
						//fprintf(stderr,"got wid=%p\n",event->xany.window);
						if(opcode==trayClass->_NET_SYSTEM_TRAY_MESSAGE_DATA)
							fprintf(stderr,"lfstray: message from dockapp: %s\n",event->xclient.data.b);
						else
							fprintf(stderr, "lfstray: SYSTEM_TRAY : unknown message type"  "\n");
						break;
					}
			}
			break;
		case DestroyNotify:
			if(trayClass->iconList.count(event->xdestroywindow.window)>0)
				{
					trayClass->firstrun=100;
					trayClass->embedClass->removeIcon(event->xdestroywindow.window);
					trayClass->resetWindow();
					trayClass->embedClass->refreshIcons();
					XDestroyWindow(trayClass->apc->display,trayClass->embedClass->currentIcon.parentWindow);
					XFreePixmap(trayClass->apc->display,trayClass->embedClass->currentIcon.background);
					trayClass->firstrun=0;
				}
			break;

		case ConfigureNotify:
			if(trayClass->firstrun<5)
				{
					trayClass->embedClass->refreshIcons();
					trayClass->firstrun++;
				}
				break;
		default:
			break;
		}
	return(false);

}