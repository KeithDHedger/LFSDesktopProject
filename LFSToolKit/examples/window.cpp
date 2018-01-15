#if 0

#©keithhedger Fri 28 Jul 20:19:00 BST 2017 kdhedger68713@gmail.com

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o windowexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs ./windowexample "$@"
retval=$?
echo "Exit code $retval"
exit $retval
#endif

#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Basic Window"

LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;

bool					mainLoop=true;
Display					*display;

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=0;
	cairo_surface_t	*surfaceto,*surfacefrom;
		
	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,BOXLABEL,false);
	display=wc->display;

	label=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,NorthGravity);
	sy+=HALFYSPACING;
	label=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,NorthGravity);
	label->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,NorthGravity);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;
	sy+=100;

	wc->LFSTK_setWindowPixmap(wc->globalLib->LFSTK_getWindowPixmap(display,wc->rootWindow),DisplayWidth(display,wc->screen),DisplayHeight(display,wc->screen));
	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				{
					ml->function(ml->gadget,&event,ml->type);
				}

			switch(event.type)
				{
					case ButtonRelease:
						break;
					case LeaveNotify:
						break;
					case Expose:
					//printf("expose\n");
						wc->LFSTK_clearWindow();
						break;

					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
						wc->globalLib->LFSTK_setCairoSurface(wc->display,wc->window,wc->visual,&wc->sfc,&wc->cr,event.xconfigurerequest.width,event.xconfigurerequest.height);
						wc->LFSTK_clearWindow();
						break;

					case ClientMessage:
					case SelectionNotify:
						{
							if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
								{
									wc->LFSTK_hideWindow();
									mainLoop=false;
								}
						}
						break;
				}
		}

	delete wc;
	XCloseDisplay(display);
	cairo_debug_reset_static_data();
	return 0;
}