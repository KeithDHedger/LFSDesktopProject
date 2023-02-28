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

	XGetWindowAttributes(this->mainClass->display,this->frameWindow,&frameattr);
	r.x=frameattr.x+this->mainClass->sideBarSize;
	r.y=frameattr.y+this->mainClass->titleBarSize;
	r.w=this->frameWindowRect.w-this->mainClass->sideBarSize*3;
	r.h=this->frameWindowRect.h-this->mainClass->titleBarSize-this->mainClass->bottomBarSize;

	this->mainClass->mainEventClass->LFSWM2_sendConfigureEvent(this->contentWindow,r);
	this->setWindowRects(true);
}

bool LFSWM2_clientClass::doResizeDraggers(XEvent *e)
{
	rectStruct		r;

	switch(e->type)
		{
			case ButtonPress:
				this->buttonDown=true;
				this->sx=e->xmotion.x;
				this->sy=e->xmotion.y;
				steps=0;
				this->setWindowRects(false);
				this->mainClass->doingMove=false;
				break;

			case ButtonRelease:
				this->buttonDown=false;
				this->sx=0;
				this->sy=0;
				this->setWindowRects(true);
				this->adjustContentWindow();
				this->mainClass->doingMove=false;
				break;
			case MotionNotify:
				if(buttonDown==true)
					{
						this->mainClass->doingMove=true;
						this->steps++;
						if(this->steps>this->smoothness)
							{
								if(e->xany.window==this->topLeftDragger)
									{
										r.x=e->xmotion.x_root-this->sx;
										r.y=e->xmotion.y_root-this->sy;
										r.w=this->contentWindowRect.w-(e->xmotion.x-this->sx);
										r.h=this->contentWindowRect.h-(e->xmotion.y-this->sy);
									}
								if(e->xany.window==this->topRightDragger)
									{
										r.x=this->frameWindowRect.x;
										r.y=e->xmotion.y_root-this->sy;
										r.w=this->contentWindowRect.w+(e->xmotion.x-this->sx);
										r.h=this->contentWindowRect.h-(e->xmotion.y-this->sy);
									}
								if(e->xany.window==this->bottomLeftDragger)
									{
										r.x=e->xmotion.x_root-this->sx;
										r.y=this->frameWindowRect.y;
										r.w=this->contentWindowRect.w-(e->xmotion.x-this->sx);
										r.h=this->contentWindowRect.h+(e->xmotion.y-this->sy);
									}
								if(e->xany.window==this->bottomRightDragger)
									{
										r.x=this->frameWindowRect.x;
										r.y=this->frameWindowRect.y;
										r.w=this->contentWindowRect.w+(e->xmotion.x-this->sx);
										r.h=this->contentWindowRect.h+(e->xmotion.y-this->sy);
									}

								if(e->xany.window==this->bottomDragger)
									{
										r.x=this->frameWindowRect.x;
										r.y=this->frameWindowRect.y;
										r.w=this->contentWindowRect.w;
										r.h=this->contentWindowRect.h+(e->xmotion.y-this->sy);
									}

								if(e->xany.window==this->leftSideDragger)
									{
										r.x=e->xmotion.x_root-this->sx;
										r.y=this->frameWindowRect.y;
										r.w=this->contentWindowRect.w-(e->xmotion.x-this->sx);
										r.h=this->contentWindowRect.h;
									}
								if(e->xany.window==this->rightSideDragger)
									{
										r.x=this->frameWindowRect.x;
										r.y=this->frameWindowRect.y;
										r.w=this->contentWindowRect.w+(e->xmotion.x-this->sx);
										r.h=this->contentWindowRect.h;
									}

								this->resizeContentWindow(r);
								this->steps=0;
								break;
							}
					}
				break;

//			case Expose:
//				cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(ee.xexpose.window);
//				if(cc!=NULL)
//					cc->LFSWM2_refreshFrame((XExposeEvent*)&ee);
//				break;
		}
	return(true);
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

	retval=true;
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
							this->clientPreFSRect={
							this->frameWindowRect.x,
							this->frameWindowRect.y,
							this->frameWindowRect.w-(this->mainClass->sideBarSize*3),
							this->frameWindowRect.h-this->mainClass->bottomBarSize-this->mainClass->titleBarSize
							};

							r.x=0-BORDER_WIDTH-this->mainClass->sideBarSize;
							r.y=0-this->mainClass->titleBarSize;
							r.w=this->mainClass->monitors.at(j).w-BORDER_WIDTH-this->mainClass->sideBarSize;
							r.h=this->mainClass->monitors.at(j).h;
							this->resizeContentWindow(r);
						}
				}
		}
	else
		{
			this->resizeContentWindow(this->clientPreFSRect);
			this->isFullscreen=false;
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
			rectStruct rr=this->mainClass->monitors.at(0);
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
}

bool LFSWM2_clientClass::LFSWM2_doFrameMoveEvents(XEvent *e)
{
	LFSWM2_clientClass	*cc;

	while(true)
		{
			XEvent ee;
			XNextEvent(this->mainClass->display,&ee);

//fprintf(stderr,"------------------------");
//this->mainClass->DEBUG_printEventData(&ee,false);
//fprintf(stderr,"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");

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
	int			controlcnt=4;//TODO//reset to number of controls
	int			offset=CONTROL_GAP*controlcnt;

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
			case MotionNotify:
				{
					bool domove=false;

					if((this->frameWindow==e->xmotion.window) && (e->xmotion.state==Button1Mask))
						domove=true;
					if((this->contentWindow==e->xmotion.window) && (e->xmotion.state==Button1Mask+Mod4Mask))
						domove=true;

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
					//fprintf(stderr,">>>>>>>>>>>>>>>UnmapNotify IN eventnumber win=%p event=%p\n",e->xmap.window,e->xmap.event);
					//long data[2]= { WithdrawnState,None };
					//XChangeProperty(this->mainClass->display,this->frameWindow,this->mainClass->atoms.at("WM_STATE"),this->mainClass->atoms.at("WM_STATE"),32,PropModeReplace,(unsigned char *)data,2);
					this->mainClass->mainWindowClass->LFSWM2_setClientList(this->contentWindow,false);
					this->visible=false;
					XUnmapWindow(this->mainClass->display,this->frameWindow);
					return(true);
				}
				break;
			case DestroyNotify:
				{
					//fprintf(stderr,">>>>>>>>>>>>>>>DestroyNotify IN eventnumber win=%p event=%p\n",e->xdestroywindow.window,e->xdestroywindow.event);
					//long data[2]= { WithdrawnState,None };
				//	XChangeProperty(this->mainClass->display,this->frameWindow,this->mainClass->atoms.at("WM_STATE"),this->mainClass->atoms.at("WM_STATE"),32,PropModeReplace,(unsigned char *)data,2);

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
				//fprintf(stderr,"MapNotify window=%p \n",e->xmap.window);
				{
					this->onDesk=this->mainClass->currentDesktop;//TODO//transient for
					//long data[2]= { NormalState,None };
				//	XChangeProperty(this->mainClass->display,this->frameWindow,this->mainClass->atoms.at("WM_STATE"),this->mainClass->atoms.at("WM_STATE"),32,PropModeReplace,(unsigned char *)data,2);
					this->mainClass->mainWindowClass->LFSWM2_reloadWindowState(e->xmaprequest.window);
					XMapWindow(this->mainClass->display,e->xmaprequest.window);
					XRaiseWindow(this->mainClass->display,e->xmaprequest.window);
					return(true);
				}
				break;
			case ConfigureRequest://TODO//NEXT
				{
					//if(this->transientFor!=0)//TODO//
					//	break;
					//fprintf(stderr,"ConfigureRequest transientFor %p\n",this->transientFor);
					XWindowChanges	changes;

					this->mainClass->LFSWM2_pushXErrorHandler();
#if 0
					fprintf(stderr,"ConfigureRequest\n");
					fprintf(stderr,"type=%i 23=ConfigureRequest\n",e->xconfigurerequest.type);
					fprintf(stderr,"send_event=%i \n",e->xconfigurerequest.send_event);
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
					if(e->xconfigurerequest.detail==Below)
						{
							this->mainClass->mainWindowClass->LFSWM2_changeState(this->contentWindow,NET_WM_STATE_REMOVE,this->mainClass->atoms.at("_NET_WM_STATE_ABOVE"));
							this->mainClass->mainWindowClass->LFSWM2_changeState(this->contentWindow,NET_WM_STATE_TOGGLE,this->mainClass->atoms.at("_NET_WM_STATE_BELOW"));
							this->onBottom=!this->onBottom;
							this->mainClass->restackCnt=1;
							this->mainClass->LFSWM2_popXErrorHandler();
							return(true);
							break;
						}

					this->mainClass->LFSWM2_popXErrorHandler();
						// Copy fields from e to changes.
					changes.x=e->xconfigurerequest.x;
					changes.y=e->xconfigurerequest.y;
					changes.width=e->xconfigurerequest.width;
					changes.height=e->xconfigurerequest.height;
					changes.border_width=e->xconfigurerequest.border_width;
					changes.sibling=e->xconfigurerequest.above;
					changes.stack_mode=e->xconfigurerequest.detail;

					if(e->xconfigurerequest.send_event==false)
						{
							if(e->xconfigurerequest.value_mask==0xc)
								{
									if(e->xconfigurerequest.parent==this->mainClass->rootWindow)//TODO/ugly xterm hack!!
										XMoveResizeWindow(this->mainClass->display,e->xconfigurerequest.window,changes.x,changes.y,changes.width+(this->mainClass->sideBarSize*3),changes.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize);											
									else
										XMoveWindow(this->mainClass->display,e->xconfigurerequest.window,this->mainClass->sideBarSize,this->mainClass->titleBarSize);
									this->adjustContentWindow();
									this->mainClass->restackCnt=1;
									return(true);
								}

							changes.x=this->mainClass->sideBarSize;
							changes.y=this->mainClass->titleBarSize;
							XConfigureWindow(this->mainClass->display,e->xconfigurerequest.window,e->xconfigurerequest.value_mask,&changes);
							XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->frameWindowRect.x,this->frameWindowRect.y,changes.width+(this->mainClass->sideBarSize*3),changes.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize);
							this->mainClass->restackCnt=1;
							return(true);	
						}
					//else//TODO//MMMMMmmmmmmm???
						{
							XConfigureWindow(this->mainClass->display,e->xconfigurerequest.window,e->xconfigurerequest.value_mask,&changes);
							XMoveResizeWindow(this->mainClass->display,this->frameWindow,changes.x,changes.y,changes.width+(this->mainClass->sideBarSize*3),changes.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize);
							XMoveWindow(this->mainClass->display,this->contentWindow,this->mainClass->sideBarSize,this->mainClass->titleBarSize);
							this->contentWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->contentWindow,this->mainClass->rootWindow);
							this->frameWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->frameWindow,this->mainClass->rootWindow);
							this->LFSWM2_resizeControls();
							this->LFSWM2_resizeControls();
							this->mainClass->restackCnt=1;
							//return(true);
						}
				}
				break;
		}
	return(false);
}

void LFSWM2_clientClass::resizeContentWindow(rectStruct r)
{
	XConfigureRequestEvent	ce;

	ce.display=this->mainClass->display;
	ce.parent=this->frameWindow;
	ce.window=this->contentWindow;
	ce.type=ConfigureRequest;
	ce.serial=0;
	ce.send_event=True;
	ce.border_width=BORDER_WIDTH;//TODO//
	ce.value_mask=CWWidth|CWHeight|CWX|CWY;
	ce.detail=Above;
	ce.above=None;
	ce.x=r.x;
	ce.y=r.y;
	ce.width=r.w;
	ce.height=r.h;

	XSendEvent(this->mainClass->display,this->contentWindow,true,StructureNotifyMask,(XEvent*)&ce);
}
