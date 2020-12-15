#if 0

#©keithhedger Fri 28 Jul 20:19:00 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o windowexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./windowexample "$@"
retval=$?
echo "Exit code $retval"
rm windowexample
exit $retval
#endif

#include "../config.h"
#include "../LFSToolKit/lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Multi Hitpoint Gadget"

LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_MultiGadgetClass	*multi=NULL;

bool					mainLoop=true;
Display					*display;

bool mouseparentCB(void *p,void* ud)
{
	LFSTK_MultiGadgetClass	*gadg=NULL;
	gadg=static_cast<LFSTK_MultiGadgetClass*>(p);

	for(int j=0;j<gadg->hitRects.size();j++)
		{
			if(gadg->hitRects.at(j).gadget->window==gadg->xEvent->xbutton.subwindow)
				{
					printf("Gadget No. %i Label=\"%s\" found\n",j,gadg->hitRects.at(j).gadget->LFSTK_getLabel());
				}
		}
	return(true);
}

bool mouseCB(void *p,void* ud)
{
	printf("Normal gadget callback\n");
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=0;
	cairo_surface_t	*surfaceto,*surfacefrom;
		
	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,BOXLABEL,false);
	display=wc->display;

	label=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	label=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	multi=new LFSTK_MultiGadgetClass(wc,"",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*4);
	multi->LFSTK_setMouseCallBack(NULL,mouseparentCB,NULL);
	multi->stretchX=false;
	multi->stretchY=true;
	multi->lockY=LOCKTOCENTRE;
	multi->lockX=LOCKTOCENTRE;

	geometryStruct geom;
	multi->LFSTK_getGeom(&geom);

	std::vector<hitRect>	hrs;

	hrs.push_back({0,0,GADGETWIDTH*2,GADGETHITE,NULL});
	hrs.back().gadget=new LFSTK_buttonClass(wc,"Normal",0,0,10,10);
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,mouseCB,(void*)multi);

	hrs.push_back({0,GADGETHITE*3,GADGETWIDTH*2,GADGETHITE,NULL});
	hrs.back().gadget=new LFSTK_labelClass(wc,"Label Gadget",0,0,1,1);
	hrs.back().gadget->toParent=true;

	hrs.push_back({0,GADGETHITE*2,GADGETWIDTH*2,GADGETHITE,NULL});
	hrs.back().gadget=new LFSTK_buttonClass(wc,"Button Gadget",0,0,1,1);
	hrs.back().gadget->toParent=true;

	sy+=YSPACING+GADGETHITE*4;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	multi->LFSTK_setHitRects(hrs);

	fprintf(stderr,"%s\n",wc->globalLib->LFSTK_getGlobalString(-1,TYPEMONOFONT));
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