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
	free(this->configDir);
	if(this->iconThemeName!=NULL)
		free(this->iconThemeName);
#ifdef _ENABLEDEBUG_
	cairo_debug_reset_static_data();
#endif
	free(this->monitors);
}

/**
* Set timer
* \param seconds trigger callback every seconds.
*/
void LFSTK_applicationClass::LFSTK_setTimer(int seconds)
{
	this->timer=seconds;
	this->useTimer=true;
}

/**
* Main application class.
*/
LFSTK_applicationClass::LFSTK_applicationClass()
{
	bool					gotargb;
	XVisualInfo				visual_template;
	XVisualInfo				*visual_list=NULL;
	int						nxvisuals=0;

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
	asprintf(&this->configDir,"%s/.config/LFS",this->userHome);
	this->iconThemeName=this->globalLib->LFSTK_oneLiner("cat '%s'/lfsdesktop.rc|grep -i icontheme|awk '{print $2}'",this->configDir);
}

/**
* Add window to app.
* \paramw indowInitStruct *wi window init structure.
* \param const char *name window name.
* \note wi can be NULL, defaults are used.
* \note name can be NULL, if window is set to 1px size and type to _NET_WM_WINDOW_TYPE_DOCK.
* \note class take ownership of wi don't delete.
*/
void LFSTK_applicationClass::LFSTK_addWindow(windowInitStruct *wi,const char *name)
{
	windowInitStruct	*win;

	if(wi==NULL)
		win=new windowInitStruct;
	else
		win=wi;

	win->app=this;
	if(name==NULL)
		{
			win->name="";
			win->x=-1;
			win->y=-1;
			win->overRide=true;
			win->windowType="_NET_WM_WINDOW_TYPE_DOCK";
			win->decorated=false;
		}
	else
		win->name=name;
	win->loadVars=true;
	this->windows->push_back({new LFSTK_windowClass(win,this),false,false});
	if(this->windows->size()==1)
		this->mainWindow=this->windows->back().window;
	delete win;
}

/**
* Add tool window to app.
* \paramw indowInitStruct *wi window init structure.
* \param const char *name window name.
* \note wi can be NULL, defaults are used.
* \note class takes ownership of wi don't delete.
*/
void LFSTK_applicationClass::LFSTK_addToolWindow(windowInitStruct *wi)
{
	windowInitStruct	*win;

	if(wi==NULL)
		win=new windowInitStruct;
	else
		win=wi;

	win->app=this;
	win->name="";
	win->loadVars=true;
	win->windowType="_NET_WM_WINDOW_TYPE_DOCK";
	win->decorated=false;
	win->overRide=true;
	win->level=ABOVEALL;

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
* Run main loop,
* \note default to this->mainWindow.
*/
int LFSTK_applicationClass::LFSTK_runApp(void)
{
	XEvent	event;
	this->mainLoop=true;
	fd_set readfd;
	struct timeval tv={0,0};
 
	tv.tv_sec=this->timer;
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
							XNextEvent(this->display, &event);
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
				}
			else
				{
					bool retval=false;
					if((this->callBacks.validCallbacks & TIMERCB) && (this->callBacks.runTheCallback==true))
						retval=this->callBacks.timerCallback(this,this->mainWindow->userData);
					if(retval==true)
						{
							tv.tv_sec=this->timer;;
							FD_SET(this->displayNum,&readfd);
						}
					else
						this->useTimer=false;
				}
		}

	//-->>XFlush(this->display);
	//-->>XSync(this->display,true);
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
		free(this->monitors);

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