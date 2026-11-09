#if 0

#(c)keithhedger Fri 28 Jul 20:19:00 BST 2017 kdhedger68713@gmail.com

echo "Compiling, please wait ..."

if [ "X$1" != "X" ];then
cat>ignorelibleaks<<EOF
{
   ignore_unversioned_libs
   Memcheck:Leak
   ...
   obj:*/lib*/lib*.so
}

{
   ignore_versioned_libs
   Memcheck:Leak
   ...
   obj:*/lib*/libQt.so.*
   obj:*/lib*/libcairo.so.*
   obj:*/lib*/libX11.so.*
   obj:*/lib*/libfontconfig.so.*
   obj:*/lib*/libexpat.so.*
}

{
   ignorexcbwritev
   Memcheck:Param
   writev(vector[0])
   fun:writev
   obj:/usr/lib/libxcb.so.1.1.0
}

EOF

	USEVALGRIND="valgrind --leak-check=full --suppressions=./ignorelibleaks -s "
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
	win->windowType=win->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DIALOG"));
	win->level=ABOVEALL;
	apc->LFSTK_addWindow(win,BOXLABEL);

	wc=apc->mainWindow;
	wc->LFSTK_setDecorations(false,false,false,true);

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
	//fprintf(stderr,"winid=%p\n",label->window);

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
	wc->contextWindowPos=CONTEXTATMOUSE;


//	wc->ignoreContext=true;
//	wc->contextWindowPos=CONTEXTNORTHWEST;
//	wc->contextWindowPos=CONTEXTNORTH;
//	wc->contextWindowPos=CONTEXTNORTHEAST;

//	wc->contextWindowPos=CONTEXTEAST;
//	wc->contextWindowPos=CONTEXTSOUTHEAST;
	//wc->contextWindowPos=CONTEXTSOUTH;
	//wc->contextWindowPos=CONTEXTSOUTHWEST;
	//wc->contextWindowPos=;
//
//
//	wc->contextWindowPos=OUTSIDETOPLEFT;
//	wc->contextWindowPos=OUTSIDETOPCENTRE;
//	wc->contextWindowPos=OUTSIDETOPRIGHT;
//	wc->contextWindowPos=INSIDETOPLEFT;
//	wc->contextWindowPos=INSIDETOPCENTRE;
//	wc->contextWindowPos=INSIDETOPRIGHT;
//
//	wc->contextWindowPos=OUTSIDERIGHTTOP;
//	wc->contextWindowPos=OUTSIDERIGHTCENTRE;
//	wc->contextWindowPos=OUTSIDERIGHTBOTTOM;
//	wc->contextWindowPos=INSIDERIGHTTOP;
//	wc->contextWindowPos=INSIDERIGHTCENTRE;
//	wc->contextWindowPos=INSIDERIGHTBOTTOM;
//
//	wc->contextWindowPos=OUTSIDEBOTTOMRIGHT;
//	wc->contextWindowPos=OUTSIDEBOTTOMCENTRE;
//	wc->contextWindowPos=OUTSIDEBOTTOMLEFT;
//	wc->contextWindowPos=INSIDEBOTTOMRIGHT;
//	wc->contextWindowPos=INSIDEBOTTOMCENTRE;
//	wc->contextWindowPos=INSIDEBOTTOMLEFT;
//
//	wc->contextWindowPos=OUTSIDELEFTTOP;
//	wc->contextWindowPos=OUTSIDELEFTCENTRE;
//	wc->contextWindowPos=OUTSIDELEFTBOTTOM;
//	wc->contextWindowPos=INSIDELEFTTOP;
//	wc->contextWindowPos=INSIDELEFTCENTRE;
//	wc->contextWindowPos=INSIDELEFTBOTTOM;
//
//	wc->contextWindowPos=OUTSIDEMOUSETOP;
//	wc->contextWindowPos=OUTSIDEMOUSERIGHT;
//	wc->contextWindowPos=OUTSIDEMOUSEBOTTOM;
//	wc->contextWindowPos=OUTSIDEMOUSELEFT;

	//contextWindow->ignoreContext=true;
//	wc->LFSTK_connectCB(ButtonPress,[](std::string msg)
//		{
//			fprintf(stderr,"got ButtonPress 4000:>>%s<< button=wc->xEvent->xbutton.button=%i\n",msg.c_str(),wc->xEvent->xbutton.button);
//			if(wc->xEvent->xbutton.button==Button3 && wc->ignoreContext==true)
//				wc->LFSTK_showContextWindow();
//		});


	//wc->contextYOffset=-40;
	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}