/*
 *
 * ©K. D. Hedger. Sun 19 Nov 19:08:31 GMT 2023 keithdhedger@gmail.com

 * This file (globals.cpp) is part of LFSDock.

 * LFSDock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSDock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDock.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "globals.h"

//prefs
LFSTK_prefsClass	prefs;
std::string				configDir;
std::string				launchersDir;
std::string				configFile;
LFSTK_windowClass		*popWindow=NULL;
LFSTK_labelClass			*popLabel=NULL;
launcherList				*ll=NULL;
int						iconSize=16;
int						posMultiplier=1;

int						panelSize=2;
const monitorStruct		*mons=NULL;
int						onMonitor=0;
int						panelGravity=PANELNORTH;
const char				*panelTextColour="";
const char				*panelBGColour="";
int						extraSpace=16;

int						queueID;
msgBuffer				buffer;

const char				*desktopTheme=NULL;
bool						realMainLoop=true;

int						refreshRate=1;

//panel window
LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*mainwind=NULL;
int						launcherSide=NOLAUNCHERS;

//atoms
Atom						WM_STATE=None;
Atom						NET_WM_WINDOW_TYPE_NORMAL=None;
Atom						NET_WM_STATE_HIDDEN=None;
Atom						NET_WM_WINDOW_TYPE_DIALOG=None;
Atom						NET_WM_DESKTOP=None;
Atom						NET_WM_WINDOW_TYPE=None;
Atom						NET_WM_STATE=None;
Atom						NET_WM_NAME=None;
Atom						UTF8_STRING=None;
Atom						NET_CURRENT_DESKTOP=None;
Atom						WM_CLASS=None;
Atom						NET_WM_PID=None;

const char				*possibleError="Unknown";

LFSTK_buttonClass		*taskbuttons[20];
int						windowWidth=0;
GKeyFile					*kf=NULL;
LFSTK_findClass			*gFind;
bool						useTaskBar=false;

void sendNotify(const char *name,const char *message)//TODO//could be better
{
#ifdef _GOTNOTIFYSEND_
	std::string	com=std::string("notify-send -u low -t 2000 -i stock_dialog-info \"" + std::string(name) + " " + std::string(message) + " ...\" &");
	system(com.c_str());
#endif
}

void dropDesktopFile(const char *data,launcherList *launcher)
{
	char			*cleanstr;
	char			*command=NULL;
	char			*ptr;
	std::istringstream stream(data);
	std::string line;
	while(std::getline(stream,line))
		{
			cleanstr=apc->globalLib->LFSTK_cleanString((const char*)line.c_str());
			if((strrchr(cleanstr,'.')!=NULL) && (strcmp(strrchr(cleanstr,'.'),".desktop")==0))
				{
					asprintf(&command,"mkdir -p '%s/launchers-DOCK';cp -nP '%s' '%s/launchers-DOCK'",apc->configDir.c_str(),cleanstr,apc->configDir.c_str());
					ptr=strrchr(cleanstr,'/');
					sendNotify("Adding launcher ",++ptr);
					system(command);
					free(command);
					apc->exitValue=0;
					apc->mainLoop=false;
					free(cleanstr);
					return;
				}

			if(launcher!=NULL)
				{
					if(launcher->entry.inTerm==false)
						asprintf(&command,"%s \"%s\" &",launcher->entry.exec,cleanstr);
					else
						asprintf(&command,"%s %s \"%s\" &",prefs.LFSTK_getCString("termcommand"),launcher->entry.exec,cleanstr);
						sendNotify("Running ",launcher->entry.exec);
						system(command);
						free(cleanstr);
						free(command);
				}
		}
}

void setGadgetDetails(LFSTK_gadgetClass *gadget)
{
	gadget->LFSTK_setAlpha(1.0);
	gadget->LFSTK_setTile(NULL,0);
	gadget->gadgetDetails.bevel=BEVELNONE;
	gadget->LFSTK_setGadgetColours(GADGETBG,"#00000000","#00000000","#00000000","#00000000");
	gadget->	LFSTK_setGadgetColourPair(NORMALCOLOUR,"#00000000",panelTextColour);
}

/*
 * Check if window has given property
 */
bool hasProp(Window win,Atom atom)
{
	Atom				typeret;
	int				formatret;
	unsigned char	*propret;
	unsigned long	bytesafter,numret;

	typeret=None;
	propret=NULL;
	XGetWindowProperty(apc->display,win,atom,0,0,false,AnyPropertyType,&typeret,&formatret,&numret,&bytesafter,&propret);
	if (propret)
		XFree(propret);

	if(typeret!=None)
		return(true);

	return(false);
}

bool isVisible(Window win)
{
	bool ok;
	XWindowAttributes xwa;

	XGetWindowAttributes(apc->display,win,(XWindowAttributes*)&xwa);
	ok=(xwa.c_class==InputOutput) && (xwa.map_state==IsViewable);
	return ok;
}

Window doTreeWalk(Window wind,bool thisdesktop,std::string namecheck)
{
	Window			root,parent;
	Window			*children;
	Window			thewin;
	unsigned int		n_children;
	int				i;
	unsigned long	winid;
	char				*wname;
	void				*ptr=NULL;
	unsigned long	count=32;
	Atom				rtype;
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
			if((thisdesktop==true) && (isVisible(children[j])==false))
				{
					children[j]=None; /* Don't bother descending into this one */
					continue;
				}
			if (!hasProp(children[j],WM_STATE))
				continue;

//		propReturn		LFSTK_getSingleProp(Display *display,Window win,Atom prop,Atom wanttype);
			if (hasProp(children[j],WM_CLASS))
				{
				//fprintf(stderr,"got class\n");
				propReturn	pr=apc->globalLib->LFSTK_getSingleProp(apc->display,children[j],XInternAtom(apc->display,"WM_CLASS",false),XA_STRING);
if(namecheck.length()>0)
{
	//if(namecheck.compare(pr.strlist.at(0))==0)
	if(strcasecmp(namecheck.c_str(),pr.strlist.at(0).c_str())==0)
	{
fprintf(stderr,"strlist=%>>%s<< winid=%p\n",pr.strlist.at(0).c_str(),children[j]);
				if(wname!=NULL)
		XFree(wname);
	XFree(ptr);
	XFree(children);
	return children[j];
	}
}
				}

			/* Got one */
			thewin=children[j];
			winid=children[j];
			st=XFetchName(mainwind->app->display,children[j],&wname);
					fprintf(stderr,"wname=>>%s<<\n",wname);

			break;
		}

	thewin=None;
	/* No children matched, now descend into each child */
	for (i=(int) n_children - 1; i >= 0; i--)
		{
			if (children[i]==None)
				continue;
//			if (isHidden(children[i])==true)
//				continue;
			
			thewin=doTreeWalk(children[i],thisdesktop,namecheck);
			if (thewin != None)
			{
				return(thewin);
				break;
				}
		}

	if(wname!=NULL)
		{
	//	fprintf(stderr,"wname=>>%s<<\n",wname);
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

if(namecheck.length()>0)
{
	if(namecheck.compare(wname)==0)
		{
			if(wname!=NULL)
		XFree(wname);
	XFree(ptr);
	XFree(children);
	return thewin;
		}
}
//					for(int j=0;j<strlen(wname);j++)
//						if(!isalnum(wname[j]))
//							wname[j]=' ';
//
//					XGetWindowProperty(mainwind->app->display,winid,NET_WM_DESKTOP,0L,count,false,XA_CARDINAL,&rtype,&rfmt,&n,&rafter,(unsigned char **)&ptr);
//					windowList[windowListCnt]=new menuStruct;
//					windowList[windowListCnt]->label=strdup(wname);
//					windowList[windowListCnt++]->userData=(void*)winid;
				}
		}

	if(wname!=NULL)
		XFree(wname);
	XFree(ptr);
	XFree(children);
	return thewin;
}

Window doTreeWalkForClass(Window wind,std::string namecheck)
{
	Window			root;
	Window			parent;
	Window			*children;
	Window			thewin;
	unsigned int		n_children;
	unsigned long	winid;

	if (!XQueryTree(mainwind->app->display,wind,&root,&parent,&children,&n_children))
		return None;

	if (!children)
		return None;

	/* Check each child for WM_STATE and other validity */
	thewin=None;
	winid=-1;

	for (int j=n_children-1;j>=0;j--)
		{
			if(isVisible(children[j])==false)
				{
					children[j]=None; /* Don't bother descending into this one */
					continue;
				}
			if (!hasProp(children[j],WM_STATE))
				continue;

			if (hasProp(children[j],WM_CLASS))
				{
					propReturn	pr=apc->globalLib->LFSTK_getSingleProp(apc->display,children[j],XInternAtom(apc->display,"WM_CLASS",false),XA_STRING);

					for(int k=0;k<pr.strlist.size();k++)
						{
							if(strcasecmp(namecheck.c_str(),pr.strlist.at(k).c_str())==0)
								{
									thewin=children[j];
									XFree(children);
									return(thewin);
								}
						}
				}

			/* Got one */
			thewin=children[j];
			winid=children[j];
			break;
		}

	thewin=None;
	/* No children matched, now descend into each child */
	for (int i=(int) n_children - 1; i >= 0; i--)
		{
			if (children[i]==None)
				continue;
			
			thewin=doTreeWalkForClass(children[i],namecheck);
			if (thewin != None)
				return(thewin);
		}

	XFree(children);
	return thewin;
}

Window doTreeWalkForPID(Window wind,unsigned long pid)
{
	Window			root;
	Window			parent;
	Window			*children;
	Window			thewin;
	unsigned int		n_children;
	unsigned long	winid;

	if (!XQueryTree(mainwind->app->display,wind,&root,&parent,&children,&n_children))
		return None;

	if (!children)
		return None;

	/* Check each child for WM_STATE and other validity */
	thewin=None;
	winid=-1;

	for (int j=n_children-1;j>=0;j--)
		{
			if(isVisible(children[j])==false)
				{
					children[j]=None; /* Don't bother descending into this one */
					continue;
				}
			if(!hasProp(children[j],WM_STATE))
				continue;

			if(hasProp(children[j],NET_WM_PID))
				{
					propReturn	pr=apc->globalLib->LFSTK_getSingleProp(apc->display,children[j],NET_WM_PID,XA_CARDINAL);
					if(pr.integer==pid)
						{
							thewin=children[j];
							XFree(children);
							return(thewin);
						}
				}

			/* Got one */
			thewin=children[j];
			winid=children[j];
			break;
		}

	thewin=None;
	/* No children matched, now descend into each child */
	for (int i=(int) n_children - 1; i >= 0; i--)
		{
			if (children[i]==None)
				continue;
			
			thewin=doTreeWalkForPID(children[i],pid);
			if (thewin != None)
				return(thewin);
		}

	XFree(children);
	return thewin;
}

Window getNamedWindow(std::string name)
{
	Window win=None;
	win=doTreeWalk(apc->rootWindow,true,name);
	if(win!=None)
		{
		fprintf(stderr,"go win named %s at %p\n",name.c_str(),win);
		}
return(win);
}

Window getWindowByClass(std::string name)
{
	Window win=None;
	win=doTreeWalkForClass(apc->rootWindow,name);
	return(win);
}

Window getWindowByPID(unsigned long pid)
{
	Window win=None;
	win=doTreeWalkForPID(apc->rootWindow,pid);
	return(win);
}

void moveDock(int extra)
{
	int	psize;
	int	px,py;

	psize=windowWidth+extra;
	px=mons->x;
	py=mons->y;
	switch(panelGravity)
		{
			case PANELSOUTH:
				py=mons->y+mons->h-iconSize;
			case PANELNORTH:
				px=((mons->w/2)-(psize/2))+mons->x;
				break;
		}

	if(posMultiplier==1)
		mainwind->LFSTK_moveWindow(px,py-extraSpace,true);
	else
		mainwind->LFSTK_moveWindow(px,py,true);
}

std::string getWindowName(Window winid)
{
	propReturn	pr;
	char			*wname;
	Status		st;
	std::string	returnval="???";

	pr=apc->globalLib->LFSTK_getSingleProp(apc->display,winid,NET_WM_NAME,UTF8_STRING);
	if(pr.strlist.size()>0)
		{
			returnval=pr.strlist.at(0);
		}
	else
		{
			st=XFetchName(apc->display,winid,&wname);
			if(wname!=NULL)
				{
					returnval=wname;
					XFree(wname);
				}
		}

	return(returnval);
}

