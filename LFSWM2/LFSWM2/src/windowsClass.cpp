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
	Window				returned_root;
	Window				returned_parent;
	Window				*toplevelwindows;
	unsigned int			windowscnt;
	std::string			windowname;
	hintsDataStruct		hs;
	LFSWM2_clientClass	*cc;

	XQueryTree(this->mainClass->display,this->mainClass->rootWindow,&returned_root,&returned_parent,&toplevelwindows,&windowscnt);
	for(unsigned int i=0;i<windowscnt;++i)
		{
			hs=this->LFSWM2_getWindowHints(toplevelwindows[i]);
			if((hs.mHints!=NULL) && (hs.mHints->decorations==0))
				if(this->LFSWM2_createUnframedWindow(toplevelwindows[i])==true)
					continue;

			if(this->LFSWM2_createClient(toplevelwindows[i],hs)==false)
				this->LFSWM2_freeHints(hs);

			cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(toplevelwindows[i]);
			if(cc!=NULL)
				{
					cc->LFSWM2_setWMState();
				}
		}

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

void LFSWM2_windowClass::LFSWM2_freeHints(hintsDataStruct hs)
{
	if(hs.sh!=NULL)
		XFree(hs.sh);
	if(hs.mHints!=NULL)
		XFree(hs.mHints);
}

bool LFSWM2_windowClass::LFSWM2_createUnframedWindow(Window wid)
{
	int	wtype;

	wtype=this->LFSWM2_getWindowType(wid);
	switch(wtype)//for later
		{
			case DOCKWINDOW:
//				switch(this->mainClass->forceDockStackingOrder)
//					{
//						case FORCEABOVE:
//							this->LFSWM2_removeProp(wid,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_BELOW")));
//							this->LFSWM2_addState(wid,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_ABOVE")));
//							
//							break;
//						case FORCEBELOW:
//							this->LFSWM2_removeProp(wid,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_ABOVE")));
//							this->LFSWM2_addState(wid,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_BELOW")));
//							break;
//						default:
//							break;
//					}
				this->LFSWM2_setClientList(wid,true);
							this->mainClass->mainEventClass->LFSWM2_shuffle(wid);
							this->mainClass->mainEventClass->LFSWM2_restack();
				return(true);
				break;
			case NORMALWINDOW:
			case DIALOGWINDOW:
			case TOOLWINDOW:
			case DESKTOPWINDOW:
			case UNKNOWNTYPE:
				this->LFSWM2_setClientList(wid,true);
				return(true);
				break;
			case UTILITYWINDOW:
				{
					hintsDataStruct hs;
					hs=this->mainClass->mainWindowClass->LFSWM2_getWindowHints(wid);
					XMoveWindow(this->mainClass->display,wid,-1000000,-1000000);
					this->LFSWM2_createClient(wid,hs);
					return(true);
					break;
				}
		}

	return(false);
}

bool LFSWM2_windowClass::LFSWM2_createClient(Window id,hintsDataStruct premaphs)
{
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
			Atom					*allowed=NULL;
			int					thisdesk=-1;
			bool					noborder=false;

			setdesktop=(long int*)this->mainClass->mainWindowClass->LFSWM2_getProp(id,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_DESKTOP")),XA_CARDINAL,&nitems_return);

			if(setdesktop!=NULL)
				{
					thisdesk=*setdesktop;
					XFree(setdesktop);
				}
			else
				{
					thisdesk=this->mainClass->currentDesktop;
				}
			if(thisdesk==-1)
				thisdesk=this->mainClass->currentDesktop;

			if(premaphs.mHints!=NULL)
				{
					if(premaphs.mHints->decorations==0)
						{
							noborder=true;
						}
				}
			XGetWindowAttributes(this->mainClass->display,id,&x_window_attrs);
			if(x_window_attrs.override_redirect||x_window_attrs.map_state!=IsViewable)//TODO//
				{
					return(false);
				}

//			if(this->LFSWM2_getWindowType(id)==UNKNOWNTYPE)
//				{
//					fprintf(stderr,"id=0x%x\n",id);
//					
//				}

			cc=new LFSWM2_clientClass(this->mainClass,id);
			cc->isBorderless=noborder;
			cc->windowHints=premaphs;
			cc->renderFrame(true,premaphs.pt.x,premaphs.pt.y);

			if((this->LFSWM2_getWindowType(id)==NORMALWINDOW) )
				{
					cc->canMaximize=true;
					cc->canMinimize=true;
					cc->canResize=true;
					cc->canClose=true;
				}

			if(premaphs.mHints!=NULL)
				{
					//this->mainClass->DEBUG_printMWMHints(premaphs.mHints);
					cc->canMaximize=false;
					cc->canMinimize=false;
					cc->canClose=false;

					if((premaphs.mHints->flags & MWM_HINTS_FUNCTIONS)==MWM_HINTS_FUNCTIONS)
						{
							cc->canClose=(premaphs.mHints->functions & MWM_FUNC_CLOSE);
							cc->canMaximize=(premaphs.mHints->decorations & MWM_DECOR_MAXIMIZE);
							cc->canMinimize=(premaphs.mHints->decorations & MWM_DECOR_MINIMIZE);
							cc->canResize=(premaphs.mHints->decorations & MWM_DECOR_RESIZEH);
						}
					else if((premaphs.mHints->flags & MWM_HINTS_DECORATIONS)==MWM_HINTS_DECORATIONS)
						{
							if((premaphs.mHints->decorations & MWM_DECOR_ALL)==MWM_DECOR_ALL)
								{
									cc->canMaximize=true;
									cc->canMinimize=true;
									cc->canResize=true;
									cc->canClose=true;
								}
							else
								{
									cc->canClose=(premaphs.mHints->functions & MWM_FUNC_CLOSE);
									cc->canMaximize=(premaphs.mHints->decorations & MWM_DECOR_MAXIMIZE);
									cc->canMinimize=(premaphs.mHints->decorations & MWM_DECOR_MINIMIZE);
									cc->canResize=(premaphs.mHints->decorations & MWM_DECOR_RESIZEH);
								}
						}
				}

			allowed=(Atom*)this->LFSWM2_getProp(id,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_ALLOWED_ACTIONS")),XA_ATOM,&nitems_return);
			if(allowed!=NULL)
				{
					for(long unsigned int j=0;j<nitems_return;j++)
						{
							if((allowed[j]==this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_ACTION_MAXIMIZE_HORZ"))) || (allowed[j]==this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_ACTION_MAXIMIZE_VERT"))))
								cc->canMaximize=true;
							if(allowed[j]==this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_ACTION_MINIMIZE")))
								cc->canMinimize=true;
						}
					XFree(allowed);
				}

			if(cc->isBorderless==true)
				{
					cc->canMaximize=false;
					cc->canMinimize=false;
					cc->canResize=false;
					cc->canClose=false;
				}

bool bh=false;
			if(this->LFSWM2_getWindowType(id)==UTILITYWINDOW)
				{
					cc->canMaximize=false;
					cc->canMinimize=false;
					cc->canResize=false;
					cc->canClose=false;
					cc->isBorderless=false;
					bh=true;
					cc->windowType=UTILITYWINDOW;
				}

			if(this->LFSWM2_getWindowType(id)==UNKNOWNTYPE)
				{
					this->LFSWM2_setProp(id,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE")),XA_ATOM,32,&this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_NORMAL")),1);

					cc->canMaximize=true;
					cc->canMinimize=true;
					cc->canResize=true;
					cc->canClose=true;
					cc->isBorderless=false;
				}

			wa.win_gravity=NorthWestGravity;

			wa.colormap =this->mainClass->defaultColourmap;
			wa.border_pixel=0;

			if(bh==true)
				{
					cc->frameWindow=XCreateWindow(this->mainClass->display,DefaultRootWindow(this->mainClass->display),premaphs.xa.x,premaphs.xa.y,premaphs.xa.width,premaphs.xa.height,BORDER_WIDTH,this->mainClass->depth,InputOutput,this->mainClass->defaultVisual,CWColormap | CWBorderPixel ,&wa);
				}
			else
				{
					if(cc->isBorderless==false)
						cc->frameWindow=XCreateWindow(this->mainClass->display,DefaultRootWindow(this->mainClass->display),-1000000,-1000000,premaphs.xa.width+(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize),premaphs.xa.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize+BORDER_WIDTH,BORDER_WIDTH,this->mainClass->depth,InputOutput,this->mainClass->defaultVisual,CWColormap | CWBorderPixel ,&wa);
				}

//fprintf(stderr,"wid=0x%x\n",cc->frameWindow);
	
			if(cc->isBorderless==false)
				XSelectInput(this->mainClass->display,cc->frameWindow,SubstructureRedirectMask|ButtonPressMask|ButtonReleaseMask|ExposureMask|PointerMotionMask);

			cc->windowType=this->LFSWM2_getWindowType(id);
			cc->contentWindow=id;
			cc->contentWindowRect=this->LFSWM2_getWindowRect(id,this->mainClass->rootWindow);
			cc->framePreMaxRect=this->LFSWM2_getWindowRect(id,this->mainClass->rootWindow);
			cc->clientPreMaxRect=this->LFSWM2_getWindowRect(id,this->mainClass->rootWindow);
			cc->clientPreFSRect=this->LFSWM2_getWindowRect(id,this->mainClass->rootWindow);
			cc->framePreFSRect=this->LFSWM2_getWindowRect(id,this->mainClass->rootWindow);
			cc->contentWindowRect.y-=this->mainClass->titleBarSize;
			if(cc->isBorderless==false)
				cc->frameWindowRect={-1000000,-1000000,premaphs.xa.width+(this->mainClass->leftSideBarSize+this->mainClass->riteSideBarSize),premaphs.xa.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize+BORDER_WIDTH};

			if(cc->windowType!=UTILITYWINDOW)
				cc->frameWindowRect={premaphs.xa.x,premaphs.xa.y,premaphs.xa.width,premaphs.xa.height};

			cc->resizeMode=this->mainClass->resizeMode;
			if(cc->isBorderless==false)
				cc->frameGC=XCreateGC(this->mainClass->display,cc->frameWindow,0,0);
			this->clientList[id]=cc;
			if(cc->isBorderless==false)
				this->clientList[cc->frameWindow]=cc;

			XSetWindowAttributes attributes;//TODO//
			attributes.win_gravity=NorthWestGravity;
			if(cc->isBorderless==false)
				if(cc->windowType!=UTILITYWINDOW)
					XReparentWindow(this->mainClass->display,id,cc->frameWindow,this->mainClass->leftSideBarSize,this->mainClass->titleBarSize-(BORDER_WIDTH*2));
				else
					XReparentWindow(this->mainClass->display,id,cc->frameWindow,0,0);
			XChangeWindowAttributes(this->mainClass->display,id,CWWinGravity,&attributes);

			if(cc->isBorderless==false)
			{
				XMapWindow(this->mainClass->display,cc->frameWindow);
				XSelectInput(this->mainClass->display,cc->contentWindow,PropertyChangeMask|StructureNotifyMask|KeyReleaseMask);
				}
			
			this->LFSWM2_setWindowState(id,NormalState);

			XGrabButton(this->mainClass->display,Button1,0,id,False,ButtonPressMask,GrabModeSync,GrabModeAsync,None,None);
			XGrabButton(this->mainClass->display,Button1,(this->mainClass->modKeys),id,False,ButtonPressMask|ButtonReleaseMask|PointerMotionMask,GrabModeAsync,GrabModeAsync,None,None);
			XGrabButton(this->mainClass->display,Button2,0,id,True,0,GrabModeSync,GrabModeAsync,None,None);
			XGrabKey(this->mainClass->display,XKeysymToKeycode(this->mainClass->display,XK_Escape),(this->mainClass->modKeys),id,False,GrabModeSync,GrabModeAsync);

			Atom		v[8];
			int		vcnt=3;
			v[0]=this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_ACTION_CHANGE_DESKTOP"));
			v[1]=this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_ACTION_CLOSE"));
			v[2]=this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_ACTION_FULLSCREEN"));

			if(cc->canMaximize==true)
				{
					v[vcnt++]=this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_ACTION_MAXIMIZE_HORZ"));
					v[vcnt++]=this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_ACTION_MAXIMIZE_VERT"));
				}
			if(cc->canMinimize==true)
				{
					v[vcnt++]=this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_ACTION_MINIMIZE"));
				}

			if(cc->canResize==true)
				{
					v[vcnt++]=this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_ACTION_RESIZE"));
				}

			if(cc->canClose==true)
				{
					v[vcnt++]=this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_ACTION_CLOSE"));
				}

			cc->onDesk=thisdesk;

			this->LFSWM2_setProp(id,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_ALLOWED_ACTIONS")),XA_ATOM,32,v,vcnt);
			this->LFSWM2_setProp(cc->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_DESKTOP")),XA_CARDINAL,32,(void*)&cc->onDesk,1);

			this->LFSWM2_setClientList(id,true);

			cc->setWindowRects(false);
			if(cc->canResize==true)
				{
					if(this->mainClass->useTheme==true)
						{
							cc->leftSideDragWidth=this->theme.leftWidth;
							cc->riteSideDragWidth=this->theme.rightWidth;
						}
					else
						{
							cc->leftSideDragWidth=this->mainClass->leftSideBarSize;
							cc->riteSideDragWidth=this->mainClass->riteSideBarSize;
						}

//top left dragger
					wa.win_gravity=NorthWestGravity;
					wa.cursor=this->mainClass->topLeftCursor;

					cc->topLeftDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,0,0,cc->dragSpacing,cc->dragSpacing,0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->topLeftDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);

//top righht dragger
					wa.win_gravity=NorthEastGravity;
					wa.cursor=this->mainClass->topRightCursor;

					cc->topRightDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w-cc->dragSpacing,0,cc->dragSpacing,cc->dragSpacing,0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->topRightDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);

//bottom dragger
					wa.win_gravity=SouthWestGravity;
					wa.cursor=this->mainClass->bottomCursor;

					cc->bottomDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,0,cc->frameWindowRect.h-cc->dragSpacing,cc->frameWindowRect.w,cc->dragSpacing,0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->bottomDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);

//left side dragger
					wa.win_gravity=NorthWestGravity;
					wa.cursor=this->mainClass->leftCursor;
					cc->leftSideDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,0,cc->dragSpacing,cc->leftSideDragWidth,cc->frameWindowRect.h-(cc->dragSpacing*2),0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->leftSideDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);

//right side dragger
					wa.win_gravity=NorthEastGravity;
					wa.cursor=this->mainClass->rightCursor;
					cc->rightSideDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w-cc->riteSideDragWidth,cc->dragSpacing,cc->riteSideDragWidth,cc->frameWindowRect.h-(cc->dragSpacing*2),0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->rightSideDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);

//bottom rite dragger

					wa.win_gravity=SouthEastGravity;
					wa.cursor=this->mainClass->bottomRightCursor;
					cc->bottomRightDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w-cc->dragSpacing,cc->frameWindowRect.h-cc->dragSpacing,cc->dragSpacing,cc->dragSpacing,0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->bottomRightDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);

//bottom left dragger
					wa.win_gravity=SouthWestGravity;
					wa.cursor=this->mainClass->bottomLeftCursor;

					cc->bottomLeftDragger=XCreateWindow(this->mainClass->display,cc->frameWindow,0,cc->frameWindowRect.h-cc->dragSpacing,cc->dragSpacing,cc->dragSpacing,0,CopyFromParent,InputOnly,CopyFromParent,CWWinGravity| CWCursor,&wa);
					XSelectInput(this->mainClass->display,cc->bottomLeftDragger,ButtonPressMask|PointerMotionMask|ButtonReleaseMask);
				}

//controls
			if(cc->isBorderless==false)
				{
					wa.win_gravity=NorthWestGravity;
					wa.save_under=true;
//menu
					cc->menuControlStruct.controlName="menu";
					cc->menuButton=XCreateWindow(this->mainClass->display,cc->frameWindow,0,0,1,1,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
					cc->menuControlStruct.controlGC=XCreateGC(this->mainClass->display,cc->menuButton,0,NULL);
					XSelectInput(this->mainClass->display,cc->menuButton,ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);

//rite hand controls	
					wa.win_gravity=NorthEastGravity;
cc->controlCnt=0;
//close
					if(cc->canClose==true)
						{
							cc->closeControlStruct.controlName="close";
							cc->closeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,0,0,1,1,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
							cc->closeControlStruct.controlGC=XCreateGC(this->mainClass->display,cc->closeButton,0,NULL);
							XSelectInput(this->mainClass->display,cc->closeButton,ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
							cc->controlCnt++;
						}
//max
					if(cc->canMaximize==true)
						{
							cc->maximizeControlStruct.controlName="maximize";
							cc->maximizeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,0,0,1,1,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
							cc->maximizeControlStruct.controlGC=XCreateGC(this->mainClass->display,cc->maximizeButton,0,NULL);
							XSelectInput(this->mainClass->display,cc->maximizeButton,ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
							cc->controlCnt++;
						}
//min
					if(cc->canMinimize==true)
						{
							cc->minimizeControlStruct.controlName="hide";
							cc->minimizeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,0,0,1,1,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
							cc->minimizeControlStruct.controlGC=XCreateGC(this->mainClass->display,cc->minimizeButton,0,NULL);
							XSelectInput(this->mainClass->display,cc->minimizeButton,ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
							cc->controlCnt++;
						}
//shade
					if((this->theme.gotPart[LFSTK_UtilityClass::LFSTK_hashFromKey("shade-active")]==true) || (this->mainClass->useTheme==false))
						{
							cc->shadeControlStruct.controlName="shade";
							cc->canShade=true;
							cc->shadeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,10,10,1,1,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
							cc->shadeControlStruct.controlGC=XCreateGC(this->mainClass->display,cc->shadeButton,0,NULL);
							XSelectInput(this->mainClass->display,cc->shadeButton,ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
							cc->controlCnt++;
						}
				}

			cc->LFSWM2_setWindowName();

			unsigned long	n=0;
			Atom				*states=(Atom*)this->LFSWM2_getFullProp(id,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE")),XA_ATOM,32,&n);
//check no longer maxed
			for(long unsigned j=0;j<n;j++)
				{
					if((states[j]==this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_VERT"))) || (states[j]==this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_HORZ"))))
						cc->isMaximized=true;
					if(states[j]==this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_STICKY")))
						cc->visibleOnAllDesks=true;
				}

			XGetTransientForHint(this->mainClass->display,id,&cc->transientFor);
			if(cc->transientFor!=0)
				{
				//TODO//
				//XMoveWindow(this->mainClass->display,cc->frameWindow,20,20);
				}

			XAddToSaveSet(this->mainClass->display,id);
			if(states!=NULL)
				XFree(states);

			this->LFSWM2_setControlRects(cc);
			cc->LFSWM2_setFrameExtents();
		}
	return(true);
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

	status=XGetWindowProperty(this->mainClass->display,id,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE")),0L,sizeof (Atom),false,XA_ATOM,&da,&di,&dl,&dl,&propret);
/*
"_NET_WM_WINDOW_TYPE_NORMAL"
"_NET_WM_WINDOW_TYPE_DESKTOP"
"_NET_WM_WINDOW_TYPE_DOCK"
"_NET_WM_WINDOW_TYPE_MENU"
"_NET_WM_WINDOW_TYPE_DIALOG"
"_NET_WM_WINDOW_TYPE_TOOL"
enum {NORMALWINDOW=0,DESKTOPWINDOW,DOCKWINDOW,MENUWINDOW,DIALOGWINDOW,TOOLWINDOW,NOTIFICATION,UNKNOWNTYPE};
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
					if(strcmp(x,"_NET_WM_WINDOW_TYPE_NOTIFICATION")==0)
						retval=NOTIFICATION;
					if(strcmp(x,"_NET_WM_WINDOW_TYPE_UTILITY")==0)
						retval=UTILITYWINDOW;
						//retval=NORMALWINDOW;

					XFree(x);
				}
			XFree(propret);
		}
//	if(retval==UNKNOWNTYPE)
//		{
//			Window w=None;
//			XGetTransientForHint(this->mainClass->display,id,&w);
//			if(w==None)
//				retval=NORMALWINDOW;
//		}
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

	if(this->LFSWM2_checkValidWindow(w)==false)
		return(NULL);

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

	if(this->LFSWM2_checkValidWindow(id)==false)
		return;

	cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(id));
	if(cc==NULL)
		{
			cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(id);
			if(cc==NULL)
				return;
		}

	Atom *states=(Atom*)this->LFSWM2_getFullProp(id,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE")),XA_ATOM,32,&n);

	cc->onTop=false;
	cc->onBottom=false;
	cc->visible=true;
	cc->isHidden=false;

	for(long unsigned j=0;j<n;j++)
		{
			if(states[j]==this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_VERT"))|| states[j]==this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_MAXIMIZED_HORZ")))
				maxit=true;
			if(states[j]==this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_HIDDEN")))//TODO//about boxes
				{
					cc->visible=false;
					cc->isHidden=true;
					ishidden=true;
				}
			if(states[j]==this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_BELOW")))
				cc->onBottom=true;
			if(states[j]==this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_ABOVE")))
				cc->onTop=true;
			if(states[j]==this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE_FULLSCREEN")))
				isfull=true;
		}

	cc->LFSWM2_maxWindow(maxit);
	cc->LFSWM2_fullscreenWindow(isfull);
	if(cc->rendered==true)
		{
			if((cc->onDesk==this->mainClass->currentDesktop) && (ishidden==false))
				cc->LFSWM2_showWindow(true);
			else
				cc->LFSWM2_hideWindow(false);
		}
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
	Atom				*v=(Atom*)this->LFSWM2_getProp(w,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE")),XA_ATOM,&n);

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
	Atom				*old=(Atom*)LFSWM2_getProp(w,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE")),XA_ATOM,&n);

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
			this->LFSWM2_setProp(w,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE")),XA_ATOM,32,mynew,n+1);
			free(mynew);
		}

	if(old!=NULL)
		XFree(old);
}

void LFSWM2_windowClass::LFSWM2_removeProp(Window w,Atom state)
{
	unsigned long	n=0;
	unsigned long	k=0;
	Atom				*v=(Atom*)LFSWM2_getProp(w,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE")),XA_ATOM,&n);

	if(v==NULL)
		return;

	for(unsigned long i=0;i<n;i++)
		{
			if(v[i] != state)
				v[k++]=v[i];
		}

	this->LFSWM2_setProp(w,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_STATE")),XA_ATOM,32,v,k);

	if(v!=NULL)
		XFree(v);
}

Window LFSWM2_windowClass::LFSWM2_getParentWindow(Window id)
{
	Window			root=0;
	Window			parent=0;
	Window			*children=NULL;
    unsigned int		num_children=0;

	if(!XQueryTree(this->mainClass->display,id,&root,&parent,&children,&num_children))
		{
			return(0);
		}

	if(children!=NULL)
		XFree((char*)children);
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
//for(long unsigned j=0;j<this->windowIDList.size();j++)
//	fprintf(stderr,"this->windowIDList.at(j)=%p\n",this->windowIDList.at(j));
}

void LFSWM2_windowClass::LFSWM2_setWindowState(Window w,long state)
{
	long		data[2]={state,None};
	XChangeProperty(this->mainClass->display,w,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("WM_STATE")),this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("WM_STATE")),32,PropModeReplace,(unsigned char *)data,2);
}

void LFSWM2_windowClass::LFSWM2_setVisibilityForDesk(unsigned long desk)
{
	LFSWM2_clientClass	*cc;
	LFSWM2_clientClass	*cchold;
	LFSWM2_clientClass	*cctrans;
	long unsigned int	n=0;

	if((int)desk<0)
		return;

	for(int j=this->windowIDList.size()-1;j>=0;j--)
		{
			cc=this->LFSWM2_getClientClass(this->windowIDList.at(j));
			if(cc!=NULL)
				{
					cchold=cc;
					if(cc->visibleOnAllDesks==true)
						{
							cc->onDesk=desk;
							this->LFSWM2_setProp(cc->contentWindow,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_DESKTOP")),XA_CARDINAL,32,&cc->onDesk,1);
						}
					if(cc->onDesk==desk)
						cc->LFSWM2_showWindow(true);
					else
						cc->LFSWM2_hideWindow(false);

//transients
					for(int k=0;k<this->mainClass->mainWindowClass->windowIDList.size();k++)
						{			
							Atom *v=(Atom*)this->mainClass->mainWindowClass->LFSWM2_getProp(this->mainClass->mainWindowClass->windowIDList.at(k),this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("WM_TRANSIENT_FOR")),XA_WINDOW,&n);
							if(n>0)
								{
									if(*v==cchold->contentWindow)
										{
											cctrans=this->LFSWM2_getClientClass(this->mainClass->mainWindowClass->windowIDList.at(k));
											cctrans->onDesk=cchold->onDesk;
											if(cctrans->onDesk==desk)
												cctrans->LFSWM2_showWindow(true);
											else
												cctrans->LFSWM2_hideWindow(false);
										}
								}
						}
				}
		}
}

bool LFSWM2_windowClass::LFSWM2_checkValidWindow(Window wid)
{
	bool					dummy=0;
	XWindowAttributes	xa;

HIDEXERRORS
		dummy=XGetWindowAttributes(this->mainClass->display,wid,&xa);
	return(dummy);
}

hintsDataStruct LFSWM2_windowClass::LFSWM2_getWindowHints(Window wid,bool movewindow)
{
	hintsDataStruct		hints;
	long					dummy;
	long unsigned int	nitems_return=0;

	if(this->LFSWM2_checkValidWindow(wid)==false)
		return(hints);

	XGetWindowAttributes(this->mainClass->display,wid,&hints.xa);

	hints.sh=XAllocSizeHints();
	hints.sh->flags=0;
	hints.valid=XGetWMNormalHints(this->mainClass->display,wid,hints.sh,&dummy);

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

	hints.mHints=(motifHints*)this->mainClass->mainWindowClass->LFSWM2_getProp(wid,this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_MOTIF_WM_HINTS")),this->mainClass->atomshashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_MOTIF_WM_HINTS")),&nitems_return);

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

	cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(id);
	if(cc!=NULL)
		{
			if(cc->isBorderless==true)
				return;

			XWindowChanges		changes;
			unsigned int			value_mask=CWWidth|CWHeight;
			changes.width=w;
			changes.height=h;
			XConfigureWindow(this->mainClass->display,id,value_mask,&changes);
			if(cc->windowType!=UTILITYWINDOW)
				XResizeWindow(this->mainClass->display,cc->frameWindow,w+this->mainClass->riteSideBarSize+this->mainClass->leftSideBarSize,h+this->mainClass->titleBarSize+this->mainClass->bottomBarSize);
			else
				XResizeWindow(this->mainClass->display,cc->frameWindow,w,h);
		}
}

void LFSWM2_windowClass::LFSWM2_moveWindow(Window id,int x,int y)
{
	XMoveWindow(this->mainClass->display,id,x,y);
}

void LFSWM2_windowClass::LFSWM2_moveResizeWindow(Window id,int x,int y,int w,int h)
{
	LFSWM2_clientClass *cc;
	cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(id);
	if(cc!=NULL)
		{
			XWindowChanges		changes;
			unsigned int			value_mask=CWWidth|CWHeight|CWX|CWY;
			changes.width=w;
			changes.height=h;
			changes.x=x;
			changes.y=y;
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

	if(cc->windowType==UTILITYWINDOW)
		return;

	if(cc->isBorderless==true)
		return;

	if(this->mainClass->useTheme==true)
		{
			this->LFSWM2_refreshThemeFrame(cc);
		}
	else
		{
			r=cc->frameWindowRect;
			long unsigned	col=(this->mainClass->frameAlpha|(this->mainClass->frameBG->pixel&0xffffff));
 
			XSetClipMask(this->mainClass->display,cc->frameGC,None);
			XSetClipOrigin(this->mainClass->display,cc->frameGC,0,0);
			XSetFillStyle(this->mainClass->display,cc->frameGC,FillSolid);
			XSetForeground(this->mainClass->display,cc->frameGC,col);

//fprintf(stderr,">>>frameAlpha=%x this->mainClass->frameBG->pixel=%x both=%x\n",this->mainClass->frameAlpha,this->mainClass->frameBG->pixel,(this->mainClass->frameAlpha|(this->mainClass->frameBG->pixel)&0xffffff));
			XFillRectangle(this->mainClass->display,cc->frameWindow,cc->frameGC,0,0,r.w,r.h);

			XftDrawChange(this->mainClass->frameText->draw,cc->frameWindow);

			if(cc->name.length()>0)
				{
					r=cc->setTitlePosition();
					if(cc->nameIsUTF==true)
						XftDrawStringUtf8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)cc->name.c_str(),strlen(cc->name.c_str()));
					else
						XftDrawString8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)cc->name.c_str(),strlen(cc->name.c_str()));
				}
		}

//buttons
	if(cc->canClose==true)
		cc->LFSWM2_drawMouseLeave(cc->closeButton,this->mainClass->closeBitMap,cc->closeControlStruct);
	cc->LFSWM2_drawMouseLeave(cc->menuButton,this->mainClass->menuBitMap,cc->menuControlStruct);
	if(cc->canMaximize==true)
		cc->LFSWM2_drawMouseLeave(cc->maximizeButton,this->mainClass->maximizeBitMap,cc->maximizeControlStruct);
	if(cc->canMinimize==true)
		cc->LFSWM2_drawMouseLeave(cc->minimizeButton,this->mainClass->minimizeBitMap,cc->minimizeControlStruct);
	if(cc->canShade==true)
		cc->LFSWM2_drawMouseLeave(cc->shadeButton,this->mainClass->shadeBitMap,cc->shadeControlStruct);
}

void	 LFSWM2_windowClass::LFSWM2_reloadTheme(void)
{
	this->mainClass->prefs.LFSTK_loadVarsFromFile(this->mainClass->prefsPath.c_str());

	this->mainClass->titleBarSize=this->mainClass->prefs.LFSTK_getInt("titlebarsize");
	this->mainClass->freeFontColour(this->mainClass->frameBG);
	this->mainClass->frameBG=this->LFSWM2_xftLoadColour(this->mainClass->prefs.LFSTK_getCString("framebg"),"grey");
	this->mainClass->freeFontColour(this->mainClass->frameFG);
	this->mainClass->frameFG=this->LFSWM2_xftLoadColour(this->mainClass->prefs.LFSTK_getCString("framefg"),"white");
	this->mainClass->freeFontColour(this->mainClass->frameText);
	this->mainClass->frameText=this->LFSWM2_xftLoadColour(this->mainClass->prefs.LFSTK_getCString("textcolour"),"black");
	XftFontClose(this->mainClass->display,this->mainClass->frameFont);
	this->mainClass->frameFont=XftFontOpenName(this->mainClass->display,this->mainClass->screen,this->mainClass->prefs.LFSTK_getCString("titlefont"));
	this->mainClass->titlePosition=this->mainClass->prefs.LFSTK_getInt("titleposition");
	this->mainClass->leftSideBarSize=this->mainClass->prefs.LFSTK_getInt("leftsidebarsize");
	this->mainClass->riteSideBarSize=this->mainClass->prefs.LFSTK_getInt("ritesidebarsize");
	this->mainClass->bottomBarSize=this->mainClass->prefs.LFSTK_getInt("bottombarsize");
	this->mainClass->useTheme=this->mainClass->prefs.LFSTK_getBool("usetheme");
	this->mainClass->resizeMode=this->mainClass->prefs.LFSTK_getInt("resizemode");
	this->mainClass->modKeys=this->mainClass->prefs.LFSTK_getInt("modkeys");
	this->mainClass->frameAlpha=this->mainClass->prefs.LFSTK_getInt("framealpha");
	this->mainClass->frameAlpha=this->mainClass->frameAlpha<<24;

	if(this->mainClass->useTheme==true)
		this->LFSWM2_loadTheme(this->mainClass->prefs.LFSTK_getString("theme"));
}

void	 LFSWM2_windowClass::LFSWM2_loadTheme(std::string themename)
{
	std::string	com;
	float		a=(float)(this->mainClass->frameAlpha>>24)/256.0;
	if(a<0.5)
		a=0.5;
	com="rm -r " + this->mainClass->tmpFolderName + ";mkdir -p " + this->mainClass->tmpFolderName + ";cp -r '" + themename + "'/* " + this->mainClass->tmpFolderName;//*/
	system(com.c_str());//*/

	com=std::string(getenv("HOME")) + "/.config/LFS/themeScript " + this->mainClass->tmpFolderName + " &>/dev/null";
	//fprintf(stderr,"com=%s\n",com.c_str());
	//com="convert " + this->mainClass->tmpFolderName + "/xfwm4/{*.xpm,*.png} -set filename: '%t' -alpha set -background none -channel A -evaluate multiply " + std::to_string(a) + " +channel " + this->mainClass->tmpFolderName +"/xfwm4/%[filename:].png";//*/
	system(com.c_str());

	Imlib_Image	image;
	int			cnt=0;
	int			hite=0;
	std::string filepath;

	imlib_context_set_dither(0);
	imlib_context_set_display(this->mainClass->display);
	imlib_context_set_visual(this->mainClass->defaultVisual);

	while(themePartNames[cnt]!=NULL)
		{
			this->theme.gotPart[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=false;
			this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=0;
			this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=0;
			this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=0;
			this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=0;
			cnt++;
		}

	this->mainClass->titlePosition=2;
	this->theme.titleBarHeight=0;
	this->theme.leftWidth=0;
	this->theme.rightWidth=0;
	this->theme.bottomHeight=0;
	this->theme.buttonXSpacing=0;
	this->theme.buttonOffset=0;
	this->theme.titleAlignment=2;
	this->theme.fullWidthTitle=false;
	//this->theme.pathToTheme=this->mainClass->prefs.LFSTK_getString("theme");
	//this->theme.pathToTheme="/tmp/themeout/";
	this->theme.pathToTheme=this->mainClass->tmpFolderName;

	cnt=0;
	while(themePartNames[cnt]!=NULL)
		{
			filepath=this->theme.pathToTheme+"/xfwm4/"+themePartNames[cnt]+".png";
			//fprintf(stderr,"filepath=%s\n",filepath.c_str());
			image=imlib_load_image(filepath.c_str());
			if(image==NULL)
				{
					filepath=this->theme.pathToTheme+"/xfwm4/"+themePartNames[cnt]+".xpm";
					image=imlib_load_image(filepath.c_str());
				}

			if(image!=NULL)
				{
					this->theme.gotPart[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=true;
					imlib_context_set_image(image);

					hite=imlib_image_get_height();
					this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=imlib_image_get_width();
					if(hite>this->theme.titleBarHeight)
						this->theme.titleBarHeight=hite;
					this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=hite;

					imlib_context_set_drawable(this->mainClass->rootWindow);
					imlib_image_set_has_alpha(1);
#if 0
Pixmap picn;
					//imlib_render_pixmaps_for_whole_image(&picn,&this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]);
					imlib_free_image();
 imlib_image_decache_file	(filepath.c_str())	;
	Imlib_Image	imagef;

		imagef=imlib_load_image(filepath.c_str());
				imlib_context_set_image(imagef);
int h=imlib_image_get_height();
int w=imlib_image_get_width();
imlib_context_set_drawable(this->mainClass->rootWindow);
imlib_image_set_has_alpha(1);

Imlib_Image	img3= imlib_create_image(w,h);
imlib_context_set_image( img3 );
imlib_image_set_has_alpha( 1 );
imlib_context_set_color( 0, 0, 0, 100 );
imlib_image_fill_rectangle( 0, 0,w, h );
fprintf(stderr,"path=%s w=%i h=%i\n",filepath.c_str(),w,h);
imlib_context_set_image( imagef );
//imlib_image_set_has_alpha( 1 );
imlib_image_copy_alpha_to_image( img3, 0, 0 );
Pixmap pic1;
Pixmap mask1;
#endif
//imlib_render_pixmaps_for_whole_image(&this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])],&mask1);
imlib_render_pixmaps_for_whole_image(&this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])],&this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]);
					imlib_free_image();
imlib_image_decache_file	(filepath.c_str())	;
/*



Pixmap pn;
Pixmap pn1;
Pixmap pm;
Pixmap ppic;

Pixmap pic1;
Pixmap mask1;



		//			imlib_render_pixmaps_for_whole_image(&this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])],&this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]);
					//imlib_render_pixmaps_for_whole_image(&pic1,&mask1);
//Pixmap pic2=pic1;
//Pixmap mask2=mask1;
//imlib_free_pixmap_and_mask(pic1);
//
//	Imlib_Image	img3= imlib_create_image(this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])],this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])] );
//imlib_context_set_image( img3 );
//imlib_image_set_has_alpha( 1 );
//imlib_context_set_color( 0, 0, 0, 150 );
//imlib_image_fill_rectangle( 0, 0,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])], this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])] );

imlib_context_set_image( image );
imlib_image_set_has_alpha( 1 );
imlib_context_set_color( 0, 0, 0, 10 );

imlib_image_copy_alpha_to_image( image, 0, 0 );
//Pixmap pn;
//XFreePixmap(this->mainClass->display,this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]);
//XSync(this->mainClass->display,false);

					//imlib_render_pixmaps_for_whole_image(&ppic,&pn1);
					imlib_render_pixmaps_for_whole_image(&this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])],&this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]);
//this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=ppic;
//this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=pn1;

*/

				}
			else
				{
					this->theme.gotPart[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=false;
					this->theme.gotPart[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=false;
					this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=0;
					this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]=0;
				}
//			std::cerr<<filepath<<std::endl;
//			std::cerr<<this->theme.gotPart[themePartNames[cnt]]<<std::endl;
//			std::cerr<<this->theme.partsWidth[themePartNames[cnt]]<<std::endl;
//			std::cerr<<this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]<<std::endl;
//			std::cerr<<this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]<<" "<<this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(themePartNames[cnt])]<<std::endl;
			cnt++;
		}
	this->theme.leftWidth=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("left-active")];
	this->theme.rightWidth=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("right-active")];
	this->theme.titleBarHeight=this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey("title-3-active")];
	this->theme.bottomHeight=this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey("bottom-active")];

	this->theme.middleOffset=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("top-left-active")];
	this->theme.middleAdjust=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("top-right-active")]+this->theme.middleOffset;

	this->mainClass->titleBarSize=this->theme.titleBarHeight;
	this->mainClass->bottomBarSize=this->theme.bottomHeight;
	this->mainClass->leftSideBarSize=this->theme.leftWidth;
	this->mainClass->riteSideBarSize=this->theme.rightWidth;

//TODO//
	this->theme.closeButtonSize=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("close-active")];
	this->theme.buttonXSpacing=2;
	this->theme.buttonOffset=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("right-active")];
	this->mainClass->controlSize=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("close-active")];
	this->theme.buttonYOffset=(this->theme.titleBarHeight/2)-(this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey("close-active")]/2);
	
	this->mainClass->buttonYOffset=this->theme.buttonYOffset;
//std::cerr<<this->theme.buttonYOffset<<std::endl;
//std::cerr<<(this->theme.titleBarHeight/2)<<std::endl;
//std::cerr<<(this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey("close-active")]/2)<<std::endl;
	std::string res;

	res=this->mainClass->lfstkLib->LFSTK_grepInFile(this->theme.pathToTheme+"/xfwm4/themerc","title_alignment");
	if(res.compare("title_alignment=centre")==0)
		this->mainClass->titlePosition=2;
	if(res.compare("title_alignment=left")==0)
		this->mainClass->titlePosition=1;
	if(res.compare("title_alignment=right")==0)
		this->mainClass->titlePosition=3;
	this->theme.titleAlignment=this->mainClass->titlePosition;

	res=this->mainClass->lfstkLib->LFSTK_oneLiner("sed -n 's/button_offset=\\(.*\\)/\\1/p' '%S/xfwm4/themerc'",this->theme.pathToTheme);
	this->theme.buttonOffset=atoi(res.c_str());
	this->mainClass->buttonOffset=this->theme.buttonOffset;
	
	res=this->mainClass->lfstkLib->LFSTK_oneLiner("sed -n 's/button_spacing=\\(.*\\)/\\1/p' '%S/xfwm4/themerc'",this->theme.pathToTheme);
	this->theme.buttonXSpacing=atoi(res.c_str());
	this->mainClass->buttonXSpacing=this->theme.buttonXSpacing;

	res=this->mainClass->lfstkLib->LFSTK_oneLiner("sed -n 's/full_width_title=\\(.*\\)/\\1/p' '%S/xfwm4/themerc'",this->theme.pathToTheme);
	if(res.compare("true")==0)
		this->theme.fullWidthTitle=true;
	else
		this->theme.fullWidthTitle=false;
}

void LFSWM2_windowClass::LFSWM2_refreshThemeFrame(LFSWM2_clientClass *cc)
{
	rectStruct	r;
	bool			isactive=cc->isActive;
	std::string	fullpartname;
	std::string	activepart="-active";

	int			part1w=0;
	int			part2w=0;
	int			part3w=0;
	int			part4w=0;
	int			part5w=0;
	int			startx=0;
	int			rightpad=0;

	Pixmap		pm;
	GC			pmgc;
	GC			maskgc;
	Pixmap		mask;
	rectStruct	tr;

	if(cc->isBorderless==true)
		return;

	if(cc->windowType==UTILITYWINDOW)
		return;
	
	r=cc->frameWindowRect;

	if(isactive==false)
		activepart="-inactive";

	pm=XCreatePixmap(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w,cc->frameWindowRect.h,this->mainClass->depth);
	pmgc=XCreateGC(this->mainClass->display,pm,0,NULL);

	XSetClipOrigin(this->mainClass->display,pmgc,0,0);
	XSetFillStyle(this->mainClass->display,pmgc,FillSolid);
	XSetForeground(this->mainClass->display,pmgc,0);
	XFillRectangle(this->mainClass->display,pm,pmgc,0,0,cc->frameWindowRect.w,cc->frameWindowRect.h);

//reset window mask
	mask=XCreatePixmap(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.w,cc->frameWindowRect.h,1);
	maskgc=XCreateGC(this->mainClass->display,mask,0,NULL);

	XSetFillStyle(this->mainClass->display,maskgc,FillSolid);
	XSetClipOrigin(this->mainClass->display,maskgc,0,0);
	XSetForeground(this->mainClass->display,maskgc,0);
	XFillRectangle(this->mainClass->display,mask,maskgc,0,0,cc->frameWindowRect.w,cc->frameWindowRect.h);

//set for tiling
	XSetClipMask(this->mainClass->display,pmgc,None);
	XSetFillStyle(this->mainClass->display,pmgc,FillTiled);

	XSetClipMask(this->mainClass->display,maskgc,None);
	XSetFillStyle(this->mainClass->display,maskgc,FillTiled);

//left side
	fullpartname="left"+activepart;
	XSetTSOrigin(this->mainClass->display,pmgc,0,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,0,this->theme.titleBarHeight,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.titleBarHeight-this->theme.bottomHeight);

	XSetTSOrigin(this->mainClass->display,maskgc,0,0);
	XSetTile(this->mainClass->display,maskgc,this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,mask,maskgc,0,this->theme.titleBarHeight,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.titleBarHeight-this->theme.bottomHeight);

//rite side
	fullpartname="right"+activepart;
	XSetTSOrigin(this->mainClass->display,pmgc,cc->frameWindowRect.w-this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],this->theme.titleBarHeight);//TODO//
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,cc->frameWindowRect.w-this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],this->theme.titleBarHeight,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.titleBarHeight-this->theme.bottomHeight);

	XSetTSOrigin(this->mainClass->display,maskgc,cc->frameWindowRect.w-this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],this->theme.titleBarHeight);//TODO//
	XSetTile(this->mainClass->display,maskgc,this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,mask,maskgc,cc->frameWindowRect.w-this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],this->theme.titleBarHeight,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.titleBarHeight-this->theme.bottomHeight);

//bottom
	fullpartname="bottom"+activepart;
	XSetTSOrigin(this->mainClass->display,pmgc,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("bottom-left-active")],cc->frameWindowRect.h-this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("bottom-right-active")]-this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("bottom-left-active")],this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);

	XSetTSOrigin(this->mainClass->display,maskgc,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("bottom-left-active")],cc->frameWindowRect.h-this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XSetTile(this->mainClass->display,maskgc,this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,mask,maskgc,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("bottom-left-active")],cc->frameWindowRect.h-this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("bottom-right-active")]-this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("bottom-left-active")],this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);

//bottom left
	fullpartname="bottom-left"+activepart;
	XCopyArea(this->mainClass->display,this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],pm,pmgc,0,0,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],0,cc->frameWindowRect.h-this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);

	XCopyArea(this->mainClass->display,this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],mask,maskgc,0,0,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],0,cc->frameWindowRect.h-this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);

//botttom rite
	fullpartname="bottom-right"+activepart;
	XSetClipOrigin(this->mainClass->display,pmgc,cc->frameWindowRect.w-this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XCopyArea(this->mainClass->display,this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],pm,pmgc,0,0,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);

	XCopyArea(this->mainClass->display,this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],mask,maskgc,0,0,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.h-this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);

//titlebar
	tr=cc->setTitlePosition();

	startx=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("top-left"+activepart)];
	if(this->theme.fullWidthTitle==false)
		{
			part3w=tr.w;
			part2w=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("title-2-active")];
			part4w=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("title-4-active")];
			part1w=tr.x-part2w;
			part5w=r.w-(tr.x+tr.w+part4w);
		}
	else
		{
			part1w=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("title-1-active")]+this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("top-left-active")]+this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("top-left-active")];
			part2w=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("title-2-active")];
			part4w=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("title-4-active")];
			part3w=r.w-(part1w+part2w+part4w+cc->riteButtonsWidth+19);
			part5w=r.w-(part3w);
		}

//left title
	fullpartname="title-1"+activepart;
	XSetTSOrigin(this->mainClass->display,pmgc,0,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,startx,0,part1w,this->mainClass->titleBarSize);

	XSetTSOrigin(this->mainClass->display,maskgc,startx,0);
	XSetTile(this->mainClass->display,maskgc,this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,mask,maskgc,startx,0,part1w,this->mainClass->titleBarSize);
	startx+=part1w;

//left mid title
	fullpartname="title-2"+activepart;
	XSetTSOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,startx,0,part2w,this->mainClass->titleBarSize);

	XSetTSOrigin(this->mainClass->display,maskgc,startx,0);
	XSetTile(this->mainClass->display,maskgc,this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,mask,maskgc,startx,0,part2w,this->mainClass->titleBarSize);
	startx+=part2w;

//middle
	fullpartname="title-3"+activepart;
	XSetTSOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,startx,0,part3w,this->mainClass->titleBarSize);

	XSetTSOrigin(this->mainClass->display,maskgc,startx,0);
	XSetTile(this->mainClass->display,maskgc,this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,mask,maskgc,startx,0,part3w,this->mainClass->titleBarSize);
	startx+=part3w;

//right mid title
	fullpartname="title-4"+activepart;
	XSetTSOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,startx,0,part4w,this->mainClass->titleBarSize);

	XSetTSOrigin(this->mainClass->display,maskgc,startx,0);
	XSetTile(this->mainClass->display,maskgc,this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,mask,maskgc,startx,0,part4w,this->mainClass->titleBarSize);
	startx+=part4w;

//right title
	fullpartname="title-5"+activepart;
	XSetTSOrigin(this->mainClass->display,pmgc,startx,0);
	XSetTile(this->mainClass->display,pmgc,this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,pm,pmgc,startx,0,part5w,this->mainClass->titleBarSize);

	XSetTSOrigin(this->mainClass->display,maskgc,startx,0);
	XSetTile(this->mainClass->display,maskgc,this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)]);
	XFillRectangle(this->mainClass->display,mask,maskgc,startx,0,part5w,this->mainClass->titleBarSize);
	startx+=part4w;

//top left
	fullpartname="top-left"+activepart;
	XCopyArea(this->mainClass->display,this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],pm,pmgc,0,0,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],0,0);

	XCopyArea(this->mainClass->display,this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],mask,maskgc,0,0,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],0,0);
	
//top rite
	fullpartname="top-right"+activepart;
	XCopyArea(this->mainClass->display,this->theme.pixmaps[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],pm,pmgc,0,0,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],0);

	XCopyArea(this->mainClass->display,this->theme.masks[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],mask,maskgc,0,0,this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],cc->frameWindowRect.w-this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(fullpartname)],0);

	XSetForeground(this->mainClass->display,pmgc,this->mainClass->frameText->pixel);
	XftDrawChange(this->mainClass->frameText->draw,pm);

	if(cc->name.length()>0)
		{
			r=cc->setTitlePosition();
			if(this->theme.gotPart[LFSTK_UtilityClass::LFSTK_hashFromKey("title-2"+activepart)]==true)
				r.x=r.x+(this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("title-2"+activepart)]/2);
			if(cc->nameIsUTF==true)
				XftDrawStringUtf8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)cc->name.c_str(),strlen(cc->name.c_str()));
			else
				XftDrawString8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,r.x,r.y,(XftChar8*)cc->name.c_str(),strlen(cc->name.c_str()));
		}

//copy to frame window
	XSetClipMask(this->mainClass->display,cc->frameGC,None);
	XSetClipOrigin(this->mainClass->display,cc->frameGC,0,0);

	XCopyArea(this->mainClass->display,pm,cc->frameWindow,cc->frameGC,0,0,cc->frameWindowRect.w,cc->frameWindowRect.h,0,0);

	XSetFillStyle(this->mainClass->display,maskgc,FillSolid);
	XSetForeground(this->mainClass->display,maskgc,this->mainClass->whiteColour);
	XFillRectangle(this->mainClass->display,mask,maskgc,this->theme.leftWidth,this->theme.titleBarHeight,cc->frameWindowRect.w-this->theme.leftWidth-this->theme.rightWidth,cc->frameWindowRect.h-this->theme.titleBarHeight-this->theme.bottomHeight);

 	XShapeCombineMask(this->mainClass->display,cc->frameWindow,ShapeBounding,0,0,mask,ShapeSet);

	XFreePixmap(this->mainClass->display,pm);
	XFreeGC(this->mainClass->display,pmgc);
	XFreePixmap(this->mainClass->display,mask);
	XFreeGC(this->mainClass->display,maskgc);
}

void LFSWM2_windowClass::LFSWM2_setControlRects(LFSWM2_clientClass *cc)
{
//controls
	int	offset=0;//-DEFAULTCONTROLSIZE;
	int wh=DEFAULTCONTROLSIZE;
//menu
	if(cc->isBorderless==true)
		return;
if(cc->windowType==UTILITYWINDOW)
	return;

	if(this->mainClass->useTheme==true)
		{
			cc->menuControlStruct.controlRect=
				{
					this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("top-left-active")],
					this->mainClass->buttonYOffset,
					this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("menu-active")],
					this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey("menu-active")]
				};
			offset=this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("top-right-active")];
	}
	else
		{
			XShapeCombineMask(this->mainClass->display,cc->menuButton,ShapeBounding,0,0,None,ShapeSet);
			cc->menuControlStruct.controlRect=
				{
					this->mainClass->leftSideBarSize,
					this->mainClass->buttonYOffset,
					wh,
					wh
				};
			offset=-DEFAULTCONTROLSIZE+this->mainClass->riteSideBarSize;
		}
	XMoveResizeWindow(this->mainClass->display,cc->menuButton,cc->menuControlStruct.controlRect.x,cc->menuControlStruct.controlRect.y,cc->menuControlStruct.controlRect.w,cc->menuControlStruct.controlRect.h);
	if(cc->isFullscreen==false)
		XMapWindow(this->mainClass->display,cc->menuButton);
	else
		XUnmapWindow(this->mainClass->display,cc->menuButton);

//close
	if(cc->canClose==true)
		{
	if(this->mainClass->useTheme==true)
		{
			offset=this->theme.buttonOffset+this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("close-active")]+this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("right-active")];
			cc->closeControlStruct.controlRect=
				{
					cc->frameWindowRect.w-offset,
					this->mainClass->buttonYOffset,
					this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("close-active")],
					this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey("close-active")]
				};
		}
	else
		{
			XShapeCombineMask(this->mainClass->display,cc->closeButton,ShapeBounding,0,0,None,ShapeSet);
			offset=this->mainClass->riteSideBarSize;
			cc->closeControlStruct.controlRect=
				{
					cc->frameWindowRect.w-this->mainClass->riteSideBarSize-wh,
					this->mainClass->buttonYOffset,
					wh,
					wh
				};
		}
	XMoveResizeWindow(this->mainClass->display,cc->closeButton,cc->closeControlStruct.controlRect.x,cc->closeControlStruct.controlRect.y,cc->closeControlStruct.controlRect.w,cc->closeControlStruct.controlRect.h);
	if(cc->isFullscreen==false)
		XMapWindow(this->mainClass->display,cc->closeButton);
	else
		XUnmapWindow(this->mainClass->display,cc->closeButton);
}

//max
	if(cc->canMaximize==true)
		{
			if(this->mainClass->useTheme==true)
				{
					offset+=this->theme.buttonXSpacing+this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("maximize-active")];
					cc->maximizeControlStruct.controlRect=
						{
							cc->frameWindowRect.w-offset,
							this->mainClass->buttonYOffset,
							this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("maximize-active")],
							this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey("maximize-active")]
						};
				}
			else
				{
					XShapeCombineMask(this->mainClass->display,cc->maximizeButton,ShapeBounding,0,0,None,ShapeSet);
					offset+=DEFAULTCONTROLSPACING+DEFAULTCONTROLSIZE;
					cc->maximizeControlStruct.controlRect=
						{
							cc->frameWindowRect.w-offset-wh,
							this->mainClass->buttonYOffset,
							wh,
							wh
						};
				}
			XMoveResizeWindow(this->mainClass->display,cc->maximizeButton,cc->maximizeControlStruct.controlRect.x,cc->maximizeControlStruct.controlRect.y,cc->maximizeControlStruct.controlRect.w,cc->maximizeControlStruct.controlRect.h);
			if(cc->isFullscreen==false)
				XMapWindow(this->mainClass->display,cc->maximizeButton);
			else
				XUnmapWindow(this->mainClass->display,cc->maximizeButton);
		}

//min
	if(cc->canMinimize==true)
		{
			if(this->mainClass->useTheme==true)
				{
					offset+=this->theme.buttonXSpacing+this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("hide-active")];
					cc->minimizeControlStruct.controlRect=
						{
							cc->frameWindowRect.w-offset,
							this->mainClass->buttonYOffset,
							this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("hide-active")],
							this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey("hide-active")]
						};
				}
			else
				{
					XShapeCombineMask(this->mainClass->display,cc->minimizeButton,ShapeBounding,0,0,None,ShapeSet);
					offset+=DEFAULTCONTROLSPACING+DEFAULTCONTROLSIZE;
					cc->minimizeControlStruct.controlRect=
						{
							cc->frameWindowRect.w-offset-wh,
							this->mainClass->buttonYOffset,
							wh,
							wh
							};
						}
			XMoveResizeWindow(this->mainClass->display,cc->minimizeButton,cc->minimizeControlStruct.controlRect.x,cc->minimizeControlStruct.controlRect.y,cc->minimizeControlStruct.controlRect.w,cc->minimizeControlStruct.controlRect.h);
			if(cc->isFullscreen==false)
				XMapWindow(this->mainClass->display,cc->minimizeButton);
			else
				XUnmapWindow(this->mainClass->display,cc->minimizeButton);
		}

//shade
	cc->canShade=false;
	if((this->theme.gotPart[LFSTK_UtilityClass::LFSTK_hashFromKey("shade-active")]==true) && (this->mainClass->useTheme==true))
		{
			cc->canShade=true;
			offset+=this->theme.buttonXSpacing+this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("hide-active")];
			cc->shadeControlStruct.controlRect=
				{
					cc->frameWindowRect.w-offset,
					this->mainClass->buttonYOffset,
					this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey("shade-active")],
					this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey("shade-active")]
				};
			XMoveResizeWindow(this->mainClass->display,cc->shadeButton,cc->shadeControlStruct.controlRect.x,cc->shadeControlStruct.controlRect.y,cc->shadeControlStruct.controlRect.w,cc->shadeControlStruct.controlRect.h);
			if(cc->isFullscreen==false)
				XMapWindow(this->mainClass->display,cc->shadeButton);
			else
				XUnmapWindow(this->mainClass->display,cc->shadeButton);
		}
	else
		{
			cc->canShade=true;
			XShapeCombineMask(this->mainClass->display,cc->shadeButton,ShapeBounding,0,0,None,ShapeSet);
			offset+=DEFAULTCONTROLSPACING+DEFAULTCONTROLSIZE;
			cc->shadeControlStruct.controlRect=
				{
					cc->frameWindowRect.w-offset-wh,
					this->mainClass->buttonYOffset,
					wh,
					wh
				};		
			XMoveResizeWindow(this->mainClass->display,cc->shadeButton,cc->shadeControlStruct.controlRect.x,cc->shadeControlStruct.controlRect.y,cc->shadeControlStruct.controlRect.w,cc->shadeControlStruct.controlRect.h);
			if(cc->isFullscreen==false)
				XMapWindow(this->mainClass->display,cc->shadeButton);
			else
				XUnmapWindow(this->mainClass->display,cc->shadeButton);
		}
	cc->riteButtonsWidth=offset;
}

void LFSWM2_windowClass::LFSWM2_setControlRect(Window wid,controlData *data,std::string xname,std::string yname,std::string wname,std::string hname)
{
	data->controlRect={
	this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(xname)],
	(this->mainClass->titleBarSize/2)-(this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(hname)]/2),
	this->theme.partsWidth[LFSTK_UtilityClass::LFSTK_hashFromKey(wname)],
	this->theme.partsHeight[LFSTK_UtilityClass::LFSTK_hashFromKey(hname)]};

	XMoveResizeWindow(this->mainClass->display,wid,
	data->controlRect.x,
	data->controlRect.y,
	data->controlRect.w,
	data->controlRect.h);
}
