#if 0

#©keithhedger Wed 2 Aug 15:43:22 BST 2017 keithdhedger@gmail.com

g++ "$0" -O0 -ggdb -I/media/LinuxData/Development64/Projects/LFSDesktopProject/LFSToolKit/LFSToolKit -L/media/LinuxData/Development64/Projects/LFSDesktopProject/LFSToolKit/LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2||exit 1
LD_LIBRARY_PATH=/media/LinuxData/Development64/Projects/LFSDesktopProject/LFSToolKit/LFSToolKit/app/.libs ./a.out "$@"
retval=$?
rm ./a.out
exit $retval

#endif

#include "lfstk/LFSTKGlobals.h"

#define WINDOWWIDTH 320
#define WINDOWHITE 10
#define WINDOWMIDDLE WINDOWWIDTH/2
#define SPACING 24
#define HALFSPACING SPACING/2

enum {LNAME,LDESCRIPTION,LCOPYRITE,LEMAIL,LWEBSITE,LGPL,LNOMORELABELS};

const char	*labelTexts[]={"LFS Desktop Project","Low resource/dependency desktop for LFS",COPYRITE,PERSONAL,MYWEBSITE,"Released under GPLv3"};

#define IMAGESIZE		64

LFSTK_windowClass		*wc=NULL;
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
		
	wc=new LFSTK_windowClass(0,0,WINDOWWIDTH,WINDOWHITE,"LFS Desktop Project",false);
	display=wc->display;

	tux=new LFSTK_imageClass(wc,NULL,WINDOWMIDDLE-(IMAGESIZE/2),sy,IMAGESIZE,IMAGESIZE,BUTTONGRAV,true);
	tux->LFSTK_setImageFromPath("/usr/share/pixmaps/LFSTux.png",AUTO,true);
	tux->LFSTK_clearWindow();
	sy+=SPACING*3;


	for(int j=LNAME;j<LNOMORELABELS;j++)
		{
			labels[j]=new LFSTK_labelClass(wc,labelTexts[j],0,sy,WINDOWWIDTH,GADGETHITE);
			sy+=SPACING;
		}
	labels[LNAME]->LFSTK_setCairoFontDataParts("sB",18);
	labels[LDESCRIPTION]->LFSTK_setCairoFontDataParts("s",14);

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,WINDOWWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=SPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Close",WINDOWMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	sy+=SPACING+16;

	wc->LFSTK_resizeWindow(WINDOWWIDTH,sy,true);
	wc->LFSTK_showWindow();
	tux->LFSTK_clearWindow();

	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);

			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			if(wc->LFSTK_handleWindowEvents(&event)<0)
				mainLoop=false;
		}

	delete wc;
	XCloseDisplay(display);

	return 0;
}