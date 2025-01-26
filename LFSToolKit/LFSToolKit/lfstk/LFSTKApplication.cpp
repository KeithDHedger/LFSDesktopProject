/*
 *
 * ©K. D. Hedger. Fri  8 Jan 17:02:21 GMT 2021 keithdhedger@gmail.com

 * This file (LFSTKApplication.cpp) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "lfstk/LFSTKGlobals.h"

/**
* Destructor.
*/
LFSTK_applicationClass::~LFSTK_applicationClass()
{

	for(int j=0;j<this->windows->size();j++)
		delete this->windows->at(j).window;
	delete this->globalLib;
	delete this->windows;
	XCloseDisplay(this->display);
#ifdef _ENABLEDEBUG_
	cairo_debug_reset_static_data();
#endif
	freeAndNull((char**)&this->monitors);
}

/**
* Set timer
* \param int delay trigger callback every seconds.
* \param bool usemicroseconds use micro seconds instead of seconds.
*/
void LFSTK_applicationClass::LFSTK_setTimer(int delay,bool usemicroseconds)
{
	this->timer=delay;
	this->useTimer=true;
	this->useMicroSeconds=usemicroseconds;
}

/**
* Main application class.
*/
LFSTK_applicationClass::LFSTK_applicationClass()
{
	bool					gotargb;
	XVisualInfo			visual_template;
	XVisualInfo			*visual_list=NULL;
	int					nxvisuals=0;
	int					cnt;

	this->display=XOpenDisplay(NULL);
	if(this->display==NULL)
		exit(1);
	this->depth=32;
	this->screen=DefaultScreen(this->display);
	this->visual=DefaultVisual(this->display,this->screen);
	this->rootWindow=DefaultRootWindow(this->display);
	visual_template.screen=this->screen;
	visual_list=XGetVisualInfo(this->display,0,&visual_template,&nxvisuals);
	this->gotARGB=false;
	for(int i=0;i<nxvisuals;i++)
		{
			if(visual_list[i].depth==32 && (visual_list[i].red_mask==0xff0000 && visual_list[i].green_mask==0x00ff00 && visual_list[i].blue_mask==0x0000ff ))
				{
					this->visual=visual_list[i].visual;
					this->depth=visual_list[i].depth;
					this->gotARGB=true;
					break;
				}
		}
	XFree(visual_list);

	this->LFSTK_loadMonitorData();

	this->cm=XCreateColormap(this->display,this->rootWindow,this->visual,AllocNone);
	this->displayWidth=DisplayWidth(this->display,this->screen);
	this->displayHeight=DisplayHeight(this->display,this->screen);
	this->blackColour=BlackPixel(this->display,this->screen);
	this->whiteColour=WhitePixel(this->display,this->screen);

	this->globalLib=new LFSTK_lib(true);
	this->windows=new std::vector<windowData>;
	this->displayNum=ConnectionNumber(this->display);

	this->userHome=getenv("HOME");
	this->configDir=this->userHome + std::string("/.config/LFS");
	this->iconThemeName=this->globalLib->LFSTK_oneLiner("cat '%S'/lfsdesktop.rc|grep -i icontheme|awk '{print $2}'",this->configDir);

//set up atoms
	const char *appAtomNames[]={
						"_NET_WM_WINDOW_TYPE_DESKTOP",
						"_NET_WM_WINDOW_TYPE_DOCK",
						"_NET_WM_WINDOW_TYPE_NORMAL",
						"_NET_WM_WINDOW_TYPE_DIALOG",
						"_NET_WM_WINDOW_TYPE_TOOL",
						"_NET_WM_STATE_SKIP_TASKBAR",
						"_NET_WM_WINDOW_TYPE_MENU",
						"_NET_WM_WINDOW_TYPE_TOOLBAR",
						"_NET_WM_WINDOW_TYPE_UTILITY",
						"_NET_WM_WINDOW_TYPE_SPLASH",
						"_NET_WM_WINDOW_TYPE",

						"_NET_WM_STATE",
						"_NET_WM_STATE_STICKY",
						"_NET_WM_STATE_ABOVE",
						"_NET_WM_STATE_BELOW",
						"_NET_WM_STATE_HIDDEN",

						"_NET_WM_ALLOWED_ACTIONS",
						"_NET_WM_ACTION_CHANGE_DESKTOP",
						"_NET_WM_PID",
						"_NET_WM_NAME",

						"_NET_ACTIVE_WINDOW",
						"_NET_CURRENT_DESKTOP",
						"_NET_NUMBER_OF_DESKTOPS",
						"_NET_WM_DESKTOP",

						"WM_CLASS",
						"WM_STATE",
						"WM_DELETE_WINDOW",
						"WM_PROTOCOLS",
						"_MOTIF_WM_HINTS",
						"UTF8_STRING",
						NULL};
	cnt=0;
	while(appAtomNames[cnt]!=NULL)
		{
			this->appAtomsHashed[LFSTK_UtilityClass::LFSTK_hashFromKey(appAtomNames[cnt])]=XInternAtom(this->display,appAtomNames[cnt],false);
			//fprintf(stderr,"atom=%p atom name=%s\n",this->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey(appAtomNames[cnt])),appAtomNames[cnt]);
			cnt++;
		}
}

/**
* Return a default windowInitStruct filled in.
* \return inititialized structure.
*/
windowInitStruct* LFSTK_applicationClass::LFSTK_getDefaultWInit(void)
{
	windowInitStruct *wininit=new windowInitStruct;

	wininit->x=0;
	wininit->y=0;
	wininit->w=1;
	wininit->h=1;
	wininit->windowName="LFSTK Window";
	wininit->appName="LFSTKApplication";
	wininit->className="LFSTKApplication";
	wininit->overRide=false;
	wininit->loadVars=true;
	wininit->shutDisplayOnExit=false;
	wininit->windowType=this->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_NORMAL"));
	wininit->decorated=true;
	wininit->level=NORMAL;
	wininit->display=this->display;
	wininit->wc=NULL;
	wininit->app=this;
	return(wininit);
}

/**
* Add window to app.
* \paramw indowInitStruct *wi window init structure.
* \param const char *name window name.
* \note wi can be NULL, defaults are used.
* \note name can be NULL, if window is set to 1px size and type to _NET_WM_WINDOW_TYPE_DOCK.
* \note class take ownership of wi don't delete.
*/
void LFSTK_applicationClass::LFSTK_addWindow(windowInitStruct *wi,const char *name,const char *appname)
{
	windowInitStruct	*win;

	if(wi==NULL)
		{
			win=this->LFSTK_getDefaultWInit();
		}
	else
		win=wi;

	if(name==NULL)
		{
			win->windowName="";
			win->x=-1;
			win->y=-1;
			win->overRide=true;
			win->windowType=this->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DOCK"));
			win->decorated=false;
		}
	else
		win->windowName=name;
	if(appname!=NULL)
		win->appName=appname;
	
	this->windows->push_back({new LFSTK_windowClass(win,this),false,false});
	if(this->windows->size()==1)
		this->mainWindow=this->windows->back().window;
	if(win->level==ABOVEALL)
		this->windows->back().window->LFSTK_setKeepAbove(true);
	if(win->level==BELOWALL)
		this->windows->back().window->LFSTK_setKeepBelow(true);
	delete win;
}

/**
* Add tool window to app.
* \paramw indowInitStruct *wi window init structure.
* \param const char *name window name.
* \note wi can be NULL, defaults are used.
* \note class takes ownership of wi don't delete.
*/
void LFSTK_applicationClass::LFSTK_addToolWindow(windowInitStruct *wi,const char *appname)
{
	windowInitStruct	*win;

	if(wi==NULL)
		{
			win=this->LFSTK_getDefaultWInit();
			win->windowType=this->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_TOOL"));
		}
	else
		{
			win=wi;
		}

	win->decorated=false;
	win->overRide=true;
	if(appname!=NULL)
		win->appName=appname;

	if(win->level==ABOVEALL)
		this->windows->back().window->LFSTK_setKeepAbove(true);
	if(win->level==BELOWALL)
		this->windows->back().window->LFSTK_setKeepBelow(true);

	this->windows->push_back({new LFSTK_toolWindowClass(win,this),false,false});
	delete win;
}

/**
* Set timer callback for main loop,
* \param bool functionname (LFSTK_applicationClass *p,void* ud)
* \param void* ud User data passed to function.
*/
void LFSTK_applicationClass::LFSTK_setTimerCallBack(bool (*timer)(LFSTK_applicationClass*,void*),void* ud)
{
	this->callBacks.validCallbacks|=TIMERCB;
	this->callBacks.timerCallback=timer;
	this->callBacks.keyUserData=ud;
	this->callBacks.runTheCallback=true;
	this->callBacks.ignoreOrphanModKeys=true;
}

/**
* Set event callback for main loop,
* \param bool functionname (LFSTK_applicationClass *p,void* ud)
* \param void* ud User data passed to function.
*/
void LFSTK_applicationClass::LFSTK_setEventCallBack(bool (*evcb)(LFSTK_applicationClass*,XEvent*),void* ud)
{
//	this->callBacks.validCallbacks|=EVENTCB;
	this->eventCallback=evcb;
	//this->callBacks.keyUserData=ud;
	//this->callBacks.runTheCallback=true;
	//this->callBacks.ignoreOrphanModKeys=true;
}

/**
* Run main loop,
* \note default to this->mainWindow.
*/
int LFSTK_applicationClass::LFSTK_runApp(void)
{
	XEvent	event;
	bool		retval=false;

	this->mainLoop=true;
	fd_set readfd;
	struct timeval tv={0,0};
 

	if(this->useMicroSeconds==false)
		tv.tv_usec=this->timer*1000000;
	else	
		tv.tv_usec=this->timer;

	FD_ZERO(&readfd);
	FD_SET(this->displayNum,&readfd);
// Main loop
	this->mainWindow->LFSTK_showWindow();
	this->mainWindow->LFSTK_clearWindow(true);
	while(XPending(this->display))
		{
			XNextEvent(this->display, &event);
			this->mainWindow->LFSTK_handleWindowEvents(&event);
		}
	this->mainWindow->LFSTK_redrawAllGadgets();

	while(this->mainLoop==true)
		{
// Wait for X Event or a Timer
			if((select(this->displayNum+1,&readfd,NULL,NULL,&tv)!=0) || (this->useTimer==false))
				{
// Handle XEvents
					while(XPending(this->display))
						{
							XNextEvent(this->display,&event);

							if(this->eventCallback!=NULL)
								{
									retval=this->eventCallback(this,&event);
									if(retval==true)
										continue;
								}

							mappedListener *ml=this->mainWindow->LFSTK_getMappedListener(event.xany.window);

							if(ml!=NULL)
								ml->function(ml->gadget,&event,ml->type);

							if(this->mainWindow->LFSTK_handleWindowEvents(&event)<0)
								this->mainLoop=false;

//handle tool wndows and gadgets
							for(unsigned j=1;j<this->windows->size();j++)
								{
									if(this->windows->at(j).showing==true)
										{
											ml=this->windows->at(j).window->LFSTK_getMappedListener(event.xany.window);
											if(ml!=NULL)
												ml->function(ml->gadget,&event,ml->type);

											this->windows->at(j).window->LFSTK_handleWindowEvents(&event);
										}
								}
						}
					usleep(1000);
				}
			else
				{
					bool retval=false;
					if((this->callBacks.validCallbacks & TIMERCB) && (this->callBacks.runTheCallback==true))
						retval=this->callBacks.timerCallback(this,this->mainWindow->userData);
					if(retval==true)
						{
							if(this->useMicroSeconds==false)
								tv.tv_usec=this->timer*1000000;
							else	
								tv.tv_usec=this->timer;
							FD_SET(this->displayNum,&readfd);
						}
					else
						this->useTimer=false;
				}
		}
	XSync(this->display,false);
	return(this->exitValue);
}

/**
* Run blocking loop,
* \param int window Window number to run
* \note Window numbers are numbered from 0 and are in order of creation.
*/
int LFSTK_applicationClass::LFSTK_runWindowLoop(int window)
{
	XEvent	event;
	fd_set readfd;
	struct timeval tv={0,0};

	tv.tv_sec=this->timer;;
	FD_ZERO(&readfd);
	FD_SET(this->displayNum,&readfd);
	this->windows->at(window).loopFlag=true;
	this->windows->at(window).window->LFSTK_showWindow();

	while(this->windows->at(window).loopFlag==true)
		{
			if((select(this->displayNum+1,&readfd,NULL,NULL,&tv)!=0) || (this->useTimer==false))
				{
					while(XPending(this->display))
						{
							XNextEvent(this->display,&event);
							mappedListener *ml=this->windows->at(window).window->LFSTK_getMappedListener(event.xany.window);

							if(ml!=NULL)
								ml->function(ml->gadget,&event,ml->type);

							if(this->windows->at(window).window->LFSTK_handleWindowEvents(&event)<0)
								this->windows->at(window).loopFlag=false;
						}
				}
			else
				{
					bool retval=false;
					if((this->callBacks.validCallbacks & TIMERCB) && (this->callBacks.runTheCallback==true))
						retval=this->callBacks.timerCallback(this,USERDATA(this->windows->at(window).window->userData));
					if(retval==true)
						{
							tv.tv_sec=this->timer;;
							FD_SET(this->displayNum,&readfd);
						}
					else
						this->useTimer=false;
				}
		}
	this->windows->at(window).window->LFSTK_hideWindow();
	XSync(this->display,false);

	return(this->exitValue);
}

/**
* Run blocking loop,
* \param LFSTK_windowClass *win Window to run
* \note Window is found by object.
*/
int LFSTK_applicationClass::LFSTK_runWindowLoop(LFSTK_windowClass *win)
{
	XEvent	event;
	int		windownum=this->LFSTK_findWindow(win);

	if(windownum==-1)
		return(-1);
	else
		this->LFSTK_runWindowLoop(windownum);
	XSync(this->display,false);
	return(this->exitValue);
}

/**
* Find window.
* \param LFSTK_windowClass *win Window to find.
* \return Window number.
*/
int LFSTK_applicationClass::LFSTK_findWindow(LFSTK_windowClass *win)
{
	for(int j=0;j<this->windows->size();j++)
		{
			if(this->windows->at(j).window==win)
				return(j);
		}
	return(-1);
}

/**
* Load size and position of monitors.
*/
void LFSTK_applicationClass::LFSTK_loadMonitorData(void)//TODO//
{
	int					cnt=-1;
	XineramaScreenInfo	*p=NULL;

	if(this->monitors!=NULL)
		freeAndNull((char**)&this->monitors);

	cnt=ScreenCount(this->display);
	p=XineramaQueryScreens(this->display,&cnt);
	if(p!=NULL)
		{
			if(cnt>0)
				{
					this->monitors=(monitorStruct*)calloc(sizeof(monitorStruct),cnt);
					this->monitorCount=cnt;

					for (int j=0; j<cnt; j++)
						{
							monitors[j].x=p[j].x_org;
							monitors[j].y=p[j].y_org;
							monitors[j].w=p[j].width;
							monitors[j].h=p[j].height;
							//fprintf(stderr,"j=%i x=%i y=%i w=%i h=%i\n",j,monitors[j].x,monitors[j].y,monitors[j].w,monitors[j].h);
						}
				}
			XFree(p);
		}
}

/**
* Get number of monitors.
* \return int Monitor cnt;
*/
int LFSTK_applicationClass::LFSTK_getMonitorCount(void)
{
	return(this->monitorCount);
}

/**
* Get monitor data.
* \param monitor Monitor number.
* \return monitorsStruct* Monitor struct pointer;
* \note Do not free returned result;
*/
const monitorStruct* LFSTK_applicationClass::LFSTK_getMonitorData(int monitor)
{
	return(&(this->monitors[monitor]));
}

/**
* Get monitors array.
* \return Const pointer to monitor array.
* \note Do not free returned structure.
*/
const monitorStruct* LFSTK_applicationClass::LFSTK_getMonitors(void)
{
	return(this->monitors);
}