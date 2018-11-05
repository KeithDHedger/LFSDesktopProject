#if 0

#©keithhedger Sat 3 Nov 14:40:06 GMT 2018 kdhedger68713@gmail.com

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
LFSTK_windowClass	*subwc;
LFSTK_listGadgetClass		*list=NULL;

const char	*lst[]={"menu 1","menu 2","menu 3","menu 4","menu 5","menu 6"};
listLabelStruct				**labelLst1=NULL;

bool						mainLoop=true;
Display						*display;
//LFSTK_windowClass			*subwindows;
std::vector<LFSTK_windowClass*>		subwindows;
std::vector<int>					xoffset;
std::vector<int>					yoffset;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	subwc->LFSTK_showWindow();
	XSetInputFocus(subwc->display,subwc->window,RevertToParent,CurrentTime);
	if(ud!=NULL)
		{
			printf(">>>%s<<<\n",(const char*)ud);
		}
	return(true);
}

bool select(void *object,void* userdata)
{
	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);

	printf("List item=%i\n",list->LFSTK_getCurrentListItem());
	printf("Selected List item string=%s\n",list->LFSTK_getSelectedLabel());
	if(list->labelData[list->LFSTK_getCurrentListItem()]->imageType==FILETHUMB)
		printf("Image path=%s\n",list->labelData[list->LFSTK_getCurrentListItem()]->data.imagePath);
	printf("UserData=%p\n",userdata);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
	int		hsy;		
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

//show
	quit=new LFSTK_buttonClass(wc,"Show",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	hsy=sy;
	quit->LFSTK_setCallBack(NULL,buttonCB,NULL);
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


	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	mainLoop=true;
	//LFSTK_windowClass *subw=new LFSTK_windowClass

//	XSetWindowAttributes	wa;
//	wa.win_gravity=BUTTONGRAV;
//	wa.save_under=true;
//	Window w=XCreateWindow(wc->display,wc->window,0,100,100,800,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
//	//this->gc=XCreateGC(this->display,this->window,0,NULL);
//	wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
//	//this->LFSTK_setCairoFontData();
//	XSelectInput(wc->display,w,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);
subwindows.clear();

//XWindowAttributes xwa;
//const geometryStruct	*geom=wc->LFSTK_getWindowGeom();
//
//	XGetWindowAttributes(display,wc->window,&xwa);
//	
//	printf("x=%i y=%i\n",xwa.x,xwa.y);
//
//	subwc=new LFSTK_windowClass(0,0,400,GADGETHITE*16,"menu window",true,true,false);
//	//subwc=new LFSTK_windowClass(geom->x,geom->y,100,300,"menu window",false);
//	subwc->LFSTK_setWindowType("_NET_WM_WINDOW_TYPE_MENU");
//	//subwc->display=wc->display;
//	//this->builtMenu=false;
//	//subwc->gadgetMap.clear();
//	if(wc->globalLib->LFSTK_getUseTheme()==true)
//		subwc->LFSTK_setTile(wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMTILE),-1);
//	else
//		subwc->useTile=false;
//	//subwc->LFSTK_showWindow();
//	subwc->closeDisplayOnExit=false;
//	list=new LFSTK_listGadgetClass(subwc,"list",0,0,400,GADGETHITE*16,BUTTONGRAV,NULL,0);
////	filelist->LFSTK_setListFromFile("/etc/fstab",false);
//
//
//	labelLst1=new listLabelStruct*[6];
//	for(int j=0;j<6;j++)
//		{
//			labelLst1[j]=new listLabelStruct;
//			labelLst1[j]->label=strdup((char*)lst[j]);
//			labelLst1[j]->imageType=NOTHUMB;
//		}
//	list->LFSTK_setList(labelLst1,6);
//	list->LFSTK_setCallBack(NULL,select,NULL);
//
//
//
//
////	filelist->LFSTK_setCallBack(NULL,select,(void*)0xdeadbeaf);
//	subwc->LFSTK_clearWindow(true);
//	wc->LFSTK_clearWindow(true);
//	sy+=YSPACING+GADGETHITE*16;

LFSTK_menuClass *mc=new LFSTK_menuClass(wc,"Test Menu",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
sy+=YSPACING;


	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_clearWindow(true);
//
//								Window dw;
//								int x;
//								int y;
//								unsigned int w,h,dump;
//								XGetWindowAttributes(display,wc->window,&xwa);
//								 XGetGeometry(display,wc->window,&dw,&x,&y,&w,&h,&dump,&dump);
//								 XTranslateCoordinates( display, wc->window,wc->rootWindow, 0, 0, &x, &y, &dw );
//								subwc->LFSTK_moveWindow(x,y,true);

//	XMapWindow(wc->display,subwc->window);
//XReparentWindow(subwc->display,subwc->window, wc->window, 0, 0);
//Display *mdisplay=XOpenDisplay(NULL);

	//subwindows.push_back(subwc);
	//xoffset.push_back(DIALOGMIDDLE-HALFGADGETWIDTH+((subwindows.size()-1)*GADGETWIDTH));
	//yoffset.push_back(((subwindows.size())*GADGETHITE));



	while(mainLoop==true)
		{
			if(XPending(wc->display))
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
							case Expose:
//								if(event.xany.window==wc->window)
//									{
//									for(int j=0;j<subwindows.size();j++)
//										{
//										Window dw;
//										int x;
//										int y;
//										unsigned int w,h,dump;
//										 XTranslateCoordinates( display, wc->window,wc->rootWindow, 0, 0, &x, &y, &dw );
//										subwindows.at(j)->LFSTK_moveWindow(x+xoffset.at(j),y+hsy+yoffset.at(j),true);
//										}
//									}
									break;
							case ButtonRelease:
								printf("ButtonRelease...\n");
								break;
						}
				}

			for(int j=0;j<subwindows.size();j++)
			{
			if(XPending(subwindows.at(j)->display))
				{
					XNextEvent(subwindows.at(j)->display,&event);
//					XNextEvent(wc->display,&event);
					mappedListener *ml=subwindows.at(j)->LFSTK_getMappedListener(event.xany.window);

					if(ml!=NULL)
						ml->function(ml->gadget,&event,ml->type);
						
					subwindows.at(j)->LFSTK_handleWindowEvents(&event);

					switch(event.type)
						{	
							case Expose:
								//subwindows.at(j)->LFSTK_clearWindow(true);
								break;

							case ButtonPress:
								//XSetInputFocus(subwindows.at(j)->display,subwindows.at(j)->window,RevertToParent,CurrentTime);
								break;
							case ButtonRelease:
								printf("ButtonRelease...\n");
								break;
							case EnterNotify:
								printf("enter...\n");
								break;
							case LeaveNotify:
								printf("leave...\n");
								break;
							case FocusOut:
								subwindows.at(j)->LFSTK_hideWindow();
								printf("focus out\n");
								break;
							case FocusIn:
								printf("focus in\n");
								break;
						}
				}
				}
		}

	delete wc;
	XCloseDisplay(display);
	cairo_debug_reset_static_data();
	return 0;
}