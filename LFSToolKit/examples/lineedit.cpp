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

#define BOXLABEL	"Line Edit"

LFSTK_windowClass	*wc=NULL;
bool				mainLoop=true;
Display				*display;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

bool doKeyUp(void *p,void* ud)
{
	LFSTK_lineEditClass	*gadg=NULL;
	gadg=static_cast<LFSTK_lineEditClass*>(p);

	if(gadg->LFSTK_getKey()!=XK_VoidSymbol)
		{
			printf(">>%s<<\n",gadg->LFSTK_getCStr());
			printf("Keycode=0x%x\n",gadg->LFSTK_getKey());
			printf("Keysym=XK_%s\n",gadg->LFSTK_getKeySym());
			printf("Modifier=0x%x\n",gadg->LFSTK_getModifier());
		}
	return(true);
}

bool doMouseUp(void *p,void* ud)
{
	LFSTK_lineEditClass	*gadg=NULL;
	gadg=static_cast<LFSTK_lineEditClass*>(p);

	printf("button=%X state=%X\n",gadg->mouseEvent->button,gadg->mouseEvent->state & ControlMask);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
	std::vector<hitRect>	hrs;
	LFSTK_ExpanderGadgetClass	*multi=NULL;

	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,BOXLABEL,false);

	display=wc->display;

	multi=new LFSTK_ExpanderGadgetClass(wc,"",0,0,DIALOGWIDTH,GADGETHITE*3);
	multi->gadgetStretch=SPACESPREADY;

	hrs.push_back({0,0,DIALOGWIDTH,GADGETHITE,new LFSTK_labelClass(wc,BOXLABEL,0,0,1,1)});
	hrs.back().gadget->LFSTK_setCairoFontDataParts("sB",20);
	hrs.push_back({0,0,DIALOGWIDTH,GADGETHITE,new LFSTK_labelClass(wc,COPYRITE,0,0,1,1)});
	hrs.push_back({0,0,DIALOGWIDTH,GADGETHITE,new LFSTK_labelClass(wc,PERSONAL,0,0,1,1)});
	hrs.back().gadget->LFSTK_setCairoFontDataParts("B");
	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

	sy+=YSPACING*2;

//line edit
	multi=new LFSTK_ExpanderGadgetClass(wc,"",BORDER,sy,DIALOGWIDTH-BORDER*2,GADGETHITE);
	multi->stretchX=true;
	multi->gadgetStretch=STRETCH;
	multi->gadgetAcceptsDnD=true;

	hrs.push_back({0,0,DIALOGWIDTH,GADGETHITE,new LFSTK_lineEditClass(wc,"Basic line editing class for LFSToolKit.",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,doMouseUp,NULL);
	hrs.back().gadget->LFSTK_setKeyCallBack(NULL,doKeyUp,USERDATA(12345));
	static_cast<LFSTK_lineEditClass*>(hrs.back().gadget)->LFSTK_setCallbackOnReturn(false);

	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

	sy+=YSPACING;

//bottom bit
	multi=new LFSTK_ExpanderGadgetClass(wc,"",0,sy,DIALOGWIDTH,6);
	multi->stretchX=true;
	multi->lockY=LOCKTOBOTTOM;
	multi->gadgetStretch=STRETCH;

//line
	hrs.push_back({0,0,DIALOGWIDTH,2,NULL});
	hrs.back().gadget=new LFSTK_buttonClass(wc,"--",0,0,DIALOGWIDTH,2,BUTTONGRAV);
	hrs.back().gadget->gadgetDetails.bevel=BEVELNONE;
	hrs.back().gadget->gadgetDetails.buttonTile=false;
	hrs.back().gadget->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];

	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

//quit
	multi=new LFSTK_ExpanderGadgetClass(wc,"",DIALOGMIDDLE-HALFGADGETWIDTH,sy+12,GADGETWIDTH,GADGETHITE);
	multi->lockY=LOCKTOBOTTOM;

	hrs.push_back({0,0,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Quit",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,doQuit,NULL);

	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

	sy+=YSPACING+16;

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
