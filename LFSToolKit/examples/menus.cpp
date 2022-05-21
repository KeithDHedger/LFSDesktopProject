#if 0

#(c)keithhedger Sat 3 Nov 14:40:06 GMT 2018 kdhedger68713@gmail.com

if [ "X$1" != "X" ];
then
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

#define BOXLABEL			"Menu Example"
#define NUMMENUS			16

LFSTK_applicationClass		*apc=NULL;
LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_listGadgetClass		*list=NULL;
LFSTK_menuClass				*menu=NULL;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool menuCB(void *p,void* ud)
{
	printf("Label=%s Userdata=%i\n",static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel(),ud);
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	return(true);	
}

bool buttonCB(void *p,void* ud)
{
	menu->LFSTK_showMenu();
	return(true);
	menu->mainMenuWindow->LFSTK_showWindow(true);
	return(true);
}

int main(int argc, char **argv)
{
	int	sy=BORDER;
	int	hsy;

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

//show
	quit=new LFSTK_buttonClass(wc,"Show",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	hsy=sy;
	quit->LFSTK_setMouseCallBack(NULL,buttonCB,NULL);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	sy+=YSPACING;

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_clearWindow(true);

	menu=new LFSTK_menuClass(wc,DIALOGMIDDLE-HALFGADGETWIDTH,hsy+GADGETHITE,1,1);

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_clearWindow(true);

	menuStruct	**mms=new menuStruct*[NUMMENUS];

	for (int j=0; j<NUMMENUS; j++)
		{
			mms[j]=new menuStruct;
			asprintf(&mms[j]->label,"menu %i",j);
			mms[j]->hasSubMenu=false;
			mms[j]->subMenus=NULL;
			mms[j]->userData=USERDATA(j+1);
			mms[j]->imageType=NOTHUMB;
		}
	free(mms[12]->label);
	mms[12]->label=strdup("--");
	free(mms[2]->label);
	mms[2]->label=strdup("A really long label that will over flow");

	mms[0]->imageType=FILETHUMB;
	mms[0]->data.imagePath=strdup("./AspellGUI.png");
	mms[10]->imageType=FILETHUMB;
	mms[10]->data.imagePath=strdup("./casper2.JPG");
	mms[12]->imageType=CAIROTHUMB;
	mms[12]->data.surface=wc->globalLib->LFSTK_createSurfaceFromPath("./ManPageEditor.png");

	mms[2]->hasSubMenu=true;
	mms[2]->subMenuCnt=NUMMENUS;
	mms[2]->subMenus=new menuStruct*[NUMMENUS];
	for (int j=0; j<NUMMENUS; j++)
		{
			mms[2]->subMenus[j]=new menuStruct;
			asprintf(&mms[2]->subMenus[j]->label,"sub menu -2 %i",j);
			mms[2]->subMenus[j]->hasSubMenu=false;
			mms[2]->subMenus[j]->subMenus=NULL;
			mms[2]->subMenus[j]->userData=USERDATA((j+1)*100);
			mms[2]->subMenus[j]->imageType=NOTHUMB;
		}

	mms[6]->hasSubMenu=true;
	mms[6]->subMenuCnt=NUMMENUS;

	mms[6]->subMenus=new menuStruct*[NUMMENUS];
	for (int j=0; j<NUMMENUS; j++)
		{
			mms[6]->subMenus[j]=new menuStruct;
			asprintf(&mms[6]->subMenus[j]->label,"sub menu 6 - %i",j);
			mms[6]->subMenus[j]->hasSubMenu=false;
			mms[6]->subMenus[j]->subMenus=NULL;
			mms[6]->subMenus[j]->userData=USERDATA((j+1)*10000);
			mms[6]->subMenus[j]->imageType=FILETHUMB;
			mms[6]->subMenus[j]->data.imagePath=strdup("./computer.png");
		}

	free(mms[6]->subMenus[12]->label);
	mms[6]->subMenus[12]->label=strdup("--");
	free(mms[6]->subMenus[5]->label);
	mms[6]->subMenus[5]->label=strdup("--");

	mms[6]->subMenus[4]->hasSubMenu=true;
	mms[6]->subMenus[4]->subMenuCnt=NUMMENUS;

	mms[6]->subMenus[4]->subMenus=new menuStruct*[NUMMENUS];
	for (int j=0; j<NUMMENUS; j++)
		{
			mms[6]->subMenus[4]->subMenus[j]=new menuStruct;
			asprintf(&mms[6]->subMenus[4]->subMenus[j]->label,"sub sub menu 6 - %i",j);
			mms[6]->subMenus[4]->subMenus[j]->hasSubMenu=false;
			mms[6]->subMenus[4]->subMenus[j]->subMenus=NULL;
			mms[6]->subMenus[4]->subMenus[j]->userData=(void*)(long)((j+1)*100000);
			mms[6]->subMenus[4]->subMenus[j]->imageType=FILETHUMB;
			mms[6]->subMenus[4]->subMenus[j]->data.imagePath=strdup("./audio-speakers.png");
		}

	menu->LFSTK_setMouseCallBack(NULL,menuCB,NULL);
	menu->LFSTK_addMainMenus(mms,NUMMENUS);

	int retval=apc->LFSTK_runApp();

	delete menu;
	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}