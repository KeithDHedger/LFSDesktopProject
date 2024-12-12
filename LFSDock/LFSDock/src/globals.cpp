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
std::string				whatDock;

int						iconWidth=16;
int						iconHeight=24;
int						normalY=0;
int						activeY=0;
int						extraSpace=16;
int						deskCount=1;

int						dockSize=2;
const monitorStruct		*mons=NULL;
int						onMonitor=0;
int						dockGravity=PANELSOUTH;
std::string				dockTextColour="";
std::string				dockBGColour="";
std::string				dockBGImage="";
std::string				dockBGImageLS="";
std::string				dockBGImageRS="";
bool						useBG=false;
std::string				launcherPreColour="#00000000";
bool						moveGadget=false;

int						queueID;
msgBuffer				buffer;

std::string				desktopTheme;
bool						realMainLoop=true;

int						refreshRate=500000;
bool						useMicros=true;

//dock window
LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*dockWindow=NULL;
LFSTK_windowClass		*dockBGWindow=NULL;
LFSTK_windowClass		*iconWindow=NULL;
int						holdpsize;

LFSTK_windowClass		*popActionWindow=NULL;
LFSTK_listGadgetClass	*popActionList=NULL;
bool						inSomeWindow=false;
bool						gotLaunchers=false;
int						popActionWindowYOffset=0;
LFSTK_buttonClass		*bgrs=NULL;
LFSTK_buttonClass		*bgls=NULL;
int						sidewid=0;
int						sidehite=0;

//atoms
Atom						WM_STATE=None;
Atom						NET_WM_WINDOW_TYPE_NORMAL=None;
Atom						NET_WM_STATE_HIDDEN=None;
Atom						NET_WM_WINDOW_TYPE_DIALOG=None;
Atom						NET_WM_WINDOW_TYPE=None;
Atom						NET_WM_STATE=None;
Atom						NET_WM_NAME=None;
Atom						UTF8_STRING=None;
Atom						NET_CURRENT_DESKTOP=None;
Atom						WM_CLASS=None;
Atom						NET_WM_PID=None;
Atom						NET_NUMBER_OF_DESKTOPS=None;
Atom						NET_ACTIVE_WINDOW=None;
Atom						NET_WM_DESKTOP=None;

const char				*possibleError="Unknown";

LFSTK_buttonClass		*taskbuttons[MAXTASKBUTTONS];
int						windowWidth=0;
LFSTK_findClass			*gFind;
bool						useTaskBar=false;

void sendNotify(std::string name,std::string message)//TODO//could be better
{
#ifdef _GOTNOTIFYSEND_
	std::string	com=std::string("notify-send -u low -t 2000 -i stock_dialog-info \"" + name + " " + message + " ...\"");
	//fprintf(stderr,"sendNotify com=>>%s<<\n",com.c_str());
	system(com.c_str());
#endif
}

void dropDesktopFile(const char *data,void *launcher)//TODO//
{
	char					*cleanstr=NULL;
	char					*command=NULL;
	char					*ptr=NULL;
	std::istringstream	stream(data);
	std::string			line;
	std::string			files="";

	while(std::getline(stream,line))
		{
			cleanstr=strdup(apc->globalLib->LFSTK_cleanString(line).c_str());
			files+="\""+std::string(cleanstr)+"\" ";
			if((strrchr(cleanstr,'.')!=NULL) && (strcmp(strrchr(cleanstr,'.'),".desktop")==0))//TODO//
				{
					asprintf(&command,"mkdir -p '%s';cp -nP '%s' '%s'",launchersDir.c_str(),cleanstr,launchersDir.c_str());
					ptr=strrchr(cleanstr,'/');
					sendNotify("Adding launcher ",++ptr);
					XSync(apc->display,false);
					sleep(1);
					system(command);
					freeAndNull(&command);
					apc->exitValue=0;
					apc->mainLoop=false;
					freeAndNull(&cleanstr);
					return;
				}
			else
				{
					freeAndNull(&cleanstr);
				}

		}
	if(launcher!=NULL)
		{
			launcherDataStruct lds=launchersArray.at((unsigned long)launcher);
			if(lds.inTerm==false)
				{
					asprintf(&command,"%s %s &",lds.exec.c_str(),files.c_str());//TODO//
				}
			else
				{
					asprintf(&command,"%s %s \"%s\" &",prefs.LFSTK_getCString("termcommand"),lds.exec.c_str(),files.c_str());
				}
			sendNotify("Running ",lds.exec);
			system(command);
			freeAndNull(&command);
		}
}

void setGadgetDetails(LFSTK_gadgetClass *gadget)
{
	gadget->LFSTK_setAlpha(1.0);
	gadget->LFSTK_setStyle(BEVELNONE);

//	if(useBG==true)
//		{
//			gadget->LFSTK_setTile(dockBGImage.c_str(),-1);
//			gadget->gadgetDetails.geomRelativeToMainWindow=true;
//		}
//	else
		{
			gadget->LFSTK_setTile(NULL,0);
			gadget->LFSTK_setGadgetColours(GADGETBG,"#00000000",launcherPreColour,"#00000000","#00000000");
			gadget->	LFSTK_setGadgetColourPair(NORMALCOLOUR,"#00000000","#00000000");
		}
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

Window doTreeWalkForClass(Window wind,std::string namecheck)
{
	Window			root;
	Window			parent;
	Window			*children;
	Window			thewin;
	unsigned int		n_children;
	unsigned long	winid;

	if (!XQueryTree(dockWindow->app->display,wind,&root,&parent,&children,&n_children))
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
							if(strcasecmp(namecheck.c_str(),pr.strlist.at(k).c_str())==0)//TODO//
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

	if (!XQueryTree(dockWindow->app->display,wind,&root,&parent,&children,&n_children))
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

void resizeDock(int wid,int hite)
{
	dockWindow->LFSTK_resizeWindow(wid,hite,true);
	dockBGWindow->LFSTK_resizeWindow(wid,hite,true);
	if(bgrs!=NULL)
		bgrs->LFSTK_moveGadget(wid-sidewid,0);
	dockWindow->LFSTK_clearWindow(true);	
	dockBGWindow->LFSTK_clearWindow(true);	
}

void moveDock(int extra)
{
	int	psize;
	int	px,py;

	psize=windowWidth+extra;
	py=mons->y;

	if(dockGravity==PANELSOUTH)
		py=mons->y+mons->h-iconWidth-extraSpace;
	px=((mons->w/2)-(psize/2))+mons->x;

	dockWindow->LFSTK_moveWindow(px,py,true);
	dockBGWindow->LFSTK_moveWindow(px,py,true);
}

std::string getWindowName(Window winid)
{
	propReturn	pr;
	char			*wname=NULL;
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

