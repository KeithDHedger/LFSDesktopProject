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

#define NELEM(v) (sizeof v / sizeof v[0])

void LFSWM2_windowClass::init(void)
{
	this->LFSWM2_buildClientList();
}

void LFSWM2_windowClass::LFSWM2_buildClientList(void)
{
	Window	returned_root;
	Window	returned_parent;
	Window	*toplevelwindows;
	unsigned int windowscnt;
	unsigned long n=0;

	std::string	windowname;

	//XGrabServer(this->mainClass->display);

	XQueryTree(this->mainClass->display,this->mainClass->rootWindow,&returned_root,&returned_parent,&toplevelwindows,&windowscnt);
	for(unsigned int i=0;i<windowscnt;++i)
		this->LFSWM2_createClient(toplevelwindows[i]);

	//XUngrabServer(this->mainClass->display);
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

	this->mainClass->mainEventClass->LFSWM2_restack();
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
	Visual				*visual=DefaultVisual(this->mainClass->display,this->mainClass->screen);
	Colormap				colormap=DefaultColormap(this->mainClass->display,this->mainClass->screen);

	if((draw=XftDrawCreate(this->mainClass->display,this->mainClass->rootWindow,this->mainClass->defaultVisual,colormap))==NULL)
		return NULL;

	if(!XftColorAllocName(this->mainClass->display,this->mainClass->defaultVisual,colormap,name,&color))
		{
			fprintf(stderr,"Can't alloc colour %s, using fallback %s",name,fallback);	
			if(!XftColorAllocName(this->mainClass->display,this->mainClass->defaultVisual,colormap,fallback,&color))
				{	
					XftDrawDestroy(draw);
					return NULL;
				}
		}

	XAllocNamedColor(this->mainClass->display,colormap,name,&sc,&tc);
	c=new struct fontColour;
	c->draw=draw;
	c->color=color;
	c->visual=visual;
	c->colormap=colormap;
	c->pixel=sc.pixel;

	return c;
}

void LFSWM2_windowClass::LFSWM2_createClient(Window id)
{
	if(this->LFSWM2_getWindowType(id)==DOCKWINDOW)
		{
			this->LFSWM2_setClientList(id,true);
			XRaiseWindow(this->mainClass->display,id);
			return;
		}

	//XGrabServer(this->mainClass->display);

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
			int					offy;
			int					offx;

			if(this->LFSWM2_getWindowType(id)==this->mainClass->atoms.at("_NET_WM_WINDOW_TYPE_DOCK"))
				return;

			XGetWindowAttributes(this->mainClass->display,id,&x_window_attrs);
			if(x_window_attrs.override_redirect||x_window_attrs.map_state!=IsViewable)
				return;

			if(x_window_attrs.x-2<0)
				offx=0;
			else
				offx=x_window_attrs.x-2;

			if(x_window_attrs.y-16-2<0)
				offy=0;
			else
				offy=x_window_attrs.y-16-2;

			cc=new LFSWM2_clientClass(this->mainClass,id);
			cc->frameWindow=XCreateSimpleWindow(this->mainClass->display,this->mainClass->rootWindow,offx,offy,x_window_attrs.width+(this->mainClass->sideBarSize*2),x_window_attrs.height+this->mainClass->titleBarSize+this->mainClass->bottomBarSize+BORDER_WIDTH,BORDER_WIDTH,this->mainClass->frameFG->pixel,this->mainClass->frameBG->pixel);

			XSelectInput(this->mainClass->display,cc->frameWindow,SubstructureRedirectMask|SubstructureNotifyMask|ButtonPressMask|PointerMotionMask|ButtonReleaseMask|ExposureMask|PropertyChangeMask);

			cc->windowType=this->LFSWM2_getWindowType(id);
			cc->contentWindow=id;
			cc->clientWindowRect=this->LFSWM2_getWindowRect(id,this->mainClass->rootWindow);
			cc->framePreMaxRect=this->LFSWM2_getWindowRect(id,this->mainClass->rootWindow);
			cc->clientPreMaxRect=this->LFSWM2_getWindowRect(id,this->mainClass->rootWindow);
			cc->clientWindowRect.y-=this->mainClass->titleBarSize;
			cc->frameWindowRect=this->LFSWM2_getWindowRect(cc->frameWindow,this->mainClass->rootWindow);
			this->clientList[id]=cc;
			this->clientList[cc->frameWindow]=cc;

			XReparentWindow(this->mainClass->display,id,cc->frameWindow,this->mainClass->sideBarSize-1,this->mainClass->titleBarSize-(BORDER_WIDTH*2)-1);
			XMapWindow(this->mainClass->display,cc->frameWindow);

			this->LFSWM2_setWindowState(id,NormalState);

			this->mainClass->mainEventClass->LFSWM2_sendConfigureEvent(id,cc->clientWindowRect);
			XGrabButton(this->mainClass->display,Button1,0,id,False,ButtonPressMask,GrabModeSync,GrabModeAsync,None,None);
			XGrabButton(this->mainClass->display,Button1,Mod4Mask,id,False,ButtonPressMask|ButtonReleaseMask|PointerMotionMask,GrabModeAsync,GrabModeAsync,None,None);

			XSelectInput(this->mainClass->display,cc->contentWindow,PropertyChangeMask|StructureNotifyMask);

			Atom	v[7];

			v[0]=this->mainClass->atoms.at("_NET_WM_ACTION_CHANGE_DESKTOP");
			v[1]=this->mainClass->atoms.at("_NET_WM_ACTION_CLOSE");
			v[2]=this->mainClass->atoms.at("_NET_WM_ACTION_FULLSCREEN");
			v[3]=this->mainClass->atoms.at("_NET_WM_ACTION_MAXIMIZE_HORZ");
			v[4]=this->mainClass->atoms.at("_NET_WM_ACTION_MAXIMIZE_VERT");
			v[5]=this->mainClass->atoms.at("_NET_WM_ACTION_MINIMIZE");
			v[6]=this->mainClass->atoms.at("_NET_WM_STATE_BELOW");

			int nd=this->mainClass->currentDesktop;
			cc->onDesk=this->mainClass->currentDesktop;
			this->LFSWM2_setProp(id,this->mainClass->atoms.at("_NET_WM_ALLOWED_ACTIONS"),XA_ATOM,32,v,7);
			//this->LFSWM2_setProp(cc->contentWindow,this->mainClass->atoms.at("_NET_WM_DESKTOP"),XA_CARDINAL,32,(void*)&cc->onDesk,1);
			this->LFSWM2_setProp(cc->contentWindow,this->mainClass->atoms.at("_NET_WM_DESKTOP"),XA_CARDINAL,32,(void*)&nd,1);
//setprop(root,NET_CLIENT_LIST_STACKING,XA_WINDOW,32,v,n);

			this->LFSWM2_setClientList(id,true);

//controls
			wa.win_gravity=NorthEastGravity;
			wa.save_under=true;

			int	offset;
			int wposy=(this->mainClass->titleBarSize/2)-(deleteWindowSize/2);
			int wh=this->mainClass->titleBarSize-(BORDER_WIDTH*2);
//close
			offset=CONTROL_GAP;
			wposy=(this->mainClass->titleBarSize/2)-(deleteWindowSize/2)-(1);
			this->mainClass->closeControlStruct.boundingBox={cc->frameWindowRect.width-offset,0,wh,wh};
			this->mainClass->closeControlStruct.pixmapBox={wposy,wposy,8,8};
			this->mainClass->closeControlStruct.startX=cc->frameWindowRect.width-offset;

			cc->closeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.width-offset,0,this->mainClass->titleBarSize,wh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWSaveUnder,&wa);
			XSelectInput(this->mainClass->display,cc->closeButton,ButtonPressMask|EnterWindowMask|LeaveWindowMask);
			XMapWindow(this->mainClass->display,cc->closeButton);
//max
			offset+=CONTROL_GAP;
			this->mainClass->maximizeControlStruct.boundingBox={cc->frameWindowRect.width-offset,0,wh,wh};
			this->mainClass->maximizeControlStruct.pixmapBox={wposy,wposy,8,8};
			this->mainClass->maximizeControlStruct.startX=cc->frameWindowRect.width-offset;
			cc->maximizeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.width-offset,0,this->mainClass->titleBarSize,wh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWSaveUnder,&wa);
			XSelectInput(this->mainClass->display,cc->maximizeButton,ButtonPressMask|EnterWindowMask|LeaveWindowMask);
			XMapWindow(this->mainClass->display,cc->maximizeButton);

//min
			offset+=CONTROL_GAP;
			this->mainClass->minimizeControlStruct.boundingBox={cc->frameWindowRect.width-offset,0,wh,wh};
			this->mainClass->minimizeControlStruct.pixmapBox={wposy,wposy,8,8};
			this->mainClass->minimizeControlStruct.startX=cc->frameWindowRect.width-offset;
			cc->minimizeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.width-offset,0,this->mainClass->titleBarSize,wh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWSaveUnder,&wa);
			XSelectInput(this->mainClass->display,cc->minimizeButton,ButtonPressMask|EnterWindowMask|LeaveWindowMask);
			XMapWindow(this->mainClass->display,cc->minimizeButton);

//shade
			offset+=CONTROL_GAP;
			this->mainClass->shadeControlStruct.boundingBox={cc->frameWindowRect.width-offset,0,wh,wh};
			this->mainClass->shadeControlStruct.pixmapBox={wposy,wposy,8,8};
			this->mainClass->shadeControlStruct.startX=cc->frameWindowRect.width-offset;
			cc->shadeButton=XCreateWindow(this->mainClass->display,cc->frameWindow,cc->frameWindowRect.width-offset,0,this->mainClass->titleBarSize,wh,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity|CWSaveUnder,&wa);
			XSelectInput(this->mainClass->display,cc->shadeButton,ButtonPressMask|EnterWindowMask|LeaveWindowMask);
			XMapWindow(this->mainClass->display,cc->shadeButton);

			cc->LFSWM2_setWindowName();
				unsigned long		n=0;

			Atom *states=(Atom*)this->LFSWM2_getFullProp(id,this->mainClass->atoms.at("_NET_WM_STATE"),XA_ATOM,32,&n);
//check no longer maxed
			if(n>1)
				{
					if((n>1) && (states[0]==this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT")) && (states[1]==this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ")))
						cc->isMaximized=true;
					else
						cc->isMaximized=false;
				}
			this->LFSWM2_reloadWindowState(id);

			this->mainClass->mainEventClass->LFSWM2_moveToTop(cc->contentWindow);
			this->mainClass->mainEventClass->LFSWM2_restack();
			if(cc->windowType==NORMALWINDOW)
				XAddToSaveSet(this->mainClass->display,id);
		}
	XSync(this->mainClass->display,false);
//	XUngrabServer(this->mainClass->display);
}

rectStruct LFSWM2_windowClass::LFSWM2_getWindowRect(Window id,Window parent,bool dotranslate)
{
	XWindowAttributes	attr;
	rectStruct			r={0,0,0,0};
	Window				wr;

	XGetWindowAttributes(this->mainClass->display,id,&attr);
	if(dotranslate==true)
		XTranslateCoordinates(this->mainClass->display,id,parent,attr.x,attr.y,&r.x,&r.y,&wr);
	r.width=attr.width;
	r.height=attr.height;
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
	int				retval=NORMALWINDOW;

	status=XGetWindowProperty(this->mainClass->display,id,this->mainClass->atoms.at("_NET_WM_WINDOW_TYPE"),0L,sizeof (Atom),false,XA_ATOM,&da,&di,&dl,&dl,&propret);
/*
"_NET_WM_WINDOW_TYPE_NORMAL"
"_NET_WM_WINDOW_TYPE_DESKTOP"
"_NET_WM_WINDOW_TYPE_DOCK"
"_NET_WM_WINDOW_TYPE_MENU"
"_NET_WM_WINDOW_TYPE_DIALOG"
"_NET_WM_WINDOW_TYPE_TOOL"
enum {NORMALWINDOW=0,DESKTOPWINDOW,DOCKWINDOW,MENUWINDOW,DIALOGWINDOW,TOOLWINDOW};
*/
	if (status==Success&&propret)
		{
			Atom		atom=((Atom *)propret)[0];
			char		*x=XGetAtomName(this->mainClass->display,atom);//TODO//
			if(x!=NULL)
				{
					if(strcmp(x,"_NET_WM_WINDOW_TYPE_NORMAL")==0)
						retval=NORMALWINDOW;
					if(strcmp(x,"_NET_WM_WINDOW_TYPE_DESKTOP")==0)
						retval=DESKTOPWINDOW;
					if(strcmp(x,"_NET_WM_WINDOW_TYPE_DOCK")==0)
						retval=DOCKWINDOW;
					if(strcmp(x,"_NET_WM_WINDOW_TYPE_MENU")==0)
						retval=MENUWINDOW;
					if(strcmp(x,"_NET_WM_WINDOW_TYPE_DIALOG")==0)
						retval=DIALOGWINDOW;
					if(strcmp(x,"_NET_WM_WINDOW_TYPE_TOOL")==0)
						retval=TOOLWINDOW;

					//fprintf(stderr,"window type atom=%s\n",x);
					XFree(x);
				}
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
	void *ptr=NULL;
	unsigned long count=32;
	Atom rtype;
	int rfmt;
	unsigned long rafter;
	for (;;)
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
	void					*prop_return=NULL;
	Atom					actual_type_return=0;
	int					actual_format_return=0;
	unsigned long		bytes_after_return=0;
	int					result=-1;

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
	Window				w=id;
	unsigned long		n=0;
	bool					handled;
	bool					unmax=true;

//	csetfull(c,false);
//	c->followdesk=false;
//	c->skiptaskbar=false;
//	c->isAbove=false;
//	c->isBelow=false;

	cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(this->mainClass->mainWindowClass->LFSWM2_getParentWindow(id));
	if(cc==NULL)
		return;

	Atom *states=(Atom*)this->LFSWM2_getFullProp(id,this->mainClass->atoms.at("_NET_WM_STATE"),XA_ATOM,32,&n);

//check no longer maxed
	if(cc->isMaximized==true)
		{
			bool unmaxit=true;
			if(n>1)
				{
					if((n==2) && (states[0]==this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT")) || (states[1]==this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ")))
						unmaxit=false;
				}

			cc->LFSWM2_maxWindow();
		}

//check for maxim
	if((n>1) && (cc->isMaximized==false))
		{
			if((states[0]==this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT")) && (states[1]==this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ")))
				{
					cc->LFSWM2_maxWindow();
				}
		}

	if((n>0) && (states[0]==this->mainClass->atoms.at("_NET_WM_STATE_ABOVE")))
		{
			cc->onTop=true;
		}

	if((n>0) && (states[0]==this->mainClass->atoms.at("_NET_WM_STATE_BELOW")))
		{
		fprintf(stderr,"_NET_WM_STATE_BELOW\n");
			cc->onBottom=true;
		}

//	for (unsigned int i=0; i<n; i++)
//		{
//			//if((states[i]
//			fprintf(stderr,"states[%i]=%s\n",i,XGetAtomName(this->mainClass->display,states[i]));
//			
//		}

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

			if(states[i]==NET_WM_STATE_ABOVE)
				{
					c->isAbove=true;
					handled=true;
				}

			if(states[i]==NET_WM_STATE_BELOW)
				{
					fprintf(stderr,"there>>>>\n");
					c->isBelow=true;
					handled=true;
				}
#endif			
//			if((states[i]==NET_WM_STATE_MAXIMIZED_HORZ) || (states[i]==NET_WM_STATE_MAXIMIZED_VERT))
//				{
//					unmax=false;
//					if( (c->frame!=NULL) && (c->frame->isMaximized==false) )
//						maximizeWindow(c,666);
//					handled=true;
//				}
#if 0
			if(states[i]==NET_WM_STATE_HIDDEN)
				{
					if(c->frame->isMaximized==false)
						maximizeWindow(c,666);
					handled=true;
				}

			if(handled==false)
				removestate(w,states[i]);
		}

	if((c->frame!=NULL) && ((unmax==true) and (c->frame->isMaximized==true)))
		maximizeWindow(c,666);

	if (states != NULL)
		XFree(states);
#endif
}

bool LFSWM2_windowClass::LFSWM2_hasState(Window w,Atom state)
{
	unsigned long	n=0;
	bool				found=false;
	Atom				*v=(Atom*)this->LFSWM2_getProp(w,this->mainClass->atoms.at("_NET_WM_STATE"),XA_ATOM,&n);

	for(unsigned long i=0;i<n;i++)
		if(v[i]==state)
			{
				found=true;
				break;
			}
	if(v!=NULL)
		XFree(v);
	return found;
}

void LFSWM2_windowClass::LFSWM2_changeState(Window id,int how,Atom state)
{
//this->mainClass->DEBUG_printAtom(state);
	LFSWM2_clientClass	*cc;
	bool					*clientprop=NULL;

	cc=this->mainClass->mainWindowClass->LFSWM2_getClientClass(id);
	switch(how)
		{
			case NET_WM_STATE_REMOVE:
				this->LFSWM2_removeProp(id,state);
				//if((cc!=NULL) && (state==this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN")))
				//	cc->isMinimized=false;
					
				break;
			case NET_WM_STATE_ADD:
				this->LFSWM2_addState(id,state);
				//if((cc!=NULL) && (state==this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN")))
				//	cc->isMinimized=true;
				break;
			case NET_WM_STATE_TOGGLE://TODO//
				//if((cc!=NULL) && (state==this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN")))
				//	cc->isMinimized=!cc->isMinimized;
				if (LFSWM2_hasState(id,state))
					this->LFSWM2_removeProp(id,state);
				else
					this->LFSWM2_addState(id,state);
				break;
		}

	if(cc!=NULL)
		{
			if(state=this->mainClass->atoms.at("_NET_WM_STATE_HIDDEN"))
				clientprop=&cc->isMinimized;
			else if((state=this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT")) || (state=this->mainClass->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ")))
				clientprop=&cc->isMaximized;

			switch(how)
				{
					case NET_WM_STATE_REMOVE:
						*clientprop=false;
						break;
					case NET_WM_STATE_ADD:
						*clientprop=true;
						break;
					case NET_WM_STATE_TOGGLE:
						*clientprop=!*clientprop;
						break;
				}
		}
}

void LFSWM2_windowClass::LFSWM2_addState(Window w,Atom state)
{
	unsigned long	n=0;
	Bool				present=False;
	Atom				*old=(Atom*)LFSWM2_getProp(w,this->mainClass->atoms.at("_NET_WM_STATE"),XA_ATOM,&n);

	for(unsigned long i=0;i<n;i++)
		if(old[i]==state)
			{
				present=True;
				break;
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

void LFSWM2_windowClass::LFSWM2_refreshFrame(LFSWM2_clientClass *cc,XExposeEvent *e)//TODO//prevent flicker
{
	rectStruct r=cc->frameWindowRect;

	if(e==NULL)
		return;
//fprintf(stderr,"x=%i y=%i w=%i h=%i\n",e->x,e->y,e->width,e->height);
	if(e!=NULL)
		{
			if(e->count!=0)
				return;
		}	

	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameBG->pixel);
	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
	XFillRectangle(this->mainClass->display,cc->frameWindow,this->mainClass->mainGC,0,0,r.width,r.height);

	XSetForeground(this->mainClass->display,this->mainClass->mainGC,this->mainClass->frameText->pixel);
	XSetClipMask(this->mainClass->display,this->mainClass->mainGC,None);
	XftDrawChange(this->mainClass->frameText->draw,cc->frameWindow);
	if(cc->name.length()>0)
		{
			if(cc->nameIsUTF==true)
				XftDrawStringUtf8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,10,12,(XftChar8 *)cc->name.c_str(),strlen(cc->name.c_str()));
			else
				XftDrawString8(this->mainClass->frameText->draw,&(this->mainClass->frameText->color),this->mainClass->frameFont,10,12,(XftChar8 *)cc->name.c_str(),strlen(cc->name.c_str()));
		}

//buttons
	cc->drawMouseLeave(cc->closeButton,this->mainClass->closeBitMap,this->mainClass->closeControlStruct);
	cc->drawMouseLeave(cc->maximizeButton,this->mainClass->maximizeBitMap,this->mainClass->maximizeControlStruct);
	cc->drawMouseLeave(cc->minimizeButton,this->mainClass->minimizeBitMap,this->mainClass->minimizeControlStruct);
	cc->drawMouseLeave(cc->shadeButton,this->mainClass->shadeBitMap,this->mainClass->shadeControlStruct);
}

Window LFSWM2_windowClass::LFSWM2_getParentWindow(Window id)
{
	Window		root=0;
	Window		parent=0;
	Window		*children=NULL;
    unsigned int	num_children=0;

	if(!XQueryTree(this->mainClass->display,id,&root,&parent,&children,&num_children))
		return(0);

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
			for(int j=0;j<this->windowIDList.size();j++)
				{
					if(this->windowIDList.at(j)==id)
						{
							this->windowIDList.erase(this->windowIDList.begin()+j);
							break;
						}
				}
		}
	this->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_CLIENT_LIST"),XA_WINDOW,32,this->windowIDList.data(),this->windowIDList.size());
	this->LFSWM2_setProp(this->mainClass->rootWindow,this->mainClass->atoms.at("_NET_CLIENT_LIST_STACKING"),XA_WINDOW,32,this->windowIDList.data(),this->windowIDList.size());
}

void LFSWM2_windowClass::LFSWM2_setWindowState(Window w,long state)
{
	long data[2]= {state,None};
	XChangeProperty(this->mainClass->display,w,this->mainClass->atoms.at("WM_STATE"),this->mainClass->atoms.at("WM_STATE"),32,PropModeReplace,(unsigned char *)data,2);
}