#if 0

#©keithhedger Sat 13 Oct 17:20:15 BST 2018 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

APPNAME=$(basename $0 .cpp)

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o $APPNAME||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./$APPNAME "$@"

retval=$?
rm $APPNAME
exit $retval

#endif

#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Basic Window"

LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_buttonClass			*button=NULL;
LFSTK_toggleButtonClass		*useDbg=NULL;

bool						mainLoop=true;
Display						*display;
const char					*dbg;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	char	*command;
	if(ud!=NULL)
		{
			asprintf(&command,"%s %s &",(const char*)ud,dbg);
			system(command);
			free(command);
		}
	return(true);
}

bool doDbg(void *p,void* ud)
{
	char	*command;
	if(p!=NULL)
		{
			if(useDbg->LFSTK_getValue()==true)
				dbg="dbg";
			else
				dbg="";
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;

	dbg="";

	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"Gadgets",false);
	display=wc->display;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

	useDbg=new LFSTK_toggleButtonClass(wc,"Use Valgrind",BORDER,sy,GADGETWIDTH,GADGETHITE,NorthWestGravity);
	useDbg->LFSTK_setValue(false);
	useDbg->LFSTK_setCallBack(NULL,doDbg,NULL);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//run examples
//lineedit
	button=new LFSTK_buttonClass(wc,"Line Edit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./lineedit.cpp");
	sy+=YSPACING;
//multi line
	button=new LFSTK_buttonClass(wc,"Multi Line Edit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./multilineedit.cpp");
	sy+=YSPACING;
//get icon path
	button=new LFSTK_buttonClass(wc,"Get Icon Path",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./geticonpath.cpp");
	sy+=YSPACING;
//basic buttons
	button=new LFSTK_buttonClass(wc,"Buttons",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./basic.cpp");
	sy+=YSPACING;
//toggles
	button=new LFSTK_buttonClass(wc,"Toggles",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./toggle.cpp");
	sy+=YSPACING;
//basic menus
	button=new LFSTK_buttonClass(wc,"Basic Menu",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./basicmenu.cpp");
	sy+=YSPACING;
//sub menus
	button=new LFSTK_buttonClass(wc,"Sub Menus",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./submenus.cpp");
	sy+=YSPACING;
//dialogs
	button=new LFSTK_buttonClass(wc,"Dialogs",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./dialogs.cpp");
	sy+=YSPACING;
//fontdialog
	button=new LFSTK_buttonClass(wc,"Font Dialog",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./fontdialog.cpp");
	sy+=YSPACING;
//list
	button=new LFSTK_buttonClass(wc,"List",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./list.cpp");
	sy+=YSPACING;
//window
	button=new LFSTK_buttonClass(wc,"Window",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./window.cpp");
	sy+=YSPACING;
//subwindow
	button=new LFSTK_buttonClass(wc,"Sub Window",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./subwindowtest.cpp");
	sy+=YSPACING;
//image
	button=new LFSTK_buttonClass(wc,"Image",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./image.cpp");
	sy+=YSPACING;
//image buttons
	button=new LFSTK_buttonClass(wc,"Image Buttons",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./imagebutton.cpp");
	sy+=YSPACING;
//lfsabout
	button=new LFSTK_buttonClass(wc,"LFS About",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./lfsabout.cpp");
	sy+=YSPACING;
//run command
	button=new LFSTK_buttonClass(wc,"Run Command",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,buttonCB,(void*)"./lfsruncommand.cpp");
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
				ml->function(ml->gadget,&event,ml->type);

			if(wc->LFSTK_handleWindowEvents(&event)<0)
				mainLoop=false;
		}

	delete wc;
	XCloseDisplay(display);
	cairo_debug_reset_static_data();
	return 0;
}