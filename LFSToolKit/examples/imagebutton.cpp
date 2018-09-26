#if 0

#©keithhedger Fri 4 Aug 20:10:13 BST 2017 kdhedger68713@gmail.com

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o imagebuttonexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs ./imagebuttonexample "$@"
retval=$?
echo "Exit code $retval"
exit $retval
#endif

#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Image Buttons"

LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*imagebuttonL=NULL;
LFSTK_buttonClass			*imagebuttonC=NULL;
LFSTK_buttonClass			*imagebuttonR=NULL;
LFSTK_buttonClass			*imagebuttonT=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;

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
			printf(">>>%p<<<\n",(const char*)ud);
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
		
	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"Image Button Example",false);
	display=wc->display;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//image button toolbutton
	imagebuttonT=new LFSTK_buttonClass(wc,"Tool Button",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*2,BUTTONGRAV);
	imagebuttonT->LFSTK_setCallBack(NULL,buttonCB,(void*)0xdeadbeef0);
	imagebuttonT->LFSTK_setImageFromPath("./AspellGUI.png",TOOLBAR,true);
	sy+=YSPACING*2;

//image button left
	imagebuttonL=new LFSTK_buttonClass(wc,"Left Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*2,BUTTONGRAV);
	imagebuttonL->LFSTK_setCallBack(NULL,buttonCB,(void*)0xdeadbeef1);
	imagebuttonL->LFSTK_setImageFromPath("./casper2.JPG",LEFT,true);
	imagebuttonL->LFSTK_setLabelGravity(CENTRE);
	sy+=YSPACING*2;

//image button centre
	imagebuttonC=new LFSTK_buttonClass(wc,"",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*2,BUTTONGRAV);
	imagebuttonC->LFSTK_setCallBack(NULL,buttonCB,(void*)0xdeadbeef2);
	imagebuttonC->LFSTK_setImageFromPath("./casper2.JPG",CENTRE,true);

	sy+=YSPACING*2;

//image button right
	imagebuttonR=new LFSTK_buttonClass(wc,"Right Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*2,BUTTONGRAV);
	imagebuttonR->LFSTK_setCallBack(NULL,buttonCB,(void*)0xdeadbeef3);
	imagebuttonR->LFSTK_setImageFromPath("./casper2.JPG",RIGHT,true);
	imagebuttonR->LFSTK_setLabelGravity(CENTRE);
	imagebuttonR->gadgetDetails.reserveSpace=4;
	sy+=YSPACING*2;

//image button menu
	imagebuttonR=new LFSTK_buttonClass(wc,"Menu Image Button",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	imagebuttonR->LFSTK_setCallBack(NULL,buttonCB,(void*)0xdeadbeef4);
	imagebuttonR->LFSTK_setImageFromPath("./casper2.JPG",MENU,true);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
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