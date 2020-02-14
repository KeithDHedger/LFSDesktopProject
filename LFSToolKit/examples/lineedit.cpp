#if 0

#©keithhedger Sat 5 Aug 19:18:08 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o lineeditexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./lineeditexample "$@"
retval=$?
echo "Exit code $retval"
rm lineeditexample
exit $retval
#endif

#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Line Edit"

LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_lineEditClass			*editbox=NULL;

bool						mainLoop=true;
Display						*display;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

bool doKeyUp(void *p,void* ud)
{
	printf(">>%s<<\n",editbox->LFSTK_getCStr());
	return(true);
}

bool buttonCB(void *p,void* ud)
{
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
		
	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,BOXLABEL,false);

	display=wc->display;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//line edit
	editbox=new LFSTK_lineEditClass(wc,"Basic line editing class for LFSToolKit.",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
//	editbox=new LFSTK_lineEditClass(wc,"A123456789012345678901234567890123456789012345678901234567890123456789012345678Z",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);

	editbox->LFSTK_setCallBack(NULL,doKeyUp,USERDATA(12345));
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
printf("quitx=%i quity=%i\n",DIALOGMIDDLE-HALFGADGETWIDTH,sy);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);
	sy+=YSPACING;

	wc->acceptOnThis=false;
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
