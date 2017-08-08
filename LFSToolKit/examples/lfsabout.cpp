#if 0

#©keithhedger Wed 2 Aug 15:43:22 BST 2017 kdhedger68713@gmail.com

g++ "$0" -O0 -ggdb -I/media/LinuxData/Development64/Projects/LFSDesktopProject/LFSToolKit/LFSToolKit -L/media/LinuxData/Development64/Projects/LFSDesktopProject/LFSToolKit/LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2||exit 1
LD_LIBRARY_PATH=/media/LinuxData/Development64/Projects/LFSDesktopProject/LFSToolKit/LFSToolKit/app/.libs ./a.out "$@"
retval=$?
rm ./a.out
exit $retval

#endif

#include "lfstk/LFSTKGlobals.h"

#define WINDOWWIDTH			320
#define WINDOWHITE			10
#define WINDOWMIDDLE		WINDOWWIDTH/2

#define GADEGETWIDTH		96
#define GADEGETHITE			24
#define HALFGADEGETWIDTH	GADEGETWIDTH/2

#define BORDER				20
#define YSPACING			24
#define HALFYSPACING		YSPACING/2
#define BUTTONGRAV			NorthWestGravity

#define COPYRITE			"Copyright © 2013-2016 K.D.Hedger"
#define PERSONAL			"keithhedger@keithhedger.darktech.org"
#define	MYWEBSITE			"http://keithhedger.darktech.org/"

enum		{LNAME,LDESCRIPTION,LCOPYRITE,LEMAIL,LWEBSITE,LGPL,LNOMORELABELS};

const char	*labelTexts[]={"LFS Desktop Project","Low resource/dependency desktop for LFS","© 2015-2016 K.D.Hedger",PERSONAL,MYWEBSITE,"Released under GPLv3"};

#define IMAGESIZE		64

LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_imageClass		*tux;
LFSTK_labelClass		*labels[LNOMORELABELS];

bool					mainLoop=true;
Display					*display;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
		
	wc=new LFSTK_windowClass(0,0,WINDOWWIDTH,WINDOWHITE,"Gadgets",false);
	display=wc->display;

	tux=new LFSTK_imageClass(wc,NULL,WINDOWMIDDLE-(IMAGESIZE/2),sy,IMAGESIZE,IMAGESIZE,BUTTONGRAV,true);
	tux->LFSTK_setImageFromPath("/usr/share/pixmaps/LFSTux.png",AUTO,true);
	tux->LFSTK_clearWindow();
	sy+=YSPACING*3;


	for(int j=LNAME;j<LNOMORELABELS;j++)
		{
			labels[j]=new LFSTK_labelClass(wc,labelTexts[j],0,sy,WINDOWWIDTH,GADEGETHITE,BUTTONGRAV);
			sy+=YSPACING;
		}
	labels[LNAME]->LFSTK_setCairoFontDataParts("sB",18);
	labels[LDESCRIPTION]->LFSTK_setCairoFontDataParts("s",14);

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,WINDOWWIDTH,GADEGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Close",WINDOWMIDDLE-HALFGADEGETWIDTH,sy,GADEGETWIDTH,GADEGETHITE,BUTTONGRAV);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);
	sy+=YSPACING+16;

	wc->LFSTK_resizeWindow(WINDOWWIDTH,sy,true);
	wc->LFSTK_showWindow();
	tux->LFSTK_clearWindow();

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
	return 0;
}