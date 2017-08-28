#if 0

#©keithhedger Thu 3 Aug 12:48:12 BST 2017 kdhedger68713@gmail.com

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o submenuexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs ./submenuexample "$@"
retval=$?
echo "Exit code $retval"
exit $retval
#endif

#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Sub Menus"
#define MAXMAINMENUS		8
#define MENUWIDTH			DIALOGWIDTH-(BORDER*2)

LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_menuButtonClass		*menu=NULL;

menuItemStruct				*mainMenus;
const char					*mainMenuNames[]={"Menu 1","Menu 2","Menu 3","Menu 4","--","bool mainLoop=true;","menuItemStruct *mainMenus;","LFSTK_menuButtonClass *mb=NULL;"};

menuItemStruct				*subMenus;
const char					*subMenuNames[]={"Sub Menu 1","Sub Menu 2","Sub Menu 3","Sub Menu 4","--","bool mainLoop=true;","menuItemStruct *mainMenus;","LFSTK_menuButtonClass *mb=NULL;"};

bool						mainLoop=true;
Display						*display;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

bool menuCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(ud==NULL)
		return(true);

	printf("Selected Menu Label:%s\n",menuitem->label);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;

	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"Menus With Sub-Menus Example",false);
	display=wc->display;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

	subMenus=new menuItemStruct[MAXMAINMENUS];

	for(int j=0;j<MAXMAINMENUS;j++)
		{
			subMenus[j].label=(char*)subMenuNames[j];
			subMenus[j].imagePath=NULL;
		}
	subMenus[0].imagePath="./audio-speakers.png";
	subMenus[2].imagePath="./ManPageEditor.png";

	mainMenus=new menuItemStruct[MAXMAINMENUS];

	for(int j=0;j<MAXMAINMENUS;j++)
		{
			mainMenus[j].label=(char*)mainMenuNames[j];
			mainMenus[j].imagePath=NULL;
		}
	mainMenus[0].imagePath="./casper2.JPG";
	mainMenus[2].imagePath="./computer.png";

	mainMenus[2].subMenus=subMenus;
	mainMenus[2].subMenuCnt=MAXMAINMENUS;

	mainMenus[0].subMenus=subMenus;
	mainMenus[0].subMenuCnt=MAXMAINMENUS;
	
	menu=new LFSTK_menuButtonClass(wc,"Main Menu",BORDER,sy,MENUWIDTH,GADGETHITE,BUTTONGRAV);
	menu->LFSTK_setCallBack(NULL,menuCB,NULL);
	menu->LFSTK_addMenus(mainMenus,MAXMAINMENUS);
	menu->LFSTK_setLabelGravity(LEFT);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				{
					ml->function(ml->gadget,&event,ml->type);
				}

			switch(event.type)
				{
				case ButtonRelease:
					break;
				case LeaveNotify:
					break;
				case Expose:
					//printf("expose\n");
					wc->LFSTK_clearWindow();
					break;

				case ConfigureNotify:
					wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
					wc->globalLib->LFSTK_setCairoSurface(wc->display,wc->window,wc->visual,&wc->sfc,&wc->cr,event.xconfigurerequest.width,event.xconfigurerequest.height);
					wc->LFSTK_clearWindow();
					break;

				case ClientMessage:
				case SelectionNotify:
				{
					if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
						{
							wc->LFSTK_hideWindow();
							mainLoop=false;
						}
				}
				break;
				}
		}

	delete wc;
	XCloseDisplay(display);
	delete[] mainMenus;
	delete[] subMenus;

	return 0;
}