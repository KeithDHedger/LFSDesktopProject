/*
 *
 * ©K. D. Hedger. Sun 19 Nov 19:08:13 GMT 2023 keithdhedger@gmail.com

 * This file (main.cpp) is part of LFSDock.

 * LFSDock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSDock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDock.  If not, see <http://www.gnu.org/licenses/>.
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
	onMonitor=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("onmonitor"));
	panelGravity=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("panelgrav"));
	panelTextColour=prefs.LFSTK_getCString(prefs.LFSTK_hashFromKey("textcolour"));
	panelBGColour=prefs.LFSTK_getCString(prefs.LFSTK_hashFromKey("panelbgcolour"));
}

void addGadgets(void)
{
	int	offset=0;

	for(int j=0; j<prefs.LFSTK_getStringObject("gadgetsleft")->length();j++)
		{
			switch(prefs.LFSTK_getStringObject("gadgetsleft")->at(j))
				{
				case 'C':
					offset+=addClock(offset,normalY,NorthWestGravity);
					break;
				case 's':
					offset+=8;
					break;
				case 'L':
					if(gotLaunchers==false)
						{
							gotLaunchers=true;
							offset+=addLaunchers(offset,normalY,panelGravity);
						}
					else
						printError("Duplicate launcher widget");
					break;
				case 'S':
					offset+=addSlider(offset,normalY,panelGravity);
					break;
				case 'D':
					offset+=addDesktopSwitcer(offset,normalY,panelGravity);
					break;
				case 'T':
					useTaskBar=true;
					break;
				}
		}

	windowWidth=offset;
	for(int j=0;j<20;j++)
		{
			taskbuttons[j]=new LFSTK_buttonClass(dockWindow,"",windowWidth+(j*iconWidth),normalY,iconWidth,iconHeight);
			setGadgetDetails(taskbuttons[j]);
			taskbuttons[j]->LFSTK_setAlpha(1.0);
			taskbuttons[j]->LFSTK_setMouseCallBack(NULL,taskListCB,NULL);
			taskbuttons[j]->LFSTK_setMouseMoveCallBack(taskSwitcherEnterCB,taskSwitcherExitCB,NULL);
		}
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
		{
			fs::create_directories(launchersDir);
			std::string	command="cp /usr/share/applications/lfsappearance.desktop " + launchersDir;
			system(command.c_str());
		}
	if(!fs::exists(fs::status(configFile)))
		{
			std::ofstream rcfile;
			rcfile.open (configFile);
			rcfile<<"font Arial:size=10\n";
			rcfile<<"onmonitor 0\n";
			rcfile<<"panelgrav 2\n";
			rcfile<<"panelsize 2\n";
			rcfile<<"textcolour FF000000\n";
			rcfile<<"gadgetsleft LCDsssT\n";
			rcfile<<"termcommand kkterminal -m -l -e \n";
			rcfile<<"panelbgcolour #00000000\n";
			rcfile.close();
		}
}

int main(int argc,char **argv)
{
	char				*env;
	int				psize;
	int				key=666;
	propReturn		pr;
	windowInitStruct	*win;//=new windowInitStruct;;
	std::string		lc="#a0f0f0f0";
	std::string		pc="#60a0a0a0";
	std::string		ac="#60404040";
	launcherList		*freell;

	configDir=getenv("HOME") + std::string("/.config/LFS/");
	launchersDir=configDir + std::string("launchers-DOCK");
	configFile=configDir + std::string("lfsdock.rc");
	sanityCheck();

	prefs.prefsMap={
						{prefs.LFSTK_hashFromKey("panelsize"),{TYPEINT,"panelsize","",false,1}},
						{prefs.LFSTK_hashFromKey("onmonitor"),{TYPEINT,"onmonitor","",false,0}},
						{prefs.LFSTK_hashFromKey("panelgrav"),{TYPEINT,"panelgrav","",false,0}},
						{prefs.LFSTK_hashFromKey("textcolour"),{TYPESTRING,"textcolour","black",false,0}},
						{prefs.LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand","xterm -e ",false,0}},
						{prefs.LFSTK_hashFromKey("gadgetsleft"),{TYPESTRING,"gadgetsleft","l",false,0}},
						{prefs.LFSTK_hashFromKey("font"),{TYPESTRING,"font","",false,0}},
						{prefs.LFSTK_hashFromKey("panelbgcolour"),{TYPESTRING,"panelbgcolour","",false,0}},
					};
	realMainLoop=true;

	XSetErrorHandler(errHandler);
	kf=g_key_file_new();
	while(realMainLoop==true)
		{
	gFind=new LFSTK_findClass;
	gFind->LFSTK_setDepth(1,1);
	gFind->LFSTK_setFileTypes(".desktop");
	gFind->LFSTK_setFullPath(true);
	gFind->LFSTK_findFiles("/usr/share/applications",false);

			apc=new LFSTK_applicationClass();
			apc->LFSTK_addWindow(NULL,NULL);

			dockWindow=apc->mainWindow;
			dockWindow->LFSTK_initDnD(true);
			dockWindow->LFSTK_setWindowDropCallBack(windowDrop,(void*)0xdeadbeef);

			win=apc->LFSTK_getDefaultWInit();
			win->windowType=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_MENU"));
	
			apc->LFSTK_addToolWindow(win);
			popActionWindow=apc->windows->back().window;
			popActionList=new LFSTK_listGadgetClass(popActionWindow,"list",0,0,2000,2000);

			popActionList->LFSTK_setStyle(BEVELNONE);
			popActionList->LFSTK_setLabelAutoColour(true);
			popActionList->LFSTK_setListItemsColours(GADGETBG,lc,pc,ac,lc);
			popActionList->LFSTK_setListItemsColours(GADGETFG,"red","red","red","red");
			popActionList->LFSTK_setGadgetColourPair(NORMALCOLOUR,lc,"red");
			popActionList->LFSTK_setMouseCallBack(NULL,popActionWindowSelect,NULL);
			//popActionList->LFSTK_setMouseMoveCallBack(popActionListEnterCB,popActionListExitCB,NULL);

//TODO//
			WM_STATE=XInternAtom(dockWindow->app->display,"WM_STATE",False);
			NET_WM_WINDOW_TYPE_NORMAL=XInternAtom(dockWindow->app->display,"_NET_WM_WINDOW_TYPE_NORMAL",False);
			NET_WM_STATE_HIDDEN=XInternAtom(dockWindow->app->display,"_NET_WM_STATE_HIDDEN",False);
			NET_WM_WINDOW_TYPE_DIALOG=XInternAtom(dockWindow->app->display,"_NET_WM_WINDOW_TYPE_DIALOG",False);
			//NET_WM_DESKTOP=XInternAtom(dockWindow->app->display,"_NET_WM_DESKTOP",False);
			NET_WM_WINDOW_TYPE=XInternAtom(dockWindow->app->display,"_NET_WM_WINDOW_TYPE",False);
			NET_WM_STATE=XInternAtom(dockWindow->app->display,"_NET_WM_STATE",False);
			NET_WM_NAME=XInternAtom(dockWindow->app->display,"_NET_WM_NAME",False);
			UTF8_STRING=XInternAtom(dockWindow->app->display,"UTF8_STRING",False);
			NET_CURRENT_DESKTOP=XInternAtom(dockWindow->app->display,"_NET_CURRENT_DESKTOP",False);
			WM_CLASS=XInternAtom(dockWindow->app->display,"WM_CLASS",False);
			NET_WM_PID=XInternAtom(dockWindow->app->display,"_NET_WM_PID",False);
			NET_NUMBER_OF_DESKTOPS=XInternAtom(dockWindow->app->display,"_NET_NUMBER_OF_DESKTOPS",False);
			NET_ACTIVE_WINDOW=XInternAtom(dockWindow->app->display,"_NET_ACTIVE_WINDOW",False);

			env=dockWindow->globalLib->LFSTK_oneLiner("sed -n '2p' %s/lfsappearance.rc",apc->configDir.c_str());
			key=atoi(env);
			freeAndNull(&env);

			apc->LFSTK_setTimer(refreshRate,true);
			apc->LFSTK_setTimerCallBack(timerCB,NULL);

			if((queueID=msgget(key,IPC_CREAT|0660))==-1)
				fprintf(stderr,"Can't create message queue\n");

			asprintf(&env,"%s",configFile.c_str());

			loadPrefs(env);
			freeAndNull(&env);

			switch(panelSize)
				{
					case 2:
						iconWidth=48;
						break;
					case 3:
						iconWidth=64;
						break;
					case 4:
						iconWidth=96;
						break;
					default:
						iconWidth=32;
				}

			iconHeight=iconWidth+(iconWidth/2);
			extraSpace=iconWidth/4;

			if(panelGravity==1)
				{
					normalY=-extraSpace;
					activeY=0;
				}
			else
				{
					normalY=0;
					activeY=-extraSpace;
				}

			desktopTheme=dockWindow->globalLib->desktopIconTheme.c_str();
			mons=apc->LFSTK_getMonitorData(onMonitor);
			dockWindow->LFSTK_setTile(NULL,0);
			dockWindow->LFSTK_setWindowColourName(NORMALCOLOUR,"#00000000");
			windowWidth=0;
			pr=apc->globalLib->LFSTK_getSingleProp(apc->display,apc->rootWindow,NET_NUMBER_OF_DESKTOPS,XA_CARDINAL);
			deskCount=pr.integer;

			addGadgets();
			if(windowWidth==0)
				{
					fprintf(stderr,"Not using empty dock ...\n");
					exit(0);
				}

			psize=windowWidth;

			dockWindow->LFSTK_resizeWindow(psize,iconWidth+extraSpace,true);
			moveDock(0);

			dockWindow->LFSTK_showWindow(true);
			dockWindow->LFSTK_setKeepAbove(true);

			if(useTaskBar==true)
				updateTaskBar(true);

			int retval=apc->LFSTK_runApp();
			while(ll!=NULL)
				{
					freell=ll;
					if(ll->icon!=NULL)
						freeAndNull(&ll->icon);
					if(ll->entry.name!=NULL)
						freeAndNull(&ll->entry.name);
					if(ll->entry.exec!=NULL)
						freeAndNull(&ll->entry.exec);
					ll=ll->next;
					delete freell;
				}

			freeAndNull(&iconL);
			freeAndNull(&iconM);
			freeAndNull(&iconH);
			clockButton=NULL;
			switchButton=NULL;
			gotLaunchers=false;
			useTaskBar=false;
			holdtasks.clear();
			filltasks.clear();
			tasks.clear();

			delete apc;
			ll=NULL;
		}
	cairo_debug_reset_static_data();
	g_key_file_free(kf);
	delete gFind;
	return 0;
}