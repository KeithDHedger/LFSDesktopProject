#if 0

#©keithhedger Sat 5 Aug 16:52:45 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o fontdialogexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./fontdialogexample "$@"
retval=$?
echo "Exit code $retval"
rm fontdialogexample
exit $retval
#endif

#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Font Dialog"
#define MAXMAINMENUS		25

LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_listGadgetClass		*fontlist=NULL;
LFSTK_toggleButtonClass		*boldcheck=NULL;
LFSTK_toggleButtonClass		*italiccheck=NULL;
LFSTK_lineEditClass			*fontsize=NULL;
LFSTK_lineEditClass			*preview=NULL;
LFSTK_fontDialogClass		*fontdialog=NULL;

bool						mainLoop=true;
Display						*display;
unsigned					maxFonts=0;
char						**fontsAZ=NULL;
unsigned					size=10;
const char					*fontname=NULL;
unsigned					selectnumber=0;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	const fontDataStruct	*fd;
	if(ud!=NULL)
		{
			fontdialog->LFSTK_showDialog("");
			fd=fontdialog->LFSTK_getFontData(false);
			if(fd->isValid==true)
				printf("fontString=%s\n",fd->fontString);
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
	char	*sizestr[4]={0,};
	
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

	fontdialog=new LFSTK_fontDialogClass(wc,"Select Font",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	fontdialog->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SELECT");
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