/*
 *
 * ©K. D. Hedger. Mon 12 Oct 16:45:21 BST 2015 kdhedger68713@gmail.com

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

#include "windowlist.h"

LFSTK_menuButtonClass	*windowMenu=NULL;
LFSTK_menuButtonClass	*windowDeskMenu=NULL;
menuItemStruct			windowList[MAXWINDOWSINLIST];
menuItemStruct			windowDeskList[MAXWINDOWSINLIST];
char					windowBuffer[4096];
int						windowDeskListCnt=0;
int						windowListCnt=0;
int						updateWindowCnt=0;
const char				*possibleError="Unknown";
int						currentDesktop;

void sendClientMessage(Window win,const char *msg,unsigned long data0,unsigned long data1,unsigned long data2,unsigned long data3,unsigned long data4)
{
	XEvent	event;
	long	mask=SubstructureRedirectMask|SubstructureNotifyMask;

	event.xclient.type=ClientMessage;
	event.xclient.serial=0;
	event.xclient.send_event=True;
	event.xclient.message_type=XInternAtom(mainwind->display,msg,False);
	event.xclient.window=win;
	event.xclient.format=32;
	event.xclient.data.l[0]=data0;
	event.xclient.data.l[1]=data1;
	event.xclient.data.l[2]=data2;
	event.xclient.data.l[3]=data3;
	event.xclient.data.l[4]=data4;

	XSendEvent(mainwind->display,mainwind->rootWindow,False,mask,&event);
}

bool windowDeskMenuCB(void *p,void* ud)
{
	menuItemStruct	*menu=(menuItemStruct*)ud;
	Window			winid=0;
	unsigned long	desktop=0;

	if(menu==NULL)
		return(true);
	winid=(Window)(menu->userData);
	possibleError="Can't activate window";
	sendClientMessage(winid,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
	return(true);
}

void resetMenus(void)
{
	//for(int j=0; j<windowListCnt+1; j++)
	for(int j=0; j<MAXWINDOWSINLIST; j++)
		if(windowList[j].label!=NULL)
			{
				free((char*)windowList[j].label);
				windowList[j].label=NULL;
			}
	windowListCnt=-1;
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
	int				n;
	unsigned char	*data;
	Atom			*atoms;
	int				status,real_format;
	Atom			real_type;
	unsigned long	items_read,items_left;
	bool			result=false;

	status=XGetWindowProperty(mainwind->display,wind,atomtype,0L,1L,false,XA_ATOM,&real_type,&real_format,&items_read,&items_left,&data);
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
	unsigned long	desktop;
	void			*ptr=NULL;
	unsigned long	count=32;
	Atom			rtype;
	int				rfmt;
	unsigned long	rafter;
	unsigned long	n=0;
	XTextProperty	textpropreturn;

	if (!XQueryTree(mainwind->display,wind,&root,&parent,&children,&n_children))
		return None;

	if (!children)
		return None;

	/* Check each child for WM_STATE and other validity */
	thewin=None;
	wname=NULL;
	winid=-1;
	desktop=-1;
	for (int j=n_children-1; j>=0; j--)
		{
			if((thisdesktop==true) && (isVisible(mainwind->display, children[j])==false))
				{
					children[j]=None; /* Don't bother descending into this one */
					continue;
				}
			if (!hasProp(mainwind->display, children[j],WM_STATE))
				continue;

			if (!hasWindowProp(children[j],NET_WM_WINDOW_TYPE_NORMAL,NET_WM_WINDOW_TYPE))
				continue;

			/* Got one */
			thewin=children[j];
			winid=children[j];
			XFetchName(mainwind->display,children[j],&wname);
			if(wname==NULL)
				{
					if(XGetWMName(mainwind->display,children[j],&textpropreturn)!=0)
						wname=strdup((char*)textpropreturn.value);
					else
						{
							printError("Can't determine window name...");
							wname=strdup("Untitled...");
						}
				}
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
			if(XGetWindowProperty(mainwind->display,winid,NET_WM_DESKTOP,0L,count,false,XA_CARDINAL,&rtype,&rfmt,&n,&rafter,(unsigned char **)&ptr)==Success)
				desktop=(long)(*(char*)ptr);

			if(thisdesktop==true)
				{
					windowDeskList[windowDeskListCnt].bc=NULL;
					windowDeskList[windowDeskListCnt].subMenus=NULL;
					windowDeskList[windowDeskListCnt].subMenuCnt=desktop;
					windowDeskList[windowDeskListCnt].useIcon=false;
					windowDeskList[windowDeskListCnt].label=strdup(wname);
					windowDeskList[windowDeskListCnt].userData=(void*)winid;
					windowDeskListCnt++;
				}
			else
				{
					windowList[windowListCnt].bc=NULL;
					windowList[windowListCnt].subMenus=NULL;
					windowList[windowListCnt].subMenuCnt=desktop;
					windowList[windowListCnt].useIcon=false;
					windowList[windowListCnt].label=strdup(wname);
					windowList[windowListCnt].userData=(void*)winid;
					windowListCnt++;
				}
		}

	if(wname!=NULL)
		XFree(wname);

	XFree(children);
	return thewin;

}

int getCurrentDesktop(void)
{
	char	*desknum=NULL;

	desknum=mainwind->globalLib->LFSTK_oneLiner("%s","xprop -root |grep '_NET_CURRENT_DESKTOP(CARDINAL)'|head -n1|awk -F'=' '{print $2}'");
	if(desknum!=NULL)
		{
			currentDesktop=atoi(desknum);
			free(desknum);
		}
	else
		currentDesktop=1;
}

void updateWindowMenu(void)
{
	Window			win;

	updateWindowCnt++;
	if(updateWindowCnt>=WINDOWREFRESH)
		updateWindowCnt=0;
	else
		return;

	if(windowListCnt>-1)
		resetMenus();

	getCurrentDesktop();

	if(windowDeskMenu!=NULL)
		{
			win=mainwind->rootWindow;
			windowDeskListCnt=0;
			while(win!=None)
				win=doTreeWalk(win,true);

			if(windowDeskListCnt>0)
				{
					windowDeskMenu->LFSTK_updateMenus(windowDeskList,windowDeskListCnt);
					windowDeskMenu->LFSTK_setActive(true);
				}
			else
				windowDeskMenu->LFSTK_setActive(false);
		}

	if(windowMenu!=NULL)
		{
			windowListCnt=0;
			win=mainwind->rootWindow;
			while(win!=None)
				win=doTreeWalk(win,false);
			windowMenu->LFSTK_updateMenus(windowList,windowListCnt);
		}
}

bool windowMenuCB(void *p,void* ud)
{
	menuItemStruct	*menu=(menuItemStruct*)ud;
	Window			winid=0;
	unsigned long	desktop=0;

	if(menu==NULL)
		return(true);
	winid=(Window)(menu->userData);
	desktop=(unsigned long)menu->subMenuCnt;
	possibleError="Can't switch desktop";
	sendClientMessage(mainwind->rootWindow,"_NET_CURRENT_DESKTOP",desktop,0,0,0,0);
	possibleError="Can't activate window";
	sendClientMessage(winid,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
	updateWindowCnt=WINDOWREFRESH+100;
	updateWindowMenu();
	return(true);
}

int addWindowDeskMenu(int x,int y,int grav,bool fromleft)
{
	const char	*icon=NULL;
	int			xpos=x;
	int			ypos=y;
	int			width=0;
	int			height=0;
	int			thisgrav=grav;
	int			iconsize=16;


	if(windowDeskMenu!=NULL)
		{
			printError("Duplicate current desktop window selector");
			return(0);
		}

	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);

	windowDeskMenu=new LFSTK_menuButtonClass(mainwind,"",xpos,ypos,width,height,thisgrav);
	icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"remote-desktop","");
	if(icon!=NULL)
		windowDeskMenu->LFSTK_setImageFromPath(icon,iconsize,iconsize);
	else
		windowDeskMenu->LFSTK_setIconFromPath(DATADIR "/pixmaps/windows.png",iconsize);
	windowDeskMenu->LFSTK_setCallBack(NULL,windowDeskMenuCB,NULL);

	windowDeskListCnt=-1;
	windowListCnt=-1;
	updateWindowCnt=WINDOWREFRESH;
	updateWindowMenu();
	useAlarm=true;
	return(width);
}

int addWindowMenu(int x,int y,int grav,bool fromleft)
{
	const char	*icon=NULL;
	int			xpos=x;
	int			ypos=y;
	int			width=0;
	int			height=0;
	int			thisgrav=grav;
	int			iconsize=16;

	if(windowMenu!=NULL)
		{
			printError("Duplicate window selector");
			return(0);
		}

	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);

	windowMenu=new LFSTK_menuButtonClass(mainwind,"",xpos,ypos,width,height,thisgrav);
	icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"computer","");
	if(icon!=NULL)
		windowMenu->LFSTK_setIconFromPath(icon,iconsize);
	else
		windowMenu->LFSTK_setIconFromPath(DATADIR "/pixmaps/windows.png",iconsize);
	windowMenu->LFSTK_setCallBack(NULL,windowMenuCB,NULL);

	windowDeskListCnt=-1;
	windowListCnt=-1;
	updateWindowCnt=WINDOWREFRESH;
	updateWindowMenu();
	useAlarm=true;
	return(width);
}

