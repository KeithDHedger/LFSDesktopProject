/*
 *
 * ©K. D. Hedger. Sun  4 Sep 14:01:13 BST 2016 kdhedger68713@gmail.com

 * This file (lfsfontselect.cpp) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <lfstk/LFSTKGlobals.h>

#define WIDTH 600
#define HITE 400
#define MAXPREVIEW 4

LFSTK_buttonClass	*previews[MAXPREVIEW];
LFSTK_windowClass	*mainWindow;

int main(int argc, char **argv)
{
	bool	mainLoop;
	XEvent	event;

	mainWindow=new LFSTK_windowClass(0,0,WIDTH,HITE,"Font Selector",false);

	for(int j=0;j<MAXPREVIEW;j++)
		{
			previews[j]=new LFSTK_buttonClass(mainWindow,"XXX",0,(j*24)+4,WIDTH,24,NorthWestGravity);
		}

	mainWindow->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",mainWindow->LFSTK_gadgetCount());
	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(mainWindow->display,&event);
			mappedListener *ml=mainWindow->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			switch(event.type)
				{
					case LeaveNotify:
						break;
					case Expose:
						mainWindow->LFSTK_clearWindow();
						break;

					case ConfigureNotify:
						mainWindow->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						break;

					case ClientMessage:
					case SelectionNotify:
						{
							if(event.xclient.message_type==XInternAtom(mainWindow->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(mainWindow->display, "WM_DELETE_WINDOW", 1))
							mainLoop=false;
						}
						break;
				}
		}


	mainWindow->LFSTK_hideWindow();	
	for(int j=0;j<MAXPREVIEW;j++)
		{
			delete previews[j];
		}
	delete mainWindow;
	return 0;
}
