#if 0

#(c)keithhedger Wed 2 Aug 15:43:22 BST 2017 kdhedger68713@gmail.com

echo "Compiling, please wait ..."

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo  glib-2.0) -llfstoolkit -lImlib2 -o imageexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./imageexample "$@"
retval=$?
echo "Exit code $retval"
rm imageexample
exit $retval
#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Drag Image"
#define BOXLABEL2		"Right Click For Context"
#define IMAGESIZE		64

enum {BUTTONMOUNT=0,BUTTONUNMOUNT,BUTTONEJECT,BUTTONOPEN,BUTTONADDICON,BUTTONREMOVEICON,NOMOREBUTONS};

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_imageClass			*tux;

const char				*diskLabelData[]={"Mount","Unmount","Eject","Open","Custom Icon","Remove Icon",NULL};
const char				*diskThemeIconData[]={"drive-harddisk","media-eject","media-eject","document-open","list-add","list-remove"};
//char						*iconpath=NULL;
std::string				iconpath;
LFSTK_windowClass		*diskWindow=NULL;
LFSTK_buttonClass		*diskButtons[NOMOREBUTONS];
bool						sticky=false;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	if(p!=NULL)
		{
			if(static_cast<LFSTK_imageClass*>(p)->isDoubleClick==true)
				{
					printf("was a double click\n");
					sticky=!sticky;
					wc->LFSTK_setSticky(sticky);
				}
		}
	return(true);
}

bool contextCB(void *p,void* ud)
{
	LFSTK_windowClass	*lwc=static_cast<LFSTK_gadgetClass*>(p)->wc;
	int					winnum;
	if(p!=NULL)
		{
			//winnum=lwc->app->LFSTK_findWindow(lwc);
			lwc->app->windows->at(winnum).loopFlag=false;
			//wc->popupLoop=false;
			//printf("data=%p\n win=%i",ud,static_cast<LFSTK_gadgetClass*>(p)->windowNumber);
			//apc->windows->at(static_cast<LFSTK_gadgetClass*>(p)->windowNumber).loopFlag=false;
			printf("ud=%p gadget=%p tux=%p\n",ud,lwc->popupFromGadget,tux);
			diskWindow->LFSTK_hideWindow();
		}
	return(true);
}

bool mouseUpCB(void *p,void* ud)
{
	geometryStruct	geom;
	fprintf(stderr,"mouseUpCB\n");
	if(p!=NULL)
		{
			static_cast<LFSTK_imageClass*>(p)->LFSTK_getGeom(&geom);
//DEBUGFUNC("%b",static_cast<LFSTK_imageClass*>(p)->isDoubleClick);
			if(static_cast<LFSTK_imageClass*>(p)->isDoubleClick==true)
				fprintf(stderr,"double click x=%i y=%i\n",geom.x,geom.y);
		//	else
		//		fprintf(stderr,"x=%i y=%i\n",geom.x,geom.y);
		}
	return(true);
}

bool mouseDownCB(void *p,void* ud)
{
	geometryStruct	geom;
	fprintf(stderr,"mouseDownCB\n");
//	if(p!=NULL)
//		{
//			static_cast<LFSTK_imageClass*>(p)->LFSTK_getGeom(&geom);
////DEBUGFUNC("%b",static_cast<LFSTK_imageClass*>(p)->isDoubleClick);
//			if(static_cast<LFSTK_imageClass*>(p)->isDoubleClick==true)
//				fprintf(stderr,"double click x=%i y=%i\n",geom.x,geom.y);
//		//	else
//		//		fprintf(stderr,"x=%i y=%i\n",geom.x,geom.y);
//		}
	return(true);
}
bool moveCB(LFSTK_gadgetClass*p,void* ud)
{
	if(ud!=NULL)
		{
			XEvent			event;
			geometryStruct	geom;
			printf(">>>Mouse In %s<<<\n",(const char*)ud);

			//p->LFSTK_getGeomWindowRelative(&geom,apc->rootWindow);	
			//popWindow->LFSTK_moveWindow(geom.x,geom.y-GADGETHITE,true);
			//popWindow->LFSTK_showWindow();
			//popWindow->LFSTK_clearWindow(true);
		}
	return(true);
}

bool exitCB(LFSTK_gadgetClass*p,void* ud)
{
	if(ud!=NULL)
		{
			printf(">>>Mouse Out %s<<<\n",(const char*)ud);
			//popWindow->LFSTK_hideWindow();
		}
	return(true);
}

int main(int argc, char **argv)
{
	int				sy=BORDER;
	windowInitStruct	*win;

	apc=new LFSTK_applicationClass();
	apc->globalLib->LFSTK_setUseTheme(false);
	win=apc->LFSTK_getDefaultWInit();
	win->windowName=BOXLABEL;
	win->windowType=win->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DIALOG"));
	win->level=ABOVEALL;
	apc->LFSTK_addWindow(win,BOXLABEL);
	wc=apc->mainWindow;

	wc->LFSTK_setWindowPixmap(apc->globalLib->LFSTK_getWindowPixmap(apc->display,apc->rootWindow),DIALOGWIDTH,sy);
	tux=new LFSTK_imageClass(wc,NULL,DIALOGMIDDLE-(IMAGESIZE/2),sy,IMAGESIZE,IMAGESIZE,BUTTONGRAV,true);
	tux->LFSTK_setImageFromPath("./AspellGUI.png",PRESERVEASPECT,true);
	tux->LFSTK_setCanDrag(true);
	tux->LFSTK_setTile(NULL,0);
	tux->userData=USERDATA("tux image");
//	tux->LFSTK_allowXMovement(false);
//	tux->LFSTK_allowYMovement(false);
//	tux->LFSTK_setLimits(10,-1,440,-1);
//	tux->LFSTK_setLimits(-1,10,-1,440);
	tux->LFSTK_setLimits(10,10,440,120);
	tux->LFSTK_setMouseCallBack(mouseDownCB,mouseUpCB,NULL);
	tux->LFSTK_setMouseMoveCallBack(moveCB,exitCB,USERDATA("Left Enter/Exit"));
	
	geometryStruct tg;
	tux->LFSTK_getGeom(&tg);
	fprintf(stderr,"wid=%i\n",tg.w);
	sy+=YSPACING*3;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,BOXLABEL2,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=YSPACING/2;

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

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_setWindowPixmap(apc->globalLib->LFSTK_getWindowPixmap(apc->display,apc->rootWindow),apc->displayWidth,apc->displayHeight);


//disks
	win=new windowInitStruct;
	win->app=apc;
	win->windowName="";
	win->loadVars=true;
	win->x=100;
	win->y=100;
	win->w=200;
	win->h=200;
	win->wc=wc;
	win->windowType=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_NORMAL"));

	win->decorated=false;
	win->overRide=true;
	win->level=ABOVEALL;

	apc->LFSTK_addWindow(win,"hello");
	diskWindow=apc->windows->back().window;

	sy=0;
	for(int j=BUTTONMOUNT;j<NOMOREBUTONS;j++)
		{
			diskButtons[j]=new LFSTK_buttonClass(diskWindow,diskLabelData[j],0,sy,GADGETWIDTH*2,24,NorthWestGravity);
			diskButtons[j]->LFSTK_setMouseCallBack(NULL,contextCB,(void*)(long)(j+1));
			iconpath=diskWindow->globalLib->LFSTK_findThemedIcon("gnome",diskThemeIconData[j],"");
			diskButtons[j]->LFSTK_setImageFromPath(iconpath,LEFT,true);
			sy+=GADGETHITE;
		}
	diskWindow->LFSTK_resizeWindow(GADGETWIDTH+GADGETHITE,sy,true);
	tux->LFSTK_setContextWindow(diskWindow);
	tux->contextWindowPos=CONTEXTABOVECENTRE;
	tux->windowNumber=apc->windows->size()-1;

	wc->LFSTK_setWindowPixmap(apc->globalLib->LFSTK_getWindowPixmap(apc->display,apc->rootWindow),apc->displayWidth,apc->displayHeight);

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}