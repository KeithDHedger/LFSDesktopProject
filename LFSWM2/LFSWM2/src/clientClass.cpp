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
		XUnmapSubwindows(this->mainClass->display,this->frameWindow);
		XUnmapWindow(this->mainClass->display,this->frameWindow);
		XDestroySubwindows(this->mainClass->display,this->frameWindow);
	this->mainClass->LFSWM2_popXErrorHandler();
}

void LFSWM2_clientClass::LFSWM2_setWindowName(void)
{
	char					*namex=NULL;
	unsigned long		n=0;
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
	XDrawRectangle(this->mainClass->display,id,this->mainClass->mainGC,1,1,data.boundingBox.width-2,data.boundingBox.height-2);
}

void LFSWM2_clientClass::LFSWM2_drawMouseLeave(Window id,Pixmap pm,controlData data)
{
	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameBG->pixel);
	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
	XFillRectangle(this->mainClass->display,id,this->mainClass->mainGC,0,0,data.boundingBox.width+10,data.boundingBox.height+10);
 
  	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameText->pixel);
  //	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->whiteColour);
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
	XSync(this->mainClass->display,false);
	this->mainClass->mainWindowClass->LFSWM2_destroyClient(this->frameWindow);
}

void LFSWM2_clientClass::adjustContentWindow(void)
{
	XResizeWindow(this->mainClass->display,this->contentWindow,
	this->frameWindowRect.width-(this->mainClass->sideBarSize*2),
	this->frameWindowRect.height-(this->mainClass->bottomBarSize+BORDER_WIDTH)-this->mainClass->titleBarSize);
	this->contentWindowRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->contentWindow,this->mainClass->rootWindow);
}

bool LFSWM2_clientClass::doResizeDraggers(XEvent *e)
{
	switch(e->type)
		{
			case ButtonPress:
				this->buttonDown=true;
				this->sx=e->xmotion.x;
				this->sy=e->xmotion.y;
				steps=0;
				this->setWindowRects(false);
				break;

			case ButtonRelease:
				this->buttonDown=false;
				this->sx=0;
				this->sy=0;
				this->setWindowRects(true);
				this->adjustContentWindow();
				break;
			case MotionNotify:
				if(buttonDown==true)
					{
						XConfigureRequestEvent	ce;
						this->steps++;
						if(this->steps>this->smoothness)
							{
								if(e->xany.window==this->topLeftDragger)
									{
										ce.x=e->xmotion.x_root-this->sx;
										ce.y=e->xmotion.y_root-this->sy;
										ce.width=this->contentWindowRect.width-(e->xmotion.x-this->sx);
										ce.height=this->contentWindowRect.height-(e->xmotion.y-this->sy);
									}
								if(e->xany.window==this->topRightDragger)
									{
										ce.x=this->frameWindowRect.x;
										ce.y=e->xmotion.y_root-this->sy;
										ce.width=this->contentWindowRect.width+(e->xmotion.x-this->sx);
										ce.height=this->contentWindowRect.height-(e->xmotion.y-this->sy);
									}
								if(e->xany.window==this->bottomLeftDragger)
									{
										ce.x=e->xmotion.x_root-this->sx;
										ce.y=this->frameWindowRect.y;
										ce.width=this->contentWindowRect.width-(e->xmotion.x-this->sx);
										ce.height=this->contentWindowRect.height+(e->xmotion.y-this->sy);
									}
								if(e->xany.window==this->bottomRightDragger)
									{
										ce.x=this->frameWindowRect.x;
										ce.y=this->frameWindowRect.y;
										ce.width=this->contentWindowRect.width+(e->xmotion.x-this->sx);
										ce.height=this->contentWindowRect.height+(e->xmotion.y-this->sy);
									}

								if(e->xany.window==this->bottomDragger)
									{
										ce.x=this->frameWindowRect.x;
										ce.y=this->frameWindowRect.y;
										ce.width=this->contentWindowRect.width;
										ce.height=this->contentWindowRect.height+(e->xmotion.y-this->sy);
									}

								if(e->xany.window==this->leftSideDragger)
									{
										ce.x=e->xmotion.x_root-this->sx;
										ce.y=this->frameWindowRect.y;
										ce.width=this->contentWindowRect.width-(e->xmotion.x-this->sx);
										ce.height=this->contentWindowRect.height;
									}
								if(e->xany.window==this->rightSideDragger)
									{
										ce.x=this->frameWindowRect.x;
										ce.y=this->frameWindowRect.y;
										ce.width=this->contentWindowRect.width+(e->xmotion.x-this->sx);
										ce.height=this->contentWindowRect.height;
									}

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

								XSendEvent(this->mainClass->display,this->contentWindow,true,StructureNotifyMask,(XEvent*)&ce);
								this->steps=0;
								break;
							}
					}
				break;
		}
	return(true);
}

bool LFSWM2_clientClass::LFSWM2_handleControls(XEvent *e)
{
	bool					retval=false;
	XClientMessageEvent	em;
	Pixmap				pm;
	struct controlData	data;

	if((e->xany.window==this->topLeftDragger) || (e->xany.window==this->bottomRightDragger) || (e->xany.window==this->topRightDragger) || (e->xany.window==this->bottomLeftDragger)|| (e->xany.window==this->bottomDragger)|| (e->xany.window==this->leftSideDragger)|| (e->xany.window==this->rightSideDragger))
		{
			retval=this->doResizeDraggers(e);
		}

	if((e->xany.window!=this->closeButton) && (e->xany.window!=this->maximizeButton) && (e->xany.window!=this->minimizeButton) && (e->xany.window!=this->shadeButton))
		return(retval);

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

	switch(e->type)
		{
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
						retval=true;//TOFIX
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
						this->LFSWM2_maxWindow();
					}
					
				if(e->xbutton.window==this->minimizeButton)
					{
						XIconifyWindow(this->mainClass->display,this->contentWindow,this->mainClass->screen);

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

void LFSWM2_clientClass::LFSWM2_minWindow(void)
{
	this->isMinimized=true;
	this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN"));
	this->LFSWM2_hideWindow();
}

void LFSWM2_clientClass::LFSWM2_fullscreenWindow(void)
{
	if(this->isFullscreen==true)
		{
			XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->framePreMaxRect.x,this->framePreMaxRect.y,this->framePreMaxRect.width,this->framePreMaxRect.height);
			XMoveResizeWindow(this->mainClass->display,this->contentWindow,this->clientPreMaxRect.x,this->clientPreMaxRect.y,this->clientPreMaxRect.width,this->clientPreMaxRect.height);
			this->isFullscreen=false;
		}
	else
		{		
			pointStruct	fp={this->frameWindowRect.x,this->frameWindowRect.y};
			for(unsigned int j=0;j<this->mainClass->numberOfMonitors;j++)
				{
					geometryStruct	mg={this->mainClass->monitors.at(j).x,this->mainClass->monitors.at(j).y,(unsigned)this->mainClass->monitors.at(j).width,(unsigned)this->mainClass->monitors.at(j).height,0};
					if(this->mainClass->lfstkLib->LFSTK_pointInRect(&fp,&mg))
						{
							this->framePreMaxRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->frameWindow,this->mainClass->rootWindow,false);
							this->clientPreMaxRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->contentWindow,this->mainClass->rootWindow,false);
							XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->mainClass->monitors.at(j).x,this->mainClass->monitors.at(j).y,this->mainClass->monitors.at(j).width,this->mainClass->monitors.at(j).height);
							XSync(this->mainClass->display,false);
							this->setWindowRects(true);
							this->adjustContentWindow();
						}
				}
			this->isFullscreen=true;
		}
	this->setWindowRects();
}

void LFSWM2_clientClass::LFSWM2_maxWindow(void)
{
	if(this->isMaximized==true)
		{
			XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->framePreMaxRect.x,this->framePreMaxRect.y,this->framePreMaxRect.width,this->framePreMaxRect.height);
			XMoveResizeWindow(this->mainClass->display,this->contentWindow,this->clientPreMaxRect.x,this->clientPreMaxRect.y,this->clientPreMaxRect.width,this->clientPreMaxRect.height);
			this->isMaximized=false;
		}
	else
		{
			rectStructure rr=this->mainClass->monitors.at(0);
			rr.width=this->mainClass->monitors.at(0).width-((this->mainClass->sideBarSize+BORDER_WIDTH)*3);//TODO//
			rr.height=this->mainClass->monitors.at(0).height-(this->mainClass->titleBarSize+this->mainClass->bottomBarSize+(BORDER_WIDTH*2));
			rr.x=this->mainClass->sideBarSize;
			rr.y=this->mainClass->titleBarSize;

			this->framePreMaxRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->frameWindow,this->mainClass->rootWindow,false);
			this->clientPreMaxRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->contentWindow,this->mainClass->rootWindow,false);
//TODO// screen size?
			XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->mainClass->monitors.at(0).x,this->mainClass->monitors.at(0).y,this->mainClass->monitors.at(0).width,this->mainClass->monitors.at(0).height);
			XMoveResizeWindow(this->mainClass->display,this->contentWindow,rr.x,rr.y,rr.width,rr.height);
			this->isMaximized=true;
		}
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
	XResizeWindow(this->mainClass->display,this->bottomDragger,this->frameWindowRect.width,this->dragsize);
	XResizeWindow(this->mainClass->display,this->leftSideDragger,this->dragsize,this->frameWindowRect.height-(this->dragsize*2));
	XResizeWindow(this->mainClass->display,this->rightSideDragger,this->dragsize,this->frameWindowRect.height-(this->dragsize*2));
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
			for(int g=0;g<n;g++)
				{
					//this->mainClass->DEBUG_printAtom(states[g]);
					this->mainClass->mainWindowClass->LFSWM2_changeState(e->xproperty.window,NET_WM_STATE_ADD,states[g]);
				}
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atoms.at("_NET_WM_STATE_STICKY")))
		{
			this->visibleOnAllDesks=true;
		}

//	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atoms.at("_NET_WM_STATE_FULLSCREEN")))
//		{
//			//cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(id));
//			//	if(cc==NULL)
////struct geometryStruct
////{
////	int						x,y;
////	unsigned				w,h;
////	unsigned				monitor;
////};
////bool LFSTK_lib::LFSTK_pointInRect(pointStruct *point,geometryStruct *geom)
//
//			fprintf(stderr,"got _NET_WM_STATE_FULLSCREEN TODO\n");
//geometryStruct	 nm={this->frameWindowRect.x,this->frameWindowRect.y,(unsigned)this->frameWindowRect.width,(unsigned)this->frameWindowRect.height,0};
//pointStruct		fp={this->frameWindowRect.x,this->frameWindowRect.y};
//			for(unsigned int j=0;j<this->mainClass->numberOfMonitors;j++)
//				{
//geometryStruct	 fg={this->mainClass->monitors.at(j).x,this->mainClass->monitors.at(j).y,(unsigned)this->mainClass->monitors.at(j).width,(unsigned)this->mainClass->monitors.at(j).height,0};
//				fprintf(stderr,"min num=%i\n",j);
//				if(this->mainClass->lfstkLib->LFSTK_pointInRect(&fp,&fg))
//				{
//						fprintf(stderr,"in monitor\n");
//						this->LFSWM2_fullscreenWindow();
//						break;
//						
//				}
//				//this->monitors
//				//if((this->frameWindowRect.x>this->mainClass->monitorData[j].monX) && (this->frameWindowRect.xn<this->mainClass->monitorData[j].monW+monitorData[j].monX) && (this->frameWindowRect.y>this->mainClass->monitorData[j].monY) && (this->frameWindowRect.y<this->mainClass->monitorData[j].this->mainClass->monH+monitorData[j].monY))
//					//{
//					//}
//				}
//			/*
//			for(unsigned int j=0;j<numberOfMonitors;j++)
//		{
//			if((root_x_return>monitorData[j].monX) && (root_x_return<monitorData[j].monW+monitorData[j].monX) && (root_y_return>monitorData[j].monY) && (root_y_return<monitorData[j].monH+monitorData[j].monY))
//				return(j);
//		}
//
//			*/
//		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atoms.at("_NET_WM_STATE_BELOW")))
		{
			this->onBottom=true;
			XLowerWindow(this->mainClass->display,this->contentWindow);
			XSync(this->mainClass->display,false);
			this->mainClass->needsRestack=true;
		}

	if(this->mainClass->mainWindowClass->LFSWM2_hasState(e->xproperty.window,this->mainClass->atoms.at("_NET_WM_STATE_ABOVE")))
		{
			this->onTop=true;
			XRaiseWindow(this->mainClass->display,this->contentWindow);
			this->mainClass->mainWindowClass->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,32,&this->contentWindow,1);
			XSync(this->mainClass->display,false);
			this->mainClass->needsRestack=true;
		}
}
/*
		Atom		v[8];

			v[0]=this->mainClass->atoms.at("_NET_WM_ACTION_CHANGE_DESKTOP");
			v[1]=this->mainClass->atoms.at("_NET_WM_ACTION_CLOSE");
			v[2]=this->mainClass->atoms.at("_NET_WM_ACTION_FULLSCREEN");
			v[3]=this->mainClass->atoms.at("_NET_WM_ACTION_MAXIMIZE_HORZ");
			v[4]=this->mainClass->atoms.at("_NET_WM_ACTION_MAXIMIZE_VERT");
			v[5]=this->mainClass->atoms.at("_NET_WM_ACTION_MINIMIZE");
			v[6]=this->mainClass->atoms.at("_NET_WM_STATE_ABOVE");
			v[7]=this->mainClass->atoms.at("_NET_WM_STATE_BELOW");

*/
