#if 0

#(c)keithhedger Fri 28 Jul 20:19:00 BST 2017 kdhedger68713@gmail.com

echo "Compiling, please wait ..."

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo glib-2.0) -llfstoolkit -lImlib2 -o windowexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./windowexample "$@"
retval=$?
echo "Exit code $retval"
rm windowexample
exit $retval
#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Basic Window"


LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass			*label=NULL;

//context window
enum {BUTTONMOUNT=0,BUTTONUNMOUNT,BUTTONEJECT,BUTTONOPEN,BUTTONADDICON,BUTTONREMOVEICON,NOMOREBUTONS};
const char				*contextLabelData[]={"Mount","Unmount","Eject","Open","Custom Icon","Remove Icon",NULL};
const char				*contextThemeIconData[]={"drive-harddisk","media-eject","media-eject","document-open","list-add","list-remove"};
std::string				iconpath;
LFSTK_windowClass		*contextWindow=NULL;
LFSTK_buttonClass		*contextButtons[NOMOREBUTONS];


bool contextCB(void *p,void* ud)
{
	LFSTK_windowClass	*lwc=static_cast<LFSTK_gadgetClass*>(p)->wc;
	int					winnum;
	if(p!=NULL)
		{
			winnum=lwc->app->LFSTK_findWindow(lwc);
			lwc->app->windows->at(winnum).loopFlag=false;
			printf("ud=%p label=%s\n",ud,contextLabelData[((long unsigned)ud)-1]);
			contextWindow->LFSTK_hideWindow();
			apc->windows->at(winnum).loopFlag=false;
		}
	return(true);
}

bool windowDrop(LFSTK_windowClass *lwc,void* ud)
{
	if(lwc!=NULL)
		printf("dropped %s on window @x/y %i %i\n",lwc->droppedData.data,lwc->droppedData.x,lwc->droppedData.y);
	return(true);
}

int main(int argc, char **argv)
{
	windowInitStruct *win;
	int	sy=0;

	apc=new LFSTK_applicationClass();

	win=apc->LFSTK_getDefaultWInit();
	win->windowName=BOXLABEL;
	//win->windowType=win->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_NORMAL"));
	win->level=ABOVEALL;
	apc->LFSTK_addWindow(win,BOXLABEL);

	//apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKExample");
	wc=apc->mainWindow;
//	fprintf(stderr,">>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
//	fprintf(stderr,"winid=%p\n",wc->window);

//wc->LFSTK_setTile(NULL,0);
//wc->usePixmap=true;

	wc->passEventToRoot=true;

	wc->LFSTK_initDnD(true);
	wc->LFSTK_setWindowDropCallBack(windowDrop,USERDATA(0xdeadbeef));
	//wc->dropCB.runTheCallback=false;

	label=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	label=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,100);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;
	sy+=200;
	fprintf(stderr,"winid=%p\n",label->window);

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	//wc->LFSTK_setDecorations(true,true,true,true);
	wc->LFSTK_showWindow();
	//fprintf(stderr,"%s\n",wc->globalLib->LFSTK_getGlobalString(-1,TYPEMONOFONT));


	win=apc->LFSTK_getDefaultWInit();
	win->windowType=win->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_MENU"));
	win->level=ABOVEALL;
	win->decorated =false;
	win->overRide =true;
	apc->LFSTK_addWindow(win,"windowpopup");
	contextWindow=apc->windows->back().window;

	sy=0;
	for(int j=BUTTONMOUNT;j<NOMOREBUTONS;j++)
		{
			contextButtons[j]=new LFSTK_buttonClass(contextWindow,contextLabelData[j],0,sy,GADGETWIDTH,24,NorthWestGravity);
			contextButtons[j]->LFSTK_setMouseCallBack(NULL,contextCB,(void*)(long)(j+1));
			iconpath=contextWindow->globalLib->LFSTK_findThemedIcon("gnome",contextThemeIconData[j],"");
			contextButtons[j]->LFSTK_setImageFromPath(iconpath,LEFT,true);
			sy+=GADGETHITE;
		}
	contextWindow->LFSTK_resizeWindow(GADGETWIDTH,sy,true);
	wc->LFSTK_setContextWindow(contextWindow);
	wc->contextWindowPos=CONTEXTBELOW;
	//wc->contextYOffset=-40;
	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}