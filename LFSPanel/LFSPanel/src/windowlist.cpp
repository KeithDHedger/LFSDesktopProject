/*
 *
 * ©K. D. Hedger. Mon 12 Oct 16:45:21 BST 2015 keithdhedger@gmail.com

 * This file (windowList.cpp) is part of LFSPanel.

 * LFSPanel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation,either version 3 of the License,or
 * at your option) any later version.

 * LFSPanel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSPanel.  If not,see <http://www.gnu.org/licenses/>.
 */

#include "globals.h"

//all windows
LFSTK_buttonClass	*windowAll=NULL;
menuStruct			**windowAllList=NULL;
LFSTK_menuClass		*windowAllMenu=NULL;
int					windowAllListCnt=0;

//this desktop list
LFSTK_buttonClass	*windowDesk=NULL;
menuStruct			**windowDeskList=NULL;
LFSTK_menuClass		*windowDeskMenu=NULL;
int					windowDeskListCnt=0;

const char			*possibleError="Unknown";
int					currentDesktop;

Window doTreeWalk(Window wind,bool thisdesktop);
void updateWindowMenu(bool global);

void sendClientMessage(Window win,const char *msg,unsigned long data0,unsigned long data1,unsigned long data2,unsigned long data3,unsigned long data4)
{
	XEvent	event;
	long	mask=SubstructureRedirectMask|SubstructureNotifyMask;

	event.xclient.type=ClientMessage;
	event.xclient.serial=0;
	event.xclient.send_event=True;
	event.xclient.message_type=XInternAtom(mainwind->app->display,msg,False);
	event.xclient.window=win;
	event.xclient.format=32;
	event.xclient.data.l[0]=data0;
	event.xclient.data.l[1]=data1;
	event.xclient.data.l[2]=data2;
	event.xclient.data.l[3]=data3;
	event.xclient.data.l[4]=data4;

	XSendEvent(mainwind->app->display,mainwind->app->rootWindow,False,mask,&event);
}

//all windows
bool windowAllCB(void *p,void* ud)
{
	geometryStruct geom;
	if(ud==windowDeskMenu)
		{
			LFSTK_menuClass	*mc=static_cast<LFSTK_menuClass*>(ud);
			if(mc->mainMenuCnt>0)
				{
					mc->LFSTK_freeMenus(mc->mainMenu,mc->mainMenuCnt);
					windowDeskList=new menuStruct*[MAXWINDOWSINLIST];
					windowDeskListCnt=0;
				}
			updateWindowMenu(false);
		}

	if(ud==windowAllMenu)
		{
			LFSTK_menuClass	*mc=static_cast<LFSTK_menuClass*>(ud);
			if(mc->mainMenuCnt>0)
				{
					mc->LFSTK_freeMenus(mc->mainMenu,mc->mainMenuCnt);
					windowAllList=new menuStruct*[MAXWINDOWSINLIST];
					windowAllListCnt=0;
				}
				updateWindowMenu(true);
			}

	if(static_cast<LFSTK_menuClass*>(ud)->mainMenuCnt==0)
		return(true);

	static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getGeom(&geom);

	static_cast<LFSTK_menuClass*>(ud)->x=geom.x;
	static_cast<LFSTK_menuClass*>(ud)->LFSTK_showMenu();
	return(true);
}

bool windowAllMenuCB(void *p,void* ud)
{
	Window			winid=0;
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();

	winid=(Window)(ud);
	possibleError="Can't activate window";
	sendClientMessage(winid,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
	return(true);
}

/*
 * Check if window has given property
 */
bool hasProp(Display *dpy,Window win,Atom atom)
{
	Atom			typeret;
	int				formatret;
	unsigned char	*propret;
	unsigned long	bytesafter,numret;

	typeret=None;
	propret=NULL;
	XGetWindowProperty(dpy,win,atom,0,0,false,AnyPropertyType,&typeret,&formatret,&numret,&bytesafter,&propret);
	if (propret)
		XFree(propret);

	if(typeret!=None)
		return(true);

	return(false);
}

bool hasWindowProp(Window wind,Atom atom,Atom atomtype)
{
	unsigned char	*data;
	Atom			*atoms;
	int				status,real_format;
	Atom			real_type;
	unsigned long	items_read,items_left;
	bool			result=false;

	status=XGetWindowProperty(mainwind->app->display,wind,atomtype,0L,1L,false,XA_ATOM,&real_type,&real_format,&items_read,&items_left,&data);
	if(status==Success)
		{
			atoms=(Atom *)data;
			if(items_read && (atoms[0]==atom))
				result=true;
			XFree(data);
		}
	return(result);
}

/*
 * Check if window is iconized
 */
bool isHidden(Window win)
{
	if( hasWindowProp(win,NET_WM_STATE,NET_WM_STATE_HIDDEN) )
		return(true);
	return(false);

}

/*
 * Check if window is viewable
 */
bool isVisible(Display *dpy,Window win)
{
	bool ok;
	MyXWindowAttributes xwa;

	XGetWindowAttributes(dpy, win, (XWindowAttributes*)&xwa);

	ok=(xwa.classe==InputOutput) && (xwa.map_state==IsViewable);

	return ok;
}

Window doTreeWalk(Window wind,bool thisdesktop)
{
	Window			root,parent;
	Window			*children;
	Window			thewin;
	unsigned int	n_children;
	int				i;
	unsigned long	winid;
	char			*wname;
	void			*ptr=NULL;
	unsigned long	count=32;
	Atom			rtype;
	int				rfmt;
	unsigned long	rafter;
	unsigned long	n=0;
	XTextProperty	textpropreturn;
	Status			st;

	if (!XQueryTree(mainwind->app->display,wind,&root,&parent,&children,&n_children))
		return None;

	if (!children)
		return None;

	/* Check each child for WM_STATE and other validity */
	thewin=None;
	wname=NULL;
	winid=-1;

	for (int j=n_children-1;j>=0; j--)
		{
			if((thisdesktop==true) && (isVisible(mainwind->app->display,children[j])==false))
				{
					children[j]=None; /* Don't bother descending into this one */
					continue;
				}
			if (!hasProp(mainwind->app->display,children[j],WM_STATE))
				continue;

			/* Got one */
			thewin=children[j];
			winid=children[j];
			st=XFetchName(mainwind->app->display,children[j],&wname);
		}

	thewin=None;
	/* No children matched, now descend into each child */
	for (i=(int) n_children - 1; i >= 0; i--)
		{
			if (children[i]==None)
				continue;
			if (isHidden(children[i])==true)
				continue;
			
			thewin=doTreeWalk(children[i],thisdesktop);
			if (thewin != None)
				break;
		}

	if(winid!=-1)
		{
			ptr=NULL;
			count=32;
			n=0;

			if(strlen(wname)==0)
				{
					st=XGetWindowProperty( mainwind->app->display,winid,NET_WM_NAME,0,count,false,UTF8_STRING,&rtype,&rfmt,&n,&rafter,(unsigned char **)&ptr);
					if(st==Success && n != 0 && ptr != NULL)
						wname=strdup((char*)ptr);
				}

			for(int j=0;j<strlen(wname);j++)
				if(!isalnum(wname[j]))
					wname[j]=' ';

			XGetWindowProperty(mainwind->app->display,winid,NET_WM_DESKTOP,0L,count,false,XA_CARDINAL,&rtype,&rfmt,&n,&rafter,(unsigned char **)&ptr);
			if(thisdesktop==true)
				{
					windowDeskList[windowDeskListCnt]=new menuStruct;
					windowDeskList[windowDeskListCnt]->label=strdup(wname);
					windowDeskList[windowDeskListCnt++]->userData=(void*)winid;
				}
			else
				{
					windowAllList[windowAllListCnt]=new menuStruct;
					windowAllList[windowAllListCnt]->label=strdup(wname);
					windowAllList[windowAllListCnt++]->userData=(void*)winid;
				}
		}

	if(wname!=NULL)
		XFree(wname);

	XFree(ptr);
	XFree(children);
	return thewin;
}

void getCurrentDesktop(void)
{
	char	*desknum=NULL;

	desknum=mainwind->globalLib->LFSTK_oneLiner("%s","/usr/bin/xprop -root |/bin/grep '_NET_CURRENT_DESKTOP(CARDINAL)'|/bin/head -n1|/usr/bin/awk -F'=' '{print $2}'");
	if(desknum!=NULL)
		{
			currentDesktop=atoi(desknum);
			free(desknum);
		}
	else
		currentDesktop=1;
}

void updateWindowMenu(bool global)
{
	Window			win;

	getCurrentDesktop();
	win=mainwind->app->rootWindow;

	if(global==false)
		{
			windowDeskListCnt=0;
			while(win!=None)
				win=doTreeWalk(win,true);
			if(windowDeskListCnt>0)
				windowDeskMenu->LFSTK_addMainMenus(windowDeskList,windowDeskListCnt);
		}

	if(global==true)
		{
			windowAllListCnt=0;
			while(win!=None)
				win=doTreeWalk(win,false);
			if(windowAllListCnt>0)
				windowAllMenu->LFSTK_addMainMenus(windowAllList,windowAllListCnt);
		}
}

int addWindowDeskMenu(int x,int y,int grav,bool fromleft)
{
	char	*icon=NULL;
	int		xpos=x;
	int		ypos=y;
	int		width=0;
	int		height=0;
	int		thisgrav=grav;
	int		iconsize=16;

	if(windowDeskMenu!=NULL)
		{
			printError("Duplicate current desktop window selector");
			return(0);
		}

	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);

	windowDesk=new LFSTK_buttonClass(mainwind,"",xpos,ypos,width,height,thisgrav);
	icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"remote-desktop","");
	if(icon!=NULL)
		{
			windowDesk->LFSTK_setImageFromPath(icon,LEFT,true);
			free(icon);
		}
	else
		windowDesk->LFSTK_setImageFromPath(DATADIR "/pixmaps/windows.png",LEFT,true);

	windowDeskMenu=new LFSTK_menuClass(mainwind,xpos,ypos+panelHeight,1,1);

	windowDesk->LFSTK_setMouseCallBack(NULL,windowAllCB,(void*)windowDeskMenu);
	windowDeskMenu->LFSTK_setMouseCallBack(NULL,windowAllMenuCB,NULL);
	windowDeskListCnt=-1;
	windowAllListCnt=-1;
	windowDeskList=new menuStruct*[MAXWINDOWSINLIST];
	for(int j=0;j<MAXWINDOWSINLIST;j++)
		windowDeskList[j]=NULL;
	
	return(width);
}

int addWindowMenu(int x,int y,int grav,bool fromleft)
{
	char	*icon=NULL;
	int		xpos=x;
	int		ypos=y;
	int		width=0;
	int		height=0;
	int		thisgrav=grav;
	int		iconsize=16;

	if(windowAll!=NULL)
		{
			printError("Duplicate window selector");
			return(0);
		}
	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);

	windowAll=new LFSTK_buttonClass(mainwind,"",xpos,ypos,width,height,thisgrav);
	icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"computer","");

	if(icon!=NULL)
		{
			windowAll->LFSTK_setImageFromPath(icon,LEFT,true);
			free(icon);
		}
	else
		windowAll->LFSTK_setImageFromPath(DATADIR "/pixmaps/windows.png",LEFT,true);


	windowAllMenu=new LFSTK_menuClass(mainwind,xpos,ypos+panelHeight,1,1);

	windowAll->LFSTK_setMouseCallBack(NULL,windowAllCB,(void*)windowAllMenu);
	windowAllMenu->LFSTK_setMouseCallBack(NULL,windowAllMenuCB,NULL);

	windowDeskListCnt=-1;
	windowAllListCnt=-1;
	windowAllList=new menuStruct*[MAXWINDOWSINLIST];
	for(int j=0;j<MAXWINDOWSINLIST;j++)
		windowAllList[j]=NULL;

	return(width);
}
