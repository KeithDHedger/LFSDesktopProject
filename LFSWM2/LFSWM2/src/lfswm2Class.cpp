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

const char *atomNames[]={"_NET_WM_WINDOW_TYPE_MENU","_NET_ACTIVE_WINDOW","_NET_CLIENT_LIST","_NET_CLIENT_LIST_STACKING","_NET_CLOSE_WINDOW","_NET_CURRENT_DESKTOP","_NET_DESKTOP_GEOMETRY","_NET_DESKTOP_VIEWPORT","_NET_FRAME_EXTENTS","_NET_NUMBER_OF_DESKTOPS","_NET_REQUEST_FRAME_EXTENTS","_NET_SUPPORTED","_NET_SUPPORTING_WM_CHECK","_NET_WM_ACTION_CHANGE_DESKTOP","_NET_WM_ACTION_CLOSE","_NET_WM_ACTION_FULLSCREEN","_NET_WM_ACTION_MINIMIZE","_NET_WM_ALLOWED_ACTIONS","_NET_WM_DESKTOP","_NET_WM_ICON_NAME","_NET_WM_NAME","_NET_WM_STATE","_NET_WM_STATE_ABOVE","_NET_WM_STATE_BELOW","_NET_WM_STATE_FULLSCREEN","_NET_WM_STATE_HIDDEN","_NET_WM_STATE_SKIP_TASKBAR","_NET_WM_VISIBLE_ICON_NAME","_NET_WM_VISIBLE_NAME","_NET_WM_WINDOW_TYPE","_NET_WM_WINDOW_TYPE_DOCK","_NET_WORKAREA","_NET_WM_WINDOW_TYPE_DESKTOP","_NET_WM_WINDOW_TYPE_NORMAL","_NET_WM_STATE_STICKY","_NET_WM_ACTION_MAXIMIZE_HORZ","_NET_WM_ACTION_MAXIMIZE_VERT","_NET_WM_STATE_MAXIMIZED_VERT","_NET_WM_STATE_MAXIMIZED_HORZ","UTF8_STRING","MOTIF_WM_HINTS","WM_CHANGE_STATE","WM_DELETE_WINDOW","WM_PROTOCOLS","WM_STATE","_NET_WM_WINDOW_TYPE_DIALOG","_NET_WM_WINDOW_TYPE_TOOL","_MOTIF_WM_HINTS","_NET_WM_PID","_NET_WM_USER_TIME",NULL};

LFSWM2_Class::~LFSWM2_Class(void)
{
}

LFSWM2_Class::LFSWM2_Class(void)
{
	int					cnt=0;
	XineramaScreenInfo	*p=NULL;
	rectStruct			monrect;

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
							monrect.width=p[x].width;
							monrect.height=p[x].height;
							this->monitors.push_back(monrect);
							//monrect.debugPrint();
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
	XSetErrorHandler(&LFSWM2_Class::LFSWM2_xError);

//	this->frameBG=this->mainWindowClass->LFSWM2_xftLoadColour("#5E3300","#808080");
	this->frameBG=this->mainWindowClass->LFSWM2_xftLoadColour("#4194FE","#808080");
	this->frameFG=this->mainWindowClass->LFSWM2_xftLoadColour("#948069","#808080");
	this->frameText=this->mainWindowClass->LFSWM2_xftLoadColour("rgb:ff/ff/ff","#808080");
	this->frameFont=XftFontOpenName(this->display,this->screen,"sans-serif:size=14:bold");

	this->closeBitMap=XCreateBitmapFromData(this->display,this->rootWindow,(const char*)deleteWindowBits,deleteWindowSize,deleteWindowSize);
	this->maximizeBitMap=XCreateBitmapFromData(this->display,this->rootWindow,(const char*)maximizeWindowBits,maximizeWindowSize,maximizeWindowSize);
	this->minimizeBitMap=XCreateBitmapFromData(this->display,this->rootWindow,(const char*)minimizeWindowBits,minimizeWindowSize,minimizeWindowSize);
	this->shadeBitMap=XCreateBitmapFromData(this->display,this->rootWindow,(const char*)shadeWindowBits,shadeWindowSize,shadeWindowSize);

	//Cursor curs=XCreateFontCursor(this->display,XC_left_ptr);
	//XDefineCursor(this->display,this->rootWindow,curs);

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

	this->mainWindowClass->init();

	XDefineCursor(this->display,this->rootWindow,this->rootCursor);

	this->messages=new LFSWM2_messageClass(this);
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

	Atom globals[]=
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
			this->atoms.at("_NET_WM_ACTION_MAXIMIZE_HORZ"),
			this->atoms.at("_NET_WM_ACTION_MAXIMIZE_VERT"),
			this->atoms.at("_NET_WM_STATE_MAXIMIZED_VERT"),
			this->atoms.at("_NET_WM_STATE_MAXIMIZED_HORZ")
		};

	this->mainWindowClass->LFSWM2_setProp(this->rootWindow,this->atoms.at("_NET_SUPPORTED"),XA_ATOM,32,globals,NELEM(globals));
	long geometry[2]={this->displayWidth,this->displayHeight};
	this->mainWindowClass->LFSWM2_setProp(this->rootWindow,this->atoms.at("_NET_DESKTOP_GEOMETRY"),XA_CARDINAL,32,geometry,2);
}

int LFSWM2_Class::LFSWM2_wmDetected(Display *display,XErrorEvent *e)
{
  fprintf(stderr,"already running wm ... TODO\n");
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
	fprintf(stderr,"Received X error:\nRequest: %i %s\n%i %s Resource ID: %x\n",int(e->request_code),X_REQUEST_CODE_NAMES[e->request_code],int(e->error_code),error_text,e->resourceid);
  return 0;
}

unsigned long LFSWM2_Class::LFSWM2_getDesktopCount(void)
{
	return(this->currentDesktop);
//
//	unsigned long nd=DEFAULT_NUMBER_OF_DESKTOPS;
//	unsigned long n;
//	unsigned long *p=(long unsigned int*)this->mainWindowClass->LFSWM2_getFullProp(this->rootWindow,this->atoms.at("_NET_NUMBER_OF_DESKTOPS"),XA_CARDINAL,32,&n);
//
//	if(p!=NULL)
//		{
//			if(n==1)
//				nd=*p & 0xffffffffUL;
//			XFree(p);
//		}
//	return nd;
}

void LFSWM2_Class::LFSWM2_setDeskCount(unsigned long val)
{
	if (val==0||val>=0xffffffffUL)
		return;

	this->mainWindowClass->LFSWM2_setProp(this->rootWindow,this->atoms.at("_NET_NUMBER_OF_DESKTOPS"),XA_CARDINAL,32,&val,1);
	this->numberOfDesktops=val;
	this->needsRestack=true;

//	
//	Desk		oldval=this->numberOfDesktops;
//	this->numberOfDesktops=val;
//
//	if (val >= oldval)
//		this->ewmh_notifyndesk(val);
}

//void LFSWM2_Class::ewmh_notifyndesk(unsigned long n)
//{
//	long		*viewport=(long int*)malloc(n*2*sizeof(long));
//	long		*workarea=(long int*)malloc(n*4*sizeof(long));
//
//	for(unsigned long i=0;i<n;i++)
//		{
//			viewport[2*i+0]=0;
//			viewport[2*i+1]=0;
//
//			workarea[4*i+0]=0;
//			workarea[4*i+1]=0;
//			workarea[4*i+2]=this->displayWidth;
//			workarea[4*i+3]=this->displayHeight;
//		}
//
//	this->mainWindowClass->LFSWM2_setProp(this->rootWindow,this->atoms.at("_NET_DESKTOP_VIEWPORT"),XA_CARDINAL,32,viewport,n*2);
//	this->mainWindowClass->LFSWM2_setProp(this->rootWindow,this->atoms.at("_NET_WORKAREA"),XA_CARDINAL,32,workarea,n*4);
//	free(workarea);
//	free(viewport);
//
//	this->mainWindowClass->LFSWM2_setProp(this->rootWindow,this->atoms.at("_NET_NUMBER_OF_DESKTOPS"),XA_CARDINAL,32,&n,1);
//	this->deskCount=n;
//}

void LFSWM2_Class::LFSWM2_setCurrentDesktop(unsigned long i)
{
	LFSWM2_clientClass	*cc;

	if(this->currentDesktop!=i)
		{
			this->needsRestack=true;
			this->mainWindowClass->LFSWM2_setProp(this->rootWindow,this->atoms.at("_NET_CURRENT_DESKTOP"),XA_CARDINAL,32,&i,1);
			this->currentDesktop=i;
			this->needsRestack=true;

			//for(int j=0;j<this->mainWindowClass->windowIDList.size();j++)
			for(int j=this->mainWindowClass->windowIDList.size()-1;j>=0;j--)
				{
					cc=this->mainWindowClass->LFSWM2_getClientClass(this->mainWindowClass->windowIDList.at(j));
					if(cc!=NULL)
						{
							if(cc->onDesk==this->currentDesktop)
								cc->LFSWM2_showWindow();
							else
								cc->LFSWM2_hideWindow();
						}
				}
		}
}

#ifdef __DEBUG__
void LFSWM2_Class::DEBUG_printAtom(Atom a)
{
	fprintf(stderr,"Atom %p name=%s\n",a,XGetAtomName(this->display,a));
}
#endif
