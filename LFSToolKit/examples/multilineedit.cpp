#if 0
#(c)keithhedger Sat 5 Aug 19:18:08 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o multilineeditexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./multilineeditexample "$@"
retval=$?
echo "Exit code $retval"
rm multilineeditexample
exit $retval
#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Multi-Line Edit"

LFSTK_applicationClass		*apc=NULL;
LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_multiLineEditClass	*editbox=NULL;

const char* test_string =
    "Lorem ipsum dolor sit amet,consectetur "
    "adipiscing elit.\nCras efficitur dui risus, "
    "ac elementum orci rhoncus ac.\nPraesent risus "
    "lacus,pretium in enim at,ringilla viverra "
    "eros.\nUt non tincidunt quam. Donec aliquam "
    "fermentum lectus,eget porttitor metus "
    "fringilla quis.\nDuis nulla augue,commodo "
    "mattis eros at.\n"
    ;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool doKeyUp(void *p,void* ud)
{
	printf(">>%s<<\n",editbox->LFSTK_getCStr());
	return(true);
}

bool doDropped(void *p,propertyStruct *data,void* ud)
{
	LFSTK_gadgetClass	*gadg=NULL;

	gadg=static_cast<LFSTK_gadgetClass*>(p);
	printf("dropped %s on gadget %p @x/y %i %i\n",data->data,gadg,data->dropX,data->dropY);
	return(true);
}

void sendUTF8(XSelectionRequestEvent *sev)
{
	XSelectionEvent	ssev;

    XChangeProperty(apc->display,sev->requestor,sev->property,wc->LFSTK_getDnDAtom(XA_UTF8_STRING),8,PropModeReplace,(unsigned char *)wc->clipBuffer.c_str(),wc->clipBuffer.length());

    ssev.type=SelectionNotify;
    ssev.requestor=sev->requestor;
    ssev.selection=sev->selection;
    ssev.target=sev->target;
    ssev.property=sev->property;
    ssev.time=sev->time;

    XSendEvent(apc->display,sev->requestor,True,NoEventMask,(XEvent *)&ssev);
}

int main(int argc, char **argv)
{
	int	sy=BORDER;
	
	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL);
	wc=apc->mainWindow;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//line edit
	editbox=new LFSTK_multiLineEditClass(wc,test_string,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE*10,BUTTONGRAV);
	editbox->LFSTK_setKeyCallBack(NULL,doKeyUp,USERDATA(12345));
	editbox->LFSTK_setGadgetDropCallBack(doDropped);

	sy+=YSPACING+(GADGETHITE*10);

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

	wc->acceptOnThis=false;
	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	printf("%s\n",editbox->LFSTK_getCStr());
	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}
