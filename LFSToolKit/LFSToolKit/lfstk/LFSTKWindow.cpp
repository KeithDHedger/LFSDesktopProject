/*
 *
 * ©K. D. Hedger. Thu 23 Jul 16:14:48 BST 2015 kdhedger68713@gmail.com

 * This file (LFSTKwindow.cpp) is part of LFSToolKit.

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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <X11/Xresource.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xinerama.h>

#include <string.h>

//#include "LFSTKWindow.h"
#include "LFSTKGlobals.h"

#define _NET_WM_STATE_REMOVE	0
#define _NET_WM_STATE_ADD		1
#define _NET_WM_STATE_TOGGLE	2

struct Hints
{
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
};

/**
 * Set default colours.
 *
 */
void LFSTK_windowClass::initWindow(bool loadvars)
{
	char	*env;

	this->fontColourNames[NORMALCOLOUR]=strdup("white");
	this->fontColourNames[PRELIGHTCOLOUR]=strdup("black");
	this->fontColourNames[ACTIVECOLOUR]=strdup("white");
	this->fontColourNames[INACTIVECOLOUR]=strdup("grey80");

	this->windowColourNames[NORMALCOLOUR].name=strdup("grey50");
	this->windowColourNames[PRELIGHTCOLOUR].name=strdup("grey80");
	this->windowColourNames[ACTIVECOLOUR].name=strdup("grey40");
	this->windowColourNames[INACTIVECOLOUR].name=strdup("grey90");

	this->globalLib=new LFSTK_lib(loadvars);
	this->loadGlobalColours();
	this->isActive=true;
	this->useTile=false;


//Announce XDND support
//	Atom XdndAware=XInternAtom(this->display,"XdndAware",true);
//	Atom version=5;
//	XChangeProperty(this->display,this->window,XdndAware,XA_ATOM,32,PropModeReplace,(unsigned char*)&version,1);

}
/**
 * Reload colours from prefs.
 * \note to be fixed!
 */
void LFSTK_windowClass::LFSTK_reloadGlobals(void)
{
	char	*env;
	asprintf(&env,"%s/.config/LFS/lfstoolkit.rc",getenv("HOME"));
	this->globalLib->LFSTK_loadVarsFromFile(env,this->globalLib->LFSTK_getTKArgs());
	free(env);

	this->loadGlobalColours();
	this->isActive=true;
	this->LFSTK_clearWindow();
}

LFSTK_windowClass::~LFSTK_windowClass()
{
	if(this->fontString!=NULL)
		free(this->fontString);

	for(int j=0;j<MAXCOLOURS;j++)
		if(this->fontColourNames[j]!=NULL)
			free(this->fontColourNames[j]);

	for(int j=0;j<MAXCOLOURS;j++)
		if(this->windowColourNames[j].name!=NULL)
			free(this->windowColourNames[j].name);

	if(this->windowName!=NULL)
		free(this->windowName);

	delete this->globalLib;
	free(this->monitors);

	XFreeGC(this->display,this->gc);
	XDeleteContext(this->display,this->window,this->listeners);
	XDestroyWindow(this->display,this->window);
	XCloseDisplay(this->display);
}

LFSTK_windowClass::LFSTK_windowClass()
{
	this->initWindow(false);
}

void LFSTK_windowClass::loadGlobalColours(void)
{
	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setWindowColourName(j,this->globalLib->LFSTK_getGlobalString(j,TYPEWINDOW));

	this->LFSTK_setFontString(this->globalLib->LFSTK_getGlobalString(-1,TYPEFONT));
	for(int j=0;j<MAXCOLOURS;j++)
		this->LFSTK_setFontColourName(j,this->globalLib->LFSTK_getGlobalString(j,TYPEFONTCOLOUR));
	this->autoLabelColour=this->globalLib->LFSTK_getAutoLabelColour();
}

/**
* Set colour from name.
* \param name Colour name eg "#ff00ff".
* \return Pixel colour.
*/
unsigned long LFSTK_windowClass::LFSTK_setColour(const char *name)
{
	XColor tc,sc;
	XAllocNamedColor(this->display,this->cm,name,&sc,&tc);
	return sc.pixel;
}


/**
* Clear the window to the appropriate state.
*/
void LFSTK_windowClass::LFSTK_clearWindow(void)
{
	if(this->useTile==true)
		{
			XSetTSOrigin(this->display,this->gc,0,0);
			XSetFillStyle(this->display,this->gc,FillTiled);
			XSetTile(this->display,this->gc,this->tile[0]);
			XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);
		}
	else
		{
			XSetFillStyle(this->display,this->gc,FillSolid);
			XSetClipMask(this->display,this->gc,None);
			if(this->isActive==true)
				XSetForeground(this->display,this->gc,this->windowColourNames[NORMALCOLOUR].pixel);
			else
				XSetForeground(this->display,this->gc,this->windowColourNames[INACTIVECOLOUR].pixel);
			XFillRectangle(this->display,this->window,this->gc,0,0,this->w,this->h);
	}
}

/**
* Set child gadget listner.
* \param w Window of child gadget.
* \param l Child gadgets listner.
* \note Should only be called from child gadget constructor.
*/
void LFSTK_windowClass::LFSTK_setListener(Window w,listener *l)
{
	if (l==NULL)
		XDeleteContext(this->display,w,this->listeners);
	else
		XSaveContext(this->display,w,this->listeners,(XPointer)l);
}

/**
* Resize window.
* \param w New width.
* \param h New height.
* \param tellx Inform X (default=true).
* \note Should only be called from child gadget constructor.
*/
void LFSTK_windowClass::LFSTK_resizeWindow(int w,int h,bool tellx)
{
	this->w=w;
	this->h=h;
	if(tellx==true)
		XResizeWindow(this->display,this->window,w,h);
	this->LFSTK_clearWindow();
}

/**
* Move window.
* \param x New X.
* \param y New Y.
* \param tellx Inform X (default=true).
* \note Should only be called from child gadget constructor.
*/
void LFSTK_windowClass::LFSTK_moveWindow(int x,int y,bool tellx)
{
	this->x=x;
	this->y=y;
	if(tellx==true)
		XMoveWindow(this->display,this->window,x,y);
	this->LFSTK_clearWindow();
}


/**
* Get child gadget listner.
* \param w Window of child gadget.
* \return Listener of child gadget.
* \note For use in main event loop.
* \note eg:
* \note listener *l=wc->LFSTK_getListener(event.xany.window);
*/
listener* LFSTK_windowClass::LFSTK_getListener(Window w)
{
	listener *l=NULL;
	if (XFindContext(this->display,w,this->listeners,(XPointer *)&l)==0)
		return l;
	else
		return NULL;
}

/**
* Set default font string.
* \param s Font string.
* \note eg:
* \note "sans-serif:size=8".
*/
void LFSTK_windowClass::LFSTK_setFontString(const char *s)
{
	if(this->fontString!=NULL)
		free(this->fontString);
	this->fontString=strdup(s);
	this->font=this->globalLib->LFSTK_loadFont(this->display,this->screen,s);
}

/**
* Set window decorated.
* \param isDecorated.
*/
void LFSTK_windowClass::LFSTK_setDecorated(bool isDecorated)
{
	Atom	xa_prop[10];
	Hints	hints;

	if(isDecorated==false)
		{
			hints.flags=2;
			hints.decorations=0;
			hints.functions=0;
			hints.inputMode=0;
			hints.status=0;
			xa_prop[9]=XInternAtom(display,"_MOTIF_WM_HINTS",True);
			XChangeProperty(this->display,this->window,xa_prop[9],xa_prop[9],32,PropModeReplace,(unsigned char *)&hints,5);
		}
}

/**
* Get window geometry.
* \return geometry structure.
* \note Caller should free structure after use.
*/
geometryStruct *LFSTK_windowClass::LFSTK_getGeom()
{
	geometryStruct		*g=new geometryStruct;
	XWindowAttributes	xwa;
	int					x,y;
	Window				child;

	XTranslateCoordinates(this->display,this->window,this->rootWindow,0,0,&x,&y,&child );
	XGetWindowAttributes(this->display,this->window,&xwa);

	g->x=x;
	g->y=y;
	g->w=xwa.width;
	g->h=xwa.height;

	return(g);
}

/**
* Set the colour name for font.
* \param p Font state.
* \param colour Colour name.
* \note state is NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
*/
void LFSTK_windowClass::LFSTK_setFontColourName(int p,const char *colour)
{
	if(this->fontColourNames[p]!=NULL)
		free(this->fontColourNames[p]);
	this->fontColourNames[p]=strdup(colour);
}

/**
* Set the colour name for window.
* \param p Window state.
* \param colour Colour name.
* \note state is NORMALCOLOUR=0,PRELIGHTCOLOUR=1,ACTIVECOLOUR=2,INACTIVECOLOUR=3.
*/
void LFSTK_windowClass::LFSTK_setWindowColourName(int p,const char* colour)
{
	XColor tc,sc;

	if(this->windowColourNames[p].name!=NULL)
		free(this->windowColourNames[p].name);
	this->windowColourNames[p].name=strdup(colour);
	XAllocNamedColor(this->display,this->cm,colour,&sc,&tc);
	this->windowColourNames[p].pixel=sc.pixel;
}

/**
* get window sticky.
* \return isSticky.
*/
bool LFSTK_windowClass::LFSTK_getSticky(void)
{
	return(this->isSticky);
}

/**
* Set window sticky.
* \param set.
*/
void LFSTK_windowClass::LFSTK_setSticky(bool set)
{
	XClientMessageEvent	xclient;
	Atom				xa,xa1;

	memset(&xclient,0,sizeof(xclient) );
	xa=XInternAtom(this->display,"_NET_WM_STATE",False);
	xa1=XInternAtom(display,"_NET_WM_STATE_STICKY",False);

	xclient.type=ClientMessage;
	xclient.window=this->window;
	xclient.message_type=xa;
	xclient.format=32;
	if(set==true)
		xclient.data.l[0] =_NET_WM_STATE_ADD;
	else
		xclient.data.l[0] =_NET_WM_STATE_REMOVE;
	xclient.data.l[1] =xa1;
	xclient.data.l[2] = 0;
	XSendEvent(this->display,this->rootWindow,False,SubstructureRedirectMask | SubstructureNotifyMask,(XEvent *)&xclient);
	this->isSticky=set;
}

/**
* Set window type.
* \param type Window type string.
*/
void LFSTK_windowClass::LFSTK_setWindowType(const char *type)
{
	Atom	xa;
	Atom	xa_prop[1];

	xa=XInternAtom(display,"_NET_WM_WINDOW_TYPE",False);
	xa_prop[0]=XInternAtom(display,type,False);

	if(xa!=None)
		XChangeProperty(this->display,this->window,xa,XA_ATOM,32,PropModeReplace,(unsigned char *)&xa_prop,1);
}

/**
* Set window keep above.
* \param set.
* \note Must be set AFTER window has been mapped.
*/
void LFSTK_windowClass::LFSTK_setKeepAbove(bool set)
{
	XClientMessageEvent	xclient;
	Atom				xa,xa1;

	memset(&xclient,0,sizeof(xclient) );
	xa=XInternAtom(this->display,"_NET_WM_STATE",False);
	xa1=XInternAtom(display,"_NET_WM_STATE_ABOVE",False);

	xclient.type=ClientMessage;
	xclient.window=this->window;
	xclient.message_type=xa;
	xclient.format=32;
	if(set==true)
		xclient.data.l[0] =_NET_WM_STATE_ADD;
	else
		xclient.data.l[0] =_NET_WM_STATE_REMOVE;
	xclient.data.l[1] =xa1;
	xclient.data.l[2] = 0;
	XSendEvent(this->display,this->rootWindow,False,SubstructureRedirectMask | SubstructureNotifyMask,(XEvent *)&xclient);
}

/**
* Set window active.
* \param set.
* \note Must be set AFTER window has been mapped ( in expose event ).
*/
void LFSTK_windowClass::LFSTK_setActive(bool set)
{
	this->isActive=set;
}

/**
* Get window active.
* \return isActive.
*/
bool LFSTK_windowClass::LFSTK_getActive(void)
{
	return(this->isActive);
}

/**
* Load size and position of monitors.
*/
void LFSTK_windowClass::loadMonitorData(void)
{
	int					cnt=-1;
	XineramaScreenInfo	*p=NULL;

	cnt=ScreenCount(display);
	p=XineramaQueryScreens(this->display,&cnt);
	if(p!=NULL)
		{
			if(cnt>0)
				{
					this->monitors=(monitorStruct*)calloc(sizeof(monitorStruct),cnt);
					this->monitorCount=cnt;

					for (int j=0;j<cnt;j++)
						{
							monitors[j].x=p[j].x_org;
							monitors[j].y=p[j].y_org;
							monitors[j].w=p[j].width;
							monitors[j].h=p[j].height;
						}
				}
			XFree(p);
		}
}	

/**
* Get number of monitors.
* \return int Monitor cnt;
*/

int LFSTK_windowClass::LFSTK_getMonitorCount(void)
{
	return(this->monitorCount);
}

/**
* Get monitor data.
* \param monitor Monitor number.
* \return monitorsStruct* Monitor struct pointer;
* \note Do not free returned result;
*/

const monitorStruct* LFSTK_windowClass::LFSTK_getMonitorData(int monitor)
{
	return(&(this->monitors[monitor]));
}

/**
* Get window monitor.
* \return unsigned Monitor that window top left is on;
*/

int LFSTK_windowClass::LFSTK_windowOnMonitor(void)
{
	int thisx=this->x;
	int thisy=this->y;

	if(thisx<0)
		thisx=0;
	if(thisy<0)
		thisy=0;

	for(int j=0;j<this->monitorCount;j++)
		{
			if((thisx>=monitors[j].x) && (thisx<(monitors[j].x+monitors[j].w)) && (thisy>=monitors[j].y) && (thisy<(monitors[j].y+monitors[j].h)))
				return(j);
		}
	return(-1);
}


/**
* Main window constructor.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param name Window name.
* \param override True=ignore window manager placement.
* \param loadvars Load default variables from file, default=true.
*/
LFSTK_windowClass::LFSTK_windowClass(int x,int y,int w,int h,const char* name,bool override,bool loadvars)
{
	XSetWindowAttributes	wa;
	Atom					wm_delete_window;
	XClassHint				classHint;
	Atom					xa;
	Atom					xa_prop[3];

	this->display=XOpenDisplay(NULL);
	if(this->display==NULL)
		exit(1);

	this->x=x;
	this->y=y;
	this->w=w;
	this->h=h;
	this->fontString=NULL;
	this->isActive=false;

	this->screen=DefaultScreen(this->display);
	this->visual=DefaultVisual(this->display,this->screen);
	this->rootWindow=DefaultRootWindow(this->display);
	this->cm=DefaultColormap(this->display,this->screen);
	this->draw=XftDrawCreate(this->display,this->rootWindow,this->visual,this->cm);
	this->loadMonitorData();

	wa.win_gravity=NorthWestGravity;
	wa.override_redirect=override;
	wm_delete_window=XInternAtom(this->display,"WM_DELETE_WINDOW",0);

	this->window=XCreateWindow(this->display,this->rootWindow,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWOverrideRedirect,&wa);
	XSelectInput(this->display,this->window,StructureNotifyMask|ButtonPressMask | ButtonReleaseMask | ExposureMask|LeaveWindowMask|FocusChangeMask);

	XSetWMProtocols(this->display,this->window,&wm_delete_window,1);
	xa=XInternAtom(this->display,"_NET_WM_ALLOWED_ACTIONS",False);
	xa_prop[0]=XInternAtom(this->display,"_NET_WM_STATE_STICKY",False);
	xa_prop[1]=XInternAtom(this->display,"_NET_WM_STATE_ABOVE",False);
	xa_prop[2]=XInternAtom(this->display,"_NET_WM_ACTION_CHANGE_DESKTOP",False);

//	xa_prop[3]=XInternAtom(this->display,"_NET_WM_ACTION_CLOSE",False);
//	xa_prop[4]=XInternAtom(this->display,"_NET_WM_ACTION_BELOW",False);
//	xa_prop[5]=XInternAtom(this->display," _NET_WM_ACTION_FULLSCREEN",False);
//	xa_prop[6]=XInternAtom(this->display,"_NET_WM_ACTION_MOVE",False);
//	xa_prop[7]=XInternAtom(this->display,"_NET_WM_ACTION_RESIZE",False);
//	xa_prop[8]=XInternAtom(this->display,"_NET_WM_ACTION_MAXIMIZE_HORZ",False);
//	xa_prop[9]=XInternAtom(this->display,"_NET_WM_ACTION_MAXIMIZE_VERT",False);
//	xa_prop[10]=XInternAtom(this->display,"_NET_WM_ACTION_SHADE",False);
//	xa_prop[11]=XInternAtom(this->display,"_NET_WM_ACTION_MINIMIZE",False);
//	xa_prop[12]=XInternAtom(this->display,"_NET_WM_ACTION_STICK",False);
//	xa_prop[13]=XInternAtom(this->display,"_NET_WM_STATE_HIDDEN",False);
	if(xa!=None)
		XChangeProperty(this->display,this->window,xa,XA_ATOM,32,PropModeAppend,(unsigned char *)&xa_prop,3);

	this->LFSTK_setWindowType("_NET_WM_WINDOW_TYPE_NORMAL");
	this->windowName=strdup(name);
	XStoreName(this->display,this->window,this->windowName);
	classHint.res_name=this->windowName;
	classHint.res_class=(char*)"LFSToolKit";
	XSetClassHint(this->display,this->window,&classHint);

	this->gc=XCreateGC(this->display,this->rootWindow,0,NULL);

	this->listeners=XUniqueContext();
	this->LFSTK_setFontString((char*)DEFAULTFONT);

	this->LFSTK_setDecorated(true);
	this->initWindow(loadvars);

	if(this->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
	else
		this->useTile=false;
}

/**
* Show window and all sub windows.
* \param all Map all subwindows, default=true.
*/
void LFSTK_windowClass::LFSTK_showWindow(bool all)
{
	if(all==true)
		XMapSubwindows(this->display,this->window);
	XMapRaised(this->display,this->window);
}

/**
* Hide window.
*/
void LFSTK_windowClass::LFSTK_hideWindow(void)
{
	XUnmapWindow(this->display,this->window);
	XFlush(this->display);
	XSync(this->display,true);
}

/**
* Set XWindow property.
* \param property Property to set.
* \param type Type of Atom(s).
* \param format Format of Atom(s).
* \param dataptr Pointer to the data.
* \param propcnt Number of elements in array.
*/
void LFSTK_windowClass::LFSTK_setXProperty(Atom property,Atom type,int format,void *dataptr,int propcnt)
{
	XChangeProperty(this->display,this->window,property,type,format,PropModeReplace,(const unsigned char*)dataptr,propcnt);
}

/**
* Get monitors array.
* \return Const pointer to monitor array.
* \note Do not free returned structure.
*/
const monitorStruct* LFSTK_windowClass::LFSTK_getMonitors(void)
{
	return(this->monitors);
}

/**
* Set window background tile.
* \param path Path to image file.
* \param size Size of image or -1.
*/
void LFSTK_windowClass::LFSTK_setTile(const char *path,int size)
{
	if(this->globalLib->LFSTK_setPixmapsFromPath(this->display,this->visual,this->cm,this->window,path,&this->tile[0],&this->tile[1],size)==true)
		this->useTile=true;
	else
		this->useTile=false;
}

/**
* Send WM message to window.
* \param msg Message atome eg "_NET_ACTIVE_WINDOW".
* \param data0..data4 Data to send or 0.
*/
void LFSTK_windowClass::LFSTK_sendMessage(const char *msg,unsigned long data0,unsigned long data1,unsigned long data2,unsigned long data3,unsigned long data4)
{
	XEvent	event;
	long	mask=SubstructureRedirectMask|SubstructureNotifyMask;

	event.xclient.type=ClientMessage;
	event.xclient.serial=0;
	event.xclient.send_event=True;
	event.xclient.message_type=XInternAtom(this->display,msg,False);
	event.xclient.window=this->window;
	event.xclient.format=32;
	event.xclient.data.l[0]=data0;
	event.xclient.data.l[1]=data1;
	event.xclient.data.l[2]=data2;
	event.xclient.data.l[3]=data3;
	event.xclient.data.l[4]=data4;
	XSendEvent(this->display,this->rootWindow,False,mask,&event);
}

