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
	this->mainClass->mainWindowClass->LFSWM2_setClientList(this->contentWindow,false);

	XRemoveFromSaveSet(this->mainClass->display,this->contentWindow);	
	XUnmapSubwindows(this->mainClass->display,this->frameWindow);
	XUnmapWindow(this->mainClass->display,this->frameWindow);
	XDestroySubwindows(this->mainClass->display,this->frameWindow);
	this->mainClass->mainWindowClass->LFSWM2_deleteClientEntry(this->frameWindow);
	this->mainClass->mainWindowClass->LFSWM2_deleteClientEntry(this->contentWindow);
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

void LFSWM2_clientClass::drawMouseEnter(Window id,controlData data,Pixmap pm)
{
	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameFG->pixel);
	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
	XSetLineAttributes(this->mainClass->display, this->mainClass->mainGC, 2, LineSolid, CapNotLast, JoinMiter);
	XDrawRectangle(this->mainClass->display,id,this->mainClass->mainGC,1,1,data.boundingBox.width-2,data.boundingBox.height-2);
}

void LFSWM2_clientClass::drawMouseLeave(Window id,Pixmap pm,controlData data)
{
	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameBG->pixel);
	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
	XFillRectangle(this->mainClass->display,id,this->mainClass->mainGC,0,0,data.boundingBox.width+10,data.boundingBox.height+10);
 
  	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->whiteColour);
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

bool LFSWM2_clientClass::LFSWM2_handleControls(XEvent *e)
{
	bool					retval=false;
	XClientMessageEvent	em;
	Pixmap				pm;
	struct controlData	data;

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
				//fprintf(stderr,"EnterNotify\n");
				this->drawMouseEnter(e->xany.window,data,pm);
				break;
			case LeaveNotify:
				//fprintf(stderr,"LeaveNotify IN\n");
				this->drawMouseLeave(e->xany.window,pm,data);
				//fprintf(stderr,"LeaveNotify OUT\n");
				break;
			case ButtonPress:
				if(e->xbutton.window==this->closeButton)
					{
						this->LFSWM2_sendCloseWindow();
//						em.type=ClientMessage;
//						em.window=this->contentWindow;
//						em.message_type=this->mainClass->atoms.at("WM_PROTOCOLS");
//						em.format=32;
//						em.data.l[0]=this->mainClass->atoms.at("WM_DELETE_WINDOW");;
//						em.data.l[1]=CurrentTime;
//						XSendEvent(this->mainClass->display,this->contentWindow,false,NoEventMask,(XEvent *)&em);
//						XSync(this->mainClass->display,false);
//						this->mainClass->mainWindowClass->LFSWM2_destroyClient(this->frameWindow);
						retval=true;
					}

				if(e->xbutton.window==this->maximizeButton)
					{
						retval=true;//TOFIX
						#if 0
						this->mainClass->mainWindowClass->LFSWM2_changeState(this->contentWindow,NET_WM_STATE_TOGGLE,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ"));
						this->mainClass->mainWindowClass->LFSWM2_changeState(this->contentWindow,NET_WM_STATE_TOGGLE,this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ"));
						#else
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
						#endif
						this->LFSWM2_maxWindow();
					}
					
				if(e->xbutton.window==this->minimizeButton)
					{
						XIconifyWindow(this->mainClass->display,this->contentWindow,this->mainClass->screen);

						//this->LFSWM2_minWindow();
						retval=true;
//						this->isMinimized=true;
//						this->mainClass->mainWindowClass->LFSWM2_addState(this->contentWindow,this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN"));
//						XIconifyWindow(this->mainClass->display,this->contentWindow,this->mainClass->screen);
//						this->LFSWM2_hideWindow();
						//fprintf(stderr,"this->isMinimized==true iconifyc=%x iconifyf=%x\n",this->contentWindow,this->frameWindow);
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
			rectStruct rr=this->mainClass->monitors.at(0);
			rr.width=this->mainClass->monitors.at(0).width-(this->mainClass->sideBarSize+BORDER_WIDTH)*2;
			rr.height=this->mainClass->monitors.at(0).height-(this->mainClass->titleBarSize+this->mainClass->bottomBarSize+(BORDER_WIDTH*2));
			rr.x=this->mainClass->sideBarSize;
			rr.y=this->mainClass->titleBarSize;

			this->framePreMaxRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->frameWindow,this->mainClass->rootWindow,false);
			this->clientPreMaxRect=this->mainClass->mainWindowClass->LFSWM2_getWindowRect(this->contentWindow,this->mainClass->rootWindow,false);
			XMoveResizeWindow(this->mainClass->display,this->frameWindow,this->mainClass->monitors.at(0).x,this->mainClass->monitors.at(0).y,this->mainClass->monitors.at(0).width,this->mainClass->monitors.at(0).height);
			XMoveResizeWindow(this->mainClass->display,this->contentWindow,rr.x,rr.y,rr.width,rr.height);
			this->isMaximized=true;
		}

}

void LFSWM2_clientClass::LFSWM2_showWindow(void)
{
	XMapWindow(this->mainClass->display,this->frameWindow);
}

void LFSWM2_clientClass::LFSWM2_hideWindow(void)
{
	XUnmapWindow(this->mainClass->display,this->frameWindow);
}
