#if 0

#©keithhedger Sat 5 Aug 19:18:08 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o geticonpathexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./geticonpathexample "$@"
retval=$?
echo "Exit code $retval"
rm geticonpathexample
exit $retval
#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Get Path To Themed Icon"

LFSTK_applicationClass		*apc=NULL;
LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_buttonClass			*getIconPath=NULL;
LFSTK_lineEditClass			*mimeEdit=NULL;
LFSTK_lineEditClass			*catEdit=NULL;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(true);
}

bool getPath(void *p,void* ud)
{
	char	*iconpath=NULL;

	iconpath=(char*)apc->globalLib->LFSTK_findThemedIcon(apc->iconThemeName,mimeEdit->LFSTK_getCStr(),catEdit->LFSTK_getCStr());

	if(iconpath!=NULL)
		{
			printf("iconpath=%s\n",iconpath);
			free(iconpath);
		}
	else
		printf("No icon found for '%s' in '%s'\n",mimeEdit->LFSTK_getCStr(),catEdit->LFSTK_getCStr());

	return(true);
}

int main(int argc, char **argv)
{
	int				sy=BORDER;
	callbackStruct	cbs;
	
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

//mime edit
	label=new LFSTK_labelClass(wc,"Icon Name",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("B");
	sy+=GADGETHITE;

	mimeEdit=new LFSTK_lineEditClass(wc,"drive-harddisk",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//catagory edit
	label=new LFSTK_labelClass(wc,"Catagory",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("B");
	sy+=GADGETHITE;
	catEdit=new LFSTK_lineEditClass(wc,"devices",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//get path to icon
	getIconPath=new LFSTK_buttonClass(wc,"Get Path",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	cbs.mouseReleaseCallback=getPath;
	cbs.validCallbacks=(MOUSERELEASECB);
	getIconPath->LFSTK_setCallBacks(cbs);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	cbs.mouseReleaseCallback=doQuit;
	quit->LFSTK_setCallBacks(cbs);
	sy+=YSPACING;

	wc->acceptOnThis=false;
	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}
