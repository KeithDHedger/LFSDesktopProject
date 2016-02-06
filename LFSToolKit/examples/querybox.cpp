#if 0

#©keithhedger Fri 7 Aug 15:57:52 BST 2015 kdhedger68713@gmail.com

g++ -g -O0 "$0" -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft) -llfstoolkit||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs ./a.out "$@"
retval=$?
#rm ./a.out
exit $retval

#endif

#include <stdio.h>
#include <unistd.h>

#include "lfstk/LFSTKGlobals.h"

bool				mainloop=true;
int					width=300;
int					hite=84;
LFSTK_lineEditClass	*le;
LFSTK_windowClass	*mainwind;
bool				toggle=false;
LFSTK_buttonClass	*bc;//,*bc1;
LFSTK_toggleButtonClass	*sticky;
LFSTK_labelClass	*label;

bool callback(void *p,void* ud)
{
	printf("callback = %p - %s\n",ud,le->LFSTK_getBuffer()->c_str());
	toggle=!toggle;
	printf("--toggle= %i--\n",toggle);
	mainwind->LFSTK_setKeepAbove(toggle);
	mainwind->LFSTK_setSticky(toggle);
	printf(">>>>%i<<<<<\n",sticky->LFSTK_getValue());
	if(toggle==true)
		{
			sticky->LFSTK_setLabel("Un-Stick");
			label->LFSTK_setLabel("Window is sticky");
		}
	else
		{
			sticky->LFSTK_setLabel("Stick");
			label->LFSTK_setLabel("Window is normal");
		}
	label->LFSTK_clearWindow();
	sticky->LFSTK_clearWindow();

	XMapWindow(mainwind->display,mainwind->window);

	if((long)ud==1)
		{
			mainloop=false;
			return(false);
		}
	else
		{
			return(true);
		}
}

int main(int argc, char **argv)
{
	printf("Hello World!\n");
	XEvent				event;
	int					sy=24;

	bool				firstrun=true;

	mainwind=new LFSTK_windowClass(0,0,width,hite,"Query Box",false);

	//mainwind->LFSTK_setWindowColourName(NORMALCOLOUR,"grey90");
	label=new LFSTK_labelClass(mainwind,"Window is normal",0,0,width,24,NorthWestGravity);
	//label->LFSTK_setFontColourName(NORMALCOLOUR,"dark grey");
label->LFSTK_setLabelAutoColour(true);

	le=new LFSTK_lineEditClass(mainwind,"Hello World",0,sy,width,24,NorthWestGravity);

	bc=new LFSTK_buttonClass(mainwind,"OK",4,24+4+4+sy,75,24,SouthWestGravity);
	bc->LFSTK_setCallBack(NULL,callback,(void*)1);

	sticky=new LFSTK_toggleButtonClass(mainwind,"Stick",width-4-75,24+4+4+sy,75,24,SouthEastGravity);
	sticky->LFSTK_setCallBack(NULL,callback,(void*)2);
	//sticky->LFSTK_setToggleStyle(TOGGLENORMAL);
	sticky->LFSTK_setToggleStyle(TOGGLECHECK);
	sticky->LFSTK_setValue(true);
	sticky->LFSTK_setActive(true);
	mainwind->LFSTK_showWindow();
	mainwind->LFSTK_clearWindow();

	mainwind->LFSTK_setKeepAbove(toggle);
	mainwind->LFSTK_setSticky(toggle);

	mainloop=true;
	while(mainloop==true)
		{
			listener *l=mainwind->LFSTK_getListener(event.xany.window);
			if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
				l->function(l->pointer,&event,l->type);

			XNextEvent(mainwind->display,&event);
			switch(event.type)
				{
					case LeaveNotify:
						break;
					case Expose:
						mainwind->LFSTK_clearWindow();
						label->LFSTK_clearWindow();
						if(firstrun==true)
							{
								firstrun=false;
								le->LFSTK_setFocus();
							}
						break;

					case ConfigureNotify:
						mainwind->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						mainwind->LFSTK_clearWindow();
						le->LFSTK_resizeWindow(event.xconfigurerequest.width,24);
						le->LFSTK_clearWindow();
						break;

					case ClientMessage:
						if (event.xclient.message_type == XInternAtom(mainwind->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(mainwind->display, "WM_DELETE_WINDOW", 1))
							mainloop=false;
				}
		}

	delete mainwind;
	return 0;
	
}