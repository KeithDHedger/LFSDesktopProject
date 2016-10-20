/*
 *
 * ©K. D. Hedger. Mon  7 Sep 13:20:24 BST 2015 kdhedger68713@gmail.com

 * This file (lfswmprefs.cpp) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation,either version 3 of the License,or
 * at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not,see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "globals.h"
#include "appmenu.h"
#include "logout.h"
#include "clock.h"
#include "disks.h"
#include "cpu.h"
#include "windowlist.h"
#include "launchers.h"

#define RCNAME "lfspanel"

bool	mainLoop=true;

args	panelPrefs[]=
{
	{"panelheight",TYPEINT,&panelHeight},
	{"panelwidth",TYPEINT,&panelWidth},
	{"onmonitor",TYPEINT,&onMonitor},
	{"termcommand",TYPESTRING,&terminalCommand},
	{"logoutcommand",TYPESTRING,&logoutCommand},
	{"restartcommand",TYPESTRING,&restartCommand},
	{"shutdowncommand",TYPESTRING,&shutdownCommand},
	{"gadgetsright",TYPESTRING,&rightGadgets},
	{"gadgetsleft",TYPESTRING,&leftGadgets},
	{"panelpos",TYPEINT,&panelPos},
	{"panelgrav",TYPEINT,&panelGravity},
	{NULL,0,NULL}
};

void addLeftGadgets(void)
{
	int	offset=mons->x+leftOffset;

	for(int j=0; j<strlen(leftGadgets); j++)
		{
			switch(leftGadgets[j])
				{
				case 'A':
					offset+=addAppmenu(offset,mons->y,panelGravity,true);
					break;
				case 'W':
					offset+=addWindowMenu(offset,mons->y,panelGravity,true);
					break;
				case 'w':
					offset+=addWindowDeskMenu(offset,mons->y,panelGravity,true);
					break;
				case 'L':
					offset+=addLogout(offset,mons->y,panelGravity,true);
					break;
				case 'C':
					offset+=addClock(offset,mons->y,NorthWestGravity);
					break;
				case 'D':
					offset+=addDiskData(offset,mons->y,NorthWestGravity);
					break;
				case 'M':
					offset+=addCpuData(offset,mons->y,NorthWestGravity);
					break;
				case 'S':
					offset+=SPACING;
					break;
				case 'l':
					offset+=addLaunchers(offset,mons->y,panelGravity,true);
					break;
				}
		}
	offset++;
	leftOffset=offset;
}

void addRightGadgets(void)
{
	int	offset=rightOffset;
	for(int j=strlen(rightGadgets)-1; j>=0; j--)
		{
			switch(rightGadgets[j])
				{
				case 'A':
					offset-=addAppmenu(offset,mons->y,panelGravity,false);
					break;
				case 'W':
					offset-=addWindowMenu(offset,mons->y,panelGravity,false);
					break;
				case 'w':
					offset-=addWindowDeskMenu(offset,mons->y,panelGravity,false);
					break;
				case 'L':
					offset-=addLogout(offset,mons->y,panelGravity,false);
					break;
				case 'C':
					offset-=addClock(offset,mons->y,NorthEastGravity);
					break;
				case 'D':
					offset-=addDiskData(offset,mons->y,NorthEastGravity);
					break;
				case 'M':
					offset-=addCpuData(offset,mons->y,NorthEastGravity);
					break;
				case 'S':
					offset-=SPACING;
					break;
				case 'l':
					offset+=addLaunchers(offset,mons->y,panelGravity,true);
					break;
				}
		}
	rightOffset=offset;
}

int errHandler(Display *dpy,XErrorEvent *e)
{
	char buf[128];

	buf[0]=0;
	XGetErrorText(dpy,e->error_code,buf,sizeof buf);
	fprintf(stderr,"Xlib Error: %s - %s\n",buf,possibleError);
	return 0;
}

void printError(const char *err)
{
	fprintf(stderr,">>>%s<<<\n",err);
}

void  alarmCallBack(void)
{
	if(clockButton!=NULL)
		updateClock();

	if(diskButton!=NULL)
		updateDiskStats();

	if(cpuButton!=NULL)
		updateCpuStats();

	if((windowMenu!=NULL) || (windowDeskMenu!=NULL))
		updateWindowMenu();

	XFlush(mainwind->display);
}

bool XNextEventTimed(Display *dsp,XEvent *event_return,timeval *tv)
{
	if (tv== NULL)
		{
			XNextEvent(dsp,event_return);
			return(true);
		}

	if (XPending(dsp)==0)
		{
			int fd=ConnectionNumber(dsp);
			fd_set readset;
			FD_ZERO(&readset);
			FD_SET(fd,&readset);
			if (select(fd+1,&readset,NULL,NULL,tv) == 0)
				return false;
		}
	XNextEvent(dsp,event_return);
	return(true);
}

int main(int argc,char **argv)
{
	char			*env;
	XEvent			event;
	fontStruct		*tfont;
	const char		*itemfont;
	int				psize;
	int				thold;
	int				px,py;
	timeval			tv={0,0};

	useAlarm=false;
	terminalCommand=strdup("xterm -e ");
	logoutCommand=strdup("xterm");
	restartCommand=strdup("xterm");
	shutdownCommand=strdup("xterm");
	leftGadgets=strdup("A");
	rightGadgets=strdup("L");
	panelPos=PANELCENTRE;

	XSetErrorHandler(errHandler);

	mainwind=new LFSTK_windowClass(0,0,1,1,"lfs",true);
	WM_STATE=XInternAtom(mainwind->display,"WM_STATE",False);
	NET_WM_WINDOW_TYPE_NORMAL=XInternAtom(mainwind->display,"_NET_WM_WINDOW_TYPE_NORMAL",False);
	NET_WM_STATE_HIDDEN=XInternAtom(mainwind->display,"_NET_WM_STATE_HIDDEN",False);
	NET_WM_WINDOW_TYPE_DIALOG=XInternAtom(mainwind->display,"_NET_WM_WINDOW_TYPE_DIALOG",False);
	NET_WM_DESKTOP=XInternAtom(mainwind->display,"_NET_WM_DESKTOP",False);
	NET_WM_WINDOW_TYPE=XInternAtom(mainwind->display,"_NET_WM_WINDOW_TYPE",False);
	NET_WM_STATE=XInternAtom(mainwind->display,"_NET_WM_STATE",False);

	itemfont=mainwind->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMFONT);
	tfont=mainwind->globalLib->LFSTK_loadFont(mainwind->display,mainwind->screen,itemfont);
	iconSize=tfont->size;
	free(tfont);

	if(argc>1)
		{
			panelID=argv[1];
			asprintf(&env,"%s/.config/LFS/%s-%s.rc",getenv("HOME"),RCNAME,panelID);
		}
	else
		asprintf(&env,"%s/.config/LFS/%s.rc",getenv("HOME"),RCNAME);
	mainwind->globalLib->LFSTK_loadVarsFromFile(env,panelPrefs);

	desktopTheme=mainwind->globalLib->LFSTK_oneLiner("cat %s/.config/LFS/lfsdesktop.rc|grep icontheme|awk '{print $2}'",getenv("HOME"));
	mons=mainwind->LFSTK_getMonitorData(onMonitor);

	rightOffset=0;
	leftOffset=0;

	addLeftGadgets();
	addRightGadgets();

	if((leftOffset==0) && (rightOffset==0))
		{
			fprintf(stderr,"Not using empty panel ...\n");
			exit(0);
		}

	psize=leftOffset+abs(rightOffset);
	px=mons->x;
	py=mons->y;
	switch(panelGravity)
		{
		case PANELSOUTH:
			py=mons->y+mons->h-panelHeight;
		case PANELNORTH:
			switch(panelWidth)
				{
				case PANELFULL:
					panelWidth=mons->w;
					panelPos=PANELLEFT;
					break;
				case PANELSHRINK:
					panelWidth=psize;
					break;
				}
			switch(panelPos)
				{
				case PANELLEFT:
					px=mons->x;
					break;
				case PANELCENTRE:
					px=((mons->w/2)-(psize/2))+mons->x;
					break;
				case PANELRIGHT:
					px=mons->x+mons->w-psize;
					break;
				}
			break;

		case PANELEAST:
			px=mons->x+mons->w-panelHeight;
		case PANELWEST:
			switch(panelWidth)
				{
				case PANELFULL:
					panelWidth=panelHeight;
					panelHeight=mons->h;
					panelPos=PANELLEFT;
					break;
				case PANELSHRINK:
					panelWidth=panelHeight;
					panelHeight=psize;
					break;
				default:
					thold=panelWidth;
					panelWidth=panelHeight;
					panelHeight=thold;
					break;
				}
			switch(panelPos)
				{
				case PANELLEFT:
					py=mons->y;
					break;
				case PANELCENTRE:
					py=((mons->h/2)-(panelHeight/2))+mons->y;
					break;
				case PANELRIGHT:
					py=mons->y+mons->h-panelHeight;
					break;
				}
			break;
		}

	mainwind->LFSTK_resizeWindow(panelWidth,panelHeight,true);
	mainwind->LFSTK_moveWindow(px,py,true);
	mainwind->LFSTK_showWindow(true);
	mainwind->LFSTK_setKeepAbove(true);

	mainLoop=true;
	while(mainLoop==true)
		{
			if (XNextEventTimed(mainwind->display,&event,&tv) == True)
				{
					tv.tv_sec=refreshRate;
					tv.tv_usec=0;

					mappedListener *ml=mainwind->LFSTK_getMappedListener(event.xany.window);
					if(ml!=NULL)
						ml->function(ml->gadget,&event,ml->type);
					switch(event.type)
						{
						case Expose:
							mainwind->LFSTK_setActive(true);
							break;
						case ConfigureNotify:
							mainwind->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
							break;
						}
				}
			else
				{
					tv.tv_sec=refreshRate;
					tv.tv_usec=0;
					if(useAlarm==true)
						alarmCallBack();
				}
		}

	free(env);
	free(terminalCommand);

	if(desktopTheme!=NULL)
		free(desktopTheme);

	launcherList	*freell;
	while(ll!=NULL)
		{
			freell=ll;
			if(ll->icon!=NULL)
				free(ll->icon);
			if(ll->entry.name!=NULL)
				free(ll->entry.name);
			if(ll->entry.exec!=NULL)
				free(ll->entry.exec);
			ll=ll->next;
			delete freell;
		}

	delete mainwind;
	return 0;
}