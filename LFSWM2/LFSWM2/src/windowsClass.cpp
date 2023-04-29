/*
 *
 * ©K. D. Hedger. Fri 29 Jul 13:47:06 BST 2022 keithdhedger@gmail.com

 * This file (windowsClass.cpp) is part of LFSWM2.

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

#include "windowsClass.h"

LFSWM2_windowClass::~LFSWM2_windowClass(void)
{
}

LFSWM2_windowClass::LFSWM2_windowClass(LFSWM2_Class *mainclass)
{
	this->mainClass=mainclass;
}

void LFSWM2_windowClass::init(void)
{
}

void LFSWM2_windowClass::LFSWM2_buildClientList(void)
{
	Window			returned_root;
	Window			returned_parent;
	Window			*toplevelwindows;
	unsigned int		windowscnt;
	std::string		windowname;

	XQueryTree(this->mainClass->display,this->mainClass->rootWindow,&returned_root,&returned_parent,&toplevelwindows,&windowscnt);
	for(unsigned int i=0;i<windowscnt;++i)
		this->LFSWM2_createClient(toplevelwindows[i]);
	if(toplevelwindows!=NULL)
		XFree(toplevelwindows);
}

void LFSWM2_windowClass::LFSWM2_deleteClientEntry(Window id)
{
	if(this->clientList.count(id)!=0)
		{
			this->clientList.erase(id);
			this->LFSWM2_setClientList(id,false);
		}
}

LFSWM2_clientClass *LFSWM2_windowClass::LFSWM2_getClientClass(Window id)
{
	if(this->clientList.count(id)!=0)
		return(this->clientList.at(id));

	return(NULL);
}

void LFSWM2_windowClass::LFSWM2_destroyClient(Window id)
{
	LFSWM2_clientClass	*cc;

	cc=this->LFSWM2_getClientClass(id);
	if(cc!=NULL)
		{
			delete cc;
		}
	this->mainClass->restackCnt++;
}

struct fontColour* LFSWM2_windowClass::LFSWM2_xftLoadColour(const char *name,const char *fallback)
{
	XftDraw				*draw;
	XftColor				color;
	struct fontColour	*c;
	XColor				tc,sc;

	if((draw=XftDrawCreate(this->mainClass->display,this->mainClass->rootWindow,this->mainClass->defaultVisual,this->mainClass->defaultColourmap))==NULL)
		return NULL;

	if(!XftColorAllocName(this->mainClass->display,this->mainClass->defaultVisual,this->mainClass->defaultColourmap,name,&color))
		{
			fprintf(stderr,"Can't alloc colour %s, using fallback %s\n",name,fallback);
			XftColorAllocName(this->mainClass->display,this->mainClass->defaultVisual,this->mainClass->defaultColourmap,fallback,&color);
			XAllocNamedColor(this->mainClass->display,this->mainClass->defaultColourmap,fallback,&sc,&tc);
		}
	else
		XAllocNamedColor(this->mainClass->display,this->mainClass->defaultColourmap,name,&sc,&tc);

	c=new struct fontColour;
	c->draw=draw;
	c->color=color;
	c->visual=this->mainClass->defaultVisual;
	c->colormap=this->mainClass->defaultColourmap;
	c->pixel=sc.pixel;

	return c;
}

void LFSWM2_windowClass::LFSWM2_createClient(Window id)
{
	if(this->LFSWM2_getWindowType(id)==MENUWINDOW)
		{
			this->LFSWM2_setClientList(id,true);
			XRaiseWindow(this->mainClass->display,id);
			this->mainClass->restackCnt=1;
			XSetInputFocus(this->mainClass->display,id,RevertToNone,CurrentTime);
			return;
		}

	if(this->LFSWM2_getWindowType(id)==DOCKWINDOW)
		{
			this->LFSWM2_setClientList(id,true);
			XRaiseWindow(this->mainClass->display,id);
			this->mainClass->restackCnt=1;
			return;
		}

	if(this->LFSWM2_getWindowType(id)==DESKTOPWINDOW)
		{
			this->LFSWM2_setClientList(id,true);
			XLowerWindow(this->mainClass->display,id);
			this->mainClass->restackCnt=1;
			return;
		}
	if(this->clientList.count(id)>0)
		{
			XMapWindow(this->mainClass->display,this->clientList.at(id)->frameWindow);
			XRaiseWindow(this->mainClass->display,this->clientList.at(id)->frameWindow);
		}
	else
		{
			LFSWM2_clientClass	*cc;
			XWindowAttributes	x_window_attrs;
			XSetWindowAttributes	wa;
			long int				*setdesktop=NULL;
			long unsigned int	nitems_return=0;
			motifHints			*hints=NULL;
			Atom					*allowed=NULL;
			hintsDataStruct		hs;
			int					thisdesk=-1;

			hints=(motifHints*)this->mainClass->mainWindowClass->LFSWM2_getProp(id,this->mainClass->atoms.at("_MOTIF_WM_HINTS"),this->mainClass->atoms.at("_MOTIF_WM_HINTS"),&nitems_return);
			setdesktop=(long int*)this->mainClass->mainWindowClass->LFSWM2_getProp(id,this->mainClass->atoms.at("_NET_WM_DESKTOP"),XA_CARDINAL,&nitems_return);
			if(setdesktop!=NULL)
				{
					thisdesk=*setdesktop;
					XFree(setdesktop);
				}

			if(hints!=NULL)
				{
					if(hints->decorations==0)
						{
							this->LFSWM2_setClientList(id,true);
							XRaiseWindow(this->mainClass->display,id);
							this->mainClass->restackCnt=1;
							XFree(hints);
							return;
						}
				}

//char *s;
//asprintf(&s,"xprop -id 0x%x",id);
//system(s);
//this->mainClass->DEBUG_printHintsDataStruct(id);

			XGetWindowAttributes(this->mainClass->display,id,&x_window_attrs);
			if(x_window_attrs.override_redirect||x_window_attrs.map_state!=IsViewable)
				{
					if(hints!=NULL)
						XFree(hints);
					return;
				}

			cc=new LFSWM2_clientClass(this->mainClass,id);
			hs=this->LFSWM2_getWindowHints(id);
			cc->sizeHints=hs.sh;
			XMoveWindow(this->mainClass->display,id,hs.pt.x,hs.pt.y);
			cc->mwmHints=hints;

			if(hints!=NULL)
				{
					cc->canMaximize=(hints->decorations & MWM_DECOR_MAXIMIZE);
					cc->canMinimize=(hints->decorations & MWM_DECOR_MINIMIZE);
				}

			allowed=(Atom*)this->LFSWM2_getProp(id,this->mainClass->atoms.at("_NET_WM_ALLOWED_ACTIONS"),XA_ATOM,&nitems_return);
			if(allowed!=NULL)
				{
					for(long unsigned int j=0;j<nitems_return;j++)
						{
							if((allowed[j]==this->mainClass->atoms.at("_NET_WM_ACTION_MAXIMIZE_HORZ")) || (allowed[j]==this->mainClass->atoms.at("_NET_WM_ACTION_MAXIMIZE_VERT")))
								cc->canMaximize=true;
							if(allowed[j]==this->mainClass->atoms.at("_NET_WM_ACTION_MINIMIZE"))
								cc->canMinimize=true;
						}
					XFree(allowed);
				}


			if(this->LFSWM2_getWindowType(id)==NORMALWINDOW)
				{
					cc->canMaximize=true;
					cc->canMinimize=true;
					cc->canResize=true;
				}

			if((hints!=NULL) && (hints->functions==MWM_FUNC_ALL))
				{
					cc->canMaximize=true;
					cc->canMinimize=true;
					cc->canResize=true;
				}

//			cc->frameWindow=XCreateSimpleWindow(this->mainClass->display,this->mainClass->rootWindow,hs.pt.x,hs.pt.y,x_window_attrs.width+(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize),x_window_attrs.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize+BORDER_WIDTH,BORDER_WIDTH,this->mainClass->frameFG->pixel,this->mainClass->blackColour);

			cc->frameWindow=XCreateWindow(this->mainClass->display,this->mainClass->rootWindow,hs.pt.x,hs.pt.y,x_window_attrs.width+(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize),x_window_attrs.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize+BORDER_WIDTH,BORDER_WIDTH,CopyFromParent,InputOutput,CopyFromParent,0,&wa);

			XSelectInput(this->mainClass->display,cc->frameWindow,SubstructureRedirectMask|ButtonPressMask|ButtonReleaseMask|ExposureMask|PointerMotionMask);

			cc->windowType=this->LFSWM2_getWindowType(id);
			cc->contentWindow=id;
			cc->contentWindowRect=this->LFSWM2_getWindowRect(id,this->mainClass->rootWindow);
			cc->framePreMaxRect=this->LFSWM2_getWindowRect(id,this->mainClass->rootWindow);
			cc->clientPreMaxRect=this->LFSWM2_getWindowRect(id,this->mainClass->rootWindow);
			cc->clientPreFSRect=this->LFSWM2_getWindowRect(id,this->mainClass->rootWindow);
			cc->framePreFSRect=this->LFSWM2_getWindowRect(id,this->mainClass->rootWindow);
			cc->contentWindowRect.y-=this->mainClass->titleBarSize;
			cc->frameWindowRect={hs.pt.x,hs.pt.y,x_window_attrs.width+(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize),x_window_attrs.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize+BORDER_WIDTH};
			cc->resizeWindow=XCreateSimpleWindow(this->mainClass->display,cc->frameWindow,-10,-10,1,1,BORDER_WIDTH,this->mainClass->frameFG->pixel,this->mainClass->frameBG->pixel);

			cc->mask=XCreatePixmap(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w,cc->frameWindowRect.h,1);
			cc->maskGC=XCreateGC(this->mainClass->display,cc->mask,0,NULL);

			this->clientList[id]=cc;
			this->clientList[cc->frameWindow]=cc;

			XSetWindowAttributes attributes;//TODO//
			attributes.win_gravity=NorthWestGravity;
			XReparentWindow(this->mainClass->display,id,cc->frameWindow,this->mainClass->leftSideBarSize,this->mainClass->titleBarSize-(BORDER_WIDTH*2));

			XChangeWindowAttributes(this->mainClass->display,id, CWWinGravity, &attributes);
			XMapWindow(this->mainClass->display,cc->frameWindow);
			XSelectInput(this->mainClass->display,cc->contentWindow,PropertyChangeMask|StructureNotifyMask);
			this->LFSWM2_setWindowState(id,NormalState);

			this->mainClass->mainEventClass->LFSWM2_sendConfigureEvent(id,cc->contentWindowRect);
			XGrabButton(this->mainClass->display,Button1,0,id,False,ButtonPressMask,GrabModeSync,GrabModeAsync,None,None);
			XGrabButton(this->mainClass->display,Button1,Mod4Mask,id,False,ButtonPressMask|ButtonReleaseMask|PointerMotionMask,GrabModeAsync,GrabModeAsync,None,None);

			Atom		v[8];
			int		vcnt=3;
			v[0]=this->mainClass->atoms.at("_NET_WM_ACTION_CHANGE_DESKTOP");
			v[1]=this->mainClass->atoms.at("_NET_WM_ACTION_CLOSE");
			v[2]=this->mainClass->atoms.at("_NET_WM_ACTION_FULLSCREEN");

		if(cc->canMaximize==true)
			{
				v[vcnt++]=this->mainClass->atoms.at("_NET_WM_ACTION_MAXIMIZE_HORZ");
				v[vcnt++]=this->mainClass->atoms.at("_NET_WM_ACTION_MAXIMIZE_VERT");
			}
		if(cc->canMinimize==true)
			{
				v[vcnt++]=this->mainClass->atoms.at("_NET_WM_ACTION_MINIMIZE");
			}

		if(cc->canResize==true)
			{
				v[vcnt++]=this->mainClass->atoms.at("_NET_WM_ACTION_RESIZE");
			}

			if(setdesktop!=NULL)
				cc->onDesk=thisdesk;
			else
				cc->onDesk=this->mainClass->currentDesktop;

			this->LFSWM2_setProp(id,this->mainClass->atoms.at("_NET_WM_ALLOWED_ACTIONS"),XA_ATOM,32,v,vcnt);
			this->LFSWM2_setProp(cc->contentWindow,this->mainClass->atoms.at("_NET_WM_DESKTOP"),XA_CARDINAL,32,(void*)&cc->onDesk,1);

			this->LFSWM2_setClientList(id,true);
			if(cc->canResize==true)
				{
//top left dragger
					wa.win_gravity=NorthWestGravity;
					wa.cursor=this->mainClass->topLeftCursor;

					cc->topLeftDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,0,0,cc->dragsize,cc->dragsize,0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->topLeftDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);

//top righht dragger
					wa.win_gravity=NorthEastGravity;
					wa.cursor=this->mainClass->topRightCursor;

					cc->topRightDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w-cc->dragsize,0,cc->dragsize,cc->dragsize,0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->topRightDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);

//bottom dragger
					wa.win_gravity=SouthWestGravity;
					wa.cursor=this->mainClass->bottomCursor;

					cc->bottomDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,0,cc->frameWindowRect.h-cc->dragsize,cc->frameWindowRect.w,cc->dragsize,0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->bottomDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);

//left side dragger
					wa.win_gravity=NorthWestGravity;
					wa.cursor=this->mainClass->leftCursor;
					cc->leftSideDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,0,cc->dragsize,cc->dragsize,cc->frameWindowRect.h-(cc->dragsize*2),0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->leftSideDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);

//right side dragger
					wa.win_gravity=NorthEastGravity;
					wa.cursor=this->mainClass->rightCursor;
					cc->rightSideDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w-cc->dragsize,cc->dragsize,cc->dragsize,cc->frameWindowRect.h-(cc->dragsize*2),0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->rightSideDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);

//bottom rite dragger
					wa.win_gravity=SouthEastGravity;
					wa.cursor=this->mainClass->bottomRightCursor;

					cc->bottomRightDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w-cc->dragsize,cc->frameWindowRect.h-cc->dragsize,cc->dragsize,cc->dragsize,0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->bottomRightDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);

//bottom left dragger
					wa.win_gravity=SouthWestGravity;
					wa.cursor=this->mainClass->bottomLeftCursor;

					cc->bottomLeftDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,0,cc->frameWindowRect.h-cc->dragsize,cc->dragsize,cc->dragsize,0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->bottomLeftDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);
				}

//controls
			wa.win_gravity=NorthWestGravity;
			wa.save_under=true;
			int	offset;
			int wh;
			int wposy;
//menu
			cc->menuControlStruct.controlName="menu";
			cc->menuControlStruct.startX=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("top-left-active")];

			if(this->mainClass->useTheme==true)
				{
					wh=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("menu-active")];
					int menuh=this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey("menu-active")];
					wposy=(this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey("top-left-active")]/2)-(menuh/2);
					cc->menuButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->menuControlStruct.startX,wposy,wh,menuh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
				}
			else
				{
//					cc->menuControlStruct.startX=cc->frameWindowRect.w-offset-wh;
//					cc->menuButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->menuControlStruct.startX,wposy,wh,wh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
//					cc->menuControlStruct.controlGC=XCreateGC(this->mainClass->display,cc->closeButton,0,NULL);
				}
			XSelectInput(this->mainClass->display,cc->menuButton,ButtonPressMask|EnterWindowMask|LeaveWindowMask);

//rite hand controls	
			wa.win_gravity=NorthEastGravity;
			wh=defaultControlSize;
			wposy=(this->mainClass->titleBarSize/2)-(wh/2);
			
//close
			offset=this->mainClass->riteSideBarSize;
			cc->closeControlStruct.controlName="close";
			cc->closeControlStruct.startX=cc->frameWindowRect.w-offset;

			if(this->mainClass->useTheme==true)
				{
					wh=this->theme.closeButtonSize;
					offset=this->theme.buttonOffset+wh+this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("right-active")];				
					wposy=this->mainClass->buttonYOffset;
					cc->closeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w-offset,wposy,this->mainClass->titleBarSize,wh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
				}
			else
				{
					cc->closeControlStruct.startX=cc->frameWindowRect.w-offset-wh;
					cc->closeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->closeControlStruct.startX,wposy,wh,wh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
					cc->closeControlStruct.controlGC=XCreateGC(this->mainClass->display,cc->closeButton,0,NULL);
				}
			XSelectInput(this->mainClass->display,cc->closeButton,ButtonPressMask|EnterWindowMask|LeaveWindowMask);
			cc->controlCnt=1;
//max
			if(cc->canMaximize==true)
				{
					cc->maximizeControlStruct.controlName="maximize";
					cc->maximizeControlStruct.startX=cc->frameWindowRect.w-offset;

					if(this->mainClass->useTheme==true)
						{
							wh=this->theme.closeButtonSize;
							offset+=this->theme.buttonXSpacing+this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("maximize-active")];		
							wposy=this->mainClass->buttonYOffset;
							cc->maximizeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w-offset,wposy,this->mainClass->titleBarSize,wh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
						}
					else
						{
							offset+=defaultControlSpacing+defaultControlSize;
							cc->maximizeControlStruct.startX=cc->frameWindowRect.w-offset-wh;
							cc->maximizeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->maximizeControlStruct.startX,wposy,wh,wh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
							cc->maximizeControlStruct.controlGC=XCreateGC(this->mainClass->display,cc->maximizeButton,0,NULL);
						}
					XSelectInput(this->mainClass->display,cc->maximizeButton,ButtonPressMask|EnterWindowMask|LeaveWindowMask);
					cc->controlCnt++;
				}
//min
			if(cc->canMinimize==true)
				{
					cc->minimizeControlStruct.controlName="hide";
					cc->minimizeControlStruct.startX=cc->frameWindowRect.w-offset;

					if(this->mainClass->useTheme==true)
						{
							wh=this->theme.closeButtonSize;
							offset+=this->theme.buttonXSpacing+this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("maximize-active")];		
							wposy=this->mainClass->buttonYOffset;
							cc->minimizeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w-offset,wposy,this->mainClass->titleBarSize,wh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
						}
					else
						{
							offset+=defaultControlSpacing+defaultControlSize;
							cc->minimizeControlStruct.startX=cc->frameWindowRect.w-offset-wh;
							cc->minimizeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->minimizeControlStruct.startX,wposy,wh,wh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
							cc->minimizeControlStruct.controlGC=XCreateGC(this->mainClass->display,cc->minimizeButton,0,NULL);
						}
					XSelectInput(this->mainClass->display,cc->minimizeButton,ButtonPressMask|EnterWindowMask|LeaveWindowMask);
					cc->controlCnt++;
				}
//shade
//this->theme.partsWidth["hide-active"]
//this->theme.gotPart[themePartNames[cnt]]=false;
			//if((this->theme.gotPart["shade-active"]==true) || (this->mainClass->useTheme==false))
			if((this->theme.gotPart[this->mainClass->prefs.LFSTK_hashFromKey("shade-active")]==true) || (this->mainClass->useTheme==false))
				{
					cc->shadeControlStruct.controlName="shade";
					cc->shadeControlStruct.startX=cc->frameWindowRect.w-offset;
					cc->canShade=true;

					if(this->mainClass->useTheme==true)
						{
							wh=this->theme.closeButtonSize;
							offset+=this->theme.buttonXSpacing+this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("maximize-active")];		
							wposy=this->mainClass->buttonYOffset;
							cc->shadeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w-offset,wposy,this->mainClass->titleBarSize,wh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
						}
					else
						{
							offset+=defaultControlSpacing+defaultControlSize;
							cc->shadeControlStruct.startX=cc->frameWindowRect.w-offset-wh;
							cc->shadeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->shadeControlStruct.startX,wposy,wh,wh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
							cc->shadeControlStruct.controlGC=XCreateGC(this->mainClass->display,cc->shadeButton,0,NULL);
						}
					XSelectInput(this->mainClass->display,cc->shadeButton,ButtonPressMask|EnterWindowMask|LeaveWindowMask);
					cc->controlCnt++;
				}
			cc->riteButtonsWidth=offset;
			cc->LFSWM2_setWindowName();

			unsigned long	n=0;
			Atom				*states=(Atom*)this->LFSWM2_getFullProp(id,this->mainClass->atoms.at("_NET_WM_STATE"),XA_ATOM,32,&n);
//check no longer maxed
			for(long unsigned j=0;j<n;j++)
				{
					if((states[j]==this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT")) || (states[j]==this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ")))
						cc->isMaximized=true;
					if(states[j]==this->mainClass->atoms.at("_NET_WM_STATE_STICKY"))
						cc->visibleOnAllDesks=true;
				}
			XGetTransientForHint(this->mainClass->display,id,&cc->transientFor);
			if(cc->transientFor!=0)
				{
				//TODO//
				//XMoveWindow(this->mainClass->display,cc->frameWindow,20,20);
				}
			this->LFSWM2_reloadWindowState(id);
			this->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_ACTIVE_WINDOW"),XA_WINDOW,32,(void*)&cc->contentWindow,1);

			XAddToSaveSet(this->mainClass->display,id);
			this->mainClass->restackCnt++;
			if(states!=NULL)
				XFree(states);

			XMapSubwindows(this->mainClass->display,cc->frameWindow);	
			XRaiseWindow(this->mainClass->display,id);

			if(this->mainClass->useTheme==true)
				{
					this->LFSWM2_refreshThemeFrame(cc);
					return;
				}
		}

	XSync(this->mainClass->display,false);
}

rectStruct LFSWM2_windowClass::LFSWM2_getWindowRect(Window id,Window parent,bool dotranslate)
{
	XWindowAttributes	attr;
	rectStruct		r={0,0,0,0};
	Window				wr;

	XGetWindowAttributes(this->mainClass->display,id,&attr);
	if(dotranslate==true)
		XTranslateCoordinates(this->mainClass->display,id,parent,attr.x,attr.y,&r.x,&r.y,&wr);
	r.w=attr.width;
	r.h=attr.height;
	r.x=attr.x;
	r.y=attr.y;

	return(r);
}

int LFSWM2_windowClass::LFSWM2_getWindowType(Window id)
{
	Atom				da;
	int				di;
	int				status;
	unsigned char	*propret=NULL;
	unsigned long	dl;
	int				retval=UNKNOWNTYPE;

	status=XGetWindowProperty(this->mainClass->display,id,this->mainClass->atoms.at("_NET_WM_WINDOW_TYPE"),0L,sizeof (Atom),false,XA_ATOM,&da,&di,&dl,&dl,&propret);
/*
"_NET_WM_WINDOW_TYPE_NORMAL"
"_NET_WM_WINDOW_TYPE_DESKTOP"
"_NET_WM_WINDOW_TYPE_DOCK"
"_NET_WM_WINDOW_TYPE_MENU"
"_NET_WM_WINDOW_TYPE_DIALOG"
"_NET_WM_WINDOW_TYPE_TOOL"
enum {NORMALWINDOW=0,DESKTOPWINDOW,DOCKWINDOW,MENUWINDOW,DIALOGWINDOW,TOOLWINDOW,UNKNOWNTYPE};
*/
	if (status==Success&&propret)
		{
			Atom		atom=((Atom *)propret)[0];
			char		*x=XGetAtomName(this->mainClass->display,atom);//TODO//
			if(x!=NULL)
				{
					if(strcmp(x,"_NET_WM_WINDOW_TYPE_NORMAL")==0)
						retval=NORMALWINDOW;
					if((strcmp(x,"_NET_WM_WINDOW_TYPE_DESKTOP")==0) || (strcmp(x,"_NET_WM_DESKTOP")==0))
						retval=DESKTOPWINDOW;
					if(strcmp(x,"_NET_WM_WINDOW_TYPE_DOCK")==0)
						retval=DOCKWINDOW;
					if(strcmp(x,"_NET_WM_WINDOW_TYPE_MENU")==0)
						retval=MENUWINDOW;
					if(strcmp(x,"_NET_WM_WINDOW_TYPE_DIALOG")==0)
						retval=DIALOGWINDOW;
					if(strcmp(x,"_NET_WM_WINDOW_TYPE_TOOL")==0)
						retval=TOOLWINDOW;

					XFree(x);
				}
			XFree(propret);
		}
	return(retval);
}

char* LFSWM2_windowClass::LFSWM2_decodeTextProp(XTextProperty *p)
{
	char		*s=NULL;
	char		**v=NULL;
	int		n=0;

	XmbTextPropertyToTextList(this->mainClass->display,p,&v,&n);
	if(n>0)
		s=strdup(v[0]);
	if(v!=NULL)
		XFreeStringList(v);

	return s;
}

void *LFSWM2_windowClass::LFSWM2_getFullProp(Window w,Atom prop,Atom type,int fmt,unsigned long *rcountp)
{
	void				*ptr=NULL;
	unsigned long	count=32;
	Atom				rtype;
	int				rfmt;
	unsigned long	rafter;

	while(true)
		{
			if(XGetWindowProperty(this->mainClass->display,w,prop,0L,count,false,type,&rtype,&rfmt,rcountp,&rafter,(unsigned char **)&ptr)!=Success)
				{
					// Error
					return NULL;
				}
			else if(rtype!=type||rfmt!=fmt)
				{
					// Does not exist (type=None),or wrong type/format
					return NULL;
				}
			else if(rafter>0)
				{
					XFree(ptr);
					ptr=NULL;
					count *= 2;
				}
			else
				{
					return ptr;
				}
		}
}

void LFSWM2_windowClass::LFSWM2_setProp(Window w,Atom prop,Atom type,int fmt,void *ptr,int nelem)
{
	XChangeProperty(this->mainClass->display,w,prop,type,fmt,PropModeReplace,(const unsigned char*)ptr,nelem);
}

void* LFSWM2_windowClass::LFSWM2_getProp(Window w,Atom prop,Atom type,long unsigned int *nitems_return)
{
	void				*prop_return=NULL;
	Atom				actual_type_return=0;
	int				actual_format_return=0;
	unsigned long	bytes_after_return=0;
	int				result=-1;

	result=XGetWindowProperty(this->mainClass->display,w,prop,0L,32,false,type,&actual_type_return,&actual_format_return,nitems_return,&bytes_after_return,(unsigned char**)&prop_return);

	if (result!=Success||bytes_after_return>0)
		{
			XFree(prop_return);
			prop_return=NULL;
			return(NULL);
		}
	
	return(prop_return);
}

void LFSWM2_windowClass::LFSWM2_reloadWindowState(Window id)
{
	LFSWM2_clientClass	*cc;
	unsigned long		n=0;
	bool					maxit=false;
	bool					ishidden=false;
	bool					isfull=false;

	cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(id));
	if(cc==NULL)
		{
			cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(id);
			if(cc==NULL)
				return;
		}

	Atom *states=(Atom*)this->LFSWM2_getFullProp(id,this->mainClass->atoms.at("_NET_WM_STATE"),XA_ATOM,32,&n);
	cc->onTop=false;
	cc->onBottom=false;
	cc->visible=true;

	for(long unsigned j=0;j<n;j++)
		{
			if(states[j]==this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT") || states[j]==this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ"))
				maxit=true;
			if(states[j]==this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN"))
				{
					cc->visible=false;
					ishidden=true;
				}
			if(states[j]==this->mainClass->atoms.at("_NET_WM_STATE_BELOW"))
				cc->onBottom=true;
			if(states[j]==this->mainClass->atoms.at("_NET_WM_STATE_ABOVE"))
				cc->onTop=true;
			if(states[j]==this->mainClass->atoms.at("_NET_WM_STATE_FULLSCREEN"))
				isfull=true;
		}

	cc->LFSWM2_maxWindow(maxit);
	cc->LFSWM2_fullscreenWindow(isfull);
	if((ishidden==false) && (cc->onDesk==this->mainClass->currentDesktop) && (cc->visible==true))
		cc->LFSWM2_showWindow();
	else
		cc->LFSWM2_hideWindow();

	this->mainClass->restackCnt++;
	XFree(states);
	return;

//TODO//
#if 0
	for (unsigned int i=0; i<n; i++)
		{
			handled=false;
			if(states[i]==NET_WM_STATE_SKIP_TASKBAR)
				{
					c->skiptaskbar=true;
					handled=true;
				}
			if(states[i]==NET_WM_STATE_FULLSCREEN)
				{
					csetfull(c,true);
					handled=true;
				}
			if(states[i]==NET_WM_STATE_STICKY)
				{
					fprintf(stderr,"here>>>>\n");
					c->followdesk=true;
					handled=true;
				}

#endif
}

bool LFSWM2_windowClass::LFSWM2_hasState(Window w,Atom state)
{
	unsigned long	n=0;
	bool				found=false;
	Atom				*v=(Atom*)this->LFSWM2_getProp(w,this->mainClass->atoms.at("_NET_WM_STATE"),XA_ATOM,&n);

	for(unsigned long i=0;i<n;i++)
		{
			if(v[i]==state)
				{
					found=true;
					break;
				}
		}
	if(v!=NULL)
		XFree(v);
	return found;
}

void LFSWM2_windowClass::LFSWM2_changeState(Window id,int how,Atom state)
{
	LFSWM2_clientClass	*cc;

	cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(id);
	switch(how)
		{
			case NET_WM_STATE_REMOVE:
				this->LFSWM2_removeProp(id,state);
				break;
			case NET_WM_STATE_ADD:
				this->LFSWM2_addState(id,state);
				break;
			case NET_WM_STATE_TOGGLE://TODO//
				if (LFSWM2_hasState(id,state))
					this->LFSWM2_removeProp(id,state);
				else
					this->LFSWM2_addState(id,state);
				break;
		}

	if(cc!=NULL)
		this->LFSWM2_reloadWindowState(cc->contentWindow);
}

void LFSWM2_windowClass::LFSWM2_addState(Window w,Atom state)
{
	unsigned long	n=0;
	Bool				present=False;
	Atom				*old=(Atom*)LFSWM2_getProp(w,this->mainClass->atoms.at("_NET_WM_STATE"),XA_ATOM,&n);

	for(unsigned long i=0;i<n;i++)
		{
			if(old[i]==state)
				{
					present=True;
					break;
				}
		}

	if(!present)
		{
			Atom		*mynew=(Atom*)malloc((n+1)*sizeof(Atom));
			memcpy(mynew,old,n*sizeof(Atom));
			mynew[n]=state;
			this->LFSWM2_setProp(w,this->mainClass->atoms.at("_NET_WM_STATE"),XA_ATOM,32,mynew,n+1);
			free(mynew);
		}

	if(old!=NULL)
		XFree(old);
}

void LFSWM2_windowClass::LFSWM2_removeProp(Window w,Atom state)
{
	unsigned long	n=0;
	Atom				*v=(Atom*)LFSWM2_getProp(w,this->mainClass->atoms.at("_NET_WM_STATE"),XA_ATOM,&n);
	unsigned long	k=0;

	for(unsigned long i=0;i<n;i++)
		{
			if(v[i] != state)
				v[k++]=v[i];
		}

	this->LFSWM2_setProp(w,this->mainClass->atoms.at("_NET_WM_STATE"),XA_ATOM,32,v,k);

	if(v!=NULL)
		XFree(v);
}

Window LFSWM2_windowClass::LFSWM2_getParentWindow(Window id)
{
	Window		root=0;
	Window		parent=0;
	Window		*children=NULL;
    unsigned int	num_children=0;

	this->mainClass->LFSWM2_pushXErrorHandler();
		if(!XQueryTree(this->mainClass->display,id,&root,&parent,&children,&num_children))
			{
				this->mainClass->LFSWM2_popXErrorHandler();
				return(0);
			}

		if(children!=NULL)
			XFree((char*)children);
	this->mainClass->LFSWM2_popXErrorHandler();
	return(parent);
}

void LFSWM2_windowClass::LFSWM2_setClientList(Window id,bool addwindow)
{
	if(addwindow==true)
		{
			this->windowIDList.push_back(id);
		}
	else
		{
			for(long unsigned j=0;j<this->windowIDList.size();j++)
				{
					if(this->windowIDList.at(j)==id)
						{
							this->windowIDList.erase(this->windowIDList.begin()+j);
							break;
						}
				}
		}
}

void LFSWM2_windowClass::LFSWM2_setWindowState(Window w,long state)
{
	long		data[2]={state,None};
	XChangeProperty(this->mainClass->display,w,this->mainClass->atoms.at("WM_STATE"),this->mainClass->atoms.at("WM_STATE"),32,PropModeReplace,(unsigned char *)data,2);
}

void LFSWM2_windowClass::LFSWM2_setVisibilityForDesk(unsigned long desk)
{
	LFSWM2_clientClass	*cc;

	if((int)desk<0)
		return;

	for(int j=this->windowIDList.size()-1;j>=0;j--)
		{
			cc=this->LFSWM2_getClientClass(this->windowIDList.at(j));
			if(cc!=NULL)
				{
					if(cc->visibleOnAllDesks==true)
						{
							cc->onDesk=desk;
							this->LFSWM2_setProp(cc->contentWindow,this->mainClass->atoms.at("_NET_WM_DESKTOP"),XA_CARDINAL,32,&cc->onDesk,1);
						}
					if((cc->onDesk==desk) && (cc->visible==true))
						cc->LFSWM2_showWindow();
					else
						cc->LFSWM2_hideWindow();
				}
		}
}

hintsDataStruct LFSWM2_windowClass::LFSWM2_getWindowHints(Window wid,bool movewindow)
{
	hintsDataStruct	hints;
	long				dummy;

	XGetWindowAttributes(this->mainClass->display,wid,&hints.xa);
	hints.sh=XAllocSizeHints();
	hints.sh->flags=0;
	XGetWMNormalHints(this->mainClass->display,wid,hints.sh,&dummy);

	if(hints.xa.x-this->mainClass->leftSideBarSize<0)
		hints.pt.x=0;
	else
		hints.pt.x=hints.xa.x;

	if(hints.xa.y-this->mainClass->leftSideBarSize-BORDER_WIDTH<0)
		hints.pt.y=0;
	else
		hints.pt.y=hints.xa.y;
	if(this->mainClass->runLevel==RL_STARTUP)
		{
			hints.pt.y-=this->mainClass->titleBarSize-BORDER_WIDTH;
			hints.pt.x-=this->mainClass->leftSideBarSize;
		}
	hints.valid=true;

	if(this->mainClass->runLevel!=RL_STARTUP && ( (hints.sh->flags & (USPosition|PPosition))==0))
		{
			Window			root_return;
			Window			child_return;
			int				root_x_return;
			int				root_y_return;
			int				win_x_return;
			int				win_y_return;
			unsigned int		mask_return;
			unsigned int		inmonitor=0;

			switch(this->mainClass->windowPlacement)
				{
					case NOPLACE:
						break;
					case UNDERMOUSE:
						{
							XQueryPointer(this->mainClass->display,wid,&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return,&mask_return);
							hints.pt.x=root_x_return-(hints.xa.width/2);
							hints.pt.y=root_y_return-(hints.xa.height/2);

							if(hints.pt.x<0)
								hints.pt.x=0;
							if(hints.pt.x+hints.xa.width>this->mainClass->displayWidth)
								hints.pt.x=this->mainClass->displayWidth-hints.xa.width;
							if(hints.pt.y<0)
								hints.pt.y=0;
							if(hints.pt.y+hints.xa.height>this->mainClass->displayHeight)
								hints.pt.y=this->mainClass->displayHeight-hints.xa.height;
						}
						break;

					case CENTREMMONITOR:
						XQueryPointer(this->mainClass->display,wid,&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return,&mask_return);
						hints.pt.x=root_x_return;
						hints.pt.y=root_y_return;
						for(unsigned int j=0;j<this->mainClass->monitors.size();j++)
							{
								if(this->mainClass->lfstkLib->LFSTK_pointInRect(&hints.pt,&(this->mainClass->monitors.at(j)))==true)
									{
										inmonitor=this->mainClass->monitors.at(j).monitor;
										break;
									}
							}
						hints.pt.x=this->mainClass->monitors.at(inmonitor).x+((this->mainClass->monitors.at(inmonitor).w/2)-(hints.xa.width/2));
						hints.pt.y=this->mainClass->monitors.at(inmonitor).y+((this->mainClass->monitors.at(inmonitor).h/2)-(hints.xa.height/2));
						break;

					case CENTRESCREEN:
						hints.pt.x=(this->mainClass->displayWidth/2)-(hints.xa.width/2);
						hints.pt.y=(this->mainClass->displayHeight/2)-(hints.xa.height/2);
						break;
				}
			if(movewindow==true)
				XMoveWindow(this->mainClass->display,wid,hints.pt.x,hints.pt.y);
		}
	return(hints);
}

void LFSWM2_windowClass::LFSWM2_resizeWindow(Window id,int w,int h)
{
	LFSWM2_clientClass *cc;
std::cerr<<"void LFSWM2_windowClass::LFSWM2_resizeWindow(Window id,int w,int h)"<<std::endl;
	cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(id);
	if(cc!=NULL)
		{
			XWindowChanges		changes;
			unsigned int			value_mask=CWWidth|CWHeight;
			changes.width=w;
			changes.height=h;
			XConfigureWindow(this->mainClass->display,id,value_mask,&changes);
			XResizeWindow(this->mainClass->display,cc->frameWindow,w+this->mainClass->riteSideBarSize+this->mainClass->leftSideBarSize,h+this->mainClass->titleBarSize+this->mainClass->bottomBarSize);
		//	XResizeWindow(this->mainClass->display,cc->frameWindow,w+6,h+28);
		}
}

void LFSWM2_windowClass::LFSWM2_moveWindow(Window id,int x,int y)
{
	LFSWM2_clientClass *cc;//TODO///

	//cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(id);
//	if(cc!=NULL)
//		{
//			XWindowChanges		changes;
//			unsigned int			value_mask=CWX|CWY;
//			changes.x=x;
//			changes.y=y;
//			XConfigureWindow(this->mainClass->display,id,value_mask,&changes);
			XMoveWindow(this->mainClass->display,id,x,y);
//		}
}

void LFSWM2_windowClass::LFSWM2_moveResizeWindow(Window id,int x,int y,int w,int h)
{
	LFSWM2_clientClass *cc;
std::cerr<<"void LFSWM2_windowClass::LFSWM2_moveResizeWindow(Window id,int x,int y,int w,int h)"<<std::endl;
	cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(id);
	if(cc!=NULL)
		{
			XWindowChanges		changes;
			unsigned int			value_mask=CWWidth|CWHeight|CWX|CWY;
			changes.width=w;
			changes.height=h;
			changes.x=x;
			changes.y=y;
//			XResizeWindow(this->mainClass->display,id,w+6,h+28);
			//XResizeWindow(this->mainClass->display,id,w+6,h+28);
			XResizeWindow(this->mainClass->display,id,w+this->mainClass->riteSideBarSize+this->mainClass->leftSideBarSize,h+this->mainClass->titleBarSize+this->mainClass->bottomBarSize);

		}
}

/*
struct themeStruct
{
	bool				useTheme;
	char				*pathToTheme;
	bool				gotPart[NUMBEROFPARTS];
	int				titleBarHeight;
	int				leftWidth;
	int				rightWidth;
	int				bottomHeight;
	int				partsWidth[NUMBEROFPARTS];
	int				partsHeight[NUMBEROFPARTS];
	Pixmap			pixmaps[NUMBEROFPARTS];
	Pixmap			masks[NUMBEROFPARTS];
	int				buttonOffset;
	int				buttonXSpacing;
	int				titleOffset;
};
*/

void LFSWM2_windowClass::LFSWM2_refreshFrame(LFSWM2_clientClass *cc,XExposeEvent *e)
{
	rectStruct	r;

	if((e!=NULL) && (e->count!=0))
		return;

	if(cc==NULL)
		return;

	if(this->mainClass->useTheme==true)
		{
			this->LFSWM2_refreshThemeFrame(cc);
			return;
		}

	r=cc->frameWindowRect;

	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
	XSetClipOrigin(this->mainClass->display,this->mainClass->mainGC,0,0);
	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameBG->pixel);

	if(this->mainClass->resizeMode==SCALERESIZE)
			XFillRectangle(this->mainClass->display,cc->frameWindow,this->mainClass->mainGC,0,0,r.w,this->mainClass->titleBarSize);
	else
		XFillRectangle(this->mainClass->display,cc->frameWindow,this->mainClass->mainGC,0,0,r.w,r.h);

	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameText->pixel);
	XftDrawChange(this->mainClass->frameText->draw,cc->frameWindow);

	if(cc->name.length()>0)
		{
			r=cc->setTitlePosition();
			if(cc->nameIsUTF==true)
				XftDrawStringUtf8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)cc->name.c_str(),strlen(cc->name.c_str()));
			else
				XftDrawString8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)cc->name.c_str(),strlen(cc->name.c_str()));
		}

//buttons
	cc->LFSWM2_drawMouseLeave(cc->closeButton,this->mainClass->closeBitMap,cc->closeControlStruct);
	cc->LFSWM2_drawMouseLeave(cc->menuButton,this->mainClass->menuBitMap,cc->menuControlStruct);
	if(cc->canMaximize==true)
		cc->LFSWM2_drawMouseLeave(cc->maximizeButton,this->mainClass->maximizeBitMap,cc->maximizeControlStruct);
	if(cc->canMinimize==true)
		cc->LFSWM2_drawMouseLeave(cc->minimizeButton,this->mainClass->minimizeBitMap,cc->minimizeControlStruct);
	if(cc->canShade==true)
		cc->LFSWM2_drawMouseLeave(cc->shadeButton,this->mainClass->shadeBitMap,cc->shadeControlStruct);
}

void	 LFSWM2_windowClass::LFSWM2_loadTheme(std::string themename)
{
//std::cerr<<"void	 LFSWM2_windowClass::LFSWM2_loadTheme(std::string) "<<themename<<std::endl;
	Imlib_Image	image;
	int			cnt=0;
	int			hite=0;
	std::string filepath;

	imlib_context_set_dither(0);
	imlib_context_set_display(this->mainClass->display);
	imlib_context_set_visual(this->mainClass->defaultVisual);

	while(themePartNames[cnt]!=NULL)
		{
			this->theme.gotPart[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]=false;
			this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]=0;
			this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]=0;
			this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]=0;
			this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]=0;
			this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]=0;
			cnt++;
		}
	this->theme.titleBarHeight=0;
	this->theme.leftWidth=0;
	this->theme.rightWidth=0;
	this->theme.bottomHeight=0;
	this->theme.buttonXSpacing=0;
	this->theme.buttonOffset=0;
	this->theme.titleAlignment=2;
	this->theme.fullWidthTitle=false;
	this->theme.pathToTheme=this->mainClass->prefs.LFSTK_getString("theme");

	cnt=0;
	while(themePartNames[cnt]!=NULL)
		{
			filepath=this->theme.pathToTheme+"/xfwm4/"+themePartNames[cnt]+".png";
			image=imlib_load_image(filepath.c_str());
			if(image==NULL)
				{
					filepath=this->theme.pathToTheme+"/xfwm4/"+themePartNames[cnt]+".xpm";
					image=imlib_load_image(filepath.c_str());
				}

			if(image!=NULL)
				{
					this->theme.gotPart[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]=true;
					imlib_context_set_image(image);

					hite=imlib_image_get_height();
					this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]=imlib_image_get_width();
					if(hite>this->theme.titleBarHeight)
						this->theme.titleBarHeight=hite;
					this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]=hite;

					imlib_context_set_drawable(this->mainClass->rootWindow);
					imlib_image_set_has_alpha(1);
					imlib_render_pixmaps_for_whole_image(&this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])],&this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]);
					imlib_free_image();
				}
			else
				{
					this->theme.gotPart[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]=false;
					this->theme.gotPart[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]=false;
					this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]=0;
					this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]=0;
				}
//			std::cerr<<filepath<<std::endl;
//			std::cerr<<this->theme.gotPart[themePartNames[cnt]]<<std::endl;
//			std::cerr<<this->theme.partsWidth[themePartNames[cnt]]<<std::endl;
//			std::cerr<<this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]<<std::endl;
//			std::cerr<<this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]<<" "<<this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(themePartNames[cnt])]<<std::endl;
			cnt++;
		}
	this->theme.leftWidth=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("left-active")];
	this->theme.rightWidth=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("right-active")];
	this->theme.titleBarHeight=this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey("title-3-active")];
	this->theme.bottomHeight=this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey("bottom-active")];

	this->theme.middleOffset=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("top-left-active")];
	this->theme.middleAdjust=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("top-right-active")]+this->theme.middleOffset;

	this->mainClass->titleBarSize=this->theme.titleBarHeight;
	this->mainClass->bottomBarSize=this->theme.bottomHeight;
	this->mainClass->leftSideBarSize=this->theme.leftWidth;
	this->mainClass->riteSideBarSize=this->theme.rightWidth;

//TODO//
	this->theme.closeButtonSize=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("close-active")];
	this->theme.buttonXSpacing=2;
	this->theme.buttonOffset=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("right-active")];
	this->mainClass->controlSize=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("close-active")];
	this->theme.buttonYOffset=(this->theme.titleBarHeight/2)-(this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey("close-active")]/2);
	
	this->mainClass->buttonYOffset=this->theme.buttonYOffset;
//std::cerr<<this->theme.buttonYOffset<<std::endl;
//std::cerr<<(this->theme.titleBarHeight/2)<<std::endl;
//std::cerr<<(this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey("close-active")]/2)<<std::endl;
	std::string res;

	res=this->mainClass->lfstkLib->LFSTK_grepInFile(this->theme.pathToTheme+"/xfwm4/themerc","title_alignment");
	if(res.compare("title_alignment=centre")==0)
		this->mainClass->titlePosition=2;
	if(res.compare("title_alignment=left")==0)
		this->mainClass->titlePosition=1;
	if(res.compare("title_alignment=right")==0)
		this->mainClass->titlePosition=3;
	this->theme.titleAlignment=this->mainClass->titlePosition;

	res=this->mainClass->lfstkLib->LFSTK_oneLiner(std::string("sed -n 's/button_offset=\\(.*\\)/\\1/p' '%s/xfwm4/themerc'"),this->theme.pathToTheme.c_str());
	this->theme.buttonOffset=atoi(res.c_str());
	this->mainClass->buttonOffset=this->theme.buttonOffset;
	
	res=this->mainClass->lfstkLib->LFSTK_oneLiner(std::string("sed -n 's/button_spacing=\\(.*\\)/\\1/p' '%s/xfwm4/themerc'"),this->theme.pathToTheme.c_str());
	this->theme.buttonXSpacing=atoi(res.c_str());
	this->mainClass->buttonXSpacing=this->theme.buttonXSpacing;

	res=this->mainClass->lfstkLib->LFSTK_oneLiner(std::string("sed -n 's/full_width_title=\\(.*\\)/\\1/p' '%s/xfwm4/themerc'"),this->theme.pathToTheme.c_str());
	if(res.compare("true")==0)
		this->theme.fullWidthTitle=true;
	else
		this->theme.fullWidthTitle=false;
	///std::<<this->theme.fullWidthTitle<<std::endl;
}

#if 0
void LFSWM2_windowClass::LFSWM2_refreshThemeFrame(LFSWM2_clientClass *cc)
{
//std::cerr<<"void LFSWM2_clientClass::LFSWM2_refreshThemeFrame(void)"<<std::endl;
	rectStruct	r;
	bool			isactive=cc->isActive;
	std::string	fullpartname;
	std::string	activepart="-active";

	Pixmap pm;
	GC		pmgc;

	r=cc->frameWindowRect;

	if(isactive==false)
		activepart="-inactive";

	pm=XCreatePixmap(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w,cc->frameWindowRect.h,this->mainClass->depth);
	pmgc=XCreateGC(this->mainClass->display,pm,0,NULL);

	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,0,0);
	XSetForeground(this->mainClass->display,pmgc,this->mainClass->frameFG->pixel);
	XSetFillStyle(this->mainClass->display,pmgc,FillSolid);
	XFillRectangle(this->mainClass->display,pm,pmgc,0,0,cc->frameWindowRect.w,cc->frameWindowRect.h);

//reset window mask
	if(cc->mask==None)
		XFreePixmap(this->mainClass->display,cc->mask);
	cc->mask=XCreatePixmap(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w,cc->frameWindowRect.h,1);
	if(cc->maskGC!=None)
		XFreeGC(this->mainClass->display,cc->maskGC);
	cc->maskGC=XCreateGC(this->mainClass->display,cc->mask,0,NULL);

	XSetFillStyle(this->mainClass->display,cc->maskGC,FillSolid);
	XSetClipOrigin(this->mainClass->display,cc->maskGC,0,0);
	XSetForeground(this->mainClass->display,cc->maskGC,this->mainClass->whiteColour);
	XFillRectangle(this->mainClass->display,cc->mask,cc->maskGC,0,0,cc->frameWindowRect.w,cc->frameWindowRect.h);
	XSetFillStyle(this->mainClass->display,pmgc,FillTiled);

//middle
	fullpartname="title-3"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,0,0);
	XSetTSOrigin(this->mainClass->display,pmgc,0,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,this->theme.middleOffset,0,cc->frameWindowRect.w-this->theme.middleAdjust,this->mainClass->titleBarSize);
	
//left side
	fullpartname="left"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,0,0);
	XSetTSOrigin(this->mainClass->display,pmgc,0,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,0,this->theme.titleBarHeight,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.titleBarHeight-this->theme.bottomHeight);

//rite side
	fullpartname="right"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,0,0);
	XSetTSOrigin(this->mainClass->display,pmgc,cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.titleBarHeight);//TODO//
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.titleBarHeight,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.titleBarHeight-this->theme.bottomHeight);

//bottom
	fullpartname="bottom"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,0,0);
	XSetTSOrigin(this->mainClass->display,pmgc,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("bottom-left-active")],cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("bottom-left-active")],cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("bottom-right-active")]-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("bottom-left-active")],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);

//bottom left
	fullpartname="bottom-left"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XSetClipOrigin(this->mainClass->display,pmgc,0,cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XCopyArea(this->mainClass->display,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],pm,pmgc,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0,cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XCopyArea(this->mainClass->display,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->mask,cc->maskGC,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0,cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);

//botttom rite
	fullpartname="bottom-right"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XSetClipOrigin(this->mainClass->display,pmgc,cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XCopyArea(this->mainClass->display,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],pm,pmgc,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XCopyArea(this->mainClass->display,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->mask,cc->maskGC,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);

//top left
	fullpartname="top-left"+activepart;
	
	XSetClipMask(this->mainClass->display,pmgc,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XSetClipOrigin(this->mainClass->display,pmgc,0,0);
	XCopyArea(this->mainClass->display,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],pm,pmgc,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0,0);
	XCopyArea(this->mainClass->display,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->mask,cc->maskGC,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0,0);

//top rite
	fullpartname="top-right"+activepart;
	
	XSetClipMask(this->mainClass->display,pmgc,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XSetClipOrigin(this->mainClass->display,pmgc,cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0);
	XCopyArea(this->mainClass->display,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],pm,pmgc,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0);
	XCopyArea(this->mainClass->display,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->mask,cc->maskGC,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0);

	XSetClipMask(this->mainClass->display,cc->maskGC,None);
	XSetClipOrigin(this->mainClass->display,cc->maskGC,0,0);
	XSetForeground(this->mainClass->display,cc->maskGC,this->mainClass->whiteColour);
	XFillRectangle(this->mainClass->display,cc->mask,cc->maskGC,this->theme.leftWidth,this->theme.titleBarHeight,cc->frameWindowRect.w-this->theme.leftWidth-this->theme.rightWidth,cc->frameWindowRect.h-this->theme.titleBarHeight-this->theme.bottomHeight);

	XSetForeground(this->mainClass->display,pmgc,this->mainClass->frameText->pixel);
	XftDrawChange(this->mainClass->frameText->draw,pm);

	if(cc->name.length()>0)
		{
			r=cc->setTitlePosition();
			if(cc->nameIsUTF==true)
				XftDrawStringUtf8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)cc->name.c_str(),strlen(cc->name.c_str()));
			else
				XftDrawString8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)cc->name.c_str(),strlen(cc->name.c_str()));
		}


	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
	XSetClipOrigin(this->mainClass->display,this->mainClass->mainGC,0,0);
	XCopyArea(this->mainClass->display,pm,cc->frameWindow,this->mainClass->mainGC,0,0,cc->frameWindowRect.w,cc->frameWindowRect.h,0,0);

	cc->LFSWM2_drawMouseLeave(cc->closeButton,this->mainClass->closeBitMap,cc->closeControlStruct);
	if(cc->canMaximize==true)
		cc->LFSWM2_drawMouseLeave(cc->maximizeButton,this->mainClass->maximizeBitMap,cc->maximizeControlStruct);
	if(cc->canMinimize==true)
		cc->LFSWM2_drawMouseLeave(cc->minimizeButton,this->mainClass->minimizeBitMap,cc->minimizeControlStruct);
	if(cc->canShade==true)
		cc->LFSWM2_drawMouseLeave(cc->shadeButton,this->mainClass->shadeBitMap,cc->shadeControlStruct);

	if(cc->isFullscreen==true)
		{
			XSetFillStyle(this->mainClass->display,cc->maskGC,FillSolid);
			XSetClipOrigin(this->mainClass->display,cc->maskGC,0,0);
			XSetForeground(this->mainClass->display,cc->maskGC,this->mainClass->whiteColour);
			XFillRectangle(this->mainClass->display,cc->mask,cc->maskGC,0,0,cc->frameWindowRect.w,cc->frameWindowRect.h);
		}
	XShapeCombineMask(this->mainClass->display,cc->frameWindow,ShapeBounding,0,0,cc->mask,ShapeSet);

	XFreePixmap(this->mainClass->display,pm);
	XFreeGC(this->mainClass->display,pmgc);
}

#else
void LFSWM2_windowClass::LFSWM2_refreshThemeFrame(LFSWM2_clientClass *cc)
{
//std::cerr<<"void LFSWM2_clientClass::LFSWM2_refreshThemeFrame(void)"<<std::endl;
	rectStruct	r;
	bool			isactive=cc->isActive;
	std::string	fullpartname;
	std::string	activepart="-active";

	int	part1w=0;
	int	part2w=0;
	int	part3w=0;
	int	part4w=0;
	int	part5w=0;

	Pixmap pm;
	GC		pmgc;

	r=cc->frameWindowRect;

	if(isactive==false)
		activepart="-inactive";

	pm=XCreatePixmap(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w,cc->frameWindowRect.h,this->mainClass->depth);
	pmgc=XCreateGC(this->mainClass->display,pm,0,NULL);

	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,0,0);
	XSetForeground(this->mainClass->display,pmgc,this->mainClass->frameFG->pixel);
	XSetFillStyle(this->mainClass->display,pmgc,FillSolid);
	XFillRectangle(this->mainClass->display,pm,pmgc,0,0,cc->frameWindowRect.w,cc->frameWindowRect.h);

//reset window mask
	if(cc->mask==None)
		XFreePixmap(this->mainClass->display,cc->mask);
	cc->mask=XCreatePixmap(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w,cc->frameWindowRect.h,1);
	if(cc->maskGC!=None)
		XFreeGC(this->mainClass->display,cc->maskGC);
	cc->maskGC=XCreateGC(this->mainClass->display,cc->mask,0,NULL);
	XSetFillStyle(this->mainClass->display,cc->maskGC,FillSolid);
	XSetClipOrigin(this->mainClass->display,cc->maskGC,0,0);
	XSetForeground(this->mainClass->display,cc->maskGC,this->mainClass->whiteColour);
	XFillRectangle(this->mainClass->display,cc->mask,cc->maskGC,0,0,cc->frameWindowRect.w,cc->frameWindowRect.h);

	XSetFillStyle(this->mainClass->display,pmgc,FillTiled);
	XSetFillStyle(this->mainClass->display,cc->maskGC,FillTiled);

	rectStruct tr=cc->setTitlePosition();
//this->mainClass->DEBUG_printRect(r,"frame windwow rect");
//this->mainClass->DEBUG_printRect(tr,"part3 rect");

	if(this->theme.fullWidthTitle==false)
		{
			part3w=tr.w;
			part2w=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("title-2-active")];
			part4w=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("title-4-active")];
			part1w=tr.x-part2w;
			part5w=r.w-(tr.x+tr.w+part4w);
		}
	else
		{
			part1w=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("top-left-active")];
			part2w=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("title-2-active")];
			part4w=this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("title-4-active")];
			part3w=r.w-(part1w+part2w+part4w+cc->riteButtonsWidth);
			part5w=r.w-(part3w);
		}

//std::cerr<<"part1="<<part1w<<" part2="<<part2w<<" part3="<<part3w<<" part4="<<part4w<<" part5="<<part5w<<std::endl;
int startx=0;
//left title
	fullpartname="title-1"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XSetClipOrigin(this->mainClass->display,pmgc,0,0);
	XSetTSOrigin(this->mainClass->display,pmgc,0,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,startx,0,part1w,this->mainClass->titleBarSize);

//	XSetFillStyle(this->mainClass->display,cc->maskGC,FillSolid);

	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTSOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,startx,0,part1w,this->mainClass->titleBarSize);
	startx+=part1w;

//left mid title
	fullpartname="title-2"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTSOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,startx,0,part2w,this->mainClass->titleBarSize);

	//XSetFillStyle(this->mainClass->display,cc->maskGC,FillTiled);
	XSetClipOrigin(this->mainClass->display,cc->maskGC,startx,0);
	XSetTSOrigin(this->mainClass->display,cc->maskGC,startx,0);
	XSetTile(this->mainClass->display,cc->maskGC,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,cc->mask,cc->maskGC,startx,0,part2w,this->mainClass->titleBarSize);


//XSetFillStyle(this->mainClass->display,cc->maskGC,FillSolid);
	startx+=part2w;

//middle
	fullpartname="title-3"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTSOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,startx,0,part3w,this->mainClass->titleBarSize);

	//XSetFillStyle(this->mainClass->display,cc->maskGC,FillTiled);
	XSetClipOrigin(this->mainClass->display,cc->maskGC,startx,0);
	XSetTSOrigin(this->mainClass->display,cc->maskGC,startx,0);
	XSetTile(this->mainClass->display,cc->maskGC,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,cc->mask,cc->maskGC,startx,0,part3w,this->mainClass->titleBarSize);
	startx+=part3w;

//right mid title
	fullpartname="title-4"+activepart;
	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTSOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,startx,0,part4w,this->mainClass->titleBarSize);

	//XSetFillStyle(this->mainClass->display,cc->maskGC,FillTiled);
	XSetClipOrigin(this->mainClass->display,cc->maskGC,startx,0);
	XSetTSOrigin(this->mainClass->display,cc->maskGC,startx,0);
	XSetTile(this->mainClass->display,cc->maskGC,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,cc->mask,cc->maskGC,startx,0,part4w,this->mainClass->titleBarSize);
	startx+=part4w;

//right title
	fullpartname="title-5"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTSOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,startx,0,part5w,this->mainClass->titleBarSize);

//	XSetFillStyle(this->mainClass->display,cc->maskGC,FillTiled);
	XSetClipOrigin(this->mainClass->display,cc->maskGC,startx,0);
	XSetTSOrigin(this->mainClass->display,cc->maskGC,startx,0);
	XSetTile(this->mainClass->display,cc->maskGC,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,cc->mask,cc->maskGC,startx,0,part5w,this->mainClass->titleBarSize);
	startx+=part4w;

//left side
	fullpartname="left"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,0,0);
	XSetTSOrigin(this->mainClass->display,pmgc,0,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,0,this->theme.titleBarHeight,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.titleBarHeight-this->theme.bottomHeight);

	//XSetClipMask(this->mainClass->display,cc->maskGC,None);
	XSetClipOrigin(this->mainClass->display,cc->maskGC,0,0);
	XSetTSOrigin(this->mainClass->display,cc->maskGC,0,0);
	XSetTile(this->mainClass->display,cc->maskGC,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,cc->mask,cc->maskGC,0,this->theme.titleBarHeight,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.titleBarHeight-this->theme.bottomHeight);

//rite side
	fullpartname="right"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,0,0);
	XSetTSOrigin(this->mainClass->display,pmgc,cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.titleBarHeight);//TODO//
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.titleBarHeight,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.titleBarHeight-this->theme.bottomHeight);

	XSetClipOrigin(this->mainClass->display,cc->maskGC,0,0);
	XSetTSOrigin(this->mainClass->display,cc->maskGC,cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.titleBarHeight);//TODO//
	XSetTile(this->mainClass->display,cc->maskGC,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,cc->mask,cc->maskGC,cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.titleBarHeight,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.titleBarHeight-this->theme.bottomHeight);

//bottom
	fullpartname="bottom"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,pmgc,0,0);
	XSetTSOrigin(this->mainClass->display,pmgc,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("bottom-left-active")],cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("bottom-right-active")]-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("bottom-left-active")],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);

	//XSetClipMask(this->mainClass->display,pmgc,None);
	XSetClipOrigin(this->mainClass->display,cc->maskGC,0,0);
	XSetTSOrigin(this->mainClass->display,cc->maskGC,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("bottom-left-active")],cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XSetTile(this->mainClass->display,cc->maskGC,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,cc->mask,cc->maskGC,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("bottom-left-active")],cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("bottom-right-active")]-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey("bottom-left-active")],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);

//bottom left
	fullpartname="bottom-left"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XSetClipOrigin(this->mainClass->display,pmgc,0,cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XCopyArea(this->mainClass->display,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],pm,pmgc,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0,cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XCopyArea(this->mainClass->display,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->mask,cc->maskGC,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0,cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);

//botttom rite
	fullpartname="bottom-right"+activepart;

	XSetClipMask(this->mainClass->display,pmgc,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XSetClipOrigin(this->mainClass->display,pmgc,cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XCopyArea(this->mainClass->display,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],pm,pmgc,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XCopyArea(this->mainClass->display,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->mask,cc->maskGC,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);

//top left
	fullpartname="top-left"+activepart;
	
	XSetClipMask(this->mainClass->display,pmgc,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XSetClipOrigin(this->mainClass->display,pmgc,0,0);
	XCopyArea(this->mainClass->display,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],pm,pmgc,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0,0);
	XCopyArea(this->mainClass->display,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->mask,cc->maskGC,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0,0);

//top rite
	fullpartname="top-right"+activepart;
	
	XSetClipMask(this->mainClass->display,pmgc,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)]);
	XSetClipOrigin(this->mainClass->display,pmgc,cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0);
	XCopyArea(this->mainClass->display,this->theme.pixmaps[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],pm,pmgc,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0);
	XCopyArea(this->mainClass->display,this->theme.masks[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->mask,cc->maskGC,0,0,this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[this->mainClass->prefs.LFSTK_hashFromKey(fullpartname)],0);

	XSetFillStyle(this->mainClass->display,cc->maskGC,FillSolid);
	XSetClipMask(this->mainClass->display,cc->maskGC,None);
	XSetClipOrigin(this->mainClass->display,cc->maskGC,0,0);
	XSetForeground(this->mainClass->display,cc->maskGC,this->mainClass->whiteColour);
	XFillRectangle(this->mainClass->display,cc->mask,cc->maskGC,this->theme.leftWidth,this->theme.titleBarHeight,cc->frameWindowRect.w-this->theme.leftWidth-this->theme.rightWidth,cc->frameWindowRect.h-this->theme.titleBarHeight-this->theme.bottomHeight);

	XSetForeground(this->mainClass->display,pmgc,this->mainClass->frameText->pixel);
	XftDrawChange(this->mainClass->frameText->draw,pm);

	if(cc->name.length()>0)
		{
			r=cc->setTitlePosition();
			if(cc->nameIsUTF==true)
				XftDrawStringUtf8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)cc->name.c_str(),strlen(cc->name.c_str()));
			else
				XftDrawString8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)cc->name.c_str(),strlen(cc->name.c_str()));
		}

	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
	XSetClipOrigin(this->mainClass->display,this->mainClass->mainGC,0,0);
	XCopyArea(this->mainClass->display,pm,cc->frameWindow,this->mainClass->mainGC,0,0,cc->frameWindowRect.w,cc->frameWindowRect.h,0,0);

	cc->LFSWM2_drawMouseLeave(cc->closeButton,this->mainClass->closeBitMap,cc->closeControlStruct);
	if(cc->canMaximize==true)
		cc->LFSWM2_drawMouseLeave(cc->maximizeButton,this->mainClass->maximizeBitMap,cc->maximizeControlStruct);
	if(cc->canMinimize==true)
		cc->LFSWM2_drawMouseLeave(cc->minimizeButton,this->mainClass->minimizeBitMap,cc->minimizeControlStruct);
	if(cc->canShade==true)
		cc->LFSWM2_drawMouseLeave(cc->shadeButton,this->mainClass->shadeBitMap,cc->shadeControlStruct);

	cc->LFSWM2_drawMouseLeave(cc->menuButton,this->mainClass->menuBitMap,cc->menuControlStruct);

	if(cc->isFullscreen==true)
		{
			XSetFillStyle(this->mainClass->display,cc->maskGC,FillSolid);
			XSetClipOrigin(this->mainClass->display,cc->maskGC,0,0);
			XSetForeground(this->mainClass->display,cc->maskGC,this->mainClass->whiteColour);
			XFillRectangle(this->mainClass->display,cc->mask,cc->maskGC,0,0,cc->frameWindowRect.w,cc->frameWindowRect.h);
		}

	XShapeCombineMask(this->mainClass->display,cc->frameWindow,ShapeBounding,0,0,cc->mask,ShapeSet);
	
	XFreePixmap(this->mainClass->display,pm);
	XFreeGC(this->mainClass->display,pmgc);
}
#endif