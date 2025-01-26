/*
 *
 * ©K. D. Hedger. Tue 21 Jan 15:58:37 GMT 2025 keithdhedger@gmail.com

 * This file (embedClass.cpp) is part of LFSTray.

 * LFSTray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSTray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSTray.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "globals.h"

LFSTray_embedClass::LFSTray_embedClass(LFSTray_trayClass *trayclass)
{
	this->tray=trayclass;
}

LFSTray_embedClass::~LFSTray_embedClass(void)
{
}

Pixmap LFSTray_embedClass::makePixmap(void)
{
	int		x;
	int		y;
	Window	cr;

	Pixmap pm=this->tray->apc->globalLib->LFSTK_getWindowPixmap(this->tray->apc->display,this->tray->apc->rootWindow);
	Pixmap back=XCreatePixmap(this->tray->apc->display,this->tray->apc->mainWindow->window,this->tray->iconSize,this->tray->iconSize,24);
	GC gc=XCreateGC(this->tray->apc->display,back,0,NULL);
	XTranslateCoordinates(this->tray->apc->display,this->tray->apc->mainWindow->window,this->tray->apc->rootWindow,0,0,&x,&y,&cr);
	XCopyArea(this->tray->apc->display,pm,back,gc,x+this->nextIconX,y+this->nextIconY,this->tray->iconSize,this->tray->iconSize,0,0);
	XFreeGC(this->tray->apc->display,gc);
//fprintf(stderr,"back=%p x+this->nextIconX=%i\n",back,x+this->nextIconX);
//XSync(this->tray->apc->display,False);
	return(back);
}

void LFSTray_embedClass::nextPosition(void)
{
	int xoffset=0;
	int yoffset=0;

	if(this->tray->vertical==false)
		{
			xoffset=this->tray->iconSize;
			yoffset=0;
		}
	else
		{
			xoffset=0;
			yoffset=this->tray->iconSize;
		}

	this->nextIconX+=xoffset;
	this->nextIconY+=yoffset;
}


bool LFSTray_embedClass::addIcon(Window wid)
{
	std::string			winname=trayClass->getWindowName(wid);
	int					pid=0;
	Atom					actual_type;
	int					actual_format;
	unsigned long		nitems;
	unsigned long		bytes_after;
	unsigned char		*prop=0;
	XWindowAttributes	attr;

	fprintf(stderr,"winname=%s\n",winname.c_str());
	//XSync(this->tray->apc->display,False);
	xerror=false;
	XErrorHandler old=XSetErrorHandler(windowErrorHandler);

	XSelectInput(this->tray->apc->display,wid,StructureNotifyMask | PropertyChangeMask | ResizeRedirectMask);
	int ret=XGetWindowProperty(this->tray->apc->display,wid,trayClass->_NET_WM_PID,0,1024,False,AnyPropertyType, &actual_type, &actual_format, &nitems, &bytes_after, &prop);
	if(ret==Success && prop)
		{
			pid=prop[1] * 256;
			pid += prop[0];
		}

//	if(XGetWindowAttributes(this->tray->apc->display,wid, &attr)==False)
//		{
//			XSelectInput(this->tray->apc->display,wid,NoEventMask);
//			XSync(this->tray->apc->display,False);
//			XSetErrorHandler(old);
//			return(false);
//		}

	//XSynchronize(this->tray->apc->display,true);
	// Create the parent window that will embed the icon
	if(this->tray->debugmsg)
		fprintf(stderr,"lfstray: XGetWindowAttributes(server.display,win=%p,&attr)\n",wid);
	if(XGetWindowAttributes(this->tray->apc->display,wid, &attr)==False)
		{
			fprintf(stderr,"no wid attr\n");
			XSelectInput(this->tray->apc->display,wid,NoEventMask);
			XSync(this->tray->apc->display,False);
			XSetErrorHandler(old);
			return FALSE;
		}

	unsigned long mask=0;
	XSetWindowAttributes set_attr;
	Visual *visual=this->tray->apc->visual;

	if(this->tray->debugmsg)
		fprintf(stderr,"addIcon: %p (%s),pid %d,visual %p,colormap %lu,depth %d,width %d,height %d"  "\n",wid,winname.c_str(),pid,(void*)attr.visual,attr.colormap,attr.depth,attr.width,attr.height);
	visual=attr.visual;
	set_attr.background_pixel=0;
	set_attr.border_pixel=0;
	set_attr.colormap=attr.colormap;
	mask=CWColormap | CWBackPixel | CWBorderPixel;

	if(this->tray->debugmsg)
		fprintf(stderr,"lfstray: XCreateWindow(...)\n");
	Window parent=XCreateWindow(this->tray->apc->display,this->tray->apc->mainWindow->window,-1000,-1000,this->tray->iconSize,this->tray->iconSize,0,attr.depth,InputOutput,visual,mask, &set_attr);

	this->tray->iconList[wid]={wid,parent,this->nextIconX,this->nextIconY,this->tray->iconSize,this->tray->iconSize,false,this->makePixmap()};

	return(true);
}

void LFSTray_embedClass::embedWindow(Window wid)
{
	sysIcons		icondata;
	XEvent		e;

	icondata=this->tray->iconList.find(wid)->second;
	if(icondata.embedded==false)
		{
			if(this->tray->debugmsg)
				fprintf(stderr,"wid=%p par=%p x=%i y=%i w=%i h=%i embedded=%i\n",icondata.iconWindow,icondata.parentWindow,icondata.x,icondata.y,icondata.w,icondata.h,icondata.embedded);

			XWithdrawWindow(this->tray->apc->display,icondata.iconWindow,this->tray->apc->screen);
			doingwhat="XReparentWindow "+std::to_string(__LINE__);
			XReparentWindow(this->tray->apc->display,icondata.iconWindow,icondata.parentWindow,0,0);

			if(this->tray->debugmsg)
				fprintf(stderr,"XMoveResizeWindow(this->tray->apc->display,wid=%p,0,0,this->tray->iconSize=%d,this->tray->iconSize=%d)\n",icondata.iconWindow,this->tray->iconSize,this->tray->iconSize);
			doingwhat="XMoveResizeWindow "+std::to_string(__LINE__);
			XMoveResizeWindow(this->tray->apc->display,icondata.iconWindow,0,0,this->tray->iconSize,this->tray->iconSize);

	// Embed into parent
			e.xclient.type=ClientMessage;
			e.xclient.serial=0;
			e.xclient.send_event=True;
			e.xclient.message_type=trayClass->_XEMBED;
			e.xclient.window=icondata.iconWindow;
			e.xclient.format=32;
			e.xclient.data.l[0]=CurrentTime;
			e.xclient.data.l[1]=XEMBED_EMBEDDED_NOTIFY;
			e.xclient.data.l[2]=0;
			e.xclient.data.l[3]=icondata.parentWindow;
			e.xclient.data.l[4]=0;
			if(this->tray->debugmsg)
				fprintf(stderr,"lfstray: XSendEvent(server.display,traywin->win,False,NoEventMask,&e)\n");
			doingwhat="XSendEvent "+std::to_string(__LINE__);
			XSendEvent(this->tray->apc->display,icondata.iconWindow,False,NoEventMask, &e);

			this->tray->iconList[wid].embedded=true;
			DEBUG_showIcons();
			XMapWindow(this->tray->apc->display,icondata.parentWindow);
		}
}


void LFSTray_embedClass::removeIcon(Window wid)
{
	if(this->tray->iconList.find(wid)!=this->tray->iconList.end())
		{
			this->currentIcon=this->tray->iconList.find(wid)->second;
			this->tray->iconList.erase(this->tray->iconList.find(wid));
		}
}

void LFSTray_embedClass::refreshIcons(void)
{
	std::map<Window,sysIcons>::iterator	it;

	this->nextIconX=0;
	this->nextIconY=0;
	Pixmap	pm;

	if(trayClass->iconList.size()>0)
		{
			for (it=trayClass->iconList.begin();it!=trayClass->iconList.end();++it)
				{
					it->second.x=nextIconX;
					it->second.y=nextIconY;

					XMoveWindow(this->tray->apc->display,it->second.parentWindow,nextIconX,nextIconY);
					pm=this->makePixmap();
					XSetWindowBackgroundPixmap(this->tray->apc->display,it->second.parentWindow,pm);
					XFreePixmap(this->tray->apc->display,it->second.background);
					it->second.background=pm;
					XWithdrawWindow(this->tray->apc->display,it->second.iconWindow,this->tray->apc->screen);
					XMapWindow(this->tray->apc->display,it->second.iconWindow);

					it->second.x=nextIconX;
					it->second.y=nextIconY;
					this->nextPosition();
				}
		}
	else
		this->tray->apc->mainWindow->LFSTK_resizeWindow(1,1,true);
}
