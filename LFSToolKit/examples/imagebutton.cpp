#if 0

#(c)keithhedger Fri 4 Aug 20:10:13 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o imagebuttonexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./imagebuttonexample "$@"
retval=$?
echo "Exit code $retval"
rm imagebuttonexample
exit $retval
#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Image Buttons"

LFSTK_applicationClass		*apc=NULL;
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

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
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
	int	sy=BORDER;
		
	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL);
	wc=apc->mainWindow;

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
	imagebuttonT->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA(0xdeadbeef0));
	imagebuttonT->LFSTK_setImageFromPath("./AspellGUI.png",TOOLBAR,true);
	sy+=YSPACING*2;

//image button left
	imagebuttonL=new LFSTK_buttonClass(wc,"Left Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*2,BUTTONGRAV);
	imagebuttonL->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA(0xdeadbeef1));
	imagebuttonL->LFSTK_setImageFromPath("./casper2.JPG",LEFT,true);
	imagebuttonL->LFSTK_setLabelGravity(CENTRE);
	sy+=YSPACING*2;

//image button centre
	imagebuttonC=new LFSTK_buttonClass(wc,"",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*2,BUTTONGRAV);
	imagebuttonC->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA(0xdeadbeef2));
	imagebuttonC->LFSTK_setImageFromPath("./casper2.JPG",CENTRE,true);

	sy+=YSPACING*2;

//image button right
	imagebuttonR=new LFSTK_buttonClass(wc,"Right Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*2,BUTTONGRAV);
	imagebuttonR->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA(0xdeadbeef3));
	imagebuttonR->LFSTK_setImageFromPath("./casper2.JPG",RIGHT,true);
	imagebuttonR->LFSTK_setLabelGravity(CENTRE);
	imagebuttonR->gadgetDetails.reserveSpace=4;
	sy+=YSPACING*2;

//image button menu
	imagebuttonR=new LFSTK_buttonClass(wc,"Menu Image Button",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	imagebuttonR->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA(0xdeadbeef4));
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
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	//wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}