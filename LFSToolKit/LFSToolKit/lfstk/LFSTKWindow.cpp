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
	unsigned long	flags;
	unsigned long	functions;
	unsigned long	decorations;
	long				inputMode;
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
					ml->gadget->LFSTK_getGeomWindowRelative(&geom,ml->gadget->wc->app->rootWindow);
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
	XStoreName(this->app->display,this->window,this->windowName);
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

	if(this->app==NULL)
		this->globalLib=new LFSTK_lib(loadvars);
	else
		this->globalLib=this->app->globalLib;
	if(loadvars==true)
		this->loadGlobalColours();
	this->isActive=true;
	this->useTile=false;
	this->gadgetMap.clear();
	//XSetBackground(this->app->display,this->gc,this->windowColourNames[NORMALCOLOUR].pixel);
	//XSetBackground(this->app->display,this->gc, WhitePixel(this->app->display, 0));
	//std::cerr<<this->windowColourNames[NORMALCOLOUR].pixel<<std::endl;
}

/**
 * Reload colours from prefs.
 * \note to be fixed!
 */
void LFSTK_windowClass::LFSTK_reloadGlobals(void)
{
	char	*env;

	asprintf(&env,"%s/.config/LFS/lfstoolkit.rc",getenv("HOME"));
	this->globalLib->prefs.LFSTK_loadVarsFromFile(env);

	this->loadGlobalColours();
	this->isActive=true;
	this->LFSTK_clearWindow();
	free(env);
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
					XFreeColors(this->app->display,this->app->cm,(long unsigned int*)&this->windowColourNames[j].pixel,1,0);
				}
		}

	if(this->windowName!=NULL)
		free(this->windowName);

	if(this->app==NULL)
		delete this->globalLib;

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
	XFreeGC(this->app->display,this->gc);
	if(this->px!=None)
		XFreePixmap(this->app->display,this->px);

	XDestroyWindow(this->app->display,this->window);
}

/**
 * Dummy init.
 */
LFSTK_windowClass::LFSTK_windowClass()
{
	return;//TODO//MMMMMmmmmm
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
* \bool updategadgets Update all gadgets ( default=false ).
* \note Set updategadgets=true when changing pixmap.
*/
void LFSTK_windowClass::LFSTK_setWindowPixmap(Pixmap pixmap,int w,int h,bool updategadgets)
{
	cairo_surface_t	*surfaceto=NULL;
	cairo_surface_t	*surfacefrom=NULL;
	cairo_t			*cr=NULL;
	cairo_status_t	status=CAIRO_STATUS_SUCCESS;

	if(pixmap==None)
		{
			this->useTile=false;
			return;
		}
	xLibErrorTK=false;
	XSetErrorHandler(xErrHandler);
	XSynchronize(this->app->display,true);
	if(this->px!=None)
		XFreePixmap(this->app->display,this->px);
	this->px=XCreatePixmap(this->app->display,this->window,w,h,this->app->depth);

	surfaceto=cairo_xlib_surface_create(this->app->display,this->px,this->app->visual,w,h);
//HMMmmmmmm??
	surfacefrom=cairo_xlib_surface_create(this->app->display,pixmap,DefaultVisual(this->app->display,this->app->screen),w,h);
	cr=cairo_create(surfaceto);
	if(status==CAIRO_STATUS_SUCCESS)
		{
			cairo_save(cr);
				cairo_reset_clip(cr);
				cairo_set_source_surface(cr,surfacefrom,0,0);
				cairo_paint(cr);
			cairo_restore(cr);

			XSetWindowBackgroundPixmap(this->app->display,this->window,this->px);
			XClearWindow(this->app->display,this->window);
			this->usePixmap=true;
		}

	if(updategadgets==true)
		{
			if(!this->gadgetMap.empty())
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
		}

	cairo_surface_destroy(surfaceto);
	cairo_surface_destroy(surfacefrom);
	cairo_destroy(cr);
	XSetErrorHandler(NULL);

	if(xLibErrorTK==true)
		{
			xLibErrorTK=false;
			this->usePixmap=false;
			this->useTile=false;
			XSetWindowBackgroundPixmap(this->app->display,this->window,None);
			XClearWindow(this->app->display,this->window);
		}
}

/**
* Forc redraw of all gadgets..
*/
void LFSTK_windowClass::LFSTK_redrawAllGadgets(void)
{
	if(!this->gadgetMap.empty())
		{
			for (std::map<int,mappedListener*>::iterator it=this->gadgetMap.begin();it!=this->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;
					if (ml!=NULL)
						{
							if(ml->gadget!=NULL)
								{
									if(ml->gadget->isMapped==false)
										ml->gadget->LFSTK_showGadget();
									ml->gadget->LFSTK_clearWindow();
								}
						}
				}
		}
	XSync(this->app->display,false);
}

/**
* Clear the window to the appropriate state.
* \param bool true=clear window on all gadgets.
* \note param is optional defaults to false.
*/
void LFSTK_windowClass::LFSTK_clearWindow(bool cleargadgets)
{
	int	state=NORMALCOLOUR;

	if(cleargadgets==true)
		this->LFSTK_redrawAllGadgets();

	if(this->usePixmap==true)
		return;

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
					cairo_set_operator(this->cr,CAIRO_OPERATOR_SOURCE);
					cairo_paint(this->cr);
					cairo_surface_flush (this->sfc);
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
		XResizeWindow(this->app->display,this->window,w,h);

//cairo_xlib_surface_set_drawable (this->sfc,
//                                 this->window,
//                                 w,
//                                 h);
//XSync(this->app->display,false);
  	this->globalLib->LFSTK_setCairoSurface(this->app->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
//	this->LFSTK_clearWindow();

//XUnmapWindow(this->app->display,this->window);
//XSync(this->app->display,false);
//XMapWindow(this->app->display,this->window);
//XSync(this->app->display,false);
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
		XMoveWindow(this->app->display,this->window,x,y);
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
			xa_prop[9]=XInternAtom(this->app->display,"_MOTIF_WM_HINTS",True);
			if(xa_prop[9]!=None)
				XChangeProperty(this->app->display,this->window,xa_prop[9],xa_prop[9],32,PropModeReplace,(unsigned char *)&hints,5);
		}
}

/**
* Set window decorations.
* \param bool canmax=true,bool canmin=true,bool canresize=true.
//motif
#define MWM_HINTS_FUNCTIONS (1L << 0)
#define MWM_HINTS_DECORATIONS (1L << 1)

#define MWM_FUNC_ALL (1L << 0)
#define MWM_FUNC_RESIZE (1L << 1)
#define MWM_FUNC_MOVE (1L << 2)
#define MWM_FUNC_MINIMIZE (1L << 3)
#define MWM_FUNC_MAXIMIZE (1L << 4)
#define MWM_FUNC_CLOSE (1L << 5)

#define MWM_DECOR_ALL (1L << 0)
#define MWM_DECOR_BORDER (1L << 1)
#define MWM_DECOR_RESIZEH (1L << 2)
#define MWM_DECOR_TITLE (1L << 3)
#define MWM_DECOR_MENU (1L << 4)
#define MWM_DECOR_MINIMIZE (1L << 5)
#define MWM_DECOR_MAXIMIZE (1L << 6)
*/
void LFSTK_windowClass::LFSTK_setDecorations(bool canmax,bool canmin,bool canresize,bool canclose)
{
	Atom		xa_prop[10];
	Hints	hints;

	hints.flags=MWM_HINTS_DECORATIONS;
	hints.decorations=MWM_DECOR_BORDER;
	hints.functions=0;
	hints.inputMode=0;
	hints.status=0;

	if(canmax==true)
		hints.decorations|=MWM_DECOR_MAXIMIZE;
	if(canmin==true)
		hints.decorations|=MWM_DECOR_MINIMIZE;
	if(canresize==true)
		hints.decorations|=MWM_DECOR_RESIZEH;
	if(canclose==true)
		hints.functions|=MWM_FUNC_CLOSE;

	xa_prop[9]=XInternAtom(this->app->display,"_MOTIF_WM_HINTS",True);
	if(xa_prop[9]!=None)
		XChangeProperty(this->app->display,this->window,xa_prop[9],xa_prop[9],32,PropModeReplace,(unsigned char *)&hints,5);

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
	XColor		tc,sc;
	std::string	str=colour;
	int			alphaint=255;

	if(this->windowColourNames[p].name!=NULL)
		free(this->windowColourNames[p].name);

	this->windowColourNames[p].name=strdup(colour);
	if(str.at(0)=='#')
		{
			if(str.length()>7)
				{
					alphaint=std::stoi (str.substr(1,2),nullptr,16);
					str.erase(str.begin()+1,str.begin()+3);
				}
		}
	XAllocNamedColor(this->app->display,this->app->cm,str.c_str(),&sc,&tc);
	this->windowColourNames[p].pixel=tc.pixel;

	this->windowColourNames[p].RGBAColour.a=alphaint/256.0;
	this->windowColourNames[p].RGBAColour.r=((this->windowColourNames[p].pixel>>16) & 0xff)/256.0;
	this->windowColourNames[p].RGBAColour.g=((this->windowColourNames[p].pixel>>8) & 0xff)/256.0;
	this->windowColourNames[p].RGBAColour.b=((this->windowColourNames[p].pixel>>0) & 0xff)/256.0;

	if(p==NORMALCOLOUR)
	{

		this->windowNormalAlpha=this->windowColourNames[NORMALCOLOUR].RGBAColour.a;
			fprintf(stderr,"windowNormalAlpha=%f\n",this->windowNormalAlpha);
		}
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
	xa=XInternAtom(this->app->display,"_NET_WM_STATE",False);
	xa1=XInternAtom(this->app->display,"_NET_WM_STATE_STICKY",False);

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
	XSendEvent(this->app->display,this->app->rootWindow,False,SubstructureRedirectMask | SubstructureNotifyMask,(XEvent *)&xclient);
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

	xa=XInternAtom(this->app->display,"_NET_WM_WINDOW_TYPE",False);
	xa_prop[0]=XInternAtom(this->app->display,type,False);

	if(xa!=None)
		XChangeProperty(this->app->display,this->window,xa,XA_ATOM,32,PropModeReplace,(unsigned char *)&xa_prop,1);
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
	xa=XInternAtom(this->app->display,"_NET_WM_STATE",False);
	xa1=XInternAtom(this->app->display,"_NET_WM_STATE_ABOVE",False);

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
//	XSendEvent(this->app->display,this->app->rootWindow,False,SubstructureRedirectMask | SubstructureNotifyMask,(XEvent *)&xclient);
	XSendEvent(this->app->display,this->window,False,SubstructureRedirectMask | SubstructureNotifyMask,(XEvent *)&xclient);
	XRaiseWindow(this->app->display,this->window);
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
	xa=XInternAtom(this->app->display,"_NET_WM_STATE",False);
	xa1=XInternAtom(this->app->display,"_NET_WM_STATE_BELOW",False);

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
	XSendEvent(this->app->display,this->app->rootWindow,False,SubstructureRedirectMask | SubstructureNotifyMask,(XEvent *)&xclient);
}

/**
* Set transient for window.
* \param w Window.
*/
void LFSTK_windowClass::LFSTK_setTransientFor(Window w)
{
	XSetTransientForHint(this->app->display,this->window,w);
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
* Get window monitor.
* \return unsigned Monitor that window top left is on;
*/

int LFSTK_windowClass::LFSTK_windowOnMonitor(void)
{
	Window				child;
	XWindowAttributes	xwa;
	int					x,y;

	XTranslateCoordinates(this->app->display,this->window,this->app->rootWindow,0,0,&x,&y,&child );

	int thisx=x;
	int thisy=y;

	if(thisx<0)
		thisx=0;
	if(thisy<0)
		thisy=0;

	for(int j=0; j<this->app->monitorCount; j++)
		{
			if((thisx>=this->app->monitors[j].x) && (thisx<(this->app->monitors[j].x+this->app->monitors[j].w)) && (thisy>=this->app->monitors[j].y) && (thisy<(this->app->monitors[j].y+this->app->monitors[j].h)))
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
	XSetWindowAttributes	wa;
	Atom					wm_delete_window;
	XClassHint				classHint;
	Atom					xa;
	Atom					xa_prop[3];

	this->setWindowGeom(wi->x,wi->y,wi->w,wi->h,WINDSETALL);

	this->fontString=NULL;
	this->isActive=false;
	this->acceptDnd=false;

	wa.win_gravity=NorthWestGravity;
	wa.override_redirect=wi->overRide;
	wm_delete_window=XInternAtom(this->app->display,"WM_DELETE_WINDOW",0);

	if(this->app->gotARGB==true)
		{
			wa.colormap=this->app->cm;
			wa.border_pixel=0;
			wa.background_pixel=0;

			this->window=XCreateWindow(this->app->display,this->app->rootWindow,wi->x,wi->y,wi->w,wi->h,0,this->app->depth,InputOutput,this->app->visual,CWColormap | CWBorderPixel |CWWinGravity|CWOverrideRedirect,&wa);
		}
	else
		{
			this->window=XCreateWindow(this->app->display,this->app->rootWindow,wi->x,wi->y,wi->w,wi->h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWOverrideRedirect,&wa);
		}

	XSelectInput(this->app->display,this->window,SubstructureRedirectMask|StructureNotifyMask|ButtonPressMask | ButtonReleaseMask | ExposureMask|LeaveWindowMask|FocusChangeMask|SelectionClear|SelectionRequest);

	XSetWMProtocols(this->app->display,this->window,&wm_delete_window,1);
	xa=XInternAtom(this->app->display,"_NET_WM_ALLOWED_ACTIONS",False);
	xa_prop[0]=XInternAtom(this->app->display,"_NET_WM_STATE_STICKY",False);
	xa_prop[1]=XInternAtom(this->app->display,"_NET_WM_STATE_ABOVE",False);
	xa_prop[2]=XInternAtom(this->app->display,"_NET_WM_STATE_BELOW",False);
	xa_prop[3]=XInternAtom(this->app->display,"_NET_WM_ACTION_CHANGE_DESKTOP",False);

	if(xa!=None)
		XChangeProperty(this->app->display,this->window,xa,XA_ATOM,32,PropModeAppend,(unsigned char *)&xa_prop,4);

	this->LFSTK_setWindowType(wi->windowType);
	this->windowName=strdup(wi->name);
	XStoreName(this->app->display,this->window,this->windowName);
	classHint.res_name=this->windowName;
	classHint.res_class=(char*)"LFSToolKit";
	XSetClassHint(this->app->display,this->window,&classHint);

	this->gc=XCreateGC(this->app->display,this->window,0,NULL);
	this->LFSTK_setFontString((char*)DEFAULTFONT);
	this->LFSTK_setDecorated(wi->decorated);
	this->initWindow(wi->loadVars);

	if(this->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
	else
		this->useTile=false;

	this->globalLib->LFSTK_setCairoSurface(this->app->display,this->window,this->app->visual,&this->sfc,&this->cr,wi->w,wi->h);

	switch(wi->level)
		{
			case BELOWALL:
				XLowerWindow(this->app->display,this->window);
				break;
			case ABOVEALL:
				XRaiseWindow(this->app->display,this->window);
				break;
		}
	this->gadgetMap.clear();
}

/**
* Main window constructor.
* \param windowInitStruct *wi.
* \note wi contains startup window params.
*/
LFSTK_windowClass::LFSTK_windowClass(windowInitStruct *wi,LFSTK_applicationClass *app)
{
	XSetWindowAttributes	wa;
	Atom					wm_delete_window;
	XClassHint			classHint;
	Atom					xa;
	Atom					xa_prop[4];
	bool					gotargb;
	XVisualInfo			visual_template;
	XVisualInfo			*visual_list=NULL;
	int					nxvisuals=0;

	this->app=app;
	this->setWindowGeom(wi->x,wi->y,wi->w,wi->h,WINDSETALL);

	this->fontString=NULL;
	this->isActive=false;
	this->acceptDnd=false;

	wa.win_gravity=NorthWestGravity;
	wa.override_redirect=wi->overRide;
	wm_delete_window=XInternAtom(this->app->display,"WM_DELETE_WINDOW",0);

	XVisualInfo vinfo;
	XMatchVisualInfo(this->app->display, DefaultScreen(this->app->display), 32, TrueColor, &vinfo);

	this->visual=vinfo.visual;
	this->cmap=XCreateColormap(this->app->display, DefaultRootWindow(this->app->display), this->visual, AllocNone);

	if(wi->app->gotARGB==true)
		{
			//fprintf(stderr,"got argb\n");
			//wa.colormap=this->app->cm;
			wa.colormap=this->cmap;
			wa.border_pixel=0;
			wa.background_pixel=0;

			//this->window=XCreateWindow(this->app->display,this->app->rootWindow,wi->x,wi->y,wi->w,wi->h,0,this->app->depth,InputOutput,this->app->visual,CWColormap | CWBorderPixel |CWWinGravity|CWOverrideRedirect,&wa);
			this->window=XCreateWindow(this->app->display,this->app->rootWindow,wi->x,wi->y,wi->w,wi->h,0,vinfo.depth,InputOutput,this->visual,(CWColormap | CWBorderPixel| CWBackPixel |CWWinGravity|CWOverrideRedirect),&wa);
				//this->setWindowGeom(0,0,wi->w,wi->h,WINDSETALL);

		}
	else
		{
		//fprintf(stderr,"no got argb\n");
		//exit(0);
			this->window=XCreateWindow(this->app->display,this->app->rootWindow,wi->x,wi->y,wi->w,wi->h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWOverrideRedirect,&wa);
		}
	XSelectInput(this->app->display,this->window,SubstructureRedirectMask|StructureNotifyMask|ButtonPressMask | ButtonReleaseMask|ButtonMotionMask | ExposureMask | EnterWindowMask|LeaveWindowMask|FocusChangeMask|SelectionClear|SelectionRequest|KeyReleaseMask|KeyPressMask);

	XSetWMProtocols(this->app->display,this->window,&wm_delete_window,1);
	xa=XInternAtom(this->app->display,"_NET_WM_ALLOWED_ACTIONS",False);
	xa_prop[0]=XInternAtom(this->app->display,"_NET_WM_STATE_STICKY",False);
	xa_prop[1]=XInternAtom(this->app->display,"_NET_WM_STATE_ABOVE",False);
	xa_prop[2]=XInternAtom(this->app->display,"_NET_WM_STATE_BELOW",False);
	xa_prop[3]=XInternAtom(this->app->display,"_NET_WM_ACTION_CHANGE_DESKTOP",False);

	if(xa!=None)
		XChangeProperty(this->app->display,this->window,xa,XA_ATOM,32,PropModeAppend,(unsigned char *)&xa_prop,4);

	this->LFSTK_setWindowType(wi->windowType);
	this->windowName=strdup(wi->name);
	XStoreName(this->app->display,this->window,this->windowName);
	classHint.res_name=this->windowName;
	classHint.res_class=(char*)"LFSToolKit";
	XSetClassHint(this->app->display,this->window,&classHint);

	this->gc=XCreateGC(this->app->display,this->window,0,NULL);
	this->LFSTK_setFontString((char*)DEFAULTFONT);
	this->LFSTK_setDecorated(wi->decorated);
	this->initWindow(wi->loadVars);

	if(this->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
	else
		this->useTile=false;

	this->globalLib->LFSTK_setCairoSurface(this->app->display,this->window,this->app->visual,&this->sfc,&this->cr,wi->w,wi->h);

	switch(wi->level)
		{
			case BELOWALL:
				XLowerWindow(this->app->display,this->window);
				break;
			case ABOVEALL:
				XRaiseWindow(this->app->display,this->window);
				break;
		}
	this->gadgetMap.clear();
	this->x=wi->x;
	this->y=wi->y;
	this->x=wi->w;
	this->y=wi->h;
}

/**
* Main window constructor.
* \param windowInitStruct *wi.
* \note wi contains startup window params.
*/
LFSTK_windowClass::LFSTK_windowClass(windowInitStruct *wi)
{
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
		XMapSubwindows(this->app->display,this->window);
	XMapWindow(this->app->display,this->window);

	memset(&xev,0,sizeof(xev));
	xev.type=ClientMessage;
	xev.xclient.display=this->app->display;
	xev.xclient.window=this->window;
	xev.xclient.message_type=XInternAtom(this->app->display,"_NET_ACTIVE_WINDOW",false);
	xev.xclient.format=32;
	xev.xclient.data.l[0]=2L;
	xev.xclient.data.l[1]=CurrentTime;

	XGetWindowAttributes(this->app->display,this->window,&wattr);
	XSendEvent(this->app->display,wattr.screen->root,false,SubstructureNotifyMask|SubstructureRedirectMask|ExposureMask|VisibilityChangeMask,&xev);
	this->isVisible=true;
}

/**
* Hide window.
*/
void LFSTK_windowClass::LFSTK_hideWindow(void)
{
	XUnmapWindow(this->app->display,this->window);
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
	XChangeProperty(this->app->display,this->window,property,type,format,PropModeReplace,(const unsigned char*)dataptr,propcnt);
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
	event.xclient.message_type=XInternAtom(this->app->display,msg,False);
	event.xclient.window=this->window;
	event.xclient.format=32;
	event.xclient.data.l[0]=data0;
	event.xclient.data.l[1]=data1;
	event.xclient.data.l[2]=data2;
	event.xclient.data.l[3]=data3;
	event.xclient.data.l[4]=data4;
	XSendEvent(this->app->display,this->app->rootWindow,False,mask,&event);
}

/**
 * Init drag and drop system.
 * \param bool accept drops onto parent window ( default=false ).
 * \note MUST be called to enable DnD on gadgets and windows.
 * \note Set when lined edit class is created.
 */
void LFSTK_windowClass::LFSTK_initDnD(bool acceptwindowdrops)
{
//Announce XDND support
	Atom XdndAware=XInternAtom(this->app->display,"XdndAware",false);
	Atom version=5;
	if(XdndAware!=None)
		XChangeProperty(this->app->display,this->window,XdndAware,XA_ATOM,32,PropModeReplace,(unsigned char*)&version,1);

	dNdAtoms[XDNDENTER]=XInternAtom(this->app->display,"XdndEnter",false);
	dNdAtoms[XDNDPOSITION]=XInternAtom(this->app->display,"XdndPosition",false);
	dNdAtoms[XDNDSTATUS]=XInternAtom(this->app->display,"XdndStatus",false);
	dNdAtoms[XDNDTYPELIST]=XInternAtom(this->app->display,"XdndTypeList",false);
	dNdAtoms[XDNDACTIONCOPY]=XInternAtom(this->app->display,"XdndActionCopy",false);
	dNdAtoms[XDNDDROP]=XInternAtom(this->app->display,"XdndDrop",false);
	dNdAtoms[XDNDLEAVE]=XInternAtom(this->app->display,"XdndLeave",false);
	dNdAtoms[XDNDFINISHED]=XInternAtom(this->app->display,"XdndFinished",false);
	dNdAtoms[XDNDSELECTION]=XInternAtom(this->app->display,"XdndSelection",false);
	dNdAtoms[XDNDPROXY]=XInternAtom(this->app->display,"XdndProxy",false);
	dNdAtoms[XA_CLIPBOARD]=XInternAtom(this->app->display,"CLIPBOARD",false);
	dNdAtoms[XA_COMPOUND_TEXT]=XInternAtom(this->app->display,"COMPOUND_TEXT",false);
	dNdAtoms[XA_UTF8_STRING]=XInternAtom(this->app->display,"UTF8_STRING",false);
	dNdAtoms[XA_TARGETS]=XInternAtom(this->app->display,"TARGETS",false);
	dNdAtoms[PRIMARY]=XInternAtom(this->app->display,"PRIMARY",false);
	dNdAtoms[SECONDARY]=XInternAtom(this->app->display,"SECONDARY",false);

	this->acceptDnd=true;
	this->toBeRequested=None;
	this->sourceWindow=None;
	this->xDnDVersion=0;
	this->dropGadget=NULL;
	this->acceptOnThis=acceptwindowdrops;
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
			XGetWindowProperty(this->app->display,src,property,0,read_bytes,False,AnyPropertyType,&actual_type,&actual_format, &nitems,&bytes_after,&ret);

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
			aname=XGetAtomName(this->app->display,a);
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
			char	*ret;
			ret=this->app->globalLib->LFSTK_cleanString((const char*)data->data);
			this->droppedData.type=DROPURI;
			this->droppedData.data=ret;
		}
}

/**
 * Handle a dNd event.
 * \param event The event.
 * \note Only for line edit class gadgets for now.
 */
void LFSTK_windowClass::LFSTK_handleDnD(XEvent *event)
{
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
					res+=((this->dropGadget!=NULL) && (this->dropGadget->gadgetAcceptsDnD));
					res+=((this->dropGadget==NULL) && (this->acceptOnThis));
					m.data.l[1]=res;
					m.data.l[2]=0; //Specify an empty rectangle
					m.data.l[3]=0;
					m.data.l[4]=dNdAtoms[XDNDACTIONCOPY]; //We only accept copying anyway.
					XSendEvent(this->app->display,event->xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
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
							XSendEvent(this->app->display,event->xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
						}
					else
						{
							this->sourceWindow=event->xclient.data.l[0];
							if(this->xDnDVersion >= 1)
								XConvertSelection(this->app->display, dNdAtoms[XDNDSELECTION],this->toBeRequested,dNdAtoms[PRIMARY],this->window, event->xclient.data.l[2]);
							else
								XConvertSelection(this->app->display, dNdAtoms[XDNDSELECTION],this->toBeRequested,dNdAtoms[PRIMARY],this->window, CurrentTime);
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

					myprops->mimeType=XGetAtomName(this->app->display,target);
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
								XConvertSelection(this->app->display,dNdAtoms[XDNDSELECTION],this->toBeRequested,dNdAtoms[XDNDSELECTION],this->window, CurrentTime);
						}
					else if(target==this->toBeRequested)
						{
							myprops->dropX=this->droppedData.x;
							myprops->dropY=this->droppedData.y;
							if(this->dropGadget!=NULL)
								this->dropGadget->LFSTK_dropData(myprops);
							else
								if(this->acceptOnThis==true)
									this->LFSTK_dropData(myprops);
							//Reply OK.
							XClientMessageEvent m;
							memset(&m, sizeof(m), 0);
							m.type=ClientMessage;
							m.display=this->app->display;
							m.window=this->sourceWindow;
							m.message_type=dNdAtoms[XDNDFINISHED];
							m.format=32;
							m.data.l[0]=this->window;//w;
							m.data.l[1]=1;
							m.data.l[2]=dNdAtoms[XDNDACTIONCOPY]; //We only ever copy.
							XSendEvent(this->app->display,this->sourceWindow, False, NoEventMask, (XEvent*)&m);
						}
					else
						return;

					XFree(myprops->data);
					XFree(myprops->mimeType);
					delete myprops;
				}
		}
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
	this->dbClick=(interval);//TODO//set in prefs
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
			XChangeProperty(this->app->display,win,pty,XA_ATOM,32,PropModeReplace,(unsigned char *)types,(int)(sizeof(types) / sizeof(Atom)));
		}
	else
		{
/* send data all at once (not using INCR) */
			XChangeProperty(this->app->display,win,pty,this->LFSTK_getDnDAtom(XA_UTF8_STRING),8,PropModeReplace,(unsigned char *)this->clipBuffer.c_str(),(int)this->clipBuffer.length());
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
		XSendEvent(this->app->display,sev->requestor,0,0,&res);
}

/**
* Handle basic window events.
* \param XEvent	*event.
* \return 0=handled, 1=not handled, -1 close window
*/
static	bool flag=false;
static	geometryStruct	oldwindowGeom{0,0,0,0};
int LFSTK_windowClass::LFSTK_handleWindowEvents(XEvent *event)
{	
			//this->LFSTK_clearWindow(false);

//if(this->customwindow==true)
//{
//			cairo_save(this->cr);
//				cairo_reset_clip (this->cr);
//				cairo_set_source_rgba(this->cr,0,0.1,.4,0.2);
//				cairo_paint(this->cr);
//			cairo_restore(this->cr);
//			return(1);
//
//}

	int	retval=0;
	switch(event->type)
		{			
			case ButtonPress:
				if(this->passEventToRoot==true)
					{
						event->xbutton.window=this->app->rootWindow;
						XSendEvent(this->app->display,this->app->rootWindow,false,ButtonPressMask,event);
					}
				break;
			case ButtonRelease:
				if(this->passEventToRoot==true)
					{
						event->xbutton.window=this->app->rootWindow;
						XSendEvent(this->app->display,this->app->rootWindow,false,ButtonReleaseMask,event);
					}
				break;

			case FocusOut:
				break;

			case EnterNotify:
				break;

			case LeaveNotify:
				if((event->xany.window==this->window) && (this->popupFromGadget!=NULL))
					return(-1);
				break;

			case Expose:
				if (event->xexpose.count==0)
					this->LFSTK_clearWindow();

				if(!this->gadgetMap.empty())
					{
						for (std::map<int,mappedListener*>::iterator it=this->gadgetMap.begin();it!=this->gadgetMap.end();++it)
							{
								mappedListener	*ml=it->second;
								if((ml!=NULL) && (ml->gadget!=NULL))
									{
										if(ml->type==MULTIGADGET)
											{	
												geometryStruct	oldwindowGeom=this->windowGeom;

												LFSTK_ExpanderGadgetClass *gadget=static_cast<LFSTK_ExpanderGadgetClass*>(ml->gadget);
												if(gadget->done==true)
													break;
												gadget->done=true;
												gadget->LFSTK_resetGadgets();
											}
									}
								}
						}
				break;

			case ConfigureNotify:
				{
					Window			w;
					Window			root_return,child_return;
					int				root_x_return,root_y_return;
					int				win_x_return,win_y_return;
					unsigned int	mask_return;
					XQueryPointer(this->app->display,this->window,&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return,&mask_return);

					if(flag==false)
						{
							oldwindowGeom=this->windowGeom;
							flag=true;
						}
					this->LFSTK_resizeWindow(event->xconfigurerequest.width,event->xconfigurerequest.height,false);
					this->LFSTK_clearWindow();

					if((this->windowGeom.w!=oldwindowGeom.w) ||(this->windowGeom.h!=oldwindowGeom.h))
						{
							if(!this->gadgetMap.empty())
								{
									for (std::map<int,mappedListener*>::iterator it=this->gadgetMap.begin();it!=this->gadgetMap.end();++it)
										{
											mappedListener	*ml=it->second;
											if((ml!=NULL) && (ml->gadget!=NULL))
												{
													if(ml->type==MULTIGADGET)
														{
															LFSTK_ExpanderGadgetClass *gadget=static_cast<LFSTK_ExpanderGadgetClass*>(ml->gadget);
															if((mask_return==0) || (gadget->liveUpdate==true))
																{
																	gadget->LFSTK_updateGadget(oldwindowGeom);
																	flag=false;
																	XEvent discard;
																	while(XCheckMaskEvent(this->app->display,EnterNotify|LeaveNotify|MotionNotify|FocusIn|FocusOut|ConfigureNotify,&discard)==true);
																}
														}
												}
										}
								}
						}
				}
				break;

			case SelectionRequest:
				if(XGetSelectionOwner(this->app->display,this->LFSTK_getDnDAtom(XA_CLIPBOARD))==this->window)
					this->sendUTF8(&event->xselectionrequest);
				break;

			case ClientMessage:
			case SelectionNotify:
				{
					if(event->xclient.message_type == XInternAtom(this->app->display, "WM_PROTOCOLS", 1) && (Atom)event->xclient.data.l[0] == XInternAtom(this->app->display, "WM_DELETE_WINDOW", 1))
						{
							retval=-1;
							break;
						}

//handle window drop events
					if((this->acceptDnd==true) )
						{
							this->LFSTK_handleDnD(event);
							if((this->droppedData.type!=DROPINVALID) && (this->acceptOnThis==true))
								{
									if((this->callBacks.validCallbacks & WINDOWDROPCB) && (this->callBacks.runTheCallback==true))
										retval=this->callBacks.droppedWindowCallback(this,this->callBacks.keyUserData);

									if(retval=true)
										{
											this->droppedData.type=DROPINVALID;
											freeAndNull(&this->droppedData.data);
										}
								}
							break;
						}
				}
				break;
		}
	return(retval);
}

/**
* Window dNd callback.
* \param dropped dropped callback.
* \note Format for callback is "bool functioname(LFSTK_windowClass *p,void* ud)"
* \note First param passed to callback is pointer to object.
* \note Second param passed to callback is user data.
*/
void LFSTK_windowClass::LFSTK_setWindowDropCallBack(bool (*dropped)(LFSTK_windowClass*,void*),void* ud)
{
	this->callBacks.validCallbacks|=WINDOWDROPCB;
	this->callBacks.droppedWindowCallback=dropped;
	this->callBacks.keyUserData=ud;
	this->callBacks.runTheCallback=true;
	this->callBacks.ignoreOrphanModKeys=true;
	this->acceptOnThis=true;
}

/**
* Delete a gadget.
* \param LFSTK_gadgetClass *gadget gadget to be deleted.
*/
bool LFSTK_windowClass::LFSTK_deleteGadget(LFSTK_gadgetClass *gadget)
{
	if(!this->gadgetMap.empty())
		{
			for (std::map<int,mappedListener*>::iterator it=this->gadgetMap.begin();it!=this->gadgetMap.end();++it)
				{
					mappedListener	*ml=it->second;
					if((ml!=NULL) && (ml->gadget==gadget))
						{
							delete ml->gadget;
							delete ml;
							return(true);
						}
				}
		}
	return(false);
}
