/*
 *
 * ©K. D. Hedger. Mon  7 Sep 13:20:24 BST 2015 keithdhedger@gmail.com

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
#include <sys/msg.h>

#include "globals.h"
#include "appmenu.h"
#include "logout.h"
#include "clock.h"
#include "disks.h"
#include "cpu.h"
#include "windowlist.h"
#include "launchers.h"
#include "slider.h"

#define RCNAME "lfspanel"
#define REFRESHMULTI 4

void loadPrefs(const char *env)
{
	prefs.LFSTK_loadVarsFromFile(env);
	panelHeight=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("panelheight"));
	panelWidth=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("panelwidth"));
	onMonitor=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("onmonitor"));
	panelPos=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("panelpos"));
	panelGravity=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("panelgrav"));
}

void addLeftGadgets(void)
{
	int	offset=leftOffset;

	for(int j=0; j<prefs.LFSTK_getStringObject("gadgetsleft")->length();j++)
		{
			switch(prefs.LFSTK_getStringObject("gadgetsleft")->at(j))
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
					offset+=panelHeight;
					break;
				case 'l':
					if(launcherSide==NOLAUNCHERS)
						{
							launcherSide=LAUNCHERINLEFT;
							offset+=addLaunchers(offset,mons->y,panelGravity,true);
						}
					else
						printError("Duplicate launcher widget");
					break;
				case 's':
					offset+=addSlider(offset,mons->y,panelGravity,true);
					break;
				}
		}
	offset++;
	leftOffset=offset;
}

void addRightGadgets(void)
{
	int	offset=rightOffset;
	for(int j=prefs.LFSTK_getStringObject("gadgetsright")->length()-1;j>=0;j--)
		{
			switch(prefs.LFSTK_getStringObject("gadgetsright")->at(j))
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
					offset-=panelHeight;
					break;
				case 'l':
					if(launcherSide==NOLAUNCHERS)
						{
							launcherSide=LAUNCHERINRITE;
							offset-=addLaunchers(offset,mons->y,panelGravity,false);
						}
					else
						printError("Duplicate launcher widget");
					break;
				case 's':
					offset-=addSlider(offset,mons->y,panelGravity,false);
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

bool windowDrop(LFSTK_windowClass *lwc,void* ud)
{
	char	*cleanstr=NULL;

	if(lwc!=NULL)
		{
			if(lwc->droppedData.type=DROPURI)
				dropDesktopFile((const char*)lwc->droppedData.data,NULL);
		}
	return(true);
}

int main(int argc,char **argv)
{
	char				*env;
	XEvent				event;
	int					psize;
	int					thold;
	int					px,py;
	timeval				tv={0,0};
	int					key=666;
	int					refreshmulti=0;
	
	prefs.prefsMap={
						{prefs.LFSTK_hashFromKey("panelheight"),{TYPEINT,"panelheight","",false,0}},
						{prefs.LFSTK_hashFromKey("panelwidth"),{TYPEINT,"panelwidth","",false,0}},
						{prefs.LFSTK_hashFromKey("onmonitor"),{TYPEINT,"onmonitor","",false,0}},

						{prefs.LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand","xterm -e ",false,0}},
						{prefs.LFSTK_hashFromKey("logoutcommand"),{TYPESTRING,"logoutcommand","xterm",false,0}},
						{prefs.LFSTK_hashFromKey("restartcommand"),{TYPESTRING,"restartcommand","xterm",false,0}},
						{prefs.LFSTK_hashFromKey("shutdowncommand"),{TYPESTRING,"shutdowncommand","xterm",false,0}},
						{prefs.LFSTK_hashFromKey("gadgetsright"),{TYPESTRING,"gadgetsright","L",false,0}},
						{prefs.LFSTK_hashFromKey("gadgetsleft"),{TYPESTRING,"gadgetsleft","l",false,0}},

						{prefs.LFSTK_hashFromKey("panelpos"),{TYPEINT,"panelpos","",false,0}},
						{prefs.LFSTK_hashFromKey("panelgrav"),{TYPEINT,"panelgrav","",false,0}}
					};
	realMainLoop=true;
	
	XSetErrorHandler(errHandler);

	while(realMainLoop==true)
		{
			panelPos=PANELCENTRE;

			apc=new LFSTK_applicationClass();
			apc->LFSTK_addWindow(NULL,NULL);

			mainwind=apc->mainWindow;
			mainwind->LFSTK_initDnD(true);
			mainwind->LFSTK_setWindowDropCallBack(windowDrop,(void*)0xdeadbeef);

			WM_STATE=XInternAtom(mainwind->app->display,"WM_STATE",False);
			NET_WM_WINDOW_TYPE_NORMAL=XInternAtom(mainwind->app->display,"_NET_WM_WINDOW_TYPE_NORMAL",False);
			NET_WM_STATE_HIDDEN=XInternAtom(mainwind->app->display,"_NET_WM_STATE_HIDDEN",False);
			NET_WM_WINDOW_TYPE_DIALOG=XInternAtom(mainwind->app->display,"_NET_WM_WINDOW_TYPE_DIALOG",False);
			NET_WM_DESKTOP=XInternAtom(mainwind->app->display,"_NET_WM_DESKTOP",False);
			NET_WM_WINDOW_TYPE=XInternAtom(mainwind->app->display,"_NET_WM_WINDOW_TYPE",False);
			NET_WM_STATE=XInternAtom(mainwind->app->display,"_NET_WM_STATE",False);
			NET_WM_NAME=XInternAtom(mainwind->app->display,"_NET_WM_NAME",False);
			UTF8_STRING=XInternAtom(mainwind->app->display,"UTF8_STRING",False);

			env=mainwind->globalLib->LFSTK_oneLiner("sed -n '2p' %s/lfsappearance.rc",apc->configDir);
			key=atoi(env);
			freeAndNull(&env);

			apc->LFSTK_setTimer(refreshRate);
			apc->LFSTK_setTimerCallBack(timerCB,NULL);

			if((queueID=msgget(key,IPC_CREAT|0660))==-1)
				fprintf(stderr,"Can't create message queue\n");

//TODO//
	//itemfont=mainwind->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMFONT);
	//tfont=mainwind->globalLib->LFSTK_loadFont(mainwind->app->display,mainwind->screen,itemfont);
	//iconSize=tfont->size;
			iconSize=32;

	//free(tfont);

			if(argc>1)
				{
					panelID=argv[1];
					asprintf(&env,"%s/%s-%s.rc",apc->configDir,RCNAME,panelID);
				}
			else
				asprintf(&env,"%s/%s.rc",apc->configDir,RCNAME);

			loadPrefs(env);

			//desktopTheme=mainwind->globalLib->LFSTK_oneLiner("cat %s/lfsdesktop.rc|grep icontheme|awk '{print $2}'",apc->configDir);
			desktopTheme=mainwind->globalLib->desktopIconTheme.c_str();
			mons=apc->LFSTK_getMonitorData(onMonitor);

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

			int retval=apc->LFSTK_runApp();

			free(env);

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

			if(!entrydata.empty())
				{
					for (std::map<int,menuEntryStruct*>::iterator it=entrydata.begin();it!=entrydata.end();++it)
						{
							menuEntryStruct	*me=it->second;
							if (me!=NULL)
								{
									if(me->name!=NULL)
										delete me->name;
									if(me->exec!=NULL)
										delete me->exec;
									if(me->icon!=NULL)
										delete me->icon;
									delete me;
								}
						}
					entrydata.clear();
				}

			delete appMenu;
			delete logoutMenu;
			delete windowMenu;
			
			free(iconL);
			free(iconM);
			free(iconH);
			logoutButton=NULL;
			diskButton=NULL;
			clockButton=NULL;
			cpuButton=NULL;
			windowAll=NULL;
			windowDesk=NULL;
			windowMenu=NULL;
			appButton=NULL;
			launcherSide=NOLAUNCHERS;
			delete apc;
		}
	cairo_debug_reset_static_data();
	return 0;
}