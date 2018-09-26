#if 0

#©keithhedger Sat 5 Aug 19:18:08 BST 2017 kdhedger68713@gmail.com

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o lineeditexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs ./lineeditexample "$@"
retval=$?
echo "Exit code $retval"
exit $retval
#endif

#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Get Path To Themed Icon"

LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_buttonClass			*getIconPath=NULL;
LFSTK_lineEditClass			*mimeEdit=NULL;
LFSTK_lineEditClass			*catEdit=NULL;

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
	//printf(">>%s<<\n",mimeEdit->LFSTK_getBuffer()->c_str());
	return(true);
}


bool getPath(void *p,void* ud)
{

	char	*iconpath=NULL;
	char        *theme=NULL;

	theme=wc->globalLib->LFSTK_oneLiner("head ~/.config/LFS/lfsdesktop2.rc|grep -i icontheme|awk '{print $2}'");
	iconpath=(char*)wc->globalLib->LFSTK_findThemedIcon(theme,mimeEdit->LFSTK_getBuffer()->c_str(),catEdit->LFSTK_getBuffer()->c_str());

	if(iconpath!=NULL)
		printf("iconpath=%s\n",iconpath);
	free(iconpath);
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

//mime edit
	label=new LFSTK_labelClass(wc,"Icon Name",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("B");
	sy+=GADGETHITE;

	mimeEdit=new LFSTK_lineEditClass(wc,"drive-harddisk",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	mimeEdit->LFSTK_setCallBack(NULL,doKeyUp,USERDATA(12345));
	sy+=YSPACING;

//catagory edit
	label=new LFSTK_labelClass(wc,"Catagory",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("B");
	sy+=GADGETHITE;
	catEdit=new LFSTK_lineEditClass(wc,"devices",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	catEdit->LFSTK_setCallBack(NULL,doKeyUp,USERDATA(12345));
	sy+=YSPACING;

//get path to icon
	getIconPath=new LFSTK_buttonClass(wc,"Get Path",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	getIconPath->LFSTK_setCallBack(NULL,getPath,NULL);
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



//	Atom XdndAware=XInternAtom(wc->display,"XdndAware",false);
//	Atom version=5;
//	XChangeProperty(wc->display,wc->window,XdndAware,XA_ATOM,32,PropModeReplace,(unsigned char*)&version,1);

//	wc->LFSTK_initDnD();
//	XEvent xev;
//	XWindowAttributes wattr;
//	memset(&xev,0,sizeof(xev));
//	xev.type=ClientMessage;
//	xev.xclient.display=wc->display;
//	xev.xclient.window=editbox->window;
//	xev.xclient.message_type=XInternAtom(wc->display,"_NET_ACTIVE_WINDOW",false);
//	xev.xclient.format=32;
//	xev.xclient.data.l[0]=2L;
//	xev.xclient.data.l[1]=CurrentTime;
//
//	XGetWindowAttributes(wc->display,editbox->window,&wattr);
//	XSendEvent(wc->display,wattr.screen->root,false,SubstructureNotifyMask|SubstructureRedirectMask,&xev);

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
//dnd for edit box
							if(wc->acceptDnd==true)
								{
									wc->LFSTK_handleDnD(&event);
									if((wc->droppedData.type!=-1) && (wc->acceptOnThis==true))
										{
											printf("dropped on window=>>%s<<\n",wc->droppedData.data);
											wc->droppedData.type=DROPINVALID;
											
										}
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
