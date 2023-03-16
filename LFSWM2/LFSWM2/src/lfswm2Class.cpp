/*
 *
 * ©K. D. Hedger. Fri 29 Jul 13:41:17 BST 2022 keithdhedger@gmail.com

 * This file (lfswm2Class.cpp) is part of LFSWM2.

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
 
#include <stdio.h>
#include <stdlib.h>
#define XK_MISCELLANY

#include <X11/keysymdef.h>
#include <X11/Xlib.h>

#include "lfswm2Class.h"

const char *atomNames[]={"_NET_WM_WINDOW_TYPE_MENU","_NET_ACTIVE_WINDOW","_NET_CLIENT_LIST","_NET_CLIENT_LIST_STACKING","_NET_CLOSE_WINDOW","_NET_CURRENT_DESKTOP","_NET_DESKTOP_GEOMETRY","_NET_DESKTOP_VIEWPORT","_NET_FRAME_EXTENTS","_NET_NUMBER_OF_DESKTOPS","_NET_REQUEST_FRAME_EXTENTS","_NET_SUPPORTED","_NET_SUPPORTING_WM_CHECK","_NET_WM_ACTION_CHANGE_DESKTOP","_NET_WM_ACTION_CLOSE","_NET_WM_ACTION_FULLSCREEN","_NET_WM_ACTION_MINIMIZE","_NET_WM_ACTION_MAXIMIZE_HORZ","_NET_WM_ACTION_MAXIMIZE_VERT","_NET_WM_ACTION_RESIZE","_NET_WM_ALLOWED_ACTIONS","_NET_WM_DESKTOP","_NET_WM_ICON_NAME","_NET_WM_NAME","_NET_WM_STATE","_NET_WM_STATE_ABOVE","_NET_WM_STATE_BELOW","_NET_WM_STATE_FULLSCREEN","_NET_WM_STATE_HIDDEN","_NET_WM_STATE_SKIP_TASKBAR","_NET_WM_VISIBLE_ICON_NAME","_NET_WM_VISIBLE_NAME","_NET_WM_WINDOW_TYPE","_NET_WM_WINDOW_TYPE_DOCK","_NET_WORKAREA","_NET_WM_WINDOW_TYPE_DESKTOP","_NET_WM_WINDOW_TYPE_NORMAL","_NET_WM_STATE_STICKY","_NET_WM_STATE_MAXIMIZED_VERT","_NET_WM_STATE_MAXIMIZED_HORZ","UTF8_STRING","MOTIF_WM_HINTS","WM_CHANGE_STATE","WM_DELETE_WINDOW","WM_PROTOCOLS","WM_STATE","_NET_WM_WINDOW_TYPE_DIALOG","_NET_WM_WINDOW_TYPE_TOOL","_MOTIF_WM_HINTS","_NET_WM_PID","_NET_WM_USER_TIME","WM_TRANSIENT_FOR","_XROOTPMAP_ID",NULL};

LFSWM2_Class::~LFSWM2_Class(void)
{
	XftFontClose(this->display,this->frameFont);
	delete this->mainEventClass;
	delete this->mainWindowClass;
	delete this->messages;
	delete this->lfstkLib;

	XFreeGC(this->display,this->mainGC);

	this->freeFontColour(this->frameText);
	this->freeFontColour(this->frameBG);
	this->freeFontColour(this->frameFG);
	XCloseDisplay(this->display);
}

void LFSWM2_Class::freeFontColour(fontColour *fc)
{
	XftDrawDestroy(fc->draw);
	delete fc;
}

LFSWM2_Class::LFSWM2_Class(int argc,char **argv)
{
	int					cnt=0;
	XineramaScreenInfo	*p=NULL;
	geometryStruct		monrect;

	this->display=XOpenDisplay(NULL);
	if(this->display==NULL)
		{
			fprintf(stderr,"Can't open display exiting ...");
			exit(1);
		}

	this->mainEventClass=new LFSWM2_eventsClass(this);
	this->mainWindowClass=new LFSWM2_windowClass(this);

	this->screen=DefaultScreen(this->display);
	this->displayWidth=DisplayWidth(this->display,this->screen);
	this->displayHeight=DisplayHeight(this->display,this->screen);
	this->depth=DefaultDepth(this->display,this->screen);

	this->rootWindow=DefaultRootWindow(this->display);
	this->defaultVisual=DefaultVisual(this->display,this->screen);
	this->defaultColourmap=DefaultColormap(this->display,this->screen);
	this->blackColour=BlackPixel(this->display,this->screen);
	this->whiteColour=WhitePixel(this->display,this->screen);
	this->mainGC=XCreateGC(this->display,this->rootWindow,0,NULL);

	cnt=ScreenCount(this->display);
	p=XineramaQueryScreens(this->display,&cnt);
	if(p!=NULL)
		{
			if(cnt>0)
				{
					this->numberOfMonitors=cnt;

					for (int x=0;x<cnt;x++)
						{
							monrect.x=p[x].x_org;
							monrect.y=p[x].y_org;
							monrect.w=p[x].width;
							monrect.h=p[x].height;
							monrect.monitor=x;
							this->monitors.push_back(monrect);
						}
				}
			free(p);
		}

	cnt=0;

//set up atoms
	while(atomNames[cnt]!=NULL)
		{
			this->atoms[atomNames[cnt]]=XInternAtom(this->display,atomNames[cnt],false);
			cnt++;
		}

	this->LFSWM2_initRootWindow();
	XSetInputFocus(this->display,rootWindow,RevertToNone,CurrentTime);
	this->lastXErrorHandler=XSetErrorHandler(&LFSWM2_Class::LFSWM2_xError);

	this->frameBG=this->mainWindowClass->LFSWM2_xftLoadColour("#4194FE","grey");
	this->frameFG=this->mainWindowClass->LFSWM2_xftLoadColour("#000000","white");
	this->frameText=this->mainWindowClass->LFSWM2_xftLoadColour("rgb:ff/ff/ff","black");
	this->frameFont=XftFontOpenName(this->display,this->screen,"sans:size=14:bold");

	this->closeBitMap=XCreateBitmapFromData(this->display,this->rootWindow,(const char*)deleteWindowBits,deleteWindowSize,deleteWindowSize);
	this->maximizeBitMap=XCreateBitmapFromData(this->display,this->rootWindow,(const char*)maximizeWindowBits,maximizeWindowSize,maximizeWindowSize);
	this->minimizeBitMap=XCreateBitmapFromData(this->display,this->rootWindow,(const char*)minimizeWindowBits,minimizeWindowSize,minimizeWindowSize);
	this->shadeBitMap=XCreateBitmapFromData(this->display,this->rootWindow,(const char*)shadeWindowBits,shadeWindowSize,shadeWindowSize);

	this->LFSWM2_setDeskCount(this->numberOfDesktops);
	this->LFSWM2_setCurrentDesktop(this->currentDesktop);
	
	this->topLeftCursor=XCreateFontCursor(this->display,XC_top_left_corner);
	this->topRightCursor=XCreateFontCursor(this->display,XC_top_right_corner);
	this->rightCursor=XCreateFontCursor(this->display,XC_right_side);
	this->bottomRightCursor=XCreateFontCursor(this->display,XC_bottom_right_corner);
	this->bottomCursor=XCreateFontCursor(this->display,XC_bottom_side);
	this->bottomLeftCursor=XCreateFontCursor(this->display,XC_bottom_left_corner);
	this->leftCursor=XCreateFontCursor(this->display,XC_left_side);
	this->rootCursor=XCreateFontCursor(this->display,XC_left_ptr);
	XDefineCursor(this->display,this->rootWindow,this->rootCursor);

	this->mainWindowClass->init();

	this->lfstkLib=new LFSTK_lib(true);
	this->cliOptions(argc,argv);

	this->messages=new LFSWM2_messageClass(this,this->msgQueueKey);
}

void LFSWM2_Class::LFSWM2_pushXErrorHandler(void)
{
	this->lastXErrorHandler=XSetErrorHandler(&LFSWM2_Class::LFSWM2_xWarnings);
}

void LFSWM2_Class::LFSWM2_popXErrorHandler(void)
{
	XSetErrorHandler(this->lastXErrorHandler);
}

void LFSWM2_Class::LFSWM2_initRootWindow(void)
{
	XSetErrorHandler(&LFSWM2_Class::LFSWM2_wmDetected);
	XSelectInput(this->display,this->rootWindow,StructureNotifyMask|ExposureMask|ButtonPress|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask|SubstructureRedirectMask|SubstructureNotifyMask|ButtonPressMask);
	XSync(this->display,false);

	std::vector<Atom>	globalAtoms=
		{
			this->atoms.at("_NET_ACTIVE_WINDOW"),
			this->atoms.at("_NET_CLIENT_LIST"),
			this->atoms.at("_NET_CLIENT_LIST_STACKING"),
			this->atoms.at("_NET_CLOSE_WINDOW"),
			this->atoms.at("_NET_CURRENT_DESKTOP"),
			this->atoms.at("_NET_DESKTOP_GEOMETRY"),
			this->atoms.at("_NET_DESKTOP_VIEWPORT"),
			this->atoms.at("_NET_FRAME_EXTENTS"),
			this->atoms.at("_NET_NUMBER_OF_DESKTOPS"),
			this->atoms.at("_NET_REQUEST_FRAME_EXTENTS"),
			this->atoms.at("_NET_SUPPORTED"),
			this->atoms.at("_NET_SUPPORTING_WM_CHECK"),
			this->atoms.at("_NET_WM_ACTION_CHANGE_DESKTOP"),
			this->atoms.at("_NET_WM_ACTION_CLOSE"),
			this->atoms.at("_NET_WM_ACTION_FULLSCREEN"),
			this->atoms.at("_NET_WM_ACTION_MINIMIZE"),
			this->atoms.at("_NET_WM_ACTION_MAXIMIZE_HORZ"),
			this->atoms.at("_NET_WM_ACTION_MAXIMIZE_VERT"),
			this->atoms.at("_NET_WM_ACTION_RESIZE"),
			this->atoms.at("_NET_WM_ALLOWED_ACTIONS"),
			this->atoms.at("_NET_WM_DESKTOP"),
			this->atoms.at("_NET_WM_ICON_NAME"),
			this->atoms.at("_NET_WM_NAME"),
			this->atoms.at("_NET_WM_STATE"),
			this->atoms.at("_NET_WM_STATE_ABOVE"),
			this->atoms.at("_NET_WM_STATE_BELOW"),
			this->atoms.at("_NET_WM_STATE_FULLSCREEN"),
			this->atoms.at("_NET_WM_STATE_HIDDEN"),
			this->atoms.at("_NET_WM_STATE_SKIP_TASKBAR"),
			this->atoms.at("_NET_WM_VISIBLE_ICON_NAME"),
			this->atoms.at("_NET_WM_VISIBLE_NAME"),
			this->atoms.at("_NET_WM_WINDOW_TYPE"),
			this->atoms.at("_NET_WM_WINDOW_TYPE_DOCK"),
			this->atoms.at("_NET_WORKAREA"),
			this->atoms.at("_NET_WM_WINDOW_TYPE_DESKTOP"),
			this->atoms.at("_NET_WM_WINDOW_TYPE_NORMAL"),
			this->atoms.at("_NET_WM_STATE_STICKY"),
			this->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT"),
			this->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ"),
			this->atoms.at("WM_CHANGE_STATE"),
			this->atoms.at("WM_DELETE_WINDOW"),
			this->atoms.at("WM_PROTOCOLS"),
			this->atoms.at("WM_STATE"),
			this->atoms.at("_MOTIF_WM_HINTS"),
			this->atoms.at("_XROOTPMAP_ID")
		};

	this->mainWindowClass->LFSWM2_setProp(this->rootWindow,this->atoms.at("_NET_SUPPORTED"),XA_ATOM,32,(void*)globalAtoms.data(),globalAtoms.size());
	long geometry[2]={this->displayWidth,this->displayHeight};
	this->mainWindowClass->LFSWM2_setProp(this->rootWindow,this->atoms.at("_NET_DESKTOP_GEOMETRY"),XA_CARDINAL,32,geometry,2);
}

int LFSWM2_Class::LFSWM2_wmDetected(Display *display,XErrorEvent *e)
{
	fprintf(stderr,"A window manager is already running, not starting ...\n");
	exit(2);
	return 0;
}

int LFSWM2_Class::LFSWM2_xWarnings(Display *display,XErrorEvent *e)
{
	return(0);
}

int LFSWM2_Class::LFSWM2_xError(Display *display,XErrorEvent *e)
{
	const char *const	X_REQUEST_CODE_NAMES[]=
		{
			"",
			"CreateWindow",
			"ChangeWindowAttributes",
			"GetWindowAttributes",
			"DestroyWindow",
			"DestroySubwindows",
			"ChangeSaveSet",
			"ReparentWindow",
			"MapWindow",
			"MapSubwindows",
			"UnmapWindow",
			"UnmapSubwindows",
			"ConfigureWindow",
			"CirculateWindow",
			"GetGeometry",
			"QueryTree",
			"InternAtom",
			"GetAtomName",
			"ChangeProperty",
			"DeleteProperty",
			"GetProperty",
			"ListProperties",
			"SetSelectionOwner",
			"GetSelectionOwner",
			"ConvertSelection",
			"SendEvent",
			"GrabPointer",
			"UngrabPointer",
			"GrabButton",
			"UngrabButton",
			"ChangeActivePointerGrab",
			"GrabKeyboard",
			"UngrabKeyboard",
			"GrabKey",
			"UngrabKey",
			"AllowEvents",
			"GrabServer",
			"UngrabServer",
			"QueryPointer",
			"GetMotionEvents",
			"TranslateCoords",
			"WarpPointer",
			"SetInputFocus",
			"GetInputFocus",
			"QueryKeymap",
			"OpenFont",
			"CloseFont",
			"QueryFont",
			"QueryTextExtents",
			"ListFonts",
			"ListFontsWithInfo",
			"SetFontPath",
			"GetFontPath",
			"CreatePixmap",
			"FreePixmap",
			"CreateGC",
			"ChangeGC",
			"CopyGC",
			"SetDashes",
			"SetClipRectangles",
			"FreeGC",
			"ClearArea",
			"CopyArea",
			"CopyPlane",
			"PolyPoint",
			"PolyLine",
			"PolySegment",
			"PolyRectangle",
			"PolyArc",
			"FillPoly",
			"PolyFillRectangle",
			"PolyFillArc",
			"PutImage",
			"GetImage",
			"PolyText8",
			"PolyText16",
			"ImageText8",
			"ImageText16",
			"CreateColormap",
			"FreeColormap",
			"CopyColormapAndFree",
			"InstallColormap",
			"UninstallColormap",
			"ListInstalledColormaps",
			"AllocColor",
			"AllocNamedColor",
			"AllocColorCells",
			"AllocColorPlanes",
			"FreeColors",
			"StoreColors",
			"StoreNamedColor",
			"QueryColors",
			"LookupColor",
			"CreateCursor",
			"CreateGlyphCursor",
			"FreeCursor",
			"RecolorCursor",
			"QueryBestSize",
			"QueryExtension",
			"ListExtensions",
			"ChangeKeyboardMapping",
			"GetKeyboardMapping",
			"ChangeKeyboardControl",
			"GetKeyboardControl",
			"Bell",
			"ChangePointerControl",
			"GetPointerControl",
			"SetScreenSaver",
			"GetScreenSaver",
			"ChangeHosts",
			"ListHosts",
			"SetAccessControl",
			"SetCloseDownMode",
			"KillClient",
			"RotateProperties",
			"ForceScreenSaver",
			"SetPointerMapping",
			"GetPointerMapping",
			"SetModifierMapping",
			"GetModifierMapping",
			"NoOperation",
		};

	const int	MAX_ERROR_TEXT_LENGTH=1024;
	char			error_text[MAX_ERROR_TEXT_LENGTH];

	XGetErrorText(display,e->error_code,error_text,sizeof(error_text));
	fprintf(stderr,"Received X error:\nRequest: %i %s\n%i %s Resource ID: %x\n",int(e->request_code),X_REQUEST_CODE_NAMES[e->request_code],int(e->error_code),error_text,(unsigned int)e->resourceid);
	return 0;
}

unsigned long LFSWM2_Class::LFSWM2_getDesktopCount(void)
{
	return(this->currentDesktop);
}

void LFSWM2_Class::LFSWM2_setDeskCount(unsigned long val)
{
	if (val==0||val>=0xffffffffUL)
		return;

	this->mainWindowClass->LFSWM2_setProp(this->rootWindow,this->atoms.at("_NET_NUMBER_OF_DESKTOPS"),XA_CARDINAL,32,&val,1);
	this->numberOfDesktops=val;
}

void LFSWM2_Class::LFSWM2_setCurrentDesktop(unsigned long i,bool force,bool dovis)
{
	if((this->currentDesktop!=i) || (force==true))
		{
			this->mainWindowClass->LFSWM2_setProp(this->rootWindow,this->atoms.at("_NET_CURRENT_DESKTOP"),XA_CARDINAL,32,&i,1);
			this->currentDesktop=i;
			if(dovis==true)
				this->mainWindowClass->LFSWM2_setVisibilityForDesk(this->currentDesktop);
		}
}

void LFSWM2_Class::printHelp(void)
{
	printf(	"Usage: lfswm2 [OPTION]\n"
			"LFS Window Manager\n"
			"-k, --key\n\tSet message queue key\n"
			"-b, --backcolour\n\tSet frame back colour\n"
			"-f, --forecolour\n\tSet frame fore colour\n"
			"-t, --textcolour\n\tSet frame text colour\n"
			"-F, --font\n\tSet font string ( default: 'sans:size=14:bold' )\n"
			"-p, --titleposition\n\tSet window name position ( 1=left. 2=center( default ), 3=right )\n"
			"-w, --windowposition\n\tSet new window position ( NOPLACE=0, UNDERMOUSE=1, CENTREMMONITOR=2 ( default ), CENTRESCREEN=3 )\n"
			" -v, --version\n\tOutput version information and exit\n"
			" -h, -?, --help\n\tPrint this help\n\n"
			"Report bugs to kdhedger@gmail.com\n"
		);
}

void LFSWM2_Class::cliOptions(int argc,char **argv)//TODO//
{
	struct option long_options[]=
		{
			{"key",1,0,'k'},
			{"backcolour",1,0,'b'},
			{"forecolour",1,0,'f'},
			{"textcolour",1,0,'t'},
			{"font",1,0,'F'},
			{"titleposition",1,0,'p'},
			{"windowposition",1,0,'w'},
			{"version",0,0,'v'},
			{"help",0,0,'?'},
			{0, 0, 0, 0}
		};

	int		c;
	int		key=-1;
	char		*prefsfile;

	while(true)
		{
			int option_index=0;
			c=getopt_long (argc,argv,"v?hk:b:f:t:F:p:w:",long_options,&option_index);
			if(c==-1)
				break;

			switch(c)
				{
					case 'k':
						key=atoi(optarg);
						break;
					case 'b':
						this->freeFontColour(this->frameBG);
						this->frameBG=this->mainWindowClass->LFSWM2_xftLoadColour(optarg,"grey");
						break;
					case 'f':
						this->freeFontColour(this->frameFG);
						this->frameFG=this->mainWindowClass->LFSWM2_xftLoadColour(optarg,"white");
						break;
					case 't':
						this->freeFontColour(this->frameText);
						this->frameText=this->mainWindowClass->LFSWM2_xftLoadColour(optarg,"black");
						break;
					case 'F':
						XftFontClose(this->display,this->frameFont);
						this->frameFont=XftFontOpenName(this->display,this->screen,optarg);
						break;
					case 'p':
						this->titlePosition=atoi(optarg);
						break;
					case 'w':
						this->windowPlacement=atoi(optarg);
						break;
					case 'v':
						printf("LFSWM2 %s\n",VERSION);
						exit(0);
						break;
					case '?':
					case 'h':
						this->printHelp();
						exit(0);
						break;
					default:
						fprintf(stderr,"?? Unknown argument ??\n");
						exit(1);
						break;
				}
		}
	prefsfile=this->lfstkLib->LFSTK_oneLiner("sed -n '2p' \"%s/.config/LFS/lfsappearance.rc\"",getenv("HOME"));
	if(prefsfile!=NULL)
		{
			this->msgQueueKey=atoi(prefsfile);
			free(prefsfile);
		}
	if(key!=-1)
		this->msgQueueKey=key;
}

#ifdef __DEBUG__
void LFSWM2_Class::DEBUG_printAtom(Atom a)
{
	char		*name=NULL;

	this->LFSWM2_pushXErrorHandler();
		name=XGetAtomName(this->display,a);
		if(name!=NULL)
			{
				fprintf(stderr,"Atom %p name=%s\n",(void*)a,name);
				XFree(name);
			}
		else
			fprintf(stderr,"Unknown atom %p\n",(void*)a);
	this->LFSWM2_popXErrorHandler();
}

void LFSWM2_Class::DEBUG_printEventData(XEvent *e,bool verbose)
{
	const char* names[][2]={{"ERROR","0"},{"ERROR","0"},{"KeyPress","0"},{"KeyRelease","0"},{"ButtonPress","0"},{"ButtonRelease","0"},{"MotionNotify","0"},{"EnterNotify","0"},{"LeaveNotify","0"},{"FocusIn","0"},{"FocusOut","0"},{"KeymapNotify","0"},{"Expose","0"},{"GraphicsExpose","0"},{"NoExpose","0"},{"VisibilityNotify","0"},{"CreateNotify","1"},{"DestroyNotify","1"},{"UnmapNotify","1"},{"MapNotify","1"},{"MapRequest","1"},{"ReparentNotify","0"},{"ConfigureNotify","1"},{"ConfigureRequest","1"},{"GravityNotify","0"},{"ResizeRequest","1"},{"CirculateNotify","0"},{"CirculateRequest","0"},{"PropertyNotify","1"},{"SelectionClear","0"},{"SelectionRequest","0"},{"SelectionNotify","0"},{"ColormapNotify","0"},{"ClientMessage","1"},{"MappingNotify","0"},{"GenericEvent","0"}};

	if(names[e->type][1][0]=='1')
		fprintf(stderr,"Event name=%s: type=%i\n",names[e->type][0],e->type);
}

void LFSWM2_Class::DEBUG_printRect(rectStruct r)
{
	fprintf(stderr,"x=%i\n",r.x);
	fprintf(stderr,"y=%i\n",r.y);
	fprintf(stderr,"width=%i\n",r.w);
	fprintf(stderr,"height=%i\n",r.h);
}

const char* LFSWM2_Class::DEBUG_printBool(bool b)
{
	if(b==true)
		return("True");
	return("False");
}

/*
//motif
#define MWM_HINTS_FUNCTIONS (1L << 0)
#define MWM_HINTS_DECORATIONS (1L << 1)

#define MWM_FUNC_ALL (1L << 0)
#define MWM_FUNC_RESIZE (1L << 1)
#define MWM_FUNC_MOVE (1L << 2)
#define MWM_FUNC_MINIMIZE (1L << 3)
#define MWM_FUNC_MAXIMIZE (1L << 4)
#define MWM_FUNC_CLOSE (1L << 5)

#define MWM_DECOR_ALL (1L << 0)
#define MWM_DECOR_BORDER (1L << 1)
#define MWM_DECOR_RESIZEH (1L << 2)
#define MWM_DECOR_TITLE (1L << 3)
#define MWM_DECOR_MENU (1L << 4)
#define MWM_DECOR_MINIMIZE (1L << 5)
#define MWM_DECOR_MAXIMIZE (1L << 6)

MWM_HINTS_DECORATIONS
*/
void LFSWM2_Class::DEBUG_printMWMHints(motifHints *h)
{
	const char	*flagnames[]={"MWM_DECOR_AL","MWM_DECOR_BORDER","MWM_DECOR_RESIZEH","MWM_DECOR_TITLE","MWM_DECOR_MENU","MWM_DECOR_MINIMIZE","MWM_DECOR_MAXIMIZE"};
	const char	*funcnames[]={"MWM_FUNC_ALL","MWM_FUNC_RESIZE","MWM_FUNC_MOVE","MWM_FUNC_MINIMIZE","MWM_FUNC_MAXIMIZE","MWM_FUNC_CLOSE"};
	bool			notall=h->decorations & MWM_DECOR_ALL;

	fprintf(stderr,"flags=%p use decorations=%s use functions=%s\n",h->flags,this->DEBUG_printBool(h->flags & MWM_HINTS_DECORATIONS),this->DEBUG_printBool(h->flags & MWM_HINTS_FUNCTIONS));
	fprintf(stderr,"decorations=%p\n",h->decorations);

	for(int j=1;j<7;j++)
		{
			if(h->decorations & 1L << j)
				{
					if(notall==false)
						fprintf(stderr,"%s ",flagnames[j]);
					else
						fprintf(stderr,"Not %s ",flagnames[j]);

				}
		}
	fprintf(stderr,"\nfunctions=%p\n",h->functions);
	for(int j=0;j<6;j++)
		{
			if(h->functions	& 1L << j)
				fprintf(stderr,"%s ",funcnames[j]);
		}

	fprintf(stderr,"\n");
}

void LFSWM2_Class::DEBUG_prinWindowAttributes(Window id)
{
	XWindowAttributes 	xa;

	XGetWindowAttributes(this->display,id,(XWindowAttributes*)&xa);

	fprintf(stderr,"window=%p\n",id);
	fprintf(stderr,"x=%i y=%i\n",xa.x,xa.y);
	fprintf(stderr,"width=%i height=%i\n",xa.width,xa.height);
	fprintf(stderr,"border_width=%p\n",xa.border_width);
	fprintf(stderr,"depth=%p\n",xa.depth);
	fprintf(stderr,"visual=%p\n",xa.visual);
	fprintf(stderr,"root=%p\n",xa.root);
	fprintf(stderr,"class=%p\n",xa.c_class);
	fprintf(stderr,"bit_gravity=%p\n",xa.bit_gravity);
	fprintf(stderr,"win_gravity=%p\n",xa.win_gravity);
	fprintf(stderr,"backing_store=%p\n",xa.backing_store);
	fprintf(stderr,"backing_planes=%p\n",xa.backing_planes);
	fprintf(stderr,"backing_pixel=%p\n",xa.backing_pixel);
	fprintf(stderr,"save_under=%s\n",this->DEBUG_printBool(xa.save_under));
	fprintf(stderr,"colormap=%p\n",xa.colormap);
	fprintf(stderr,"map_installed=%s\n",this->DEBUG_printBool(xa.map_installed));
	fprintf(stderr,"map_state=%p\n",xa.map_state);
	fprintf(stderr,"all_event_masks=%p\n",xa.all_event_masks);
	fprintf(stderr,"your_event_mask=%p\n",xa.your_event_mask);
	fprintf(stderr,"do_not_propagate_mask=%p\n",xa.do_not_propagate_mask);
	fprintf(stderr,"override_redirect=%s\n",this->DEBUG_printBool(xa.override_redirect));
	fprintf(stderr,"screen=%p\n\n",xa.screen);
}

#endif
