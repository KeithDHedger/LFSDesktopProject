#if 0

#©keithhedger Fri 7 Aug 15:57:52 BST 2015 kdhedger68713@gmail.com

g++ "$0" -O3 -I../LFSToolKit/src -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft ) -llfstoolkit -lImlib2||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs ./a.out "$@"
retval=$?
rm ./a.out
exit $retval

#endif

#include <stdio.h>
#include <unistd.h>

//#include <LFSTKWindow.h>
//#include <LFSTKButton.h>
//#include "LFSTKLabel.h"
#include "LFSTKGlobals.h"

#define		WINWIDTH 352
#define		WINHITE 240
#define		ICONBUTTON 48
#define		MIDWIN WINWIDTH/2
#define		BUTTONWITDH 64
#define		BUTTONHITE 24
#define		YSPACING BUTTONHITE
#define		BORDER 8

#define		MYEMAIL "keithhedger@keithhedger.darktech.org"
#define		MYWEBSITE "http://keithhedger.darktech.org/"

enum		{LNAME,LDESCRIPTION,LCOPYRITE,LEMAIL,LWEBSITE,LGPL,LNOMORELABELS};

const char	*labelTexts[]={"LFS Desktop Project","Low resource/dependency desktop for LFS","© 2015 K.D.Hedger",MYEMAIL,MYWEBSITE,"Released under GPLv3"};

bool		mainLoop=true;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	return(false);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
	Pixmap	ic[2];
	LFSTK_buttonClass	*icon;	
	LFSTK_windowClass	*wc;
	LFSTK_buttonClass	*quit;
	LFSTK_labelClass	*labels[LNOMORELABELS];
	
	wc=new LFSTK_windowClass(0,0,WINWIDTH,WINHITE,"About LFS Desktop",false);

	icon=new LFSTK_buttonClass(wc,"",MIDWIN-(ICONBUTTON/2),sy,ICONBUTTON,ICONBUTTON,NorthGravity);
	icon->LFSTK_setIconFromPath("/usr/share/pixmaps/LFSTux.png",ICONBUTTON-4);
	icon->LFSTK_setActive(false);

	quit=new LFSTK_buttonClass(wc,"Close",MIDWIN-(BUTTONWITDH/2),WINHITE-BUTTONHITE-BORDER,BUTTONWITDH,BUTTONHITE,SouthGravity);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);

	sy=ICONBUTTON+BORDER;
	for(int j=LNAME;j<LNOMORELABELS;j++)
		{
			labels[j]=new LFSTK_labelClass(wc,labelTexts[j],0,sy,WINWIDTH,BUTTONHITE,CenterGravity);
			labels[LNAME]->LFSTK_setFontString("sans-serif:size=8");
			sy+=YSPACING;
		}
	labels[LNAME]->LFSTK_setFontString("sans-serif:size=14:bold");
	labels[LDESCRIPTION]->LFSTK_setFontString("sans-serif:size=12");

	wc->LFSTK_showWindow();

	mainLoop=true;
	while(mainLoop==true)
		{
			listener *l=wc->LFSTK_getListener(event.xany.window);
			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);

			XNextEvent(wc->display,&event);
			switch(event.type)
				{
					case LeaveNotify:
						break;
					case Expose:
						wc->LFSTK_clearWindow();
						break;

					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						break;

					case ClientMessage:
						if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
							mainLoop=false;
				}
		}

	for(int j=LNAME;j<LNOMORELABELS;j++)
		delete labels[j];
	delete icon;
	delete quit;
	delete wc;
	return 0;
}