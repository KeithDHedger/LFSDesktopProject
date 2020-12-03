/*
 *
 * ©K. D. Hedger. Thu 23 Jul 16:14:48 BST 2015 keithdhedger@gmail.com

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
#include <map>

#include "lfstk/LFSTKGlobals.h"

#define _NET_WM_STATE_REMOVE	0
#define _NET_WM_STATE_ADD		1
#define _NET_WM_STATE_TOGGLE	2

struct Hints
{
	unsigned long  flags;
	unsigned long	functions;
	unsigned long	decorations;
	long			inputMode;
	unsigned long	status;
};

/**
 * Get current gadget map list.
 */
std::map<int,mappedListener*> *LFSTK_windowClass::LFSTK_getGadgets(void)
{
	return &(this->gadgetMap);
}

/**
 * Get a mapped listener from window id.
 * \param window is window id.
 */
mappedListener* LFSTK_windowClass::LFSTK_getMappedListener(int window)
{
	mappedListener* l=NULL;

	l=this->gadgetMap[window];
	return(l);
}

/**
 * Add a mapped listener.
 * \param mapwindow is window id.
 * \param ml is mappedListener pointer window retains ownership.
 */
void LFSTK_windowClass::LFSTK_addMappedListener(int mapwindow,mappedListener* ml)
{
	this->gadgetMap[mapwindow]=ml;
}

/**
 * \param x,y Global mouse co-ords.
 * \return found gadget or NULL.
 */
LFSTK_gadgetClass* LFSTK_windowClass::LFSTK_findGadgetByPos(int x, int y)
{
	mappedListener	*ml=NULL;
	geometryStruct	geom;
	for (std::map<int,mappedListener*>::iterator it=this->gadgetMap.begin();it!=this->gadgetMap.end();++it)
		{
			ml=it->second;
			if( (ml!=NULL) && (ml->gadget!=NULL) )
				{
					ml->gadget->LFSTK_getGlobalGeom(&geom);
//TODO//
					if((x>geom.x) && (x<geom.x+geom.w) && (y>geom.y) && (y<geom.y+geom.h) )
						return(ml->gadget);
				}
		}
	return(NULL);
}

/**
 * \return number of gadgets in window.
 */
int LFSTK_windowClass::LFSTK_gadgetCount(void)
{
	return(this->gadgetMap.size());
}

/**
 * Set default colours.
 *
 */
void LFSTK_windowClass::LFSTK_setWindowTitle(const char *title)
{
	if(this->windowName!=NULL)
		free(this->windowName);
	this->windowName=strdup(title);
	XStoreName(this->display,this->window,this->windowName);
}

/**
 * Set default colours.
 *
 */
void LFSTK_windowClass::initWindow(bool loadvars)
{
	this->fontColourNames[NORMALCOLOUR]=strdup("white");
	this->fontColourNames[PRELIGHTCOLOUR]=strdup("black");
	this->fontColourNames[ACTIVECOLOUR]=strdup("white");
	this->fontColourNames[INACTIVECOLOUR]=strdup("grey80");

	this->windowColourNames[NORMALCOLOUR].name=strdup("grey50");
	this->windowColourNames[PRELIGHTCOLOUR].name=strdup("grey80");
	this->windowColourNames[ACTIVECOLOUR].name=strdup("grey40");
	this->windowColourNames[INACTIVECOLOUR].name=strdup("grey90");

	this->globalLib=new LFSTK_lib(loadvars);
	if(loadvars==true)
		this->loadGlobalColours();
	this->isActive=true;
	this->useTile=false;
	this->gadgetMap.clear();
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

/**
 * Delete window
 * \note Attached gadgets automatically deleted so destroy user data first.
 */
LFSTK_windowClass::~LFSTK_windowClass()
{
	this->LFSTK_hideWindow();

	if(this->pattern!=NULL)
		cairo_pattern_destroy(this->pattern);

	cairo_destroy(this->cr);
	cairo_surface_destroy(this->sfc);

	if(this->fontString!=NULL)
		free(this->fontString);

	for(int j=0; j<MAXCOLOURS; j++)
		{
			if(this->fontColourNames[j]!=NULL)
				free(this->fontColourNames[j]);
			
			if(this->windowColourNames[j].name!=NULL)
				{
					free(this->windowColourNames[j].name);
					XFreeColors(this->display,this->cm,(long unsigned int*)&this->windowColourNames[j].pixel,1,0);
				}
		}

	if(this->windowName!=NULL)
		free(this->windowName);

	delete this->globalLib;
	free(this->monitors);

	if(!this->gadgetMap.empty())
		{
			for (std::map<int,mappedListener*>::iterator it=this->gadgetMap.begin();it!=this->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;
					if (ml!=NULL)
						{
							if(ml->gadget!=NULL)
								delete ml->gadget;
							delete ml;
						}
				}
			this->gadgetMap.clear();
		}

	this->clipBuffer.clear();
	this->dNdTypes.clear();
	free(this->configDir);
	XFreeGC(this->display,this->gc);
	XDestroyWindow(this->display,this->window);
	if(this->px!=None)
		XFreePixmap(this->display,this->px);

	if(this->closeDisplayOnExit==true)
		XCloseDisplay(this->display);
}

/**
 * Dummy init.
 */
LFSTK_windowClass::LFSTK_windowClass()
{
	return;
	this->initWindow(false);
	this->setWindowGeom(0,0,0,0,WINDSETALL);
	this->gadgetMap.clear();
}

void LFSTK_windowClass::loadGlobalColours(void)
{
	for(int j=0; j<MAXCOLOURS; j++)
		this->LFSTK_setWindowColourName(j,this->globalLib->LFSTK_getGlobalString(j,TYPEWINDOW));

	this->LFSTK_setFontString(this->globalLib->LFSTK_getGlobalString(-1,TYPEFONT));
	for(int j=0; j<MAXCOLOURS; j++)
		this->LFSTK_setFontColourName(j,this->globalLib->LFSTK_getGlobalString(j,TYPEFONTCOLOUR));
	this->autoLabelColour=this->globalLib->LFSTK_getAutoLabelColour();
}

/**
* Set window pixmap
* \param Pixmap Source pixmap.
* \param w Width.
* \param h Height.
*/
void LFSTK_windowClass::LFSTK_setWindowPixmap(Pixmap pixmap,int w,int h)
{
	cairo_surface_t	*surfaceto=NULL;
	cairo_surface_t	*surfacefrom=NULL;
	cairo_t			*cr=NULL;
	cairo_status_t	status;

	if(pixmap==None)
		{
			this->useTile=false;
			return;
		}
	xLibErrorTK=false;
	XSetErrorHandler(xErrHandler);
	XSynchronize(this->display,true);

	if(this->px!=None)
		XFreePixmap(this->display,this->px);
	this->px=XCreatePixmap(display,this->window,w,h,this->depth);

//HMMmmmmmm??
	surfaceto=cairo_xlib_surface_create(display,this->px,this->visual,w,h);
	surfacefrom=cairo_xlib_surface_create(display,pixmap,DefaultVisual(display,this->screen),w,h);
	cr=cairo_create(surfaceto);

	if(status==CAIRO_STATUS_SUCCESS)
		{
			cairo_save(cr);
				cairo_reset_clip(cr);
				cairo_set_source_surface(cr,surfacefrom,0,0);
				cairo_paint(cr);
			cairo_restore(cr);

			XSetWindowBackgroundPixmap(this->display,this->window,this->px);
			XClearWindow(display,this->window);
			XSync(display,false);
			this->usePixmap=true;
		}
	cairo_surface_destroy(surfaceto);
	cairo_surface_destroy(surfacefrom);
	cairo_destroy(cr);
	XSetErrorHandler(NULL);
	XSynchronize(this->display,false);

	if(xLibErrorTK==true)
		{
			xLibErrorTK=false;
			this->usePixmap=false;
			this->useTile=false;
			XSetWindowBackgroundPixmap(this->display,this->window,None);
			XClearWindow(display,this->window);
		}
	XSync(display,false);
}

/**
* Clear the window to the appropriate state.
* \param bool true=clear window on all gadgets.
* \note param is optional defaults to false.
*/
void LFSTK_windowClass::LFSTK_clearWindow(bool cleargadgets)
{
	int	state=NORMALCOLOUR;

	if(this->usePixmap==true)
		return;

	if((!this->gadgetMap.empty()) && (cleargadgets==true))
		{
			for (std::map<int,mappedListener*>::iterator it=this->gadgetMap.begin();it!=this->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;
					if (ml!=NULL)
						{
							if(ml->gadget!=NULL)
								ml->gadget->LFSTK_clearWindow();
						}
				}
		}

	if(this->isActive==false)
		state=INACTIVECOLOUR;
	if(this->useTile==true)
		{
			cairo_save(this->cr);
				cairo_reset_clip (this->cr);
				cairo_set_source(this->cr,this->pattern);
				cairo_paint(this->cr);
			cairo_restore(this->cr);
		}
	else
		{
			cairo_save(this->cr);
				cairo_reset_clip (this->cr);
				cairo_set_source_rgba(this->cr,this->windowColourNames[state].RGBAColour.r,this->windowColourNames[state].RGBAColour.g,this->windowColourNames[state].RGBAColour.b,this->windowColourNames[state].RGBAColour.a);
				cairo_paint(this->cr);
			cairo_restore(this->cr);
		}
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
	this->setWindowGeom(0,0,w,h,WINDSETWH);
	if(tellx==true)
		XResizeWindow(this->display,this->window,w,h);

	this->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
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
	this->setWindowGeom(x,y,0,0,WINDSETXY);
	if(tellx==true)
		XMoveWindow(this->display,this->window,x,y);
	this->LFSTK_clearWindow();
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
	this->windowColourNames[p].pixel=tc.pixel;

	this->windowColourNames[p].RGBAColour.r=((this->windowColourNames[p].pixel>>16) & 0xff)/256.0;
	this->windowColourNames[p].RGBAColour.g=((this->windowColourNames[p].pixel>>8) & 0xff)/256.0;
	this->windowColourNames[p].RGBAColour.b=((this->windowColourNames[p].pixel>>0) & 0xff)/256.0;
	this->windowColourNames[p].RGBAColour.a=1.0;
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
* \note MUST be set AFTER show window.
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
	xclient.data.l[2]=0;
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

	xa=XInternAtom(this->display,"_NET_WM_WINDOW_TYPE",False);
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
	xclient.data.l[2]=0;
	XSendEvent(this->display,this->window,False,SubstructureRedirectMask | SubstructureNotifyMask,(XEvent *)&xclient);
}

/**
* Set window keep below.
* \param set.
* \note Must be set AFTER window has been mapped.
*/
void LFSTK_windowClass::LFSTK_setKeepBelow(bool set)
{
	XClientMessageEvent	xclient;
	Atom				xa,xa1;

	memset(&xclient,0,sizeof(xclient) );
	xa=XInternAtom(this->display,"_NET_WM_STATE",False);
	xa1=XInternAtom(display,"_NET_WM_STATE_BELOW",False);

	xclient.type=ClientMessage;
	xclient.window=this->window;
	xclient.message_type=xa;
	xclient.format=32;
	if(set==true)
		xclient.data.l[0] =_NET_WM_STATE_ADD;
	else
		xclient.data.l[0] =_NET_WM_STATE_REMOVE;
	xclient.data.l[1] =xa1;
	xclient.data.l[2]=0;
	XSendEvent(this->display,this->window,False,SubstructureRedirectMask | SubstructureNotifyMask,(XEvent *)&xclient);
}


/**
* Set transient for window.
* \param w Window.
*/
void LFSTK_windowClass::LFSTK_setTransientFor(Window w)
{
	XSetTransientForHint(this->display,this->window,w);
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

					for (int j=0; j<cnt; j++)
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
	int thisx=this->windowGeom.x;
	int thisy=this->windowGeom.y;

	if(thisx<0)
		thisx=0;
	if(thisy<0)
		thisy=0;

	for(int j=0; j<this->monitorCount; j++)
		{
			if((thisx>=monitors[j].x) && (thisx<(monitors[j].x+monitors[j].w)) && (thisy>=monitors[j].y) && (thisy<(monitors[j].y+monitors[j].h)))
				return(j);
		}
	return(-1);
}

/**
* Common window constructor.
* \param windowInitStruct *wi.
* \note wi contains startup window params.
*/
void LFSTK_windowClass::windowClassInitCommon(windowInitStruct *wi)
{
//	DEBUGFUNC("int x=%i",wi->x);
//showFileData=false;
//	DEBUGFUNC("int y=%i",wi->y);
//	DEBUGFUNC("int w=%i",wi->w);
//	DEBUGFUNC("int h=%i",wi->h);
//	DEBUGFUNC("const char	*name=%s",wi->name);
//	DEBUGFUNC("bool override=%i",wi->overRide);
//	DEBUGFUNC("bool loadVars=%i",wi->loadVars);
//	DEBUGFUNC("bool shutDisplayOnExit=%i",wi->shutDisplayOnExit);
//	DEBUGFUNC("const char	*windowType=%s",wi->windowType);
//showFileData=true;

	XSetWindowAttributes	wa;
	Atom					wm_delete_window;
	XClassHint				classHint;
	Atom					xa;
	Atom					xa_prop[3];
	bool					gotargb;
	XVisualInfo				visual_template;
	XVisualInfo				*visual_list=NULL;
	int						nxvisuals=0;

	this->display=XOpenDisplay(NULL);
	if(this->display==NULL)
		exit(1);

	this->depth=32;
	this->setWindowGeom(wi->x,wi->y,wi->w,wi->h,WINDSETALL);

	this->fontString=NULL;
	this->isActive=false;
	this->acceptDnd=false;

	this->screen=DefaultScreen(this->display);
	this->visual=DefaultVisual(this->display,this->screen);
	this->rootWindow=DefaultRootWindow(this->display);
	this->loadMonitorData();

	wa.win_gravity=NorthWestGravity;
	wa.override_redirect=wi->overRide;
	wm_delete_window=XInternAtom(this->display,"WM_DELETE_WINDOW",0);

	visual_template.screen=this->screen;
	visual_list=XGetVisualInfo(this->display,0,&visual_template,&nxvisuals);
	gotargb=false;
	for(int i=0;i<nxvisuals;i++)
		{
			if(visual_list[i].depth==32 && (visual_list[i].red_mask==0xff0000 && visual_list[i].green_mask==0x00ff00 && visual_list[i].blue_mask==0x0000ff ))
				{
					this->visual=visual_list[i].visual;
					this->depth=visual_list[i].depth;
					gotargb=true;
					break;
				}
		}
	XFree(visual_list);

	this->cm=XCreateColormap(this->display,this->rootWindow,this->visual,AllocNone);
	if(gotargb==true)
		{
			wa.colormap=this->cm;
			wa.border_pixel=0;
			wa.background_pixel=0;

			this->window=XCreateWindow(this->display,this->rootWindow,wi->x,wi->y,wi->w,wi->h,0,this->depth,InputOutput,this->visual,CWColormap | CWBorderPixel |CWWinGravity|CWOverrideRedirect,&wa);
		}
	else
		{
			this->window=XCreateWindow(this->display,this->rootWindow,wi->x,wi->y,wi->w,wi->h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWOverrideRedirect,&wa);
		}

	XSelectInput(this->display,this->window,SubstructureRedirectMask|StructureNotifyMask|ButtonPressMask | ButtonReleaseMask | ExposureMask|LeaveWindowMask|FocusChangeMask|SelectionClear|SelectionRequest);

	XSetWMProtocols(this->display,this->window,&wm_delete_window,1);
	xa=XInternAtom(this->display,"_NET_WM_ALLOWED_ACTIONS",False);
	xa_prop[0]=XInternAtom(this->display,"_NET_WM_STATE_STICKY",False);
	xa_prop[1]=XInternAtom(this->display,"_NET_WM_STATE_ABOVE",False);
	xa_prop[2]=XInternAtom(this->display,"_NET_WM_STATE_BELOW",False);
	xa_prop[3]=XInternAtom(this->display,"_NET_WM_ACTION_CHANGE_DESKTOP",False);

	if(xa!=None)
		XChangeProperty(this->display,this->window,xa,XA_ATOM,32,PropModeAppend,(unsigned char *)&xa_prop,4);

	this->LFSTK_setWindowType(wi->windowType);
	this->windowName=strdup(wi->name);
	XStoreName(this->display,this->window,this->windowName);
	classHint.res_name=this->windowName;
	classHint.res_class=(char*)"LFSToolKit";
	XSetClassHint(this->display,this->window,&classHint);

	this->gc=XCreateGC(this->display,this->window,0,NULL);
	this->LFSTK_setFontString((char*)DEFAULTFONT);
	this->LFSTK_setDecorated(wi->decorated);
	this->initWindow(wi->loadVars);

	if(this->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
	else
		this->useTile=false;

	this->userHome=getenv("HOME");
	asprintf(&this->configDir,"%s/.config/LFS",this->userHome);
	this->closeDisplayOnExit=wi->shutDisplayOnExit;

	this->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,wi->w,wi->h);

	switch(wi->level)
		{
			case BELOWALL:
				XLowerWindow(this->display,this->window);
				break;
			case ABOVEALL:
				XRaiseWindow(this->display,this->window);
				break;
		}
	this->gadgetMap.clear();
}

/**
* Main window constructor.
* \param windowInitStruct *wi.
* \note wi contains startup window params.
*/
LFSTK_windowClass::LFSTK_windowClass(windowInitStruct *wi)
{
printf("LFSTK_windowClass::LFSTK_windowClass(windowInitStruct *wi)\n");
	this->windowClassInitCommon(wi);
	this->gadgetMap.clear();
	
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
LFSTK_windowClass::LFSTK_windowClass(int x,int y,int w,int h,const char* name,bool override,bool loadvars,bool shutdisplayonexit)
{
//printf("LFSTK_windowClass::LFSTK_windowClass(int x,int y,int w,int h,const char* name,bool override,bool loadvars,bool shutdisplayonexit)\n");
	windowInitStruct *wi=new windowInitStruct;
	wi->x=x;
	wi->y=y;
	wi->w=w;
	wi->h=h;
	wi->name=name;
	wi->overRide=override;
	wi->loadVars=loadvars;
	wi->shutDisplayOnExit=shutdisplayonexit;

	this->windowClassInitCommon(wi);
	this->gadgetMap.clear();
	delete wi;
}

/**
* Show window and all sub windows.
* \param all Map all subwindows, default=true.
*/
void LFSTK_windowClass::LFSTK_showWindow(bool all)
{
	XEvent xev;
	XWindowAttributes wattr;

	if(all==true)
		XMapSubwindows(this->display,this->window);
	XMapWindow(this->display,this->window);

	memset(&xev,0,sizeof(xev));
	xev.type=ClientMessage;
	xev.xclient.display=this->display;
	xev.xclient.window=this->window;
	xev.xclient.message_type=XInternAtom(this->display,"_NET_ACTIVE_WINDOW",false);
	xev.xclient.format=32;
	xev.xclient.data.l[0]=2L;
	xev.xclient.data.l[1]=CurrentTime;

	XGetWindowAttributes(this->display,this->window,&wattr);
	XSendEvent(this->display,wattr.screen->root,false,SubstructureNotifyMask|SubstructureRedirectMask,&xev);
//	XSetInputFocus(this->display,this->window,None,CurrentTime);
	this->isVisible=true;
}

/**
* Hide window.
*/
void LFSTK_windowClass::LFSTK_hideWindow(void)
{
	XUnmapWindow(this->display,this->window);
	this->mainLoop=false;
	XFlush(this->display);
	//XSync(this->display,true);
	XSync(this->display,false);
	//XSetInputFocus(this->display,PointerRoot,RevertToParent,CurrentTime);
	this->isVisible=false;
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
	cairo_surface_t	*tempimage;
	cairo_status_t	cs=CAIRO_STATUS_SUCCESS;
	char			*suffix=NULL;

	if(this->pattern!=NULL)
		{
			cairo_pattern_destroy(this->pattern);
			this->pattern=NULL;
		}

	if((path==NULL) || (access(path,F_OK)!=0))
		{
			this->useTile=false;
			return;
		}

	suffix=strrchr((char*)path,'.');
	if((suffix!=NULL) && (strcasecmp(suffix,".png")==0))
		{
			tempimage=cairo_image_surface_create_from_png(path);
			cs=cairo_surface_status(tempimage);
		}
	else
		cs=CAIRO_STATUS_INVALID_FORMAT;

	if(cs!=CAIRO_STATUS_SUCCESS)
		{
			tempimage=this->globalLib->LFSTK_cairo_image_surface_create_from_jpeg(path);
			cs=cairo_surface_status(tempimage);
			if(tempimage==NULL)
				printf("Unkown Format : %s\n",path);
		}

	if(cs==CAIRO_STATUS_SUCCESS)
		{
			this->pattern=cairo_pattern_create_for_surface(tempimage);
			cairo_surface_destroy(tempimage);
			cairo_pattern_set_extend (pattern,CAIRO_EXTEND_REPEAT);
			this->useTile=true;
		}
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

/**
 * Init drag and drop system.
 * \note Set when lined edit class is created.
 */
void LFSTK_windowClass::LFSTK_initDnD(void)
{
//Announce XDND support
	Atom XdndAware=XInternAtom(this->display,"XdndAware",false);
	Atom version=5;
	if(XdndAware!=None)
		XChangeProperty(this->display,this->window,XdndAware,XA_ATOM,32,PropModeReplace,(unsigned char*)&version,1);

	dNdAtoms[XDNDENTER]=XInternAtom(this->display,"XdndEnter",false);
	dNdAtoms[XDNDPOSITION]=XInternAtom(this->display,"XdndPosition",false);
	dNdAtoms[XDNDSTATUS]=XInternAtom(this->display,"XdndStatus",false);
	dNdAtoms[XDNDTYPELIST]=XInternAtom(this->display,"XdndTypeList",false);
	dNdAtoms[XDNDACTIONCOPY]=XInternAtom(this->display,"XdndActionCopy",false);
	dNdAtoms[XDNDDROP]=XInternAtom(this->display,"XdndDrop",false);
	dNdAtoms[XDNDLEAVE]=XInternAtom(this->display,"XdndLeave",false);
	dNdAtoms[XDNDFINISHED]=XInternAtom(this->display,"XdndFinished",false);
	dNdAtoms[XDNDSELECTION]=XInternAtom(this->display,"XdndSelection",false);
	dNdAtoms[XDNDPROXY]=XInternAtom(this->display,"XdndProxy",false);
	dNdAtoms[XA_CLIPBOARD]=XInternAtom(this->display,"CLIPBOARD",false);
	dNdAtoms[XA_COMPOUND_TEXT]=XInternAtom(this->display,"COMPOUND_TEXT",false);
	dNdAtoms[XA_UTF8_STRING]=XInternAtom(this->display,"UTF8_STRING",false);
	dNdAtoms[XA_TARGETS]=XInternAtom(this->display,"TARGETS",false);
	dNdAtoms[PRIMARY]=XInternAtom(this->display,"PRIMARY",false);
	dNdAtoms[SECONDARY]=XInternAtom(this->display,"SECONDARY",false);

	this->acceptDnd=true;
	this->toBeRequested=None;
	this->sourceWindow=None;
	this->xDnDVersion=0;
	this->dropGadget=NULL;

	this->dNdTypes["text/plain"]=1;
	this->dNdTypes["text/uri-list"]=2;
}

/**
 * Get DnD atom.
 * \param atomnum Requested atom.
 */
Atom LFSTK_windowClass::LFSTK_getDnDAtom(int atomnum)
{
	return(this->dNdAtoms[atomnum]);
}

//This fetches all the data from a property
propertyStruct* LFSTK_windowClass::readProperty(Window src,Atom property)
{
	Atom			actual_type;
	int				actual_format;
	unsigned long	nitems;
	unsigned long	bytes_after;
	unsigned char	*ret=0;
	propertyStruct	*props=new propertyStruct;

	int read_bytes=1024;

	//Keep trying to read the property until there are no bytes unread.
	do
		{
			if(ret!=0)
				XFree(ret);
			XGetWindowProperty(this->display,src,property,0,read_bytes,False,AnyPropertyType,&actual_type,&actual_format, &nitems,&bytes_after,&ret);

			read_bytes *= 2;
		}
	while(bytes_after!=0);

	props->data=ret;
	props->format=actual_format;
	props->nitems=nitems;
	props->type=actual_type;
	props->mimeType=NULL;
	return props;
}

//Convert an atom name in to a std::string
std::string LFSTK_windowClass::getAtomName(Atom a)
{
	std::string retval;
	char	*aname;

	if(a == None)
		retval="None";
	else
		{
			aname=XGetAtomName(this->display,a);
			retval=aname;
			XFree(aname);
		}
	return(retval);
}

// This function takes a list of targets which can be converted to (atom_list, nitems)
// and a list of acceptable targets with prioritees (dNdTypes). It returns the highest
// entry in dNdTypes which is also in atom_list: ie it finds the best match.
Atom LFSTK_windowClass::pickTargetFromList(Atom* atom_list,int nitems)
{
	Atom to_be_requested=None;
	//This is higger than the maximum priority.

	int priority=INT_MAX;

	for(int i=0; i < nitems; i++)
		{
			//aname=
			std::string atom_name=this->getAtomName(atom_list[i]);;
			//See if this data type is allowed and of higher priority (closer to zero) than the present one.
			if(this->dNdTypes.find(atom_name)!= this->dNdTypes.end())
				if(priority > this->dNdTypes[atom_name])
					{
						priority=this->dNdTypes[atom_name];
						to_be_requested=atom_list[i];
					}
				atom_name.clear();
		}
	return to_be_requested;
}

// Finds the best target given a local copy of a property.
Atom LFSTK_windowClass::pickTargetFromTargets(propertyStruct* p)
{
	//The list of targets is a list of atoms, so it should have type XA_ATOM but it may have the type TARGETS instead.
	if((p->type!=XA_ATOM && p->type!=this->dNdAtoms[XA_TARGETS]) || p->format!=32)
		{
			//This would be really broken. Targets have to be an atom list and applications should support this. Nevertheless,
			//some seem broken (MATLAB 7, for instance), so ask for STRING next instead as the lowest common denominator
			if(this->dNdTypes.count("STRING"))
				return(XA_STRING);
			else
				return None;
		}
	else
		{
			Atom *atom_list=(Atom*)p->data;
			return pickTargetFromList(atom_list,p->nitems);
		}
}

// Finds the best target given up to three atoms provided (any can be None).
// Useful for part of the Xdnd protocol.
Atom LFSTK_windowClass::pickTargetFromAtoms(Atom t1, Atom t2, Atom t3)
{
	Atom atoms[3];
	int  n=0;

	if(t1!=None)
		atoms[n++]=t1;

	if(t2!=None)
		atoms[n++]=t2;

	if(t3!=None)
		atoms[n++]=t3;

	return this->pickTargetFromList(atoms, n);
}

/**
* Drop data.
* \param data Data drooped on gadget as string.
*/
void LFSTK_windowClass::LFSTK_dropData(propertyStruct* data)
{
	int	endl;

	this->droppedData.type=DROPINVALID;
	if(this->droppedData.data!=NULL)
		free(this->droppedData.data);
	this->droppedData.data=NULL;
	
	if(strcasecmp(data->mimeType,"text/plain")==0)
		{
			this->droppedData.type=DROPTEXT;
			asprintf(&(this->droppedData.data),"%s",data->data);
		}

	if(strcasecmp(data->mimeType,"text/uri-list")==0)
		{
			char	*d;
			char	*ret;
			asprintf(&d,"%s",(const char*)data->data);
			endl=strlen(d)-1;
			while ((endl >= 0) && (isspace(d[endl])) )
				{
					d[endl]=0;
					endl--;
				}
			ret=this->globalLib->LFSTK_oneLiner("echo -n \"%s\"|sed 's|^file://||;s|%%20| |g'",d);
			this->droppedData.type=DROPURI;
			this->droppedData.data=ret;
			free(d);
		}
}

/**
 * Handle a dNd event.
 * \param event The event.
 * \note Only for line edit class gadgets for now.
 */
void LFSTK_windowClass::LFSTK_handleDnD(XEvent *event)
{
	//int res=0;

	if(event->type == ClientMessage)
		{
			if(event->xclient.message_type==this->dNdAtoms[XDNDENTER])
				{
					this->dropGadget=this->LFSTK_findGadgetByPos(event->xclient.data.l[2] >> 16,event->xclient.data.l[2] & 0xffff);
					if((this->dropGadget!=NULL) && (this->dropGadget->gadgetAcceptsDnD==false))
						this->dropGadget=NULL;
					this->xDnDVersion=(event->xclient.data.l[1] >> 24);
					//more than three
					Window source=event->xclient.data.l[0];
					if(event->xclient.data.l[1] & 1)
						{
							//Fetch the list of possible conversions
							propertyStruct *props=this->readProperty(source,this->dNdAtoms[XDNDTYPELIST]);
							this->toBeRequested=this->pickTargetFromTargets(props);
							XFree(props->data);
							XFree(props->mimeType);
							delete props;
						}
					else
						{
							//Use the available list
							this->toBeRequested=this->pickTargetFromAtoms(event->xclient.data.l[2],event->xclient.data.l[3],event->xclient.data.l[4]);
						}
				}

			if(event->xclient.message_type == dNdAtoms[XDNDPOSITION])
				{
					//Xdnd: reply with an XDND status message
					int res=0;

					this->dropGadget=this->LFSTK_findGadgetByPos(event->xclient.data.l[2] >> 16,event->xclient.data.l[2] & 0xffff);
					this->droppedData.x=event->xclient.data.l[2] >> 16;
					this->droppedData.y=event->xclient.data.l[2] & 0xffff;

					XClientMessageEvent m;
					memset(&m, sizeof(m), 0);
					m.type=ClientMessage;
					m.display=event->xclient.display;
					m.window=event->xclient.data.l[0];
					m.message_type=dNdAtoms[XDNDSTATUS];
					m.format=32;
					m.data.l[0]=this->window;
					//res=0;
					res+=((this->dropGadget!=NULL) && (this->dropGadget->gadgetAcceptsDnD));
					res+=((this->dropGadget==NULL) && (this->acceptOnThis));
					m.data.l[1]=res;
					m.data.l[2]=0; //Specify an empty rectangle
					m.data.l[3]=0;
					m.data.l[4]=dNdAtoms[XDNDACTIONCOPY]; //We only accept copying anyway.
					XSendEvent(this->display,event->xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
				}

			if(event->xclient.message_type == dNdAtoms[XDNDDROP])
				{
					if((this->toBeRequested == None))
						{
							//It's sending anyway, despite instructions to the contrary.
							//So reply that we're not interested.
							XClientMessageEvent m;
							memset(&m, sizeof(m), 0);
							m.type=ClientMessage;
							m.display=event->xclient.display;
							m.window=event->xclient.data.l[0];
							m.message_type=dNdAtoms[XDNDFINISHED];
							m.format=32;
							m.data.l[0]=this->window;
							m.data.l[1]=0;
							m.data.l[2]=None; //Failed.
							XSendEvent(this->display,event->xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
						}
					else
						{
							this->sourceWindow=event->xclient.data.l[0];
							if(this->xDnDVersion >= 1)
								XConvertSelection(this->display, dNdAtoms[XDNDSELECTION],this->toBeRequested,dNdAtoms[PRIMARY],this->window, event->xclient.data.l[2]);
							else
								XConvertSelection(this->display, dNdAtoms[XDNDSELECTION],this->toBeRequested,dNdAtoms[PRIMARY],this->window, CurrentTime);
						}
				}
		}

	if(event->type == SelectionNotify)
		{
			Atom target=event->xselection.target;

			if(event->xselection.property == None)
				{
					return;
				}
			else
				{
					propertyStruct *myprops=this->readProperty(this->window,dNdAtoms[PRIMARY]);

					myprops->mimeType=XGetAtomName(this->display,target);
					//If we're being given a list of targets (possible conversions)
					if(target == dNdAtoms[XA_TARGETS])
						{
							this->toBeRequested=this->pickTargetFromTargets(myprops);

							if(this->toBeRequested == None)
								{
									XFree(myprops->data);
									XFree(myprops->mimeType);
									delete myprops;
									return;
								}
							else //Request the data type we are able to select
								XConvertSelection(this->display,dNdAtoms[XDNDSELECTION],this->toBeRequested,dNdAtoms[XDNDSELECTION],this->window, CurrentTime);
						}
					else if(target==this->toBeRequested)
						{
							if(this->dropGadget!=NULL)
								this->dropGadget->LFSTK_dropData(myprops);
							else
								if(this->acceptOnThis==true)
									this->LFSTK_dropData(myprops);
							//Reply OK.
							XClientMessageEvent m;
							memset(&m, sizeof(m), 0);
							m.type=ClientMessage;
							m.display=this->display;
							m.window=this->sourceWindow;
							m.message_type=dNdAtoms[XDNDFINISHED];
							m.format=32;
							m.data.l[0]=this->window;//w;
							m.data.l[1]=1;
							m.data.l[2]=dNdAtoms[XDNDACTIONCOPY]; //We only ever copy.
							XSendEvent(this->display,this->sourceWindow, False, NoEventMask, (XEvent*)&m);
						}
					else
						return;

					XFree(myprops->data);
					XFree(myprops->mimeType);
					delete myprops;
				}
		}
	XSync(this->display,false);
}

/**
 * Return windows geometry.
 * \param *geometryStruct structure to be filled in.
 * \note returned geometryStruct is owned by class.
 */
const geometryStruct *LFSTK_windowClass::LFSTK_getWindowGeom(void)
{
	return(&(this->windowGeom));
}

/**
 * Set window geometry.
 * \param int x,y,w,h.
 */
void LFSTK_windowClass::setWindowGeom(int x,int y,int w,int h,setWindowGeomFlags flags)
{
	if((flags==WINDSETXY) || (flags==WINDSETALL))
		{
			this->windowGeom.x=x;
			this->windowGeom.y=y;
		}
	if((flags==WINDSETWH) || (flags==WINDSETALL))
		{
			this->windowGeom.w=w;
			this->windowGeom.h=h;
		}
}

/**
* Set double click time.
* \param unsigned double click interval.
*/
void LFSTK_windowClass::LFSTK_setDoubleClickTime(unsigned interval)
{
	this->dbClick=(interval);
}

/**
* Send utf8 string
* \note private
*/
void LFSTK_windowClass::sendUTF8(XSelectionRequestEvent *sev)
{
	XEvent	res;
	Window	cwin;
	Window	win;
	Atom	pty;

/*	Thanks to xclip for this code.
	Web:
		https://github.com/astrand/xclip
	Email:
		astrand@lysator.liu.se
*/

/* set the window and property that is being used */
	win=sev->requestor;
	pty=sev->property;

/* put the data into an property */
	if (sev->target==this->LFSTK_getDnDAtom(XA_TARGETS))
		{
			Atom types[2]={this->LFSTK_getDnDAtom(XA_TARGETS),this->LFSTK_getDnDAtom(XA_UTF8_STRING)};
/* send data all at once (not using INCR) */
			XChangeProperty(this->display,win,pty,XA_ATOM,32,PropModeReplace,(unsigned char *)types,(int)(sizeof(types) / sizeof(Atom)));
		}
	else
		{
/* send data all at once (not using INCR) */
			XChangeProperty(this->display,win,pty,this->LFSTK_getDnDAtom(XA_UTF8_STRING),8,PropModeReplace,(unsigned char *)this->clipBuffer.c_str(),(int)this->clipBuffer.length());
		}

/* set values for the response event */
	res.xselection.property=pty;
	res.xselection.type=SelectionNotify;
	res.xselection.display=sev->display;
	res.xselection.requestor=win;
	res.xselection.selection=sev->selection;
	res.xselection.target=sev->target;
	res.xselection.time=sev->time;

/* send the response event */
		XSendEvent(this->display,sev->requestor,0,0,&res);
		XFlush(this->display);
}

/**
* Handle basic window events.
* \param XEvent	*event.
* \return 0=handled, 1=not handled, -1 close window
*/
int LFSTK_windowClass::LFSTK_handleWindowEvents(XEvent *event)
{
	int	retval=0;

	switch(event->type)
		{
			case FocusOut:
				break;

			case EnterNotify:
//				printf("enter\n");
				break;
			case LeaveNotify:
//				printf("leav\n");
				break;
			case Expose:
				if (event->xexpose.count==0)
					this->LFSTK_clearWindow();
				break;

			case ConfigureNotify:
				this->LFSTK_resizeWindow(event->xconfigurerequest.width,event->xconfigurerequest.height,false);
				this->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,event->xconfigurerequest.width,event->xconfigurerequest.height);
				this->LFSTK_clearWindow();
				break;

			case SelectionRequest:
				if(XGetSelectionOwner(this->display,this->LFSTK_getDnDAtom(XA_CLIPBOARD))==this->window)
					this->sendUTF8(&event->xselectionrequest);
				break;

			case ClientMessage:
			case SelectionNotify:
				{
					if (event->xclient.message_type == XInternAtom(this->display, "WM_PROTOCOLS", 1) && (Atom)event->xclient.data.l[0] == XInternAtom(this->display, "WM_DELETE_WINDOW", 1))
						{
							retval=-1;
							break;
						}
//dnd for edit box
					if(this->acceptDnd==true)
						{
							this->LFSTK_handleDnD(event);
							if((this->droppedData.type!=-1) && (this->acceptOnThis==true))
								{
									this->droppedData.type=DROPINVALID;
								}
						}
				}
				break;
		}
	return(retval);
}



