#if 0

#(c)keithhedger Mon 28 Aug 15:32:14 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o subwindowexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./subwindowexample "$@"
retval=$?
echo "Exit code $retval"
rm subwindowexample
exit $retval
#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Sub Window Test"

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_windowClass		*transwc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_buttonClass		*test=NULL;
LFSTK_buttonClass		*transtest=NULL;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool doSubQuit(void *p,void* ud)
{
	long	subwin=(long)ud;
	apc->windows->at(subwin).loopFlag=false;
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	LFSTK_buttonClass	*button=NULL;
	long					subwin=(long)ud;

	apc->LFSTK_runWindowLoop(subwin);
	return(true);
}

bool transButtonCB(void *p,void* ud)
{
	LFSTK_buttonClass	*button=NULL;
	long					subwin=(long)ud;

	apc->LFSTK_runWindowLoop(subwin);
	return(true);
}

bool doTransSubQuit(void *p,void* ud)
{
	long	subwin=(long)ud;
	apc->windows->at(subwin).loopFlag=false;
	return(false);
}

int cnt=1;
bool timerCB(LFSTK_applicationClass *p,void* ud)
{
	printf("From window %s\n",ud);
	printf("Timer callback number %i of 10\n",cnt);
	cnt++;
	if(cnt<11)
		return(true);
	else
		{
	//transtest->LFSTK_setGadgetSize(100,100);
			return(false);
		}
}

int main(int argc, char **argv)
{
	int					sy=BORDER;
	LFSTK_buttonClass	*button=NULL;
	
	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKExample");
	wc=apc->mainWindow;
	wc->userData=USERDATA("Main Window");
	//wc->LFSTK_setWindowColourName(NORMALCOLOUR,"#20ff0000");

	apc->LFSTK_addWindow(NULL,"SUB WINDOW");
	apc->windows->back().window->LFSTK_resizeWindow(400,400,true);
	apc->windows->back().window->userData=USERDATA("Sub Window");
	button=new LFSTK_buttonClass(apc->windows->back().window,"Close",200-HALFGADGETWIDTH,200,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setMouseCallBack(NULL,doSubQuit,(void*)(apc->windows->size()-1));

	apc->LFSTK_addWindow(NULL,"TRANSPARENT SUB WINDOW");
	apc->windows->back().window->LFSTK_resizeWindow(400,400,true);
	apc->windows->back().window->userData=USERDATA("Trans Sub Window");
	apc->windows->back().window->LFSTK_setTile(NULL,0);
	apc->windows->back().window->LFSTK_setWindowColourName(NORMALCOLOUR,"#40ff0000");
	//apc->windows->back().window->customwindow=true;
	label=new LFSTK_labelClass(apc->windows->back().window,BOXLABEL,0,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	label->LFSTK_setTile(NULL,0);

	button=new LFSTK_buttonClass(apc->windows->back().window,"Close",200-HALFGADGETWIDTH,200,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setMouseCallBack(NULL,doTransSubQuit,(void*)(apc->windows->size()-1));
//button->LFSTK_setColourName(NORMALCOLOUR,"#20ffff00");
	button->LFSTK_setTile(NULL,0);

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//test
	test=new LFSTK_buttonClass(wc,"Sub-Window",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	test->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)(apc->windows->size()-2));
	sy+=YSPACING;

	transtest=new LFSTK_buttonClass(wc,"Transparent Sub-Window",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	transtest->LFSTK_setMouseCallBack(NULL,transButtonCB,(void*)(apc->windows->size()-1));
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	apc->LFSTK_setTimer(1);
	apc->LFSTK_setTimerCallBack(timerCB,NULL);
	int retval=apc->LFSTK_runApp();

	delete apc;
	cairo_debug_reset_static_data();
	return retval;
}