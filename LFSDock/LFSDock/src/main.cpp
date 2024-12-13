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

//context window
enum {CONTEXTBUTTONPREFS=0,CONTEXTBUTTONHIDE,CONTEXTBUTTONQUIT,NOMORECONTEXTBUTONS};
const char				*mainContextLabelData[]={"Run Prefs","Iconize Dock","Quit Dock",NULL};
const char				*mainContextThemeIconData[]={"LFSTKPrefs","utilities-system-monitor","dialog-warning",NULL};
std::string				iconpath;
LFSTK_buttonClass		*mainContextButtons[NOMORECONTEXTBUTONS];

bool contextCB(void *p,void* ud)
{
	LFSTK_windowClass	*lwc=static_cast<LFSTK_gadgetClass*>(p)->wc;
	int					winnum;
	long unsigned int	whatbutton=(long unsigned int)ud-1;

	if(p!=NULL)
		{
			winnum=lwc->app->LFSTK_findWindow(lwc);
			lwc->app->windows->at(winnum).loopFlag=false;
			//printf("ud=%p label=%s\n",ud,mainContextLabelData[((long unsigned)ud)-1]);
			mainContextWindow->LFSTK_hideWindow();
			apc->windows->at(winnum).loopFlag=false;

			switch(whatbutton)
				{
					case CONTEXTBUTTONPREFS:
						{
							std::string	com;
							com="lfsdockprefs -d "+whatDock+" &";
							system(com.c_str());
						}
						break;
					case CONTEXTBUTTONHIDE:
						resizeDock(1,1);
						iconWindow->LFSTK_showWindow();
						iconWindow->LFSTK_clearWindow(true);
						if(calWindow!=NULL)
							{
								calWindow->LFSTK_hideWindow();
								apc->windows->at(apc->LFSTK_findWindow(calWindow)).showing=false;
							}
						if(scwindow!=NULL)
							{
								scwindow->LFSTK_hideWindow();
								apc->windows->at(apc->LFSTK_findWindow(scwindow)).showing=false;
							}
						break;
					case CONTEXTBUTTONQUIT:
						realMainLoop=false;
						apc->mainLoop=false;
						break;
				}
		}
	return(true);
}

void loadPrefs(std::string prefsfile)
{
	prefs.LFSTK_loadVarsFromFile(prefsfile);
	dockSize=prefs.LFSTK_getInt("docksize");
	onMonitor=prefs.LFSTK_getInt("onmonitor");
	dockGravity=prefs.LFSTK_getInt("dockgrav");
	dockTextColour=prefs.LFSTK_getString("textcolour");
	dockBGColour=prefs.LFSTK_getString("dockbgcolour");
	refreshRate=	prefs.LFSTK_getInt("refreshrate");
	useMicros=prefs.LFSTK_getBool("usemicroseconds");
	useBG=prefs.LFSTK_getBool("usebg");
	dockBGImage=prefs.LFSTK_getCString("usebgpath");
	launcherPreColour=prefs.LFSTK_getCString("prelightcolour");
	if(launcherPreColour.compare("#00000000")==0)
		moveGadget=true;
	else
		moveGadget=false;
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
	for(int j=0;j<MAXTASKBUTTONS;j++)
		{
			taskbuttons[j]=new LFSTK_buttonClass(dockWindow,"",windowWidth+(j*(iconWidth+ICONSPACE)),normalY,iconWidth,iconHeight);
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
#include <experimental/filesystem>
#include <iostream>
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
	std::string		env;
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
						{LFSTK_UtilityClass::LFSTK_hashFromKey("docksize"),{TYPEINT,"docksize","",false,1}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("onmonitor"),{TYPEINT,"onmonitor","",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("dockgrav"),{TYPEINT,"dockgrav","",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("textcolour"),{TYPESTRING,"textcolour","black",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand","xterm -e ",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("gadgetsleft"),{TYPESTRING,"gadgetsleft","l",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("font"),{TYPESTRING,"font","",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("dockbgcolour"),{TYPESTRING,"dockbgcolour","grey",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("refreshrate"),{TYPEINT,"refreshrate","",false,1}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("usemicroseconds"),{TYPEBOOL,"usemicroseconds","",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("usebg"),{TYPEBOOL,"usebg","",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("usebgpath"),{TYPESTRING,"usebgpath","",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("prelightcolour"),{TYPESTRING,"prelightcolour","#00000000",false,0}},
					};
	realMainLoop=true;

	XSetErrorHandler(errHandler);
	while(realMainLoop==true)
		{
			gFind=new LFSTK_findClass;
			gFind->LFSTK_setDepth(1,1);
			gFind->LFSTK_setFileTypes(".desktop");
			gFind->LFSTK_setFullPath(true);
			gFind->LFSTK_findFiles("/usr/share/applications",false);

			apc=new LFSTK_applicationClass();
			win=apc->LFSTK_getDefaultWInit();
			win->windowType=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DOCK"));
			win->level=ABOVEALL;
			win->decorated=false;
			win->className="LFSDOCK";
			apc->LFSTK_addWindow(win,"LFSDock");

			dockWindow=apc->mainWindow;
			dockWindow->LFSTK_initDnD(true);
			dockWindow->LFSTK_setWindowDropCallBack(windowDrop,(void*)0xdeadbeef);

			win=apc->LFSTK_getDefaultWInit();
			win->windowType=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DOCK"));
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
			popActionList->LFSTK_setMouseMoveCallBack(exitPopList,exitPopList,NULL);

			WM_STATE=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("WM_STATE"));
			NET_WM_WINDOW_TYPE_NORMAL=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_NORMAL"));
			NET_WM_STATE_HIDDEN=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_HIDDEN"));
			NET_WM_WINDOW_TYPE_DIALOG=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DIALOG"));
			NET_WM_WINDOW_TYPE=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE"));
			NET_WM_STATE=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE"));
			NET_WM_NAME=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_NAME"));
			UTF8_STRING=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("UTF8_STRING"));
			NET_CURRENT_DESKTOP=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_CURRENT_DESKTOP"));
			WM_CLASS=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("WM_CLASS"));
			NET_WM_PID=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_PID"));
			NET_NUMBER_OF_DESKTOPS=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_NUMBER_OF_DESKTOPS"));
			NET_ACTIVE_WINDOW=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_ACTIVE_WINDOW"));
			NET_WM_DESKTOP=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_DESKTOP"));

			env=dockWindow->globalLib->LFSTK_oneLiner("sed -n '2p' %S/lfsappearance.rc",apc->configDir);
			key=std::stoi(env);
			if((queueID=msgget(key,IPC_CREAT|0660))==-1)
				fprintf(stderr,"Can't create message queue\n");

			loadPrefs(configFile);

			apc->LFSTK_setTimer(refreshRate,useMicros);
			apc->LFSTK_setTimerCallBack(timerCB,NULL);

			switch(dockSize)
				{
					case 2:
						iconWidth=48;
						popActionWindowYOffset=11;
						break;
					case 3:
						iconWidth=64;
						popActionWindowYOffset=7;
						break;
					case 4:
						iconWidth=96;
						popActionWindowYOffset=-1;
						break;
					default:
						iconWidth=32;
						popActionWindowYOffset=15;
				}

			iconHeight=iconWidth+(iconWidth/2);
			extraSpace=iconWidth/4;

			if(moveGadget==false)
				{
					extraSpace=0;
					iconHeight=iconWidth;
				}

			if(dockGravity==1)
				{
					normalY=-extraSpace;
					activeY=0;
				}
			else
				{
					popActionWindowYOffset=popActionWindowYOffset*-1;
					normalY=0;
					activeY=-extraSpace;
				}

			desktopTheme=dockWindow->globalLib->desktopIconTheme;
			if(((apc->LFSTK_getMonitorCount()-1)<onMonitor) || (onMonitor<0))
				onMonitor=0;
			mons=apc->LFSTK_getMonitorData(onMonitor);
			dockWindow->LFSTK_setTile(NULL,0);
			dockWindow->LFSTK_setWindowColourName(NORMALCOLOUR,"#00000000");
			windowWidth=0;
			pr=apc->globalLib->LFSTK_getSingleProp(apc->display,apc->rootWindow,NET_NUMBER_OF_DESKTOPS,XA_CARDINAL);
			deskCount=pr.integer;

			win=apc->LFSTK_getDefaultWInit();
			win->windowType=win->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_NORMAL"));
			win->level=ABOVEALL;
			win->decorated =false;
			win->overRide =true;
			apc->LFSTK_addWindow(win,"windowpopup");
			mainContextWindow=apc->windows->back().window;
			mainContextWindow->LFSTK_setWindowColourName(NORMALCOLOUR,lc.c_str());
			int sy=0;
			for(int j=0;j<NOMORECONTEXTBUTONS;j++)
				{
					mainContextButtons[j]=new LFSTK_buttonClass(mainContextWindow,mainContextLabelData[j],0,sy,GADGETWIDTH*2,24,NorthWestGravity);
					mainContextButtons[j]->LFSTK_setMouseCallBack(NULL,contextCB,(void*)(long)(j+1));
					iconpath=mainContextWindow->globalLib->LFSTK_findThemedIcon("gnome",mainContextThemeIconData[j],"");
					mainContextButtons[j]->LFSTK_setImageFromPath(iconpath,LEFT,true);
					setGadgetDetails(mainContextButtons[j]);
					mainContextButtons[j]->LFSTK_setTile(NULL,0);
					mainContextButtons[j]->LFSTK_setFontString(prefs.LFSTK_getCString("font"),true);
					mainContextButtons[j]->LFSTK_setLabelAutoColour(true);
					mainContextButtons[j]->LFSTK_setGadgetColours(GADGETBG,lc,pc,ac,lc);
					mainContextButtons[j]->LFSTK_setGadgetColours(GADGETFG,lc,pc,ac,lc);
					sy+=GADGETHITE;
				}
			mainContextWindow->LFSTK_resizeWindow(GADGETWIDTH+GADGETHITE,sy,true);
			dockWindow->LFSTK_setContextWindow(mainContextWindow);
			if(dockGravity==PANELSOUTH)
				dockWindow->contextWindowPos=CONTEXTABOVE;

			else
				dockWindow->contextWindowPos=CONTEXTBELOW;

			addGadgets();
			if(windowWidth==0)
				{
					fprintf(stderr,"Not using empty dock ...\n");
					exit(0);
				}

			psize=windowWidth;
			win=apc->LFSTK_getDefaultWInit();
			win->windowType=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DOCK"));
			win->level=ABOVEALL;
			win->overRide=false;
			win->decorated=false;
			apc->LFSTK_addWindow(win,"DOCKBG");
//LFSTK_handleWindowEvents
			dockBGWindow=apc->windows->back().window;	
			dockBGWindow->LFSTK_setTile(dockBGImage.c_str(),-1);
			dockWindow->LFSTK_setTransientFor(dockBGWindow->window);
			apc->windows->back().showing=true;

			resizeDock(psize,iconWidth+extraSpace);
			moveDock(0);
//max hite for end caps should be 120
//size = 2 = 60
//fprintf(stderr,"dock hite=%i\n",iconWidth+extraSpace);
			if(useBG==true)
				{
					//hack :(
					LFSTK_labelClass *label=new LFSTK_labelClass(dockBGWindow,"XX",0,0,1,1,NorthGravity);
					cairo_status_t	st;
					cairo_surface_t	*tsfc;
					std::string		suffix=LFSTK_UtilityClass::LFSTK_deleteSuffix(&dockBGImage);

					dockBGImageLS=dockBGImage+"-ls."+suffix;
					dockBGImageRS=dockBGImage+"-rs."+suffix;
					dockBGWindow->LFSTK_showWindow(true);
					tsfc=apc->globalLib->LFSTK_lib::LFSTK_createSurfaceFromPath(dockBGImageLS.c_str());
					if(tsfc!=NULL)
						{
							sidehite=cairo_image_surface_get_height(tsfc);
							sidewid=cairo_image_surface_get_width(tsfc);
							cairo_surface_destroy (tsfc);
			
							bgls=new LFSTK_buttonClass(dockBGWindow,"",0,0,sidewid,sidehite);
							setGadgetDetails(bgls);
							st=bgls->LFSTK_setImageFromPath(dockBGImageLS,CENTRE,false);
							if(st!=CAIRO_STATUS_SUCCESS)
								{
									bgls=NULL;
								}
					
							bgrs=new LFSTK_buttonClass(dockBGWindow,"",psize-sidewid,0,sidewid,sidehite,NorthWestGravity);
							setGadgetDetails(bgrs);
							st=bgrs->LFSTK_setImageFromPath(dockBGImageRS,RIGHT,false);
							if(st!=CAIRO_STATUS_SUCCESS)
								{
									bgrs=NULL;
								}
						}
					dockBGWindow->LFSTK_clearWindow(true);
				}

			tasks.clear();
			holdtasks.clear();
			if(useTaskBar==true)
				updateTaskBar(true);

			showhidetActionList(NULL,popActionWindow,popActionList);
			XSync(apc->display,true);

			win=apc->LFSTK_getDefaultWInit();
			win->windowType=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DOCK"));
			win->level=ABOVEALL;
			win->overRide=true;
			win->decorated=false;
			
			if(dockGravity==PANELSOUTH)
				win->y=mons->h-32;
			else
				win->y=mons->y;

			win->x=mons->x;
			win->w=32;
			win->h=32;
			apc->LFSTK_addWindow(win,"ICONWINDOW");

			iconWindow=apc->windows->back().window;	
			apc->windows->back().showing=true;
			LFSTK_buttonClass *bgls=new LFSTK_buttonClass(iconWindow,"",0,0,32,32);
			std::string iconpath=iconWindow->globalLib->LFSTK_findThemedIcon("gnome",contextThemeIconData[BUTTONHIDE],"");
			bgls->LFSTK_setImageFromPath(iconpath,CENTRE,true);
			bgls->LFSTK_setMouseCallBack(NULL,hideCB,NULL);
			setGadgetDetails(bgls);
			holdpsize=psize;







			int retval=apc->LFSTK_runApp();

			calendarIsUp=false;

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
	return 0;
}