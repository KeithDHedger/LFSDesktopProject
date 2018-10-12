#if 0

#©keithhedger Mon 28 Aug 15:32:14 BST 2017 kdhedger68713@gmail.com

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o subwindowexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs ./subwindowexample "$@"
retval=$?
echo "Exit code $retval"
rm subwindowexample
exit $retval
#endif

#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Sub Window Test"

LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_buttonClass			*test=NULL;

bool						mainLoop=true;
Display						*display;

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
			LFSTK_windowClass		*subwc=NULL;
			LFSTK_labelClass		*label=NULL;
			bool					subwcloop=true;
			XEvent					event;
			LFSTK_buttonClass		*button=NULL;

			subwc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"SUB WINDOW",false,true,true);
			button=new LFSTK_buttonClass(subwc,"Close",200-HALFGADGETWIDTH,200,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
			subwcloop=true;
			subwc->LFSTK_resizeWindow(400,400,true);
			subwc->LFSTK_showWindow();

			while(subwcloop==true)
				{
					XNextEvent(subwc->display,&event);
					mappedListener *ml=subwc->LFSTK_getMappedListener(event.xany.window);
					if(ml!=NULL)
						{
							ml->function(ml->gadget,&event,ml->type);
						}

					switch(event.type)
						{
							case ButtonRelease:
								subwc->LFSTK_hideWindow();
								subwcloop=false;
								break;
							case LeaveNotify:
								break;
							case Expose:
								subwc->LFSTK_clearWindow();
								break;

							case ConfigureNotify:
							//printf("---->\n");
								subwc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
								subwc->globalLib->LFSTK_setCairoSurface(subwc->display,subwc->window,subwc->visual,&subwc->sfc,&subwc->cr,event.xconfigurerequest.width,event.xconfigurerequest.height);
								subwc->LFSTK_clearWindow();
								break;

							case ClientMessage:
							case SelectionNotify:
								{
									if (event.xclient.message_type == XInternAtom(subwc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(subwc->display, "WM_DELETE_WINDOW", 1))
										{
											subwc->LFSTK_hideWindow();
											subwcloop=false;
										}
								}
								break;
						}
				}
			delete subwc;
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
		
	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"Sub Window Example",false);
	display=wc->display;

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
	test=new LFSTK_buttonClass(wc,"Sub-Window",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	test->LFSTK_setCallBack(NULL,buttonCB,(void*)0xdeadbeef);
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);
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