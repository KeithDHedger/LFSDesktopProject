/*
 *
 * ©K. D. Hedger. Thu 13 Aug 16:55:34 BST 2015 kdhedger68713@gmail.com

 * This file (main.cpp) is part of LFSDesktop.

 * LFSDesktop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSDesktop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDesktop.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/shape.h>
#include <X11/StringDefs.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include <libudev.h>

#include <sys/inotify.h>
#include <limits.h>
#include <poll.h>

#include "config.h"
#include "globals.h"
#include "prefs.h"
#include "files.h"
#include "disks.h"
#include "graphics.h"

#define UNKNOWNARG -100

struct option		long_options[] =
{
					{"clean",0,0,'c'},
					{"tidy",0,0,'t'},
					{"theme",1,0,'a'},
					{"term-command",1,0,'x'},
					{"show-extension",0,0,'s'},
					{"font",1,0,'f'},
					{"fore-colour",1,0,'4'},
					{"back-colour",1,0,'b'},
					{"back-alpha",1,0,'A'},
					{"ignore",1,0,'i'},
					{"debug",0,0,'d'},
					{"version",0,0,'v'},
					{"help",0,0,'?'},
					{0,0,0,0}
};

bool				needsRefresh=true;
bool				popupLoop=true;
bool				retVal=false;
int					width=320;
int					hite=60;
LFSTK_windowClass	*wc;
Pixmap				pm[10][2];

void printhelp(void)
{
	printf("LFSDesktop - " VERSION "\n"
			"Usage: lfsdesktop [OPTION] ...\n" 
			" -c,--clean			Clean disk info/desktop cache data\n"
			" -t,--tidy				Tidy disk info/desktop cache data preserving existing\n"
			" -a,--theme			Set theme\n"
			" -x,--term-command		Set terminal command ( default xterm -e )\n"
			" -s,--show-extension	Show desktop file extension ( default no show )\n"
			" -f,--font				Font face ( Sans;0;0;10 - Fontname;Weight(0/1);Slant(0/1/2);Fontsize )\n"
			" -4,--fore-colour		Fore colour for label in RGB # notation, or colour name ( default #ffffff, '#' is compulsory for hex numbers )\n"
			" -b,--back-colour		Back colour for label in RGB # notation, or colour name ( default #000000, '#' is compulsory for hex numbers )\n"
			" -A,--back-alpha		Alpha for label between 0.0 and 1.0 ( default 1.0 )\n"
			" -i,--ignore			List of ';' seperated disk labels to NOT show on desktop\n"
			" -d,--debug			Debug\n"
			" -v,--version			output version information and exit\n"
			" -h,-?,--help			print this help\n\n"
			"Report bugs to kdhedger68713@gmail.com\n"
		  );
}

bool findIcon(int x,int y)
{

	for(int j=0;j<deskIconsCnt;j++)
		{
			if((x>=(deskIconsArray[j].x*gridSize+gridBorder))&&(x<=(deskIconsArray[j].x*gridSize+gridBorder)+iconSize)&&(y>=(deskIconsArray[j].y*gridSize+gridBorder))&&(y<=(deskIconsArray[j].y*gridSize+gridBorder)+iconSize))
				{
					fileDiskXPos=deskIconsArray[j].x;
					fileDiskYPos=deskIconsArray[j].y;
					foundDiskNumber=j;
					isDisk=!deskIconsArray[j].file;
					return(true);
				}
		}
	return(false);
}

void  alarmCallBack(int sig)
{
	signal(SIGALRM,SIG_IGN);
	needsRefresh=true;
	signal(SIGALRM,alarmCallBack);
	alarm(refreshRate);
}

bool setIconCallback(void *p,void* ud)
{
	if((long)ud==1)
		retVal=false;
	else
		retVal=true;

	return(false);
}

void doCustomIcon(bool useicon)
{
	XEvent				event;
	bool				firstrun=true;
	bool				retfromlib=true;
	if(useicon==true)
		{
			iconChooser->LFSTK_showWindow();
			iconChooser->LFSTK_setKeepAbove(true);

			while(retfromlib==true)
				{
					mappedListener *ml=iconChooser->LFSTK_getMappedListener(event.xany.window);
					if(ml!=NULL)
						retfromlib=ml->function(ml->gadget,&event,ml->type);

					XNextEvent(iconChooser->display,&event);
					switch(event.type)
						{
							case LeaveNotify:
								break;
							case Expose:
								iconChooser->LFSTK_clearWindow();
								if(firstrun==true)
									{
										firstrun=false;
										iconChooserEdit->LFSTK_setFocus();
									}
								break;

							case ConfigureNotify:
								iconChooser->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
								iconChooser->LFSTK_clearWindow();
								iconChooserEdit->LFSTK_resizeWindow(event.xconfigurerequest.width,24);
								iconChooserEdit->LFSTK_clearWindow();
								break;

							case ClientMessage:
							case SelectionNotify:
								if (event.xclient.message_type == XInternAtom(iconChooser->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(iconChooser->display, "WM_DELETE_WINDOW", 1))
									retfromlib=false;
								if(iconChooser->acceptDnd==true)
									iconChooser->LFSTK_handleDnD(&event);
						}
				}

			if(retVal==0)
				{
					deskIconsArray[foundDiskNumber].icon=strdup(iconChooserEdit->LFSTK_getBuffer()->c_str());
					fileCustomIcon=deskIconsArray[foundDiskNumber].icon;
					fileGotCustomIcon=true;
					deskIconsArray[foundDiskNumber].customicon=true;
				}

			iconChooser->LFSTK_hideWindow();
		}
	else
		{
			if(deskIconsArray[foundDiskNumber].icon!=NULL)
				free(deskIconsArray[foundDiskNumber].icon);
			deskIconsArray[foundDiskNumber].icon=NULL;
			deskIconsArray[foundDiskNumber].customicon=false;
			fileCustomIcon=NULL;
			fileGotCustomIcon=false;
		}


	if(isDisk==true)
		saveInfofile(DISKFOLDER,deskIconsArray[foundDiskNumber].label,NULL,NULL,deskIconsArray[foundDiskNumber].uuid,(char*)iconDiskType[deskIconsArray[foundDiskNumber].iconhint],deskIconsArray[foundDiskNumber].x,deskIconsArray[foundDiskNumber].y,foundDiskNumber);
	else
		saveInfofile(CACHEFOLDER,deskIconsArray[foundDiskNumber].label,deskIconsArray[foundDiskNumber].mime,deskIconsArray[foundDiskNumber].mountpoint,NULL,NULL,deskIconsArray[foundDiskNumber].x,deskIconsArray[foundDiskNumber].y,foundDiskNumber);

	needsRefresh=true;
}

void pushedButton(Widget w,XtPointer data,XtPointer  garbage)
{
	long	what=(long)data;

	switch(what)
		{
			case BUTTONOPEN:
			case BUTTONMOUNT:
			case BUTTONUNMOUNT:
			case BUTTONEJECT:
				mountDisk(what);
				break;
			case BUTTONADDICON:
				doCustomIcon(true);
				break;
			case BUTTONREMOVEICON:
				doCustomIcon(false);
				break;
		}

	needsRefresh=true;
	popupLoop=false;
}

bool inWindow(void)
{
	Window			root_return;
	Window			child_return;
	int				root_x_return;
	int				root_y_return;
	int				win_x_return;
	int				win_y_return;
	unsigned int	mask_return;

	if(XQueryPointer(wc->display,wc->rootWindow,&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return)==true)
		{
			const geometryStruct *g=wc->LFSTK_getWindowGeom();
			if((root_x_return>g->x) && (root_x_return<(g->x+(int)(g->w))) && (root_y_return>g->y) && (root_y_return<(g->y+(int)(g->h))))
				return(true);
		}
	return(false);
}

bool pushedButtonCB(void *p,void* ud)
{
	long	what=(long)ud;

	diskWindow->LFSTK_hideWindow();
	fileWindow->LFSTK_hideWindow();
	switch(what)
		{
			case BUTTONOPEN:
			case BUTTONMOUNT:
			case BUTTONUNMOUNT:
			case BUTTONEJECT:
				mountDisk(what);
				break;
			case BUTTONADDICON:
				doCustomIcon(true);
				break;
			case BUTTONREMOVEICON:
				doCustomIcon(false);
				break;
		}

	needsRefresh=true;
	popupLoop=false;
	return(false);
}

void doPopUp(int x,int y)
{
	XEvent	event;
	LFSTK_windowClass *wc;

	if(findIcon(x,y)==false)
		return;

	popupLoop=true;
	if(isDisk==true)
		wc=diskWindow;
	else
		wc=fileWindow;
	wc->LFSTK_moveWindow(x-10,y-10,true);
	wc->LFSTK_showWindow(true);
	wc->LFSTK_clearWindow();

	while(popupLoop==true)
		{
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			XNextEvent(wc->display,&event);
			switch(event.type)
				{
					case LeaveNotify:
						if(event.xany.window==wc->window)
							popupLoop=false;
						break;
					case Expose:
						wc->LFSTK_clearWindow();
						break;
					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						wc->LFSTK_clearWindow();
						break;
				}
		}
	wc->LFSTK_hideWindow();
}

void parseFontString(const char *fontstr)
{
	char	*string=strdup(fontstr);
	char	*str;

	weight=CAIRO_FONT_WEIGHT_NORMAL;
	slant=CAIRO_FONT_SLANT_NORMAL;
	fontSize=10;
	if(fontName!=NULL)
		free(fontName);
	fontName=strdup("Sans");

	str=strtok(string,":");
	while(1)
		{
			bool	found=false;
			if(str==NULL)
				break;
			if(strcasecmp(str,"bold")==0)
				{
					weight=CAIRO_FONT_WEIGHT_BOLD;
					found=true;
				}
			if(strcasecmp(str,"italic")==0)
				{
					slant=CAIRO_FONT_SLANT_ITALIC;
					found=true;
				}
			if(strcasestr(str,"size=")!=NULL)
				{
					fontSize=atoi(&str[5]);
					found=true;
				}

			if(found==false)
				{
					if(fontName!=NULL)
						free(fontName);
					fontName=strdup(str);
				}
			str=strtok(NULL,":");
		}
	free(string);
}


void setFontEtc(void)
{
	//char	*ptr;
	XColor	tc,sc;

	parseFontString(fontFace);

	XAllocNamedColor(display,DefaultColormap(display,screen),backCol,&sc,&tc);
	labelBackground=sc.pixel;
	XAllocNamedColor(display,DefaultColormap(display,screen),foreCol,&sc,&tc);
	labelForeground=sc.pixel;

	backColour.r=((labelBackground>>16) & 0xff)/256.0;
	backColour.g=((labelBackground>>8) & 0xff)/256.0;
	backColour.b=((labelBackground>>0) & 0xff)/256.0;
	backColour.a=strtod(backAlpha,NULL);

	foreColour.r=((labelForeground>>16) & 0xff)/256.0;
	foreColour.g=((labelForeground>>8) & 0xff)/256.0;
	foreColour.b=((labelForeground>>0) & 0xff)/256.0;
	foreColour.a=1.0;
}

#if 0
//struct propertyStruct
//{
//	unsigned char	*data;
//	int				format;
//	int				nitems;
//	Atom			type;
//	char			*mimeType;
//};

//enum {XDNDENTER=0,XDNDPOSITION,XDNDSTATUS,XDNDTYPELIST,XDNDACTIONCOPY,XDNDDROP,XDNDLEAVE,XDNDFINISHED,XDNDSELECTION,XDNDPROXY,XA_CLIPBOARD,XA_COMPOUND_TEXT,XA_UTF8_STRING,XA_TARGETS,PRIMARY,DNDATOMCOUNT};
Atom					dNdAtoms[DNDATOMCOUNT];
std::map<std::string,int> dNdTypes;
Atom					toBeRequested;
Window					sourceWindow;
int						xDnDVersion;
bool					acceptDnd;
//Convert an atom name in to a std::string
std::string getAtomName(Atom a)
{
	if(a == None)
		return "None";
	else
		return XGetAtomName(display,a);
}

Atom pickTargetFromList(Atom* atom_list,int nitems)
{
	Atom to_be_requested=None;
	//This is higger than the maximum priority.
	int priority=INT_MAX;

	for(int i=0; i < nitems; i++)
		{
			std::string atom_name=getAtomName(atom_list[i]);
			//See if this data type is allowed and of higher priority (closer to zero) than the present one.
			if(dNdTypes.find(atom_name)!= dNdTypes.end())
				if(priority > dNdTypes[atom_name])
					{
						priority=dNdTypes[atom_name];
						to_be_requested=atom_list[i];
					}
		}
	return to_be_requested;
}

// Finds the best target given a local copy of a property.
Atom pickTargetFromTargets(propertyStruct* p)
{
	//The list of targets is a list of atoms, so it should have type XA_ATOM but it may have the type TARGETS instead.
	if((p->type!=XA_ATOM && p->type!=dNdAtoms[XA_TARGETS]) || p->format!=32)
		{
			//This would be really broken. Targets have to be an atom list and applications should support this. Nevertheless,
			//some seem broken (MATLAB 7, for instance), so ask for STRING next instead as the lowest common denominator
			if(dNdTypes.count("STRING"))
				return(XA_STRING);
			else
				return None;
		}
	else
		{
			Atom *atom_list=(Atom*)p->data;
			return pickTargetFromList(atom_list,p->nitems);
		}
}

// Finds the best target given up to three atoms provided (any can be None).
// Useful for part of the Xdnd protocol.
Atom pickTargetFromAtoms(Atom t1, Atom t2, Atom t3)
{
	Atom atoms[3];
	int  n=0;

	if(t1!=None)
		atoms[n++]=t1;

	if(t2!=None)
		atoms[n++]=t2;

	if(t3!=None)
		atoms[n++]=t3;

	return pickTargetFromList(atoms, n);
}

propertyStruct* readProperty(Window src,Atom property)
{
	Atom			actual_type;
	int				actual_format;
	unsigned long	nitems;
	unsigned long	bytes_after;
	unsigned char	*ret=0;
	propertyStruct	*props=new propertyStruct;

	int read_bytes=1024;

	//Keep trying to read the property until there are no bytes unread.
	do
		{
			if(ret!=0)
				XFree(ret);
			XGetWindowProperty(display,src,property,0,read_bytes,False,AnyPropertyType,&actual_type,&actual_format, &nitems,&bytes_after,&ret);

			read_bytes *= 2;
		}
	while(bytes_after!=0);

	props->data=ret;
	props->format=actual_format;
	props->nitems=nitems;
	props->type=actual_type;
	props->mimeType=NULL;
	return props;
}

/**
 * Handle a dNd event.
 * \param event The event.
 * \note Only for line edit class gadgets for now.
 */
void handleDnD(XEvent *event)
{
//printf("-------------\n");
	if(event->type == ClientMessage)
		{
	printf("000000000000000000\n");
			if(event->xclient.message_type==dNdAtoms[XDNDENTER])
				{
					xDnDVersion=(event->xclient.data.l[1] >> 24);
					//more than three
					Window source=event->xclient.data.l[0];
					if(event->xclient.data.l[1] & 1)
						{
							//Fetch the list of possible conversions
							propertyStruct *props=readProperty(source,dNdAtoms[XDNDTYPELIST]);
							toBeRequested=pickTargetFromTargets(props);
							XFree(props->data);
						}
					else
						{
							//Use the available list
							toBeRequested=pickTargetFromAtoms(event->xclient.data.l[2],event->xclient.data.l[3],event->xclient.data.l[4]);
						}
				}

			if(event->xclient.message_type == dNdAtoms[XDNDPOSITION])
				{
					//Xdnd: reply with an XDND status message
					//dropGadget=this->LFSTK_findGadgetByPos(event->xclient.data.l[2] >> 16,event->xclient.data.l[2] & 0xffff);

					XClientMessageEvent m;
					memset(&m, sizeof(m), 0);
					m.type=ClientMessage;
					m.display=event->xclient.display;
					m.window=event->xclient.data.l[0];
					m.message_type=dNdAtoms[XDNDSTATUS];
					m.format=32;
					m.data.l[0]=rootWin;
					//m.data.l[1]=((toBeRequested!=None) && (this->dropGadget!=NULL) );
					m.data.l[2]=0; //Specify an empty rectangle
					m.data.l[3]=0;
					m.data.l[4]=dNdAtoms[XDNDACTIONCOPY]; //We only accept copying anyway.

					XSendEvent(display,event->xclient.data.l[0],False,NoEventMask,(XEvent*)&m);
					XFlush(display);
				}

			if(event->xclient.message_type == dNdAtoms[XDNDDROP])
				{
//					if((toBeRequested == None) || (dropGadget==NULL))
//						{
//							//It's sending anyway, despite instructions to the contrary.
//							//So reply that we're not interested.
//							XClientMessageEvent m;
//							memset(&m, sizeof(m), 0);
//							m.type=ClientMessage;
//							m.display=event->xclient.display;
//							m.window=event->xclient.data.l[0];
//							m.message_type=dNdAtoms[XDNDFINISHED];
//							m.format=32;
//							m.data.l[0]=rootWin;
//							m.data.l[1]=0;
//							m.data.l[2]=None; //Failed.
//							XSendEvent(display,event->xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
//						}
//					else
//						{
							sourceWindow=event->xclient.data.l[0];
							if(xDnDVersion >= 1)
								XConvertSelection(display, dNdAtoms[XDNDSELECTION],toBeRequested,dNdAtoms[PRIMARY],rootWin, event->xclient.data.l[2]);
							else
								XConvertSelection(display, dNdAtoms[XDNDSELECTION],toBeRequested,dNdAtoms[PRIMARY],rootWin, CurrentTime);
//						}
				}
		}
	if(event->type == SelectionNotify)
		{
			Atom target=event->xselection.target;

			if(event->xselection.property == None)
				{
					return;
				}
			else
				{
					propertyStruct *myprops=readProperty(rootWin,dNdAtoms[PRIMARY]);

					myprops->mimeType=XGetAtomName(display,target);
					//If we're being given a list of targets (possible conversions)
					if(target == dNdAtoms[XA_TARGETS])
						{
							toBeRequested=pickTargetFromTargets(myprops);

							if(toBeRequested == None)
								return;
							else //Request the data type we are able to select
								XConvertSelection(display,dNdAtoms[XDNDSELECTION],toBeRequested,dNdAtoms[XDNDSELECTION],rootWin,CurrentTime);
						}
					else if(target==toBeRequested)
						{
							//if(this->dropGadget!=NULL)
							//	this->dropGadget->LFSTK_dropData(myprops);

							//Reply OK.
							XClientMessageEvent m;
							memset(&m, sizeof(m), 0);
							m.type=ClientMessage;
							m.display=display;
							m.window=sourceWindow;
							m.message_type=dNdAtoms[XDNDFINISHED];
							m.format=32;
							m.data.l[0]=rootWin;//w;
							m.data.l[1]=1;
							m.data.l[2]=dNdAtoms[XDNDACTIONCOPY]; //We only ever copy.

							XSendEvent(display,sourceWindow,False,NoEventMask,(XEvent*)&m);
							XSync(display, False);
						}
					else
						return;

					XFree(myprops->data);
					XFree(myprops->mimeType);
				}
		}
}


void initDnD(void)
{
//Announce XDND support
	Atom XdndAware=XInternAtom(display,"XdndAware",false);
	Atom version=5;
//	if(XdndAware!=None)
		XChangeProperty(display,rootWin,XdndAware,XA_ATOM,32,PropModeReplace,(unsigned char*)&version,1);

	dNdAtoms[XDNDENTER]=XInternAtom(display,"XdndEnter",false);
	dNdAtoms[XDNDPOSITION]=XInternAtom(display,"XdndPosition",false);
	dNdAtoms[XDNDSTATUS]=XInternAtom(display,"XdndStatus",false);
	dNdAtoms[XDNDTYPELIST]=XInternAtom(display,"XdndTypeList",false);
	dNdAtoms[XDNDACTIONCOPY]=XInternAtom(display,"XdndActionCopy",false);
	dNdAtoms[XDNDDROP]=XInternAtom(display,"XdndDrop",false);
	dNdAtoms[XDNDLEAVE]=XInternAtom(display,"XdndLeave",false);
	dNdAtoms[XDNDFINISHED]=XInternAtom(display,"XdndFinished",false);
	dNdAtoms[XDNDSELECTION]=XInternAtom(display,"XdndSelection",false);
	dNdAtoms[XDNDPROXY]=XInternAtom(display,"XdndProxy",false);
	dNdAtoms[XA_CLIPBOARD]=XInternAtom(display,"CLIPBOARD",false);
	dNdAtoms[XA_COMPOUND_TEXT]=XInternAtom(display,"COMPOUND_TEXT",false);
	dNdAtoms[XA_UTF8_STRING]=XInternAtom(display,"UTF8_STRING",false);
	dNdAtoms[XA_TARGETS]=XInternAtom(display,"TARGETS",false);
	dNdAtoms[PRIMARY]=XInternAtom(display,"PRIMARY",false);

	acceptDnd=true;
	toBeRequested=None;
	sourceWindow=None;
	xDnDVersion=0;
//	dropGadget=NULL;

	dNdTypes["text/plain"]=1;
	dNdTypes["text/uri-list"]=2;
}

#endif

int main(int argc,char **argv)
{
	int					c;
	XEvent				ev;
	char				*command;
	FILE				*fw;
	char				*path;
	char				buffer[MAXBUFFER];
	pid_t				pid=getpid();
	bool				done=true;
	Time				time=0;
	bool				firstClick=false;
	bool				foundIcon=false;
	pollfd				pollstruct;
	int					fd;
	pollfd				polldisks;
	int					fhfordisks;
	long				numRead=0;
	int					oldx=-1,oldy=-1;
	bool				buttonDown=false;
	int					oldboxx=-1,oldboxy=-1;
	bool				dragging=false;
	FILE				*fp;
	bool				dotidy=false;
	const char			*diskLabelData[]={"Mount","Unmount","Eject","Open","Custom Icon","Remove Icon",NULL};
	const char			*diskThemeIconData[]={"drive-harddisk","media-eject","media-eject","document-open","list-add","list-remove"};
	char				*diskIconData[BUTTONREMOVEICON];
	LFSTK_buttonClass	*diskButtons[BUTTONREMOVEICON];
	LFSTK_buttonClass	*fileButtons[3];
	int					buttoncnt=0;
	int					sy=0;

	asprintf(&path,"%s/.config/LFS/pidfile",getenv("HOME"));
	fw=fopen(path,"r");
	if(fw!=NULL)
		{
			fgets(buffer,MAXBUFFER,fw);
			if(atoi(buffer)>1)
				kill(atoi(buffer),SIGKILL);
			fclose(fw);
		}

	fw=fopen(path,"w");
	if(fw!=NULL)
		{
			fprintf(fw,"%i",pid);
			fclose(fw);
		}
	free(path);
	nice(15);

	cairo_surface_t *sfc;

	signal(SIGALRM,alarmCallBack);

	done=true;
	asprintf(&diskInfoPath,"%s/.config/LFS/disks",getenv("HOME"));
	asprintf(&command,"mkdir -p %s 2>&1 >/dev/null",diskInfoPath);
	system(command);
	free(command);

	asprintf(&cachePath,"%s/.config/LFS/cache",getenv("HOME"));
	asprintf(&command,"mkdir -p %s 2>&1 >/dev/null",cachePath);
	system(command);
	free(command);

	asprintf(&desktopPath,"%s/Desktop",getenv("HOME"));

	asprintf(&prefsPath,"%s/.config/LFS/lfsdesktop.rc",getenv("HOME"));
	asprintf(&fontFace,"Sans;0;0;10");
	asprintf(&foreCol,"#ffffff");
	asprintf(&backCol,"#000000");
	asprintf(&backAlpha,"0x00");
	showSuffix=false;
	asprintf(&terminalCommand,"xterm -e ");

	loadVarsFromFile(prefsPath,desktopPrefs);

	while (1)
		{
			int option_index=0;
			c=getopt_long (argc,argv,"v?hctdsma:x:f:4:b:i:A:",long_options,&option_index);
			if (c==-1)
				break;

			switch (c)
				{
				case 'd':
					debugDeskFlag=true;
					break;

				case 'c':
					asprintf(&command,"rm %s/*",diskInfoPath);
					system(command);
					free(command);
					asprintf(&command,"rm %s/*",cachePath);
					system(command);
					free(command);
					break;

				case 't':
					dotidy=true;
					break;

				case 'a':
					if(iconTheme!=NULL)
						free(iconTheme);
					iconTheme=strdup(optarg);
					break;

				case 'x':
					if(terminalCommand!=NULL)
						free(terminalCommand);
					terminalCommand=strdup(optarg);
					break;

				case 's':
					showSuffix=true;
					break;

				case 'f':
					if(fontFace!=NULL)
						free(fontFace);
					fontFace=strdup(optarg);
					break;

				case '4':
					if(foreCol!=NULL)
						free(foreCol);
					foreCol=strdup(optarg);
					break;

				case 'b':
					if(backCol!=NULL)
						free(backCol);
					backCol=strdup(optarg);
					break;

				case 'A':
					if(backAlpha!=NULL)
						free(backAlpha);
					backAlpha=strdup(optarg);
					break;

				case 'i':
					if(ignores!=NULL)
						free(ignores);
					ignores=strdup(optarg);
					break;

				case 'v':
					printf("lfsdesktop %s\n",VERSION);
					return 0;
					break;

				case '?':
				case 'h':
					printhelp();
					return 0;
					break;

				default:
					fprintf(stderr,"?? Unknown argument ??\n");
					return UNKNOWNARG;
					break;
				}
		}

	if (optind < argc)
		{
			printf("non-option ARGV-elements: ");
			while (optind < argc)
				printf("%s ",argv[optind++]);
			printf("\n");
		}

	display=XOpenDisplay(NULL);
	if(display==NULL)
		exit(1);

	XSynchronize(display,true);
	screen=DefaultScreen(display);
	displayWidth=DisplayWidth(display,screen);
	displayHeight=DisplayHeight(display,screen);
	rootWin=DefaultRootWindow(display);
	visual=DefaultVisual(display,screen);
	cm=DefaultColormap(display,screen);

	createDesktopWindow();


//initDnD();

//disks
	diskWindow=new LFSTK_windowClass(0,0,64,128,"xxx",true,true);

	for(int j=BUTTONMOUNT;j<=BUTTONREMOVEICON;j++)
		{
			const char *iconpath=NULL;
			iconpath=diskWindow->globalLib->LFSTK_findThemedIcon(iconTheme,diskThemeIconData[j-1],"");
			if(iconpath!=NULL)
				diskIconData[j-1]=strdup(iconpath);
			else
				diskIconData[j-1]=NULL;
		}

	buttoncnt=0;
	sy=0;
	while(diskLabelData[buttoncnt]!=NULL)
		{
			if(buttoncnt!=BUTTONOPEN-1)
				{
					diskButtons[buttoncnt]=new LFSTK_buttonClass(diskWindow,diskLabelData[buttoncnt],0,sy,GADGETWIDTH,24,NorthWestGravity);
					diskButtons[buttoncnt]->LFSTK_setCallBack(NULL,pushedButtonCB,(void*)(long)(buttoncnt+1));
					if(diskIconData[buttoncnt]==NULL)
						{
							diskButtons[buttoncnt]->LFSTK_setLabelGravity(LEFT);
						}
					else
						{
							diskButtons[buttoncnt]->LFSTK_setImageFromPath(diskIconData[buttoncnt],LEFT,true);
							//diskButtons[buttoncnt]->LFSTK_setLabelGravity(LEFT);
						}
					sy+=GADGETHITE;
				}
			buttoncnt++;
		}
	diskWindow->LFSTK_resizeWindow(GADGETWIDTH,sy,true);
	diskWindow->LFSTK_showWindow(true);
	diskWindow->LFSTK_hideWindow();
//files
	fileWindow=new LFSTK_windowClass(0,0,64,128,"xxx",true,true);
	sy=0;
	buttoncnt=3;
	while(diskLabelData[buttoncnt]!=NULL)
		{
			fileButtons[buttoncnt]=new LFSTK_buttonClass(fileWindow,diskLabelData[buttoncnt],0,sy,GADGETWIDTH,24,NorthWestGravity);
			fileButtons[buttoncnt]->LFSTK_setCallBack(NULL,pushedButtonCB,(void*)(long)(buttoncnt+1));
			if(diskIconData[buttoncnt]==NULL)
				{
					fileButtons[buttoncnt]->LFSTK_setLabelGravity(LEFT);
				}
			else
				{
					fileButtons[buttoncnt]->LFSTK_setImageFromPath(diskIconData[buttoncnt],LEFT,true);
					//fileButtons[buttoncnt]->LFSTK_setLabelGravity(LEFT);
				}
			buttoncnt++;
			sy+=GADGETHITE;
		}
	fileWindow->LFSTK_resizeWindow(GADGETWIDTH,sy,true);
	fileWindow->LFSTK_showWindow(true);
	fileWindow->LFSTK_hideWindow();

//icon chooser
	iconChooser=new LFSTK_windowClass(0,0,width,hite,"Enter Path To Icon",false);
	LFSTK_buttonClass	*bc;

	iconChooserEdit=new LFSTK_lineEditClass(iconChooser,"",0,0,width,24,NorthWestGravity);

	bc=new LFSTK_buttonClass(iconChooser,"Apply",4,24+4+4,75,24,SouthWestGravity);
	bc->LFSTK_setCallBack(NULL,setIconCallback,(void*)1);
	bc=new LFSTK_buttonClass(iconChooser,"Cancel",width-4-75,24+4+4,75,24,SouthEastGravity);
	bc->LFSTK_setCallBack(NULL,setIconCallback,(void*)2);

	sfc=cairo_xlib_surface_create(display,drawOnThis,visual,displayWidth,displayHeight);
	cairo_xlib_surface_set_size(sfc,displayWidth,displayHeight);
	cr=cairo_create(sfc);

	gc=XCreateGC(display,drawOnThis,0,NULL);
	XSetFillStyle(display,gc,FillSolid);
	XSelectInput(display,rootWin,StructureNotifyMask |ExposureMask | ButtonPress| ButtonReleaseMask|PointerMotionMask | EnterWindowMask | LeaveWindowMask);

	blackColor=BlackPixel(display,screen);
	whiteColor=WhitePixel(display,screen);

	hcreate(100);

	setFontEtc();

	xCnt=displayWidth/gridSize;
	yCnt=displayHeight/gridSize;

	xySlot=(int**)malloc(xCnt*sizeof(int*));
	for(int j=0; j<xCnt; j++)
		xySlot[j]=(int*)malloc(yCnt*sizeof(int));

	for(int yy=0; yy<yCnt; yy++)
		for(int xx=0; xx<xCnt; xx++)
			xySlot[xx][yy]=0;

	fd=inotify_init();
	pollstruct.fd =fd;
	pollstruct.events=POLLIN;
	pollstruct.revents=0;
	inotify_add_watch(fd,desktopPath,IN_CREATE|IN_DELETE|IN_MODIFY);

	fhfordisks=inotify_init();
	polldisks.fd=fhfordisks;
	polldisks.events=POLLIN;
	polldisks.revents=0;
	inotify_add_watch(fhfordisks,"/dev/disk/by-uuid",IN_CREATE|IN_DELETE|IN_MODIFY);

	asprintf(&command,"findmnt -nf /|awk '{print $2}'");
	fp=popen(command,"r");
	free(command);
	if(fp!=NULL)
		{
			buffer[0]=0;
			fgets(buffer,MAXBUFFER,fp);
			sscanf(buffer,"%ms",&rootDev);
			pclose(fp);
		}

	deskIconsArray=(deskIcons*)calloc(deskIconsMaxCnt,sizeof(deskIcons));

//home icon
	deskIconsCnt=HOMEDATA;
	asprintf(&command,"%s/Home",cachePath);
	if(fileExists(command)==0)
		readDesktopFile("Home");
	else
		{
			deskIconsArray[HOMEDATA].x=0;
			deskIconsArray[HOMEDATA].y=0;
			deskIconsArray[HOMEDATA].installed=true;
			deskIconsArray[HOMEDATA].label=strdup("Home");
			deskIconsArray[HOMEDATA].mime=strdup("inode-directory");
			deskIconsArray[HOMEDATA].mountpoint=strdup(getenv("HOME"));
			saveInfofile(CACHEFOLDER,deskIconsArray[HOMEDATA].label,deskIconsArray[HOMEDATA].mime,deskIconsArray[HOMEDATA].mountpoint,NULL,NULL,deskIconsArray[HOMEDATA].x,deskIconsArray[HOMEDATA].y,HOMEDATA);
		}
	deskIconsArray[HOMEDATA].file=true;
	deskIconsArray[HOMEDATA].iconhint=COMPUTER;
	xySlot[deskIconsArray[HOMEDATA].x][deskIconsArray[HOMEDATA].y]=1;
	free(command);

//computer icon
	deskIconsCnt=ROOTDATA;
	asprintf(&command,"%s/Computer",cachePath);
	if(fileExists(command)==0)
		readDesktopFile("Computer");
	else
		{
			deskIconsArray[ROOTDATA].x=0;
			deskIconsArray[ROOTDATA].y=1;
			deskIconsArray[ROOTDATA].installed=true;
			deskIconsArray[ROOTDATA].label=strdup("Computer");
			deskIconsArray[ROOTDATA].mime=strdup("computer");
			deskIconsArray[ROOTDATA].mountpoint=strdup("/");
			saveInfofile(CACHEFOLDER,deskIconsArray[ROOTDATA].label,deskIconsArray[ROOTDATA].mime,deskIconsArray[ROOTDATA].mountpoint,NULL,NULL,deskIconsArray[ROOTDATA].x,deskIconsArray[ROOTDATA].y,ROOTDATA);
		}
	deskIconsArray[ROOTDATA].file=true;
	deskIconsArray[ROOTDATA].iconhint=COMPUTER;
	xySlot[deskIconsArray[ROOTDATA].x][deskIconsArray[ROOTDATA].y]=1;
	free(command);

	deskIconsCnt=RESERVED;

	fillDesk();

	if(dotidy==true)
		{
			command=oneLiner("rm \"%s\"/*;rm \"%s\"/*",cachePath,diskInfoPath);
			for (int j=0;j<deskIconsCnt;j++)
				{
					if(deskIconsArray[j].file==true)
						saveInfofile(CACHEFOLDER,deskIconsArray[j].label,deskIconsArray[j].mime,deskIconsArray[j].mountpoint,NULL,NULL,deskIconsArray[j].x,deskIconsArray[j].y,j);
					else
						saveInfofile(DISKFOLDER,deskIconsArray[j].label,NULL,NULL,deskIconsArray[j].uuid,(char*)iconDiskType[deskIconsArray[j].iconhint],deskIconsArray[j].x,deskIconsArray[j].y,j);
			
					fileDiskLabel=NULL;
					fileDiskUUID=NULL;
					fileDiskXPos=-1;
					fileDiskYPos=-1;
					fileDiskType=NULL;
				}
		}

	alarm(refreshRate);
	XdbeSwapBuffers(display,&swapInfo,1);

	while(done)
		{
			if(needsRefresh==true)
				{
					int ret=poll(&pollstruct,POLLIN,20);
					if(ret!=0)
						{
							numRead=read(fd,buffer,MAXBUFFER);
							if(numRead>0)
								fillDesk();
						}
					
					ret=poll(&polldisks,POLLIN,20);
					if(ret!=0)
						{
							numRead=read(fhfordisks,buffer,MAXBUFFER);
							if(numRead>0)
								fillDesk();
						}

					drawIcons();
					XdbeSwapBuffers(display,&swapInfo,1);
					needsRefresh=false;
				}

			if(dragging==false)
				usleep(25000);

			//XCheckWindowEvent(display,rootWin,SubstructureRedirectMask|StructureNotifyMask|ButtonPress|ButtonReleaseMask|PointerMotionMask | ExposureMask | EnterWindowMask | LeaveWindowMask|StructureNotifyMask,&ev);
			XCheckWindowEvent(display,rootWin,ButtonPress|ButtonReleaseMask|PointerMotionMask,&ev);
			switch(ev.type)
				{
			//	case SelectionNotify:
			//	case ClientMessage:
			//	handleDnD(&ev);
			//	printf(">>>>>>>>>>>>>\n");
			//	break;

				case ButtonPress:
				//debugstr("button down");
					if(ev.xbutton.button==Button3)
						{
							doPopUp(ev.xbutton.x,ev.xbutton.y);
							break;
						}
					if(ev.xbutton.button!=Button1)
						break;
					dragging=true;
					buttonDown=true;
					if(firstClick==false)
						{
							firstClick=true;
							time=ev.xbutton.time;
						}
					else
						{
							firstClick=false;
							if(ev.xbutton.time-time<800)
								{
									if(isDisk==true)
										mountDisk(BUTTONMOUNT);
									else
										mountDisk(BUTTONOPEN);
									needsRefresh=true;
								}
							else
								{
									time=ev.xbutton.time;
									firstClick=true;
								}
						}

					if(foundIcon==false)
						{
							oldx=-1;
							oldy=-1;
							foundDiskNumber=-1;
							foundIcon=findIcon(ev.xbutton.x,ev.xbutton.y);
						}

					if(foundIcon==true)
						{
							oldx=fileDiskXPos;
							oldy=fileDiskYPos;
							oldboxx=ev.xbutton.x;
							oldboxy=ev.xbutton.y;
						}
					else
						{
							fileDiskLabel=NULL;
							fileDiskUUID=NULL;
							fileDiskType=NULL;
							oldx=-1;
							oldy=-1;
							foundDiskNumber=-1;
						}

					break;
				case ButtonRelease:
					if(ev.xbutton.button!=Button1)
							break;

					dragging=false;
					buttonDown=false;
					if(foundIcon==true)
						{
							int newx,newy;
							newx=(ev.xbutton.x-gridBorder)/gridSize;
							newy=(ev.xbutton.y-gridBorder)/gridSize;
							foundIcon=false;
							if(xySlot[newx][newy]==0)
								{
									xySlot[oldx][oldy]=0;
									deskIconsArray[foundDiskNumber].x=newx;
									deskIconsArray[foundDiskNumber].y=newy;
									xySlot[newx][newy]=1;
									
									if(isDisk==true)
										saveInfofile(DISKFOLDER,deskIconsArray[foundDiskNumber].label,NULL,NULL,deskIconsArray[foundDiskNumber].uuid,(char*)iconDiskType[deskIconsArray[foundDiskNumber].iconhint],newx,newy,foundDiskNumber);
									else
										saveInfofile(CACHEFOLDER,deskIconsArray[foundDiskNumber].label,deskIconsArray[foundDiskNumber].mime,deskIconsArray[foundDiskNumber].mountpoint,NULL,NULL,newx,newy,foundDiskNumber);
									fileDiskLabel=NULL;
									fileDiskUUID=NULL;
									fileDiskXPos=-1;
									fileDiskYPos=-1;
									fileDiskType=NULL;
									needsRefresh=true;
								}

							XSetForeground(display,gc,0);
							XSetFillStyle(display,gc,FillSolid);
							newx=((ev.xbutton.x-gridBorder)/gridSize)*gridSize+(gridBorder/2);
							newy=((ev.xbutton.y-gridBorder)/gridSize)*gridSize+(gridBorder/2);
							XDrawRectangle(display,rootWin,gc,newx,newy,gridSize,gridSize);

							fileDiskLabel=NULL;
							fileDiskUUID=NULL;
							fileDiskType=NULL;
							oldx=-1;
							oldy=-1;
							alarm(refreshRate);
						}

					break;
				case MotionNotify:
				{
					int newboxx,newboxy;

					if(foundIcon==true && buttonDown==true)
						{
						//debugstr("motion");
							alarm(0);
							if((ev.xmotion.x>oldboxx+iconSize) || (ev.xmotion.x<oldboxx-iconSize) || (ev.xmotion.y>oldboxy+iconSize) || (ev.xmotion.y<oldboxy-iconSize))
								{
									int obx,oby;

									XSetForeground(display,gc,0);
									XSetFillStyle(display,gc,FillSolid);
									obx=((oldboxx-gridBorder)/gridSize)*gridSize+(gridBorder/2);
									oby=((oldboxy-gridBorder)/gridSize)*gridSize+(gridBorder/2);
									XDrawRectangle(display,rootWin,gc,obx,oby,gridSize,gridSize);

									oldboxx=ev.xmotion.x;
									oldboxy=ev.xmotion.y;

									//XSetForeground(display,gc,labelBackground);
									XSetForeground(display,gc,whiteColor);
									XSetFillStyle(display,gc,FillSolid);
									newboxx=((ev.xmotion.x-gridBorder)/gridSize)*gridSize+(gridBorder/2);
									newboxy=((ev.xmotion.y-gridBorder)/gridSize)*gridSize+(gridBorder/2);
									XDrawRectangle(display,rootWin,gc,newboxx,newboxy,gridSize,gridSize);
								}
						}
				}
				break;

				default:
					break;
				}
			ev.type=-1;
		}

	XClearWindow(display,rootWin);
	XCloseDisplay(display);
	for(int j=0; j<xCnt; j++)
		free(xySlot[j]);

	free(diskInfoPath);
	free(cachePath);
	free(prefsPath);
	free(desktopPath);
	return 0;
}
