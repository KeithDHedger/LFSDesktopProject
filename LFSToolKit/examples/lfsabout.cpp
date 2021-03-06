#if 0

#©keithhedger Wed 2 Aug 15:43:22 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o lfsaboutexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./lfsaboutexample "$@"
retval=$?
echo "Exit code $retval"
rm lfsaboutexample
exit $retval
#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#undef DIALOGWIDTH
#define DIALOGWIDTH		320
#define SPACING			24
#define HALFSPACING		SPACING/2
#define BOXLABEL		"LFS About"
enum		{LNAME,LDESCRIPTION,LCOPYRITE,LEMAIL,LWEBSITE,LGPL,LNOMORELABELS};

const char	*labelTexts[]={"LFS Desktop Project","Low resource/dependency desktop for LFS",COPYRITE,PERSONAL,MYWEBSITE,"Released under GPLv3"};

#define IMAGESIZE		64

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_imageClass		*tux;
LFSTK_labelClass		*labels[LNOMORELABELS];

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

int main(int argc, char **argv)
{
	int	sy=BORDER;
		
	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL);
	wc=apc->mainWindow;

	tux=new LFSTK_imageClass(wc,NULL,DIALOGMIDDLE-(IMAGESIZE/2),sy,IMAGESIZE,IMAGESIZE,BUTTONGRAV,true);
	tux->LFSTK_setImageFromPath("/usr/share/pixmaps/LFSTux.png",AUTO,true);
	tux->LFSTK_clearWindow();
	sy+=SPACING*3;


	for(int j=LNAME;j<LNOMORELABELS;j++)
		{
			labels[j]=new LFSTK_labelClass(wc,labelTexts[j],0,sy,DIALOGWIDTH,GADGETHITE);
			sy+=SPACING;
		}
	labels[LNAME]->LFSTK_setCairoFontDataParts("sB",18);
	labels[LDESCRIPTION]->LFSTK_setCairoFontDataParts("s",14);

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=SPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Close",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	sy+=SPACING+16;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	tux->LFSTK_clearWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}