#if 0

#©keithhedger Fri 7 Aug 15:57:52 BST 2015 kdhedger68713@gmail.com

g++ "$0" -O3 -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo) -llfstoolkit -lImlib2||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs ./a.out "$@"
retval=$?
rm ./a.out
exit $retval

#endif


/*
 *
 * ©K. D. Hedger. Thu  5 Nov 15:09:23 GMT 2015 kdhedger68713@gmail.com

 * This file (lfsabout.cpp) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <unistd.h>

#include <lfstk/LFSTKGlobals.h>

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

const char	*labelTexts[]={"LFS Desktop Project","Low resource/dependency desktop for LFS","© 2015-2016 K.D.Hedger",MYEMAIL,MYWEBSITE,"Released under GPLv3"};

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
	LFSTK_imageClass	*icon;	
	LFSTK_windowClass	*wc;
	LFSTK_buttonClass	*quit;
	LFSTK_labelClass	*labels[LNOMORELABELS];
	
	wc=new LFSTK_windowClass(0,0,WINWIDTH,WINHITE,"About LFS Desktop",false);

	icon=new LFSTK_imageClass(wc,"/usr/share/pixmaps/LFSTux.png",MIDWIN-(ICONBUTTON/2),sy,ICONBUTTON,ICONBUTTON,NorthGravity,true);
//	imag=new LFSTK_imageClass(wc,NULL,BORDER+100,sy,IMAGESIZE,IMAGESIZE,NONE,true);
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
	wc->LFSTK_setKeepAbove(true);

	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);
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

	delete wc;
	return 0;
}