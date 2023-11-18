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
#include "clock.h"
#include "launchers.h"
#include "slider.h"

#define RCNAME "lfsdock"

void loadPrefs(const char *env)
{
	prefs.LFSTK_loadVarsFromFile(env);
	panelSize=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("panelsize"));
	panelWidth=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("panelwidth"));
	onMonitor=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("onmonitor"));
	panelPos=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("panelpos"));
	panelGravity=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("panelgrav"));
	panelTextColour=prefs.LFSTK_getCString(prefs.LFSTK_hashFromKey("textcolour"));
}

void addGadgets(void)
{
	int	offset=leftOffset;

	for(int j=0; j<prefs.LFSTK_getStringObject("gadgetsleft")->length();j++)
		{
			switch(prefs.LFSTK_getStringObject("gadgetsleft")->at(j))
				{
				case 'C':
					offset+=addClock(offset,mons->y,NorthWestGravity);
					break;
				case 'S':
					offset+=iconSize;
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
					offset+=addSlider(offset,mons->y,panelGravity,true);//TODO//
					break;
				}
		}
	offset++;
	leftOffset=offset;
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

void sanityCheck(void)
{
	namespace fs=std::filesystem;

	if(!fs::exists(fs::status(launchersDir)))
		fs::create_directories(launchersDir);
	if(!fs::exists(fs::status(configFile)))
		{
			std::ofstream rcfile;
			rcfile.open (configFile);
			rcfile<<"panelpos -2\n";
			rcfile<<"onmonitor 0\n";
			rcfile<<"panelgrav 2\n";
			rcfile<<"panelwidth -2\n";
			rcfile<<"textcolour black\n";
			rcfile<<"gadgetsleft lC\n";
			rcfile<<"panelsize 3\n";
			rcfile<<"termcommand kkterminal -m -l -e \n";
			rcfile<<"font Arial:size=16\n";
			rcfile.close();
		}
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

	configDir=getenv("HOME") + std::string("/.config/LFS/");
	launchersDir=configDir + std::string("launchers-DOCK");
	configFile=configDir + std::string("lfsdock.rc");
	sanityCheck();

	prefs.prefsMap={
						{prefs.LFSTK_hashFromKey("panelsize"),{TYPEINT,"panelsize","",false,1}},
						{prefs.LFSTK_hashFromKey("panelwidth"),{TYPEINT,"panelwidth","",false,0}},
						{prefs.LFSTK_hashFromKey("onmonitor"),{TYPEINT,"onmonitor","",false,0}},

						{prefs.LFSTK_hashFromKey("panelpos"),{TYPEINT,"panelpos","",false,0}},
						{prefs.LFSTK_hashFromKey("panelgrav"),{TYPEINT,"panelgrav","",false,0}},

						{prefs.LFSTK_hashFromKey("textcolour"),{TYPESTRING,"textcolour","black",false,0}},

						{prefs.LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand","xterm -e ",false,0}},
						{prefs.LFSTK_hashFromKey("gadgetsleft"),{TYPESTRING,"gadgetsleft","l",false,0}},
						{prefs.LFSTK_hashFromKey("font"),{TYPESTRING,"font","l",false,0}},
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

			env=mainwind->globalLib->LFSTK_oneLiner("sed -n '2p' %s/lfsappearance.rc",apc->configDir.c_str());
			key=atoi(env);
			freeAndNull(&env);

			apc->LFSTK_setTimer(refreshRate);
			apc->LFSTK_setTimerCallBack(timerCB,NULL);

			if((queueID=msgget(key,IPC_CREAT|0660))==-1)
				fprintf(stderr,"Can't create message queue\n");

			asprintf(&env,"%s",configFile.c_str());

			loadPrefs(env);
			if(panelGravity==1)
				posMultiplier=-1;
			else
				posMultiplier=1;

			switch(panelSize)
				{
					case 2:
						iconSize=48;
						break;
					case 3:
						iconSize=64;
						break;
					case 4:
						iconSize=96;
						break;
					default:
						iconSize=32;
				}

			desktopTheme=mainwind->globalLib->desktopIconTheme.c_str();
			mons=apc->LFSTK_getMonitorData(onMonitor);

			leftOffset=0;

			mainwind->LFSTK_setTile(NULL,0);
			mainwind->LFSTK_setWindowColourName(NORMALCOLOUR,"#00000000");

			addGadgets();

			if(leftOffset==0)
				{
					fprintf(stderr,"Not using empty panel ...\n");
					exit(0);
				}

			psize=leftOffset;
			px=mons->x;
			py=mons->y;
			switch(panelGravity)
				{
					case PANELSOUTH:
						py=mons->y+mons->h-iconSize;
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
								default:
									px=panelPos;
									break;
							}
						break;
				}

			mainwind->LFSTK_resizeWindow(panelWidth,iconSize+extraSpace,true);
			if(posMultiplier==1)
				{
					mainwind->LFSTK_moveWindow(px,py-extraSpace,true);
				}
			else
				{
					mainwind->LFSTK_moveWindow(px,py,true);
				}
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

			free(iconL);
			free(iconM);
			free(iconH);
			clockButton=NULL;
			launcherSide=NOLAUNCHERS;
			delete apc;
		}
	cairo_debug_reset_static_data();
	return 0;
}