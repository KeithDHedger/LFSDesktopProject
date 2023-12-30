#if 0

#(c)keithhedger Sat 29 Jul 17:18:27 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o toggleexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./toggleexample "$@"
retval=$?
echo "Exit code $retval"
rm toggleexample
exit $retval
#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL		"Toggle/Check Buttons"

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;
LFSTK_toggleButtonClass	*check=NULL;
LFSTK_toggleButtonClass	*toggle=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
char					*iconH=NULL;
char					*iconM=NULL;
char					*iconL=NULL;


LFSTK_windowClass *scwindow;
LFSTK_scrollBarClass	*vsb;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	LFSTK_toggleButtonClass	*t=NULL;
	if(p!=NULL)
		{
			t=static_cast<LFSTK_toggleButtonClass*>(p);
			if(t!=NULL)
				{
					printf("Value=%i\n",t->LFSTK_getValue());
					if(t->userData!=NULL)
						{
							if(t->LFSTK_getValue()==1)
								{
									geometryStruct		geom;

									t->gadgetDetails.showBroken=true;
									t->LFSTK_getGeomWindowRelative(&geom,apc->rootWindow);
									scwindow->LFSTK_moveWindow(geom.x+geom.w,geom.y,true);
									scwindow->LFSTK_showWindow(true);
									scwindow->LFSTK_redrawAllGadgets();
									apc->windows->at(apc->LFSTK_findWindow(scwindow)).showing=true;
								}
							else
								{
									t->gadgetDetails.showBroken=false;
									scwindow->LFSTK_hideWindow();
									apc->windows->at(apc->LFSTK_findWindow(scwindow)).showing=false;
								}
							t->LFSTK_clearWindow();
						}
				}
		}

	if(ud!=NULL)
		{
			printf(">>>%s<<<\n",(const char*)ud);
			
		}
	return(true);
}

bool enterCB(LFSTK_gadgetClass*p,void* ud)
{
	printf(">>>Mouse Enter<<<\n");
	return(true);
}

bool exitCB(LFSTK_gadgetClass*p,void* ud)
{
	printf(">>>Mouse Exit<<<\n");
	return(true);
}

int main(int argc, char **argv)
{
	int	sy=BORDER;
		
	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKExample");
	wc=apc->mainWindow;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//check button	
	check=new LFSTK_toggleButtonClass(wc,"Check Box",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,CHECKBOXSIZE);
	check->LFSTK_setValue(true);
	check->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)check->LFSTK_getLabel().c_str());
	sy+=YSPACING;

//toggle button	
	toggle=new LFSTK_toggleButtonClass(wc,"Toggle",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETWIDTH);
	toggle->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)toggle->LFSTK_getLabel().c_str());
	toggle->LFSTK_setMouseMoveCallBack(enterCB,exitCB,USERDATA(0));
	toggle->LFSTK_setToggleStyle(TOGGLENORMAL);
	toggle->userData=USERDATA(1);

	iconH=apc->globalLib->LFSTK_findThemedIcon("gnome","volume-high","");
	toggle->LFSTK_setImageFromPath(iconH,TOOLBAR,true);
	//toggle->gadgetDetails.showBroken=true;
	//toggle->gadgetDetails.showLink=true;
	free(iconH);
	sy+=YSPACING*3;

	windowInitStruct	*win;

	win=new windowInitStruct;//TODO//
	win->x=100;
	win->y=100;
	win->w=16;
	win->h=100;
	//win->wc=wc;
//	win->windowType="_NET_WM_WINDOW_TYPE_DOCK";
	win->decorated=false;
	win->overRide=true;
//	win->level=ABOVEALL;
	win->app=apc;
	win->windowType=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_TOOL"));
	apc->LFSTK_addToolWindow(win);
	scwindow=apc->windows->back().window;//TODO//
	//XReparentWindow(apc->display,scwindow->window,wc->window,20,20);
	
	vsb=new LFSTK_scrollBarClass(scwindow,true,0,0,16,100);

	vsb->LFSTK_setValue(1);

//toggle->LFSTK_setContextWindow(scwindow);

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	//wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}