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

void loadPrefs(std::string prefsfile)
{
	prefs.LFSTK_loadVarsFromFile(prefsfile.c_str());
	dockSize=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("docksize"));
	onMonitor=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("onmonitor"));
	dockGravity=prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("dockgrav"));
	dockTextColour=prefs.LFSTK_getCString(prefs.LFSTK_hashFromKey("textcolour"));
	dockBGColour=prefs.LFSTK_getCString(prefs.LFSTK_hashFromKey("dockbgcolour"));
	refreshRate=	prefs.LFSTK_getInt(prefs.LFSTK_hashFromKey("refreshrate"));
	useMicros=prefs.LFSTK_getBool(prefs.LFSTK_hashFromKey("usemicroseconds"));
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
								offset+=addLaunchers(offset,normalY,dockGravity);
							}
						else
							printError("Duplicate launcher widget");
						break;
					case 'S':
						offset+=addSlider(offset,normalY,dockGravity);
						break;
					case 'D':
						offset+=addDesktopSwitcer(offset,normalY,dockGravity);
						break;
					case 'c':
						offset+=addCalendar(offset,normalY,NorthWestGravity);
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
			rcfile<<"dockgrav 2\n";
			rcfile<<"docksize 2\n";
			rcfile<<"textcolour #FF000000\n";
			rcfile<<"gadgetsleft LCDsssT\n";
			rcfile<<"termcommand kkterminal -m -l -e \n";
			rcfile<<"dockbgcolour #00000000\n";
			rcfile<<"refreshrate 500000\n";
			rcfile<<"usemicroseconds true\n";
			rcfile.close();
		}

	if(!fs::exists(fs::status(configDir + std::string("calendardates"))))
		{
			std::ofstream rcfile;
			rcfile.open (configDir + std::string("calendardates"));
			rcfile<<"11 25 1\n";
			rcfile<<"0 1 1\n";
			rcfile<<"6 2 2\n";
			rcfile<<"11 21 3\n";
			rcfile<<"5 21 3\n";
			rcfile<<"3 30 4\n";
			rcfile<<"9 31 4\n";
			rcfile.close();
		}
}

int main(int argc,char **argv)
{
	char				*env;
	int				psize;
	int				key=666;
	propReturn		pr;
	windowInitStruct	*win;
	std::string		lc="#a0f0f0f0";
	std::string		pc="#60a0a0a0";
	std::string		ac="#60404040";

	if(argc>1)
		whatDock=argv[1];
	else
		whatDock="MAINDOCK";

	configDir=getenv("HOME") + std::string("/.config/LFS/");
	launchersDir=configDir + std::string("launchers-")+whatDock;
	configFile=configDir + std::string("lfsdock-")+whatDock+".rc";

	sanityCheck();

	prefs.prefsMap={
						{prefs.LFSTK_hashFromKey("docksize"),{TYPEINT,"docksize","",false,1}},
						{prefs.LFSTK_hashFromKey("onmonitor"),{TYPEINT,"onmonitor","",false,0}},
						{prefs.LFSTK_hashFromKey("dockgrav"),{TYPEINT,"dockgrav","",false,0}},
						{prefs.LFSTK_hashFromKey("textcolour"),{TYPESTRING,"textcolour","black",false,0}},
						{prefs.LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand","xterm -e ",false,0}},
						{prefs.LFSTK_hashFromKey("gadgetsleft"),{TYPESTRING,"gadgetsleft","l",false,0}},
						{prefs.LFSTK_hashFromKey("font"),{TYPESTRING,"font","",false,0}},
						{prefs.LFSTK_hashFromKey("dockbgcolour"),{TYPESTRING,"dockbgcolour","",false,0}},
						{prefs.LFSTK_hashFromKey("refreshrate"),{TYPEINT,"refreshrate","",false,1}},
						{prefs.LFSTK_hashFromKey("usemicroseconds"),{TYPEBOOL,"usemicroseconds","",false,0}},
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
			win->windowType=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DOCK"));
			win->level=ABOVEALL;

			apc->LFSTK_addToolWindow(win);
			popActionWindow=apc->windows->back().window;
			popActionList=new LFSTK_listGadgetClass(popActionWindow,"list",0,0,2000,2000);

			popActionList->LFSTK_setStyle(BEVELNONE);
			popActionList->LFSTK_setLabelAutoColour(false);
			popActionList->LFSTK_setListItemsColours(GADGETBG,lc,pc,ac,lc);
			popActionList->LFSTK_setListItemsColours(GADGETFG,"red","red","red","red");
			popActionList->LFSTK_setGadgetColourPair(NORMALCOLOUR,lc,"red");
			popActionList->LFSTK_setMouseCallBack(taskListCBDown,popActionWindowSelect,NULL);

			WM_STATE=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("WM_STATE"));
			NET_WM_WINDOW_TYPE_NORMAL=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_NORMAL"));
			NET_WM_STATE_HIDDEN=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_WM_STATE_HIDDEN"));
			NET_WM_WINDOW_TYPE_DIALOG=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DIALOG"));
			NET_WM_WINDOW_TYPE=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE"));
			NET_WM_STATE=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_WM_STATE"));
			NET_WM_NAME=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_WM_NAME"));
			UTF8_STRING=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("UTF8_STRING"));
			NET_CURRENT_DESKTOP=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_CURRENT_DESKTOP"));
			WM_CLASS=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("WM_CLASS"));
			NET_WM_PID=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_WM_PID"));
			NET_NUMBER_OF_DESKTOPS=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_NUMBER_OF_DESKTOPS"));
			NET_ACTIVE_WINDOW=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_ACTIVE_WINDOW"));
			NET_WM_DESKTOP=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_WM_DESKTOP"));

			env=dockWindow->globalLib->LFSTK_oneLiner("sed -n '2p' %s/lfsappearance.rc",apc->configDir.c_str());
			key=atoi(env);
			freeAndNull(&env);

			if((queueID=msgget(key,IPC_CREAT|0660))==-1)
				fprintf(stderr,"Can't create message queue\n");

			loadPrefs(configFile);

			apc->LFSTK_setTimer(refreshRate,useMicros);
			apc->LFSTK_setTimerCallBack(timerCB,NULL);

			switch(dockSize)
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

			if(dockGravity==1)
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


/*
extern LFSTK_windowClass			*calWindow;
extern bool						calWindowVisible;
extern LFSTK_toggleButtonClass	*calendarButton;
extern bool						calendarIsUp;
*/
			//delete editbox;
			//delete calWindow;
			//delete calendarButton;
			calendarIsUp=false;

			freeAndNull(&iconL);
			freeAndNull(&iconM);
			freeAndNull(&iconH);
			holdtasks.clear();
			filltasks.clear();
			tasks.clear();

			launchersArray.clear();
			datesData.clear();
			delete gFind;
			delete findlaunchers;
			delete apc;
			clockButton=NULL;
			switchButton=NULL;
			calendarButton=NULL;
			editbox=NULL;
			calWindow=NULL;
			gotLaunchers=false;
			useTaskBar=false;
			
		}
	cairo_debug_reset_static_data();
	g_key_file_free(kf);
	delete gFind;
	return 0;
}