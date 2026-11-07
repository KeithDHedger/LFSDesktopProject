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

#define RCNAME "lfsdock"

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

	//if(dockGravity==1)
	//	useBG=false;

	if(useBG==true && dockGravity==2)
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
						if(prefs.LFSTK_getStringObject("gadgetsleft")->length()-1>j)
							{
								if(prefs.LFSTK_getStringObject("gadgetsleft")->at(j+1)>48 && prefs.LFSTK_getStringObject("gadgetsleft")->at(j+1)<58)
									offset+=(8*(prefs.LFSTK_getStringObject("gadgetsleft")->at(j+1)-48));
								else
									offset+=8;
							}
						else
							offset+=8;
						break;
					case 'L':
						if(gotLaunchers==false)
							{
								gotLaunchers=true;
								offset+=addLaunchers(offset,normalY,dockGravity);
								//offset-=(iconWidth);
							}
						else
							printError("Duplicate launcher widget");
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
			taskbuttons[j]=new LFSTK_buttonClass(dockWindow,"",windowWidth+(j*(iconWidth+ICONSPACE)),0,iconWidth,dockWindowHeight);
			setGadgetDetails(taskbuttons[j]);
			taskbuttons[j]->LFSTK_setAlpha(1.0);
//EnterNotify
			taskbuttons[j]->LFSTK_connectCB(EnterNotify,[j](std::string msg) {taskSwitcherEnterCB(taskbuttons[j],NULL);});
//LeaveNotify
			taskbuttons[j]->LFSTK_connectCB(LeaveNotify,[j](std::string msg) {taskSwitcherExitCB(taskbuttons[j],NULL);});
//clicks
			taskbuttons[j]->LFSTK_connectCB(ButtonPress,[j](std::string msg) {taskListCB(taskbuttons[j],NULL);});
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
	std::string		iconpath;

	if(argc>1)
		whatDock=argv[1];
	else
		whatDock="MAINDOCK";

	configDir=getenv("HOME") + std::string("/.config/LFS/");
	launchersDir=configDir + std::string("launchers-")+whatDock;
	configFile=configDir + std::string("lfsdock-")+whatDock+".rc";

	sanityCheck();

	prefs.prefsMap={
						{LFSTK_UtilityClass::LFSTK_hashFromKey("docksize"),{TYPEINT,"docksize","","",false,1}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("onmonitor"),{TYPEINT,"onmonitor","","",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("dockgrav"),{TYPEINT,"dockgrav","","",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("textcolour"),{TYPESTRING,"textcolour","","black",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand","","xterm -e ",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("gadgetsleft"),{TYPESTRING,"gadgetsleft","","l",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("font"),{TYPESTRING,"font","","",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("dockbgcolour"),{TYPESTRING,"dockbgcolour","","grey",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("refreshrate"),{TYPEINT,"refreshrate","","",false,1}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("usemicroseconds"),{TYPEBOOL,"usemicroseconds","","",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("usebg"),{TYPEBOOL,"usebg","","",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("usebgpath"),{TYPESTRING,"usebgpath","","",false,0}},
						{LFSTK_UtilityClass::LFSTK_hashFromKey("prelightcolour"),{TYPESTRING,"prelightcolour","","#00000000",false,0}},
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
						dockWindowHeight=48;
						popActionWindowYOffset=11;
						break;
					case 3:
						iconWidth=64;
						dockWindowHeight=64;
						popActionWindowYOffset=7;
						break;
					case 4:
						iconWidth=96;
						dockWindowHeight=96;
						popActionWindowYOffset=-1;
						break;
					default:
						iconWidth=32;
						dockWindowHeight=32;
						popActionWindowYOffset=15;
				}

			extraSpace=iconWidth/4;
			iconHeight=iconWidth+16;

			if(moveGadget==false)
				{
					extraSpace=0;
					iconHeight=iconWidth;
				}
			else
				{
					extraSpace=iconWidth/4;
					iconHeight=iconWidth+16;
				}

			dockWindowHeight=iconHeight;
			if(dockGravity==1)
				{
					activeY=0;
				}
			else
				{
					popActionWindowYOffset=popActionWindowYOffset*-1;
					normalY=0;
					activeY=-extraSpace;
					activeY=0;
				}

			desktopTheme=dockWindow->globalLib->desktopIconTheme;
			if(((apc->LFSTK_getMonitorCount()-1)<onMonitor) || (onMonitor<0))
				onMonitor=0;
			mons=apc->LFSTK_getMonitorData(onMonitor);

			if(useBG==true)
				{
					dockWindow->LFSTK_setTile(dockBGImage.c_str(),-1);
				}
			else
				{
					dockWindow->LFSTK_setTile(NULL,0);
					dockWindow->LFSTK_setWindowColourName(NORMALCOLOUR,dockBGColour.c_str());
				}

			windowWidth=0;
			pr=apc->globalLib->LFSTK_getSingleProp(apc->display,apc->rootWindow,NET_NUMBER_OF_DESKTOPS,XA_CARDINAL);
			deskCount=pr.integer;

			win=apc->LFSTK_getDefaultWInit();
			win->windowType=win->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_MENU"));
			win->level=ABOVEALL;
			win->decorated =false;
			win->overRide =true;
			apc->LFSTK_addWindow(win,"windowpopup");
			mainContextWindow=apc->windows->back().window;
			mainContextWindow->LFSTK_setWindowColourName(NORMALCOLOUR,lc.c_str());
			int sy=0;
			int offset=-2;
			for(int j=CONTEXTMENUPREFS;j<NOMORECONTEXTMENUS;j++)
				{
					mainContextButtons[j+offset]=new LFSTK_buttonClass(mainContextWindow,labelData[j],0,sy,GADGETWIDTH*2,24,NorthWestGravity);
					mainContextButtons[j+offset]->LFSTK_connectCB(ButtonPress,[j,offset](std::string msg) {contextCB(mainContextButtons[j+offset],USERDATA(j));});

					iconpath=mainContextWindow->globalLib->LFSTK_findThemedIcon("gnome",themeIconData[j],"");
					mainContextButtons[j+offset]->LFSTK_setImageFromPath(iconpath,LEFT,true);
					setGadgetDetails(mainContextButtons[j+offset]);
					mainContextButtons[j+offset]->LFSTK_setLabelGravity(LEFT);
					mainContextButtons[j+offset]->LFSTK_setImageGravity(LEFT);
					mainContextButtons[j+offset]->LFSTK_setTile(NULL,0);
					mainContextButtons[j+offset]->LFSTK_setFontString(prefs.LFSTK_getCString("font"),true);
					mainContextButtons[j+offset]->LFSTK_setLabelAutoColour(true);
					mainContextButtons[j+offset]->LFSTK_setGadgetColours(GADGETBG,lc,pc,ac,lc);
					mainContextButtons[j+offset]->LFSTK_setGadgetColours(GADGETFG,lc,pc,ac,lc);
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

			resizeDock(psize,iconWidth+extraSpace);
			moveDock(0);

			if(useBG==true)
				{
					cairo_status_t	st;
					cairo_surface_t	*tsfc;
					std::string		suffix=LFSTK_UtilityClass::LFSTK_deleteSuffix(&dockBGImage);

					dockBGImageLS=dockBGImage+"-ls."+suffix;
					tsfc=apc->globalLib->LFSTK_lib::LFSTK_createSurfaceFromPath(dockBGImageLS.c_str());
					if(tsfc!=NULL)
						{
							sidehite=cairo_image_surface_get_height(tsfc);
							sidewid=cairo_image_surface_get_width(tsfc);
							cairo_surface_destroy (tsfc);
	
							bgls=new LFSTK_buttonClass(dockWindow,"",0,0,sidewid,sidehite);
							bgls->LFSTK_setStyle(BEVELNONE);
							bgls->LFSTK_setUseWindowTile(false);
							bgls->LFSTK_setTile(NULL,0);
							bgls->LFSTK_setImageGravity(TOP);
							bgls->LFSTK_setLabelGravity(TOP);
							bgls->LFSTK_setGadgetColours(GADGETBG,"#00000000","#00000000","#00000000","#00000000");
							bgls->LFSTK_setGadgetColourPair(NORMALCOLOUR,"#00000000","#00000000");
							bgls->pad=0;
							if(dockGravity==2)
								{
									bgls->flip=0;
									bgls->LFSTK_moveGadget(0,0);
								}
							else
								{
									bgls->flip=2;
									bgls->LFSTK_moveGadget(0,dockWindowHeight-sidehite);
								}
							st=bgls->LFSTK_setImageFromPath(dockBGImageLS,CENTRE,false);
							if(st!=CAIRO_STATUS_SUCCESS)
								bgls=NULL;
					
							bgrs=new LFSTK_buttonClass(dockWindow,"",psize-sidewid,0,sidewid,sidehite);
							bgrs->LFSTK_setStyle(BEVELNONE);
							bgrs->LFSTK_setUseWindowTile(false);
							bgrs->LFSTK_setTile(NULL,0);
							bgrs->LFSTK_setImageGravity(TOP);
							bgrs->LFSTK_setLabelGravity(TOP);
							bgrs->LFSTK_setGadgetColours(GADGETBG,"#00000000","#00000000","#00000000","#00000000");
							bgrs->LFSTK_setGadgetColourPair(NORMALCOLOUR,"#00000000","#00000000");
							bgrs->pad=0;

							bgrs->LFSTK_setStyle(BEVELNONE);
							bgrs->LFSTK_setUseWindowTile(true);
							bgrs->LFSTK_setImageGravity(TOP);
							bgrs->LFSTK_setLabelGravity(TOP);
							bgrs->pad=0;	

							if(dockGravity==2)
								{
									bgrs->flip=1;
									bgrs->LFSTK_moveGadget(sidewid,sidehite);
								}
							else
								{
									bgrs->flip=3;
									bgrs->LFSTK_moveGadget(sidewid,dockWindowHeight-sidehite);
								}

							st=bgrs->LFSTK_setImageFromPath(dockBGImageLS,CENTRE,false);
							if(st!=CAIRO_STATUS_SUCCESS)
								bgrs=NULL;
						}

					dockWindow->LFSTK_clearWindow(true);
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
				{
					win->y=mons->h-ICONIZESIZE;
					win->x=mons->x;
				}
			else
				{
					win->y=mons->y;
					win->x=mons->w-ICONIZESIZE;
				}

			win->w=ICONIZESIZE;
			win->h=ICONIZESIZE;
			apc->LFSTK_addWindow(win,"ICONWINDOW");

			iconWindow=apc->windows->back().window;	
			apc->windows->back().showing=true;
			buttonHide=new LFSTK_buttonClass(iconWindow,"",0,0,ICONIZESIZE,ICONIZESIZE);
			std::string iconpath=iconWindow->globalLib->LFSTK_findThemedIcon("gnome",themeIconData[CONTEXTMENUHIDE],"");
			buttonHide->LFSTK_setImageFromPath(iconpath,CENTRE,true);
			buttonHide->LFSTK_setMouseCallBack(hideCB,NULL,NULL);
			setGadgetDetails(buttonHide);

			buttonHide->LFSTK_setImageGravity(CENTRE);
			buttonHide->LFSTK_setTile(NULL,0);
			buttonHide->LFSTK_setFontString(prefs.LFSTK_getCString("font"),true);
			buttonHide->LFSTK_setLabelAutoColour(false);
			buttonHide->LFSTK_setGadgetColours(GADGETBG,"#00000000","#00000000","#00000000","#00000000");
			buttonHide->LFSTK_setContextWindow(NULL);
iconWindow->LFSTK_setContextWindow(NULL);

			holdpsize=psize;

			if(useTaskBar==true)
				updateTaskBar(true);

			dockWindow->LFSTK_showWindow(true);
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