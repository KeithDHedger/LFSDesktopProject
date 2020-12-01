#if 0

#©keithhedger Fri 28 Jul 20:19:00 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o basicexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./basicexample "$@"
retval=$?
echo "Exit code $retval"
rm basicexample
exit $retval
#endif

#include "../config.h"
#include "../LFSToolKit/lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Basic Button Window"

LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_buttonClass			*inactivebutton=NULL;
LFSTK_buttonClass			*normalbutton=NULL;
LFSTK_buttonClass			*ignoredButton=NULL;
LFSTK_buttonClass			*leftButton=NULL;
LFSTK_buttonClass			*centreButton=NULL;
LFSTK_buttonClass			*rightButton=NULL;

bool						mainLoop=true;
Display						*display;
bool						show=true;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	if(ud!=NULL)
		{
			printf(">>>%s<<<\n",(const char*)ud);
			if(strcmp((const char*)ud,"Active")==0)
				{
					show=!show;
					if(show==true)
						inactivebutton->LFSTK_showGadget();
					else
						inactivebutton->LFSTK_hideGadget();
					wc->LFSTK_clearWindow();
				}
		}
	return(true);
}

bool keyCB(void *p,void* ud)
{
	LFSTK_buttonClass	*b=NULL;
	if(p!=NULL)
		{
			b=static_cast<LFSTK_buttonClass*>(p);
			if(b!=NULL)
				DEBUGKEY((XKeyEvent*)b->xEvent)
		}

	if(ud!=NULL)
		{
			printf(">>>%s<<<\n",(const char*)ud);
		}
	return(true);
}

bool mouseCB(void *p,void* ud)
{
	DEBUG
	if(ud!=NULL)
		{
			printf(">>>%s<<<\n",(const char*)ud);
		}
	return(true);
}


int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
	windowInitStruct	*wi;

	wi=new windowInitStruct;
	wi->x=100;
	wi->y=100;
	wi->w=DIALOGWIDTH;
	wi->h=DIALOGHITE;
	wi->name=BOXLABEL;
//tests
//	wi->decorated=true;
//	wi->windowType="_NET_WM_WINDOW_TYPE_NORMAL";
//	wi->overRide=true;
//	wi->level=BELOWALL;

	wc=new LFSTK_windowClass(wi);
	display=wc->display;
	delete wi;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,NorthGravity);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,NorthGravity);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,NorthGravity);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//inactivebutton
	inactivebutton=new LFSTK_buttonClass(wc,"Inactive",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,NorthGravity);
	inactivebutton->LFSTK_setActive(false);
	sy+=YSPACING;

//normal button
	normalbutton=new LFSTK_buttonClass(wc,"Show/Hide",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,NorthGravity);
	normalbutton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"Active");
	sy+=YSPACING;

//ignoredButton button
	ignoredButton=new LFSTK_buttonClass(wc,"Ignore CB",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,NorthGravity);
	ignoredButton->LFSTK_setIgnores(&ignoredButton->mouseCB,false,true);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//left
	leftButton=new LFSTK_buttonClass(wc,"Label Left",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,NorthGravity);
	leftButton->LFSTK_setLabelGravity(LEFT);
	//leftButton->LFSTK_setCallBack(NULL,mouseCB,(void*)"Left");
	leftButton->LFSTK_setKeyCallBack(NULL,keyCB,(void*)"Key Left");
	leftButton->LFSTK_setMouseCallBack(NULL,mouseCB,(void*)"Mouse Left");
	//leftButton->LFSTK_setIgnores(&leftButton->mouseCB,false,true);
	//leftButton->LFSTK_setIgnores(&leftButton->keyCB,false,true);
	sy+=YSPACING;
//centre
	centreButton=new LFSTK_buttonClass(wc,"Label Centre",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,NorthGravity);
	centreButton->LFSTK_setLabelGravity(CENTRE);
	centreButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"Centre");
	sy+=YSPACING;
//rite
	rightButton=new LFSTK_buttonClass(wc,"Label Right",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,NorthGravity);
	rightButton->LFSTK_setLabelGravity(RIGHT);
	rightButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"Right");
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//left image
	leftButton=new LFSTK_buttonClass(wc,"Left Label+Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,NorthGravity);
	leftButton->LFSTK_setImageFromPath("./casper2.JPG",LEFT,true);
	leftButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"Left Label+Image");
	sy+=YSPACING;
//centre image
	centreButton=new LFSTK_buttonClass(wc,"Centre Label+Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,NorthGravity);
	centreButton->LFSTK_setImageFromPath("./casper2.JPG",LEFT,true);
	centreButton->LFSTK_setLabelGravity(CENTRE);
	centreButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"Centre Label+Image");
	sy+=YSPACING;
//rite image
	rightButton=new LFSTK_buttonClass(wc,"Right Label+Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,NorthGravity);
	rightButton->LFSTK_setImageFromPath("./casper2.JPG",LEFT,true);
	rightButton->LFSTK_setLabelGravity(RIGHT);
	rightButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"Right Label+Image");
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,NorthGravity);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
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
				ml->function(ml->gadget,&event,ml->type);

			if(wc->LFSTK_handleWindowEvents(&event)<0)
				mainLoop=false;
		}

	delete wc;
	XCloseDisplay(display);
	cairo_debug_reset_static_data();
	return 0;
}