/*
 *
 * ©K. D. Hedger. Fri  9 Nov 10:59:11 GMT 2018 keithdhedger@gmail.com

 * This file (LFSTKToolWindow.cpp) is part of LFSToolKit.

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

LFSTK_toolWindowClass::~LFSTK_toolWindowClass()
{
}

/**
* Common window constructor.
* \param windowInitStruct *wi.
* \note wi contains startup window params.
*/
void LFSTK_toolWindowClass::windowClassInitCommon(windowInitStruct *wi)
{
	XSetWindowAttributes	wa;
	Atom					wm_delete_window;
	XClassHint				classHint;
	Atom					xa;
	Atom					xa_prop[3];

	this->app=wi->app;
	this->setWindowGeom(wi->x,wi->y,wi->w,wi->h,WINDSETALL);

	this->fontString=NULL;
	this->isActive=false;
	this->acceptDnd=false;
	this->loadMonitorData();

	wa.win_gravity=NorthWestGravity;
	wa.override_redirect=wi->overRide;
	wm_delete_window=XInternAtom(this->app->display,"WM_DELETE_WINDOW",0);

	if(wi->app->gotARGB==true)
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

LFSTK_toolWindowClass::LFSTK_toolWindowClass(windowInitStruct *wi,LFSTK_applicationClass *app)
{
	this->windowClassInitCommon(wi);
	this->gadgetMap.clear();
}

LFSTK_toolWindowClass::LFSTK_toolWindowClass(Display *disp,LFSTK_windowClass *wc,const char *windowtype,int x,int y,int w,int h,const char* name,LFSTK_applicationClass *app)
{
	windowInitStruct *wi=new windowInitStruct;
	wi->x=x;
	wi->y=y;
	wi->w=w;
	wi->h=h;
	wi->name=name;
	wi->overRide=true;
	wi->loadVars=true;
	wi->shutDisplayOnExit=false;
	wi->windowType=windowtype;
	wi->decorated=false;
	wi->level=ABOVEALL;
	wi->display=disp;
	wi->wc=wc;
	wi->app=app;

	this->windowClassInitCommon(wi);
	this->gadgetMap.clear();
	delete wi;
}

LFSTK_toolWindowClass::LFSTK_toolWindowClass()
{
}
