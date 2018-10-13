#if 0

#©keithhedger Wed 2 Aug 15:43:22 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -D_ENABLEDEBUG_ -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o imageexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./imageexample "$@"
retval=$?
echo "Exit code $retval"
rm imageexample
exit $retval
#endif

#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL		"Drag Image"
#define BOXLABEL2		"Right Click For Context"
#define IMAGESIZE		64

LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_imageClass		*tux;

bool					mainLoop=true;
Display					*display;
enum {BUTTONMOUNT=0,BUTTONUNMOUNT,BUTTONEJECT,BUTTONOPEN,BUTTONADDICON,BUTTONREMOVEICON,NOMOREBUTONS};

const char	*diskLabelData[]={"Mount","Unmount","Eject","Open","Custom Icon","Remove Icon",NULL};
const char	*diskThemeIconData[]={"drive-harddisk","media-eject","media-eject","document-open","list-add","list-remove"};
char	*iconpath=NULL;
LFSTK_windowClass	*diskWindow=NULL;
LFSTK_buttonClass	*diskButtons[NOMOREBUTONS];

bool	sticky=false;
bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
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
	if(p!=NULL)
		{
			wc->popupLoop=false;
			DEBUGFUNC("gadget label=%s",(char*)wc->popupFromGadget->userData);
			printf("data=%p\n",ud);
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;

	windowInitStruct *wi=new windowInitStruct;
	wi->w=DIALOGWIDTH;
	wi->h=DIALOGHITE;
	wi->name="Draggaable Image Example";

	wc=new LFSTK_windowClass(wi);
	display=wc->display;
	delete	wi;
	wc->LFSTK_setWindowPixmap(wc->globalLib->LFSTK_getWindowPixmap(display,wc->rootWindow),DIALOGWIDTH,sy);

	tux=new LFSTK_imageClass(wc,NULL,DIALOGMIDDLE-(IMAGESIZE/2),sy,IMAGESIZE,IMAGESIZE,NorthGravity,true);
	tux->LFSTK_setImageFromPath("/usr/share/pixmaps/LFSTux.png",PRESERVEASPECT,true);
	tux->LFSTK_setCallBack(NULL,buttonCB,NULL);
	tux->LFSTK_setCanDrag(true);
	tux->LFSTK_snapSize(1);
	tux->LFSTK_setTile(NULL,0);
	tux->LFSTK_setAlpha(0.5);
	tux->userData=(void*)"tux image";

	sy+=YSPACING*3;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	label->LFSTK_setUseWindowPixmap(true);
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,BOXLABEL2,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setUseWindowPixmap(true);
	sy+=YSPACING/2;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	copyrite->LFSTK_setUseWindowPixmap(true);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	personal->LFSTK_setUseWindowPixmap(true);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	seperator->LFSTK_setUseWindowPixmap(true);
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,NorthGravity);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

//disks
	diskWindow=new LFSTK_windowClass(100,100,200,200,"xxx",true,true);
	sy=0;
	for(int j=BUTTONMOUNT;j<NOMOREBUTONS;j++)
		{
			diskButtons[j]=new LFSTK_buttonClass(diskWindow,diskLabelData[j],0,sy,GADGETWIDTH,24,NorthWestGravity);
			diskButtons[j]->LFSTK_setCallBack(NULL,contextCB,(void*)(long)(j+1));
			iconpath=diskWindow->globalLib->LFSTK_findThemedIcon("gnome",diskThemeIconData[j],"");
			diskButtons[j]->LFSTK_setImageFromPath(iconpath,LEFT,true);
			free(iconpath);
			sy+=GADGETHITE;
		}
	diskWindow->LFSTK_resizeWindow(GADGETWIDTH,sy,true);
	tux->LFSTK_setContextWindow(diskWindow);

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

			switch(event.type)
				{
					case ConfigureNotify:
						//DEBUGFUNC("ConfigureNotify","");
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
						wc->LFSTK_setWindowPixmap(wc->globalLib->LFSTK_getWindowPixmap(display,wc->rootWindow),event.xconfigurerequest.width,event.xconfigurerequest.height);
						XClearWindow(wc->display,wc->window);
						break;
				}
		}

	delete wc;
	XCloseDisplay(display);
	cairo_debug_reset_static_data();
	return 0;
}