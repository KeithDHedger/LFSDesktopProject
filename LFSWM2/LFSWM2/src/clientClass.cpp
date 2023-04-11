/*
 *
 * ©K. D. Hedger. Tue  9 Aug 20:42:38 BST 2022 keithdhedger@gmail.com

 * This file (clientClass.cpp) is part of LFSWM2.

 * LFSWM2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSWM2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWM2.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "clientClass.h"

LFSWM2_clientClass::LFSWM2_clientClass(LFSWM2_Class *mainclass,Window id)
{
	this->mainClass=mainclass;
}

LFSWM2_clientClass::~LFSWM2_clientClass(void)
{
	this->mainClass->LFSWM2_pushXErrorHandler();
		this->mainClass->mainWindowClass->LFSWM2_setClientList(this->contentWindow,false);

		this->mainClass->mainWindowClass->LFSWM2_deleteClientEntry(this->frameWindow);
		this->mainClass->mainWindowClass->LFSWM2_deleteClientEntry(this->contentWindow);

		XRemoveFromSaveSet(this->mainClass->display,this->contentWindow);	
		if(this->transientFor==None)
			XReparentWindow(this->mainClass->display,this->contentWindow,this->mainClass->rootWindow,this->frameWindowRect.x+this->mainClass->sideBarSize,frameWindowRect.y+this->mainClass->titleBarSize);
		else
			XReparentWindow(this->mainClass->display,this->contentWindow,this->mainClass->rootWindow,-10000,-10000);
		XUnmapWindow(this->mainClass->display,this->frameWindow);

		if(this->sizeHints!=NULL)
			XFree(this->sizeHints);
		if(this->sizeHints!=NULL)
			XFree(this->mwmHints);
	this->mainClass->LFSWM2_popXErrorHandler();
}

void LFSWM2_clientClass::LFSWM2_setWindowName(void)
{
	char					*namex=NULL;
	XTextProperty		p;
	long unsigned int	nitems_return;

	this->name="";
	this->mainClass->LFSWM2_pushXErrorHandler();

	namex=(char*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->contentWindow,this->mainClass->atoms.at("_NET_WM_NAME"),this->mainClass->atoms.at("UTF8_STRING"),&nitems_return);
	if (namex!=NULL)
		{
			this->name=namex;
			this->nameIsUTF=true;
		}
	else
		{
			if(XGetWMName(this->mainClass->display,this->contentWindow,&p)!=0)
				{
					namex=this->mainClass->mainWindowClass->LFSWM2_decodeTextProp(&p);
					if(namex!=NULL)
						{
							this->name=namex;
							this->nameIsUTF=false;;
						}
					XFree(p.value);
				}
		}

	if(namex!=NULL)
		free(namex);

	this->mainClass->LFSWM2_popXErrorHandler();
}

void LFSWM2_clientClass::drawMouseEnter(Window id,Pixmap pm,controlData data)
{
	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameFG->pixel);
	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
	XSetLineAttributes(this->mainClass->display, this->mainClass->mainGC, 2, LineSolid, CapNotLast, JoinMiter);
	XDrawRectangle(this->mainClass->display,id,this->mainClass->mainGC,1,1,data.boundingBox.w-2,data.boundingBox.h-2);
}

void LFSWM2_clientClass::LFSWM2_drawMouseLeave(Window id,Pixmap pm,controlData data)
{
	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameBG->pixel);
	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
	XFillRectangle(this->mainClass->display,id,this->mainClass->mainGC,0,0,data.boundingBox.h+10,data.boundingBox.h+10);
 
  	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameText->pixel);
	XSetClipOrigin(this->mainClass->display,this->mainClass->mainGC,data.pixmapBox.x,data.pixmapBox.y);
	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,pm);
	XFillRectangle(this->mainClass->display,id,this->mainClass->mainGC,0,0,20,20);
}

void LFSWM2_clientClass::LFSWM2_sendCloseWindow(void)
{
	XClientMessageEvent	em;

	em.type=ClientMessage;
	em.window=this->contentWindow;
	em.message_type=this->mainClass->atoms.at("WM_PROTOCOLS");
	em.format=32;
	em.data.l[0]=this->mainClass->atoms.at("WM_DELETE_WINDOW");;
	em.data.l[1]=CurrentTime;
	XSendEvent(this->mainClass->display,this->contentWindow,false,NoEventMask,(XEvent *)&em);
	this->visible=false;
	XSync(this->mainClass->display,false);
}

void LFSWM2_clientClass::adjustContentWindow(void)
{
	XWindowAttributes	frameattr;
	rectStruct		r;

	if(this->isFullscreen==true)
		return;

	XGetWindowAttributes(this->mainClass->display,this->frameWindow,&frameattr);
	r.x=frameattr.x+this->mainClass->sideBarSize;
	r.y=frameattr.y+this->mainClass->titleBarSize;
	r.w=this->frameWindowRect.w-this->mainClass->sideBarSize*2;
	r.h=this->frameWindowRect.h-this->mainClass->titleBarSize-this->mainClass->bottomBarSize;

	this->mainClass->mainEventClass->LFSWM2_sendConfigureEvent(this->contentWindow,r);
	this->setWindowRects(true);
}

bool LFSWM2_clientClass::doResizeDraggers(XEvent *e)
{
	rectStruct				r;
	XConfigureRequestEvent	ce;

	switch(e->type)
		{
			case ButtonPress:
				{
					this->LFSWM2_unSpecial();
					this->buttonDown=true;
					this->sx=e->xmotion.x;
					this->sy=e->xmotion.y;
					steps=0;
					this->setWindowRects(false);
				}
				break;

			case ButtonRelease:
				this->buttonDown=false;
				this->sx=0;
				this->sy=0;
				this->setWindowRects(true);
				this->resizeContentWindow(0,0,true);
				this->mainClass->doingMove=false;
				this->isShaded=false;
				this->adjustContentWindow();
				break;

			case MotionNotify:
				if(buttonDown==true)
					{
						this->mainClass->doingMove=true;
						this->steps++;
						if(this->steps>this->smoothness)
							{
								r.x=this->frameWindowRect.x;
								r.y=this->frameWindowRect.y;
								r.w=this->frameWindowRect.w;
								r.h=this->frameWindowRect.h;
								ce.value_mask=0;

								if(e->xany.window==this->topLeftDragger)
									{
										r.x=e->xmotion.x_root-this->sx;
										r.y=e->xmotion.y_root-this->sy;
										r.w=this->frameWindowRect.w-(e->xmotion.x-this->sx);
										r.h=this->frameWindowRect.h-(e->xmotion.y-this->sy);
										ce.value_mask=CWHeight|CWWidth|CWY|CWX|CWMyframe;
									}
								if(e->xany.window==this->topRightDragger)
									{
										r.y=e->xmotion.y_root-this->sy;
										r.w=this->frameWindowRect.w+(e->xmotion.x-this->sx);
										r.h=this->frameWindowRect.h-(e->xmotion.y-this->sy);
										ce.value_mask=CWHeight|CWWidth|CWY|CWMyframe;
									}
								if(e->xany.window==this->bottomLeftDragger)
									{
										r.x=e->xmotion.x_root-this->sx;
										r.w=this->frameWindowRect.w-(e->xmotion.x-this->sx);
										r.h=this->frameWindowRect.h+(e->xmotion.y-this->sy);
										ce.value_mask=CWHeight|CWX|CWMyframe;
									}
								if(e->xany.window==this->bottomRightDragger)
									{
										r.w=this->frameWindowRect.w+(e->xmotion.x-this->sx);
										r.h=this->frameWindowRect.h+(e->xmotion.y-this->sy);
										ce.value_mask=CWHeight|CWWidth|CWMyframe;
									}

								if(e->xany.window==this->bottomDragger)
									{
										r.h=this->frameWindowRect.h+(e->xmotion.y-this->sy);
										ce.value_mask=CWHeight|CWMyframe;
									}

								if(e->xany.window==this->leftSideDragger)
									{
										r.x=e->xmotion.x_root-this->sx;
										r.w=this->frameWindowRect.w-(e->xmotion.x-this->sx);
										ce.value_mask=CWWidth|CWX|CWMyframe;
									}
								if(e->xany.window==this->rightSideDragger)
									{
										r.w=this->frameWindowRect.w+(e->xmotion.x-this->sx);
										ce.value_mask=CWWidth|CWMyframe;
									}

								ce.display=this->mainClass->display;
								ce.parent=this->frameWindow;
								ce.window=this->contentWindow;
								ce.type=ConfigureRequest;
								ce.serial=0;
								ce.send_event=false;
								ce.border_width=BORDER_WIDTH;//TODO//
								ce.detail=Above;
								ce.above=None;
								ce.x=r.x;
								ce.y=r.y;
								ce.width=r.w;
								ce.height=r.h;

								XSendEvent(this->mainClass->display,this->contentWindow,true,StructureNotifyMask,(XEvent*)&ce);
								this->steps=0;
								break;
							}
					}
				break;
		}
	return(true);
}

void LFSWM2_clientClass::resizeContentWindow(int w,int h,bool useframerect)
{
	if(useframerect==true)
		{
			XResizeWindow(this->mainClass->display,this->contentWindow,this->frameWindowRect.w-(this->mainClass->sideBarSize*2),this->frameWindowRect.h-(this->mainClass->titleBarSize+this->mainClass->bottomBarSize));
			this->contentWindowRect={this->contentWindowRect.x,this->contentWindowRect.y,this->frameWindowRect.w-(this->mainClass->sideBarSize*2),this->frameWindowRect.h-(this->mainClass->titleBarSize+this->mainClass->bottomBarSize)};
		}
	else
		{
			XResizeWindow(this->mainClass->display,this->contentWindow,w-(this->mainClass->sideBarSize*2),h-(this->mainClass->titleBarSize+this->mainClass->bottomBarSize));
			this->contentWindowRect={this->contentWindowRect.x,this->contentWindowRect.y,w-(this->mainClass->sideBarSize*2),h-(this->mainClass->titleBarSize+this->mainClass->bottomBarSize)};
		}
}

void LFSWM2_clientClass::resizeFrameWindow(void)
{
	XResizeWindow(this->mainClass->display,this->frameWindow,this->contentWindowRect.w+(this->mainClass->sideBarSize*2),this->contentWindowRect.h+(this->mainClass->titleBarSize+this->mainClass->bottomBarSize));
}

void LFSWM2_clientClass::LFSWM2_unSpecial(void)
{
	if(this->isMaximized==true)
		{
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ"));
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT"));
			this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN"));
			this->isMaximized=false;
		}
	this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(this->contentWindow);
}

bool LFSWM2_clientClass::LFSWM2_handleControls(XEvent *e)
{
	bool					retval=false;
	Pixmap				pm=0;
	struct controlData	data;

	if((e->xany.window==this->topLeftDragger) || (e->xany.window==this->bottomRightDragger) || (e->xany.window==this->topRightDragger) || (e->xany.window==this->bottomLeftDragger)|| (e->xany.window==this->bottomDragger)|| (e->xany.window==this->leftSideDragger)|| (e->xany.window==this->rightSideDragger))
		{
			retval=this->doResizeDraggers(e);
			return(true);
		}

	if((e->xany.window!=this->closeButton) && (e->xany.window!=this->maximizeButton) && (e->xany.window!=this->minimizeButton) && (e->xany.window!=this->shadeButton))
		return(false);

	if(e->xany.window==this->closeButton)
		{
			pm=this->mainClass->closeBitMap;
			data=this->mainClass->closeControlStruct;
		}
	if(e->xany.window==this->maximizeButton)
		{
			pm=this->mainClass->maximizeBitMap;
			data=this->mainClass->maximizeControlStruct;
		}
	if(e->xany.window==this->minimizeButton)
		{
			pm=this->mainClass->minimizeBitMap;
			data=this->mainClass->minimizeControlStruct;
		}
	if(e->xany.window==this->shadeButton)
		{
			pm=this->mainClass->shadeBitMap;
			data=this->mainClass->shadeControlStruct;
		}

	retval=false;
	switch(e->type)
		{
			case Expose:
				fprintf(stderr,"Expose\n");
				break;
			case EnterNotify:
				this->drawMouseEnter(e->xany.window,pm,data);
				break;
			case LeaveNotify:
				this->LFSWM2_drawMouseLeave(e->xany.window,pm,data);
				break;
			case ButtonPress:
				if(e->xbutton.window==this->closeButton)
					{
						this->LFSWM2_sendCloseWindow();
						retval=true;
					}

				if(e->xbutton.window==this->maximizeButton)
					{
						if(this->isMaximized==true)
							{
								this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ"));
								this->mainClass->mainWindowClass->LFSWM2_removeProp(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT"));
							}
						else
							{
								this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ"));
								this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT"));
							}
						this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(this->contentWindow);
						retval=true;
					}

				if(e->xbutton.window==this->minimizeButton)
					{
						this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN"));
						this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(this->contentWindow);
						this->LFSWM2_hideWindow();
						retval=true;
					}

				if(e->xbutton.window==this->shadeButton)
					{
						if(this->isShaded==false)
							{
								this->clientPreShade=this->frameWindowRect.h;
								XResizeWindow(this->mainClass->display,this->frameWindow,this->frameWindowRect.w,this->mainClass->titleBarSize+this->mainClass->bottomBarSize);
								XMoveWindow(this->mainClass->display,this->contentWindow,this->frameWindowRect.w+1,this->frameWindowRect.y);
							}
						else
							{
								XResizeWindow(this->mainClass->display,this->frameWindow,this->frameWindowRect.w,this->clientPreShade);
								XMoveWindow(this->mainClass->display,this->contentWindow,this->mainClass->sideBarSize,this->mainClass->titleBarSize);
							}
						this->isShaded=!this->isShaded;
						retval=true;
					}

				break;
			default:
				retval=false;
		}

	return(retval);
}

void LFSWM2_clientClass::LFSWM2_fullscreenWindow(bool isfull,bool force)
{
	rectStruct	r;

	if((isfull==this->isFullscreen) && (force==false))
		return;

	if(isfull==true)
		{
			pointStruct	fp={this->frameWindowRect.x,this->frameWindowRect.y};
			for(int j=0;j<this->mainClass->numberOfMonitors;j++)
				{
					geometryStruct	mg={this->mainClass->monitors.at(j).x,this->mainClass->monitors.at(j).y,(unsigned)this->mainClass->monitors.at(j).w,(unsigned)this->mainClass->monitors.at(j).h,0};
					if(this->mainClass->lfstkLib->LFSTK_pointInRect(&fp,&mg))
						{
							this->setWindowRects();
							this->clientPreFSRect=this->frameWindowRect;
							XMoveResizeWindow(this->mainClass->display,this->frameWindow,mg.x,mg.y,mg.w,mg.h);
							XMoveResizeWindow(this->mainClass->display,this->contentWindow,mg.x,mg.y,mg.w,mg.h);
						}
				}
		}
	else
		{
			XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->clientPreFSRect.x,this->clientPreFSRect.y,this->clientPreFSRect.w,this->clientPreFSRect.h);
			XMoveResizeWindow(this->mainClass->display,this->contentWindow,this->mainClass->sideBarSize,this->mainClass->titleBarSize,this->clientPreFSRect.w-(this->mainClass->sideBarSize*3),this->clientPreFSRect.h-this->mainClass->bottomBarSize-this->mainClass->titleBarSize);
		}

	this->isFullscreen=isfull;
	this->setWindowRects();
}

void LFSWM2_clientClass::LFSWM2_maxWindow(bool ismaxed,bool force)
{
	if((ismaxed==this->isMaximized) && (force==false))
		return;

	if(ismaxed==true)
		{
			this->framePreMaxRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->frameWindow,this->mainClass->rootWindow,false);
			this->clientPreMaxRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->contentWindow,this->mainClass->rootWindow,false);
			rectStruct rr={this->mainClass->monitors.at(0).x,this->mainClass->monitors.at(0).y,(int)this->mainClass->monitors.at(0).w,(int)this->mainClass->monitors.at(0).h};
			rr.w=this->mainClass->monitors.at(0).w-((this->mainClass->sideBarSize+BORDER_WIDTH)*3);//TODO//
			rr.h=this->mainClass->monitors.at(0).h-(this->mainClass->titleBarSize+this->mainClass->bottomBarSize+(BORDER_WIDTH*2));
			rr.x=this->mainClass->sideBarSize;
			rr.y=this->mainClass->titleBarSize;

			XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->mainClass->monitors.at(0).x,this->mainClass->monitors.at(0).y,this->mainClass->monitors.at(0).w,this->mainClass->monitors.at(0).h);
			XMoveResizeWindow(this->mainClass->display,this->contentWindow,rr.x,rr.y,rr.w,rr.h);
		}
	else
		{
			XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->framePreMaxRect.x,this->framePreMaxRect.y,this->framePreMaxRect.w,this->framePreMaxRect.h);
			XMoveResizeWindow(this->mainClass->display,this->contentWindow,this->clientPreMaxRect.x,this->clientPreMaxRect.y,this->clientPreMaxRect.w,this->clientPreMaxRect.h);
		}
	this->isMaximized=ismaxed;
	this->setWindowRects();
}

void LFSWM2_clientClass::setWindowRects(bool resize)
{
	this->contentWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->contentWindow,this->mainClass->rootWindow);
	this->frameWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->frameWindow,this->mainClass->rootWindow);

	if(resize==true)
		this->LFSWM2_resizeControls();
}

void LFSWM2_clientClass::LFSWM2_showWindow(void)
{
		XMapWindow(this->mainClass->display,this->frameWindow);
}

void LFSWM2_clientClass::LFSWM2_hideWindow(void)
{
		XUnmapWindow(this->mainClass->display,this->frameWindow);
}

void LFSWM2_clientClass::LFSWM2_resizeControls(void)
{
	XResizeWindow(this->mainClass->display,this->bottomDragger,this->frameWindowRect.w,this->dragsize);
	XResizeWindow(this->mainClass->display,this->leftSideDragger,this->dragsize,this->frameWindowRect.h-(this->dragsize*2));
	XResizeWindow(this->mainClass->display,this->rightSideDragger,this->dragsize,this->frameWindowRect.h-(this->dragsize*2));
}

void LFSWM2_clientClass::LFSWM2_setWMState(XEvent *e)
{
	long unsigned int	n;
	Atom					*states;

	this->onBottom=false;
	this->onTop=false;
	this->visibleOnAllDesks=false;

	states=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getFullProp(e->xproperty.window,this->mainClass->atoms.at("_NET_WM_STATE"),XA_ATOM,32,&n);

	if(n>0)
		{
			for(long unsigned int g=0;g<n;g++)
				{
					//this->mainClass->DEBUG_printAtom(states[g]);
					this->mainClass->mainWindowClass->LFSWM2_changeState(e->xproperty.window,NET_WM_STATE_ADD,states[g]);
				}
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atoms.at("_NET_WM_STATE_STICKY")))
		{
			this->visibleOnAllDesks=true;
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atoms.at("_NET_WM_STATE_BELOW")))
		{
			this->onBottom=true;
			XLowerWindow(this->mainClass->display,this->contentWindow);
			XSync(this->mainClass->display,false);
			this->mainClass->restackCnt++;
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atoms.at("_NET_WM_STATE_ABOVE")))
		{
			this->onTop=true;
			XRaiseWindow(this->mainClass->display,this->contentWindow);
			this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,32,&this->contentWindow,1);
			XSync(this->mainClass->display,false);
			this->mainClass->restackCnt++;
		}
	if(states!=NULL)
		XFree(states);
}

bool LFSWM2_clientClass::LFSWM2_doFrameMoveEvents(XEvent *e)
{
	LFSWM2_clientClass	*cc;

	while(true)
		{
			XEvent ee;
			XNextEvent(this->mainClass->display,&ee);

			switch (ee.type)
				{
					case Expose:
						cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(ee.xexpose.window);
						if(cc!=NULL)
							cc->LFSWM2_refreshFrame((XExposeEvent*)&ee);
						break;
					case MotionNotify:
						{
							int xdiff=ee.xbutton.x_root-e->xbutton.x_root;
							int ydiff=ee.xbutton.y_root-e->xbutton.y_root;
							XMoveWindow(this->mainClass->display,this->frameWindow,this->frameWindowRect.x+xdiff,this->frameWindowRect.y+ydiff);
						}
						break;
					case ButtonRelease:
						{
							this->adjustContentWindow();
							return(true);
						}
						break;
				}
		}

	return(true);
}

rectStruct LFSWM2_clientClass::setTitlePosition(void)
{
	rectStruct	r=this->frameWindowRect;
	XGlyphInfo	extents;
	int			namewidth;
	int			offset=(this->controlCnt+1)*(deleteWindowSize)+(CONTROL_GAP);

	if(this->nameIsUTF==true)
		XftTextExtentsUtf8(this->mainClass->display,this->mainClass->frameFont,(XftChar8*)this->name.c_str(),this->name.length(),&extents);	
	else
		XftTextExtents8(this->mainClass->display,this->mainClass->frameFont,(XftChar8*)this->name.c_str(),this->name.length(),&extents);

	r.y=(extents.y/2)+(this->mainClass->titleBarSize/2);
	namewidth=extents.width-extents.x;
	switch(this->mainClass->titlePosition)
		{
			case 1:
				r.x=CONTROL_GAP;
				break;
			case 2:
				r.x=((r.w-offset)/2)-(namewidth/2);
				break;
			case 3:
				r.x=r.w-namewidth-offset;
				break;
		}
	return(r);
}

void LFSWM2_clientClass::LFSWM2_refreshFrame(XExposeEvent *e)//TODO//prevent flicker
{
	rectStruct	r;

	if((e!=NULL) && (e->count!=0))
		return;

	r=this->frameWindowRect;

	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameBG->pixel);
	XFillRectangle(this->mainClass->display,this->frameWindow,this->mainClass->mainGC,0,0,r.w,r.h);

	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameText->pixel);
	XftDrawChange(this->mainClass->frameText->draw,this->frameWindow);

	if(this->name.length()>0)
		{
			r=this->setTitlePosition();
			if(this->nameIsUTF==true)
				XftDrawStringUtf8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)this->name.c_str(),strlen(this->name.c_str()));
			else
				XftDrawString8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)this->name.c_str(),strlen(this->name.c_str()));
		}

//buttons
	this->LFSWM2_drawMouseLeave(this->closeButton,this->mainClass->closeBitMap,this->mainClass->closeControlStruct);
	this->LFSWM2_drawMouseLeave(this->maximizeButton,this->mainClass->maximizeBitMap,this->mainClass->maximizeControlStruct);
	this->LFSWM2_drawMouseLeave(this->minimizeButton,this->mainClass->minimizeBitMap,this->mainClass->minimizeControlStruct);
	this->LFSWM2_drawMouseLeave(this->shadeButton,this->mainClass->shadeBitMap,this->mainClass->shadeControlStruct);
}

bool LFSWM2_clientClass::LFSWM2_handleEvents(XEvent *e)
{
//this->mainClass->DEBUG_printEventData(e,false);

	switch(e->type)
		{
			case ButtonRelease:
				this->adjustContentWindow();
				break;
			case MotionNotify:
				{
					bool domove=false;

					if((this->frameWindow==e->xmotion.window) && (e->xmotion.state==Button1Mask))
						domove=true;
					if((this->contentWindow==e->xmotion.window) && (e->xmotion.state==Button1Mask+Mod4Mask))
						domove=!this->isFullscreen;

					if(domove==true)
						{
							this->LFSWM2_doFrameMoveEvents(e);
							this->mainClass->restackCnt=0;
							return(true);
						}
					break;
				}
			case Expose:
				{
					if(e->xexpose.count>0)
						break;

					this->LFSWM2_refreshFrame((XExposeEvent*)e);
				}
				break;

			case UnmapNotify:
				{
					this->mainClass->mainWindowClass->LFSWM2_setClientList(this->contentWindow,false);
					this->visible=false;
					XUnmapWindow(this->mainClass->display,this->frameWindow);
					return(true);
				}
				break;
			case DestroyNotify:
				{
					bool					loop=false;
					LFSWM2_clientClass	*cc;
					do
						{
							loop=false;
contloop:
							for(auto& x:this->mainClass->mainWindowClass->clientList)
								{
				    					cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(x.first);
									if((cc!=NULL) && (cc->transientFor==this->contentWindow))
										{
				   							cc->LFSWM2_sendCloseWindow();
				   							XUnmapWindow(this->mainClass->display, cc->frameWindow);
				   							delete cc;
				   							goto contloop;
   										}
 	 							}
						}
					while(loop==true);

					delete this;
					return(true);
				}
				break;
			case MapNotify:
			//	std::cerr<<"MapNotify"<<std::endl;
				break;

			case ConfigureNotify:
#if 0
					fprintf(stderr,"ConfigureNotify\n");
					fprintf(stderr,"type=%i 23=ConfigureRequest\n",e->xconfigure.type);
					fprintf(stderr,"send_event=%s \n",this->mainClass->DEBUG_printBool(e->xconfigure.send_event));
					fprintf(stderr,"display=%p \n",e->xconfigure.display);
					fprintf(stderr,"window=%p \n",(void*)e->xconfigure.window);
					fprintf(stderr,"x=%i \n",e->xconfigure.x);
					fprintf(stderr,"y=%i \n",e->xconfigure.y);
					fprintf(stderr,"width=%i \n",e->xconfigure.width);
					fprintf(stderr,"height=%i \n",e->xconfigure.height);
					fprintf(stderr,"border_width=%i \n",e->xconfigure.border_width);
					fprintf(stderr,"above=%p \n",(void*)e->xconfigure.above);
#endif
				break;

			case ConfigureRequest://TODO//NEXT
				{
					XRaiseWindow(this->mainClass->display,this->frameWindow);
					this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,32,&this->contentWindow,1);
					XSetInputFocus(this->mainClass->display,this->contentWindow,RevertToNone,CurrentTime);

					int			woff=4;
					int			hoff=28;
					rectStruct	r;
#if 0
					fprintf(stderr,"ConfigureRequest\n");
					fprintf(stderr,"type=%i 23=ConfigureRequest\n",e->xconfigurerequest.type);
					fprintf(stderr,"send_event=%s \n",this->mainClass->DEBUG_printBool(e->xconfigurerequest.send_event));
					fprintf(stderr,"display=%p \n",e->xconfigurerequest.display);
					fprintf(stderr,"parent=%p \n",(void*)e->xconfigurerequest.parent);
					fprintf(stderr,"window=%p \n",(void*)e->xconfigurerequest.window);
					fprintf(stderr,"x=%i \n",e->xconfigurerequest.x);
					fprintf(stderr,"y=%i \n",e->xconfigurerequest.y);
					fprintf(stderr,"width=%i \n",e->xconfigurerequest.width);
					fprintf(stderr,"height=%i \n",e->xconfigurerequest.height);
					fprintf(stderr,"border_width=%i \n",e->xconfigurerequest.border_width);
					fprintf(stderr,"above=%p \n",(void*)e->xconfigurerequest.above);
					fprintf(stderr,"detail=%x \n",e->xconfigurerequest.detail);
					fprintf(stderr,"value_mask=%p \n",(void*)e->xconfigurerequest.value_mask);
#endif
					//if(this->transientFor!=0)//TODO//
					//	break;

					if(e->xconfigurerequest.value_mask==0xc && e->xconfigurerequest.send_event==0x0)
						{
							return(false);
						}
					if(e->xconfigurerequest.value_mask==0xf && e->xconfigurerequest.send_event==0x0)
						{
							return(false);
						}

					if(((e->xconfigurerequest.value_mask & CWMyframe)==CWMyframe) && (e->xconfigurerequest.window==this->contentWindow))
						{
							if(e->xconfigurerequest.send_event==true)
								{
									switch(e->xconfigurerequest.value_mask)
										{
											case 0x104:
											case 0x105:
											case 0x108:
											case 0x109:
											case 0x10c:
											case 0x10e:
											case 0x10f:
												r={e->xconfigurerequest.x,e->xconfigurerequest.y,e->xconfigurerequest.width,e->xconfigurerequest.height};
												XMoveResizeWindow(this->mainClass->display,this->frameWindow,r.x,r.y,r.w,r.h);
												this->frameWindowRect=r;
												break;
										}
								}
						}
					else
						{
							if((e->xconfigurerequest.value_mask==0xc) && (e->xconfigurerequest.send_event==0x0))
								{
									rectStruct	r={e->xconfigurerequest.x,e->xconfigurerequest.y,e->xconfigurerequest.width+woff,e->xconfigurerequest.height+hoff};
									XMoveResizeWindow(this->mainClass->display,this->frameWindow,r.x,r.y,r.w,r.h);
									this->frameWindowRect=r;
									break;
								}

							if((e->xconfigurerequest.value_mask==0xf) && (e->xconfigurerequest.send_event==0x0))
								{
									rectStruct	r={e->xconfigurerequest.x,e->xconfigurerequest.y,e->xconfigurerequest.width+woff,e->xconfigurerequest.height+hoff};
									XMoveResizeWindow(this->mainClass->display,this->frameWindow,r.x,r.y,r.w,r.h);
									this->frameWindowRect=r;
								}
						}

					r.x=this->frameWindowRect.x+this->mainClass->sideBarSize;
					r.y=this->frameWindowRect.y+this->mainClass->titleBarSize;
					r.w=this->frameWindowRect.w-this->mainClass->sideBarSize*2;
					r.h=this->frameWindowRect.h-this->mainClass->titleBarSize-this->mainClass->bottomBarSize;
					XResizeWindow(this->mainClass->display,this->contentWindow,r.w,r.h);
					this->adjustContentWindow();
					this->mainClass->restackCnt=1;

					if(e->xconfigurerequest.detail==Below)
						{
							this->mainClass->mainWindowClass->LFSWM2_changeState(this->contentWindow,NET_WM_STATE_REMOVE,this->mainClass->atoms.at("_NET_WM_STATE_ABOVE"));
							this->mainClass->mainWindowClass->LFSWM2_changeState(this->contentWindow,NET_WM_STATE_TOGGLE,this->mainClass->atoms.at("_NET_WM_STATE_BELOW"));
							this->onBottom=!this->onBottom;
							this->mainClass->restackCnt=1;
							return(true);
							break;
						}
					return(false);
				}
				break;
		}
	return(false);
}

void LFSWM2_clientClass::resizeContentWindow(rectStruct r,bool moveorigin)
{
	XConfigureRequestEvent	ce;

	ce.display=this->mainClass->display;
	ce.parent=this->frameWindow;
	ce.window=this->contentWindow;
	ce.type=ConfigureRequest;
	ce.serial=0;
	ce.send_event=True;
	ce.border_width=BORDER_WIDTH;//TODO//

	if(moveorigin==true)
		ce.value_mask=CWWidth|CWHeight|CWX|CWY;
	else
		ce.value_mask=CWWidth|CWHeight;

	ce.detail=Above;
	ce.above=None;
	ce.x=r.x;
	ce.y=r.y;
	ce.width=r.w;
	ce.height=r.h;

	XSendEvent(this->mainClass->display,this->contentWindow,true,StructureNotifyMask,(XEvent*)&ce);
}
