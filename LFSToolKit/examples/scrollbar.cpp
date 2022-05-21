#if 0

#(c)keithhedger Sun 14 Oct 17:44:18 BST 2018 kdhedger68713@gmail.com

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

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Scrollbar"

LFSTK_applicationClass		*apc=NULL;
LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_buttonClass			*test=NULL;
LFSTK_scrollBarClass		*vsb=NULL;
LFSTK_scrollBarClass		*hsb=NULL;
LFSTK_toggleButtonClass		*reverse=NULL;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool setval(void *p,void* ud)
{
	hsb->LFSTK_setValue(10);
	return(true);
}

bool doReverse(void *p,void* ud)
{
	int holdval;
	holdval=hsb->LFSTK_getValue();
	hsb->reverse=static_cast<LFSTK_toggleButtonClass*>(p)->LFSTK_getValue();
	fprintf(stderr,"true=%i %i\n",true,static_cast<LFSTK_toggleButtonClass*>(p)->LFSTK_getValue());
	hsb->LFSTK_setValue(holdval);

	holdval=vsb->LFSTK_getValue();
	vsb->reverse=static_cast<LFSTK_toggleButtonClass*>(p)->LFSTK_getValue();;
	vsb->LFSTK_setValue(holdval);
	return(true);
}

bool valChanged(void *p,void* ud)
{
	LFSTK_scrollBarClass	*sb=NULL;
	if(p!=NULL)
		{
			sb=static_cast<LFSTK_scrollBarClass*>(p);
			if(sb!=NULL)
				fprintf(stderr,"value=%i\n",sb->LFSTK_getValue());
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

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//hscrollbar
	hsb=new LFSTK_scrollBarClass(wc,false,DIALOGMIDDLE-200,sy,400+GADGETHITE,SCROLLBARWIDTH,BUTTONGRAV);
	hsb->LFSTK_setMouseCallBack(NULL,valChanged,NULL);
	hsb->LFSTK_setScale(1,100);
//	hsb->reverse=true;
	hsb->LFSTK_setValue(25);
	sy+=YSPACING;

//vscrollbar
fprintf(stderr,"sy=%i\n",sy);
//	vsb=new LFSTK_scrollBarClass(wc,true,BORDER,sy,GADGETHITE/2,200+GADGETHITE,BUTTONGRAV);
	vsb=new LFSTK_scrollBarClass(wc,true,BORDER,sy,SCROLLBARWIDTH,200+GADGETHITE,BUTTONGRAV);
	vsb->LFSTK_setMouseCallBack(NULL,valChanged,NULL);
	//vsb->LFSTK_setAllowKBControl(false);
	vsb->LFSTK_setScale(0,1985);
	vsb->LFSTK_setPageScroll(100);
//	vsb->reverse=true;
	vsb->LFSTK_setValue(250);
	sy+=YSPACING+200;
	
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

//reverse
	reverse=new LFSTK_toggleButtonClass(wc,"Reverse",DIALOGMIDDLE-HALFGADGETWIDTH,DIALOGMIDDLE,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	reverse->LFSTK_setToggleStyle(TOGGLENORMAL);
	reverse->LFSTK_setMouseCallBack(NULL,doReverse,NULL);
//test setval
	test=new LFSTK_buttonClass(wc,"Set to 10",DIALOGMIDDLE-HALFGADGETWIDTH,DIALOGMIDDLE+GADGETHITE*2,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	test->LFSTK_setMouseCallBack(NULL,setval,NULL);

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}