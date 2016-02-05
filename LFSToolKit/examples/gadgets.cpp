#if 0

#©keithhedger Fri 7 Aug 15:57:52 BST 2015 kdhedger68713@gmail.com

g++ "$0" -O3 -I../LFSToolKit/src -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs ./a.out "$@"
retval=$?
rm ./a.out
exit $retval

#endif

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <climits>
#include <cstring>
using namespace std;
#include <map>

//#include <LFSTKWindow.h>
//#include <LFSTKButton.h>
//#include "LFSTKMenuButton.h"
//#include "LFSTKLineEdit.h"
//#include "LFSTKLabel.h"
//#include "LFSTKToggleButton.h"
//#include "LFSTKImage.h"
//#include "LFSTKLib.h"
#include "LFSTKGlobals.h"

#define WIDTH			400
#define HITE			600
#define HALF			WIDTH/2
#define BWIDTH			96
#define BHITE			24
#define BX				HALF-(BWIDTH/2)
#define BORDER			20
#define YSPACING		32
#define BGRAV			NorthWestGravity
#define IMAGESIZE		64

#define MAXMAINMENUS	8
#define MAXSUBMENUS		30

LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_buttonClass		*bc=NULL;
LFSTK_menuButtonClass	*mb=NULL;
LFSTK_menuButtonClass	*mbwithsubs=NULL;
LFSTK_toggleButtonClass *tb=NULL;
LFSTK_toggleButtonClass *tbnormal=NULL;
LFSTK_lineEditClass		*le=NULL;
LFSTK_buttonClass		*iconButton=NULL;
LFSTK_imageClass		*image=NULL;
LFSTK_buttonClass		*imageButton;
menuItemStruct			*mainMenus;
menuItemStruct			*mainMenusWithSubs;
menuItemStruct			*subMenus;

const char				*mainMenuNames[]={"Menu 1","Menu 2","Menu 3","Menu 4","--","bool mainLoop=true;","menuItemStruct *mainMenus;","LFSTK_menuButtonClass *mb=NULL;"};
const char				*subMenuNames[]={"Sub Menu 1","Sub Menu 2","Sub Menu 3","Sub Menu 4"};
LFSTK_buttonClass		*quit=NULL;

bool					mainLoop=true;
int						iconSize=16;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	if(ud!=NULL)
		printf(">>>%s<<<\n",(const char*)ud);
	printf("Line edit=%s\n",le->LFSTK_getBuffer()->c_str());
	return(true);
}

bool menuCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(ud==NULL)
		return(true);

	printf("Selected Menu Label:%s\n",menuitem->label);
	return(true);
}

//Convert an atom name in to a std::string
string GetAtomName(Display* disp, Atom a)
{
	if(a == None)
		return "None";
	else
		return XGetAtomName(disp, a);
}


int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
	Pixmap	ic[2];
	
	wc=new LFSTK_windowClass(0,0,WIDTH,HITE,"Gadgets",false);

	label=new LFSTK_labelClass(wc,"Available Gadgets",BORDER,sy,WIDTH-(BORDER*2),BHITE,BGRAV);
	sy+=(BORDER*2);

//button
	bc=new LFSTK_buttonClass(wc,"Button",BORDER,sy,BWIDTH,BHITE,NorthWestGravity);
	bc->LFSTK_setCallBack(NULL,buttonCB,(void*)bc->LFSTK_getLabel());
	//bc->LFSTK_setStyle(BEVELIN);
	//bc->LFSTK_setActive(true);
	sy+=YSPACING;

//check button	
	tb=new LFSTK_toggleButtonClass(wc,"Check",BORDER,sy,BWIDTH,BHITE,NorthWestGravity);
	tb->LFSTK_setCallBack(NULL,buttonCB,(void*)tb->LFSTK_getLabel());
	sy+=YSPACING;

//toggle button	
	tbnormal=new LFSTK_toggleButtonClass(wc,"Toggle",BORDER,sy,BWIDTH,BHITE,NorthWestGravity);
	tbnormal->LFSTK_setCallBack(NULL,buttonCB,(void*)tbnormal->LFSTK_getLabel());
	tbnormal->LFSTK_setToggleStyle(TOGGLENORMAL);
	tbnormal->LFSTK_setIconFromPath("./api.png");
	sy+=YSPACING;

//menu button
	wc->globalLib->LFSTK_setPixmapsFromPath(wc->display,wc->visual,wc->cm,wc->window,"/usr/share/icons/gnome/48x48/devices/computer.png",&ic[0],&ic[1],iconSize);
	mainMenus=new menuItemStruct[MAXMAINMENUS];

	for(int j=0;j<MAXMAINMENUS;j++)
		{
			mainMenus[j].label=mainMenuNames[j];
			mainMenus[j].userData=NULL;
			mainMenus[j].bc=NULL;
			mainMenus[j].subMenus=NULL;
			mainMenus[j].useIcon=true;
			mainMenus[j].useImage=false;
			mainMenus[j].icon[0]=ic[0];
			mainMenus[j].icon[1]=ic[1];
			mainMenus[j].iconSize=iconSize;
		}

	wc->globalLib->LFSTK_setPixmapsFromPath(wc->display,wc->visual,wc->cm,wc->window,"system-lock-screen.png",&mainMenus[0].icon[0],&mainMenus[0].icon[1],iconSize);
	mainMenus[1].useIcon=false;
	mainMenus[1].useImage=true;
	mainMenus[1].image=imlib_load_image_immediately_without_cache("system-lock-screen.png");
	mainMenus[1].imageWidth=iconSize;
	mainMenus[1].imageHeight=iconSize;
	//wc->globalLib->LFSTK_setPixmapsFromPath(wc->display,wc->visual,wc->cm,wc->window,"./ROOTKKEdit.png",&mainMenus[2].icon[0],&mainMenus[2].icon[1],48);

	mb=new LFSTK_menuButtonClass(wc,"Main Menu",BORDER,sy,BWIDTH,BHITE,BGRAV);
	mb->LFSTK_setIconFromPath("./BookMark.png");
	mb->LFSTK_setCallBack(NULL,menuCB,NULL);
	mb->LFSTK_addMenus(mainMenus,MAXMAINMENUS);
	mb->LFSTK_setLabelOriention(LEFT);
	sy+=YSPACING;

//menu button with sub menu
	mainMenusWithSubs=new menuItemStruct[MAXMAINMENUS];
	for(int j=0;j<MAXMAINMENUS;j++)
		{
			mainMenusWithSubs[j].label=mainMenuNames[j];
			mainMenusWithSubs[j].userData=NULL;
			mainMenusWithSubs[j].bc=NULL;
			mainMenusWithSubs[j].subMenus=NULL;
			mainMenusWithSubs[j].useIcon=false;
			mainMenusWithSubs[j].useImage=false;
		}
//sub menus
	subMenus=new menuItemStruct[MAXSUBMENUS];
	for(int j=0;j<MAXSUBMENUS;j++)
		{
			subMenus[j].label=subMenuNames[0];
			subMenus[j].userData=NULL;
			subMenus[j].bc=NULL;
			subMenus[j].subMenus=NULL;
			subMenus[j].useIcon=true;
			subMenus[j].icon[0]=ic[0];
			subMenus[j].icon[1]=ic[1];
			subMenus[j].iconSize=iconSize;

	subMenus[j].useIcon=false;
	subMenus[j].useImage=true;
	subMenus[j].image=imlib_load_image_immediately_without_cache("system-lock-screen.png");
	subMenus[j].imageWidth=iconSize;
	subMenus[j].imageHeight=iconSize;




		}
	wc->globalLib->LFSTK_setPixmapsFromPath(wc->display,wc->visual,wc->cm,wc->window,"/usr/share/icons/gnome/48x48/devices/audio-speakers.png",&subMenus[2].icon[0],&subMenus[2].icon[1],16);
//add sub menus
	mainMenusWithSubs[3].subMenus=subMenus;
	mainMenusWithSubs[3].subMenuCnt=MAXSUBMENUS;

	mainMenusWithSubs[0].useIcon=true;
	mainMenusWithSubs[0].useImage=false;
	mainMenusWithSubs[0].icon[0]=ic[0];
	mainMenusWithSubs[0].icon[1]=ic[1];
	mainMenusWithSubs[0].iconSize=iconSize;
	mainMenusWithSubs[1].useImage=false;
	mainMenusWithSubs[1].useIcon=true;
	mainMenusWithSubs[1].icon[0]=ic[0];
	mainMenusWithSubs[1].icon[1]=ic[1];
	mainMenusWithSubs[1].iconSize=iconSize;

	mainMenusWithSubs[1].subMenus=subMenus;
	mainMenusWithSubs[1].subMenuCnt=MAXSUBMENUS;
	
	mbwithsubs=new LFSTK_menuButtonClass(wc,"Sub Menus",BORDER,sy,BWIDTH,BHITE,BGRAV);
	mbwithsubs->LFSTK_setIconFromPath("./ManPageEditor.png");
	mbwithsubs->LFSTK_setCallBack(NULL,menuCB,NULL);
	mbwithsubs->LFSTK_setLabelOriention(LEFT);
	mbwithsubs->LFSTK_addMenus(mainMenusWithSubs,MAXMAINMENUS);
	sy+=YSPACING;

//icon button
	iconButton=new LFSTK_buttonClass(wc,"",BORDER,sy,68,64+4,NorthWestGravity);
	iconButton->LFSTK_setCallBack(NULL,buttonCB,(void*)iconButton->LFSTK_getLabel());
	iconButton->LFSTK_setIconFromPath("/usr/share/pixmaps/LFSTux.png",64);
	sy+=64;
	sy+=12;

//image button
	imageButton=new LFSTK_buttonClass(wc,"",BORDER,sy,68,64+4,NorthWestGravity);
	imageButton->LFSTK_setCallBack(NULL,buttonCB,(void*)iconButton->LFSTK_getLabel());
	imageButton->LFSTK_setImageFromPath("/usr/share/pixmaps/LFSTux.png",IMAGESIZE,IMAGESIZE);
	imageButton->LFSTK_setImageFromPath("/usr/share/pixmaps/AspellGUI.png",IMAGESIZE,IMAGESIZE);
	sy+=64;

	sy+=16;
	//image=new LFSTK_imageClass(wc,"/usr/share/pixmaps/LFSTux.png",BORDER,sy,IMAGESIZE,NorthWestGravity);
	image=new LFSTK_imageClass(wc,"/usr/lib64/firefox-36.0.1/browser/icons/mozicon128.png",BORDER,sy,IMAGESIZE,NorthWestGravity);

	sy+=IMAGESIZE;
	sy+=12;

//line edit
	le=new LFSTK_lineEditClass(wc,"Hello World",BORDER,sy,BWIDTH*2,BHITE,BGRAV);
	
	quit=new LFSTK_buttonClass(wc,"Quit",BX,HITE-BHITE-BORDER,BWIDTH,BHITE,SouthGravity);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);

	//wc->LFSTK_setTile("/home/keithhedger/.themes/OldBrownWood/gtk-2.0/bc.png",-1);
//	wc->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
//	iconButton->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
//	iconButton->LFSTK_setTile("/home/keithhedger/.themes/OldBrownWood/gtk-2.0/buttons/button1.png",-1);
//	quit->LFSTK_setTile("/home/keithhedger/.themes/OldBrownWood/gtk-2.0/buttons/button1.png",-1);
//	mbwithsubs->LFSTK_setTile("/home/keithhedger/.themes/OldBrownWood/gtk-2.0/menus/menufill.png",-1);
//	label->LFSTK_setTile("/home/keithhedger/.themes/OldBrownWood/gtk-2.0/bc.png",-1);
	//mb->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
//	mb->LFSTK_setTile("/home/keithhedger/.themes/OldBrownWood/gtk-2.0/buttons/button1.png",-1);

//	label->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),-1);
//	iconButton->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
//	mb->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
//	mbwithsubs->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);

	//Atoms for Xdnd
	Atom XdndEnter = XInternAtom(wc->display, "XdndEnter", False);
	Atom XdndPosition = XInternAtom(wc->display, "XdndPosition", False);
	Atom XdndStatus = XInternAtom(wc->display, "XdndStatus", False);
	Atom XdndTypeList = XInternAtom(wc->display, "XdndTypeList", False);
	Atom XdndActionCopy = XInternAtom(wc->display, "XdndActionCopy", False);
	Atom XdndDrop = XInternAtom(wc->display, "XdndDrop", False);
	Atom XdndLeave = XInternAtom(wc->display, "XdndLeave", False);
	Atom XdndFinished = XInternAtom(wc->display, "XdndFinished", False);
	Atom XdndSelection = XInternAtom(wc->display, "XdndSelection", False);
	Atom XdndProxy = XInternAtom(wc->display, "XdndProxy", False);
	Atom XA_TARGETS = XInternAtom(wc->display, "TARGETS", False);
	Atom sel = XInternAtom(wc->display, "PRIMARY", 0);
	
	
	map<string, int> myDataTypes;
if(myDataTypes.empty())
		myDataTypes["STRING"] = 1;	
	wc->LFSTK_showWindow();
	int xdnd_version=3;

	mainLoop=true;
	while(mainLoop==true)
		{
			listener *l=wc->LFSTK_getListener(event.xany.window);
			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);

			XNextEvent(wc->display,&event);
			switch(event.type)
				{
					case LeaveNotify:
						break;
					case Expose:
						wc->LFSTK_clearWindow();
//						XSetFillStyle(this->display,this->gc,FillSolid);
//			XSetClipMask(this->display,this->gc,None);
//			XSetForeground(this->display,this->gc,this->colourNames[state].pixel);
//			XFillRectangle(this->display,this->window,this->gc,g->x,g->y,g->w,g->h);
//						imagebutton->externalUpdate=false;
//						imagebutton->LFSTK_clearWindow();
//						imagebutton->externalUpdate=true;
//						imlib_context_set_display(wc->display);
//						imlib_context_set_visual(wc->visual);
//						imlib_context_set_colormap(wc->cm);
//
//						imlib_context_set_drawable(imagebutton->window);
//						imlib_context_set_image(data);
//						imlib_context_set_blend(1);
//
//						imlib_render_image_on_drawable_at_size(2,2,64,64); 
						break;

					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						break;

					case ClientMessage:
						{
						// XTranslateCoordinates
						printf("Main loop client message\n");
						cerr << "A ClientMessage has arrived:\n";
						cerr << "Type = " << GetAtomName(wc->display, event.xclient.message_type) << " (" << event.xclient.format << ")\n";

if(event.xclient.message_type == XdndPosition)
			{
				cerr << hex << "Source window = 0x" << event.xclient.data.l[0] << dec << endl;
				cerr << "Position: x=" << (event.xclient.data.l[2]  >> 16) << " y=" << (event.xclient.data.l[2] &0xffff)  << endl;
				cerr << "Timestamp = " << event.xclient.data.l[3] << " (Version >= 1 only)\n";
				
				Atom action=XdndActionCopy;
				if(xdnd_version >= 2)
					action = event.xclient.data.l[4];

				cerr << "Action = " << GetAtomName(wc->display, action) << " (Version >= 2 only)\n";
				

				//Xdnd: reply with an XDND status message
				XClientMessageEvent m;
				memset(&m, sizeof(m), 0);
				m.type = ClientMessage;
				m.display = event.xclient.display;
				m.window = event.xclient.data.l[0];
				m.message_type = XdndStatus;
				m.format=32;
				m.data.l[0] = wc->window;
				m.data.l[1] =1;// (to_be_requested != None);
				//printf(">>>>>%i<<<<<\n",(to_be_requested != None));
				m.data.l[2] = 0; //Specify an empty rectangle
				m.data.l[3] = 0;
				m.data.l[4] = XdndActionCopy; //We only accept copying anyway.

				XSendEvent(wc->display, event.xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
				XFlush(wc->display);
			}

if(event.xclient.message_type == XdndDrop)
			{
//				if(to_be_requested == None)
				{
					//It's sending anyway, despite instructions to the contrary.
					//So reply that we're not interested.
					XClientMessageEvent m;
					memset(&m, sizeof(m), 0);
					m.type = ClientMessage;
					m.display = event.xclient.display;
					m.window = event.xclient.data.l[0];
					m.message_type = XdndFinished;
					m.format=32;
					m.data.l[0] =wc->window;
					m.data.l[1] = 0;
					m.data.l[2] = None; //Failed.
					XSendEvent(wc->display, event.xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
				}
				}
#if 0
//Xdnd: reply with an XDND status message
//Atom XdndActionCopy = XInternAtom(wc->display, "XdndActionCopy", False);
//Atom XdndStatus = XInternAtom(wc->display, "XdndStatus", False);
				XClientMessageEvent m;
				memset(&m, sizeof(m), 0);
				m.type = ClientMessage;
				m.display = event.xclient.display;
				m.window = event.xclient.data.l[0];
				m.message_type = XdndStatus;
				m.format=32;
				m.data.l[0] = wc->window;
				m.data.l[1] = 0;//(to_be_requested != None);
				m.data.l[2] = 0; //Specify an empty rectangle
				m.data.l[3] = 0;
				m.data.l[4] = XdndActionCopy; //We only accept copying anyway.
//
				XSendEvent(wc->display, event.xclient.data.l[0], False, NoEventMask, (XEvent*)&m);
				XFlush(wc->display);

#endif					
						
						//cerr << "Type = " << GetAtomName(disp, e.xclient.message_type) << " (" << e.xclient.format << ")\n";

						if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
							mainLoop=false;
						}
						break;
				}
		}

	delete bc;
	delete tb;
	delete tbnormal;
	delete mb;
	delete wc;
	return 0;
}