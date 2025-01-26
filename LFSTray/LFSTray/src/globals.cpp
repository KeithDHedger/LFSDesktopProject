/*
 *
 * ©K. D. Hedger. Tue 21 Jan 13:02:58 GMT 2025 keithdhedger@gmail.com

 * This file (globals.cpp) is part of LFSTray.

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

bool			xerror=false;
int			errorcode=0;
std::string	doingwhat;

int windowErrorHandler(Display *d,XErrorEvent *e)
{
	char		buffer_return[512];
	int		length;
	xerror=true;
	errorcode=e->error_code;
	fprintf(stderr,"%s\n",doingwhat.c_str());
	XGetErrorText(trayClass->apc->display,errorcode, &buffer_return[0],256);
	fprintf(stderr,"error text=%s\n",buffer_return);
	xerror=false;
	return 0;
}

void DEBUG_printEventData(XEvent *e,bool verbose)
{
	const char* names[][2]={{"ERROR","0"},{"ERROR","0"},{"KeyPress","0"},{"KeyRelease","0"},{"ButtonPress","0"},{"ButtonRelease","0"},{"MotionNotify","0"},{"EnterNotify","0"},{"LeaveNotify","0"},{"FocusIn","0"},{"FocusOut","0"},{"KeymapNotify","0"},{"Expose","0"},{"GraphicsExpose","0"},{"NoExpose","0"},{"VisibilityNotify","0"},{"CreateNotify","1"},{"DestroyNotify","1"},{"UnmapNotify","1"},{"MapNotify","1"},{"MapRequest","1"},{"ReparentNotify","0"},{"ConfigureNotify","1"},{"ConfigureRequest","1"},{"GravityNotify","0"},{"ResizeRequest","1"},{"CirculateNotify","0"},{"CirculateRequest","0"},{"PropertyNotify","1"},{"SelectionClear","0"},{"SelectionRequest","0"},{"SelectionNotify","0"},{"ColormapNotify","0"},{"ClientMessage","1"},{"MappingNotify","0"},{"GenericEvent","0"}};

	if(names[e->type][1][0]=='1')
		fprintf(stderr,"Event name=%s: type=%i window=%p\n",names[e->type][0],e->type,e->xany.window);
}

void DEBUG_printAtom(Atom a)
{
	char		*name=NULL;
	name=XGetAtomName(trayClass->apc->display,a);
	if(name!=NULL)
		{
			fprintf(stderr,"Atom %p name=%s\n",(void*)a,name);
			XFree(name);
		}
	else
		fprintf(stderr,"Unknown atom %p\n",(void*)a);
}

void DEBUG_showIcons(void)
{
	std::map<Window,sysIcons>::iterator it;
	sysIcons		icondata;

	fprintf(stderr,"DEBUG_showIcons\n");

	for (it=trayClass->iconList.begin(); it!=trayClass->iconList.end(); ++it)
		{
			icondata=it->second;
			fprintf(stderr,"wid=%p par=%p x=%i y=%i w=%i h=%i embedded=%i pixmap=%p\n",icondata.iconWindow,icondata.parentWindow,icondata.x,icondata.y,icondata.w,icondata.h,icondata.embedded,icondata.background);
 	   }
}
