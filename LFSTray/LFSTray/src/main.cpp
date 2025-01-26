/*
 *
 * ©K. D. Hedger. Tue 21 Jan 15:56:50 GMT 2025 keithdhedger@gmail.com

 * This file (main.cpp) is part of LFSTray.

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

#include <lfstk/LFSTKGlobals.h>

#include "globals.h"

LFSTray_trayClass	*trayClass=NULL;
LFSTK_windowClass	*transwc=NULL;

int main(int argc,char **argv)
{
	LFSTK_applicationClass	*apc=NULL;
	windowInitStruct			*wi=NULL;
	apc=new LFSTK_applicationClass;
	trayClass=new LFSTray_trayClass(apc);

	trayClass->vertical=false;
	trayClass->onMonitor=0;
	trayClass->gravity=NW;
	trayClass->iconSize=32;

	wi=apc->LFSTK_getDefaultWInit();
	wi->overRide=false;

	if(argc>1)
		{
			if(strcmp(argv[1],"-below")==0)
				trayClass->isbelow=true;
		}

	if(trayClass->isbelow==true)
		wi->level=BELOWALL;
	else
		wi->level=ABOVEALL;

	wi->decorated=false;
	wi->className="LFSDOCK";
	wi->windowType=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DOCK"));

	apc->LFSTK_addWindow(wi,"TRANSPARENT SUB WINDOW");
	transwc=apc->mainWindow;

	trayClass->setTrayAtoms();

	XSetSelectionOwner(apc->display,trayClass->_NET_SYSTEM_TRAY_SCREEN,transwc->window,CurrentTime);
	fprintf(stderr,"XGetSelectionOwner=%p\n",XGetSelectionOwner(apc->display,trayClass->_NET_SYSTEM_TRAY_SCREEN));
	if(XGetSelectionOwner(apc->display,trayClass->_NET_SYSTEM_TRAY_SCREEN) != transwc->window)
		{
			fprintf(stderr,"lfstray: cannot find systray manager"  "\n");
			return(1);
		}

	XClientMessageEvent ev;
	ev.type=ClientMessage;
	ev.window=apc->rootWindow;
	ev.message_type=trayClass->MANAGER;
	ev.format=32;
	ev.data.l[0]=CurrentTime;
	ev.data.l[1]=trayClass->_NET_SYSTEM_TRAY_SCREEN;
	ev.data.l[2]=transwc->window;
	ev.data.l[3]=0;
	ev.data.l[4]=0;
	XSendEvent(apc->display,apc->rootWindow,False,StructureNotifyMask, (XEvent *)&ev);

	transwc->LFSTK_setSticky(true);
	trayClass->resetWindow();

	printf("Number of gadgets in window=%i\n",transwc->LFSTK_gadgetCount());

	apc->LFSTK_setEventCallBack(eventCB,(void*)0xdeadbeef);

	int retval=apc->LFSTK_runApp();

	delete trayClass;
	delete apc;
	cairo_debug_reset_static_data();
	return retval;
}