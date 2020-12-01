#if 0

#©keithhedger Sat 3 Nov 14:40:06 GMT 2018 kdhedger68713@gmail.com

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

#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL				"Menu Example"
#define NUMMENUS				16

LFSTK_windowClass				*wc=NULL;
LFSTK_labelClass				*label=NULL;
LFSTK_labelClass				*personal=NULL;
LFSTK_labelClass				*copyrite=NULL;
LFSTK_buttonClass				*seperator=NULL;
LFSTK_buttonClass				*quit=NULL;
//LFSTK_toolWindowClass			*subwc;
//LFSTK_windowClass				*subwc;
LFSTK_listGadgetClass			*list=NULL;
LFSTK_menuClass					*menu=NULL;

bool							mainLoop=true;
Display							*display;
//std::vector<LFSTK_windowClass*>	subwindows;
//std::vector<int>				xoffset;
//std::vector<int>				yoffset;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

bool menuCB(void *p,void* ud)
{
	printf("\n\np=%p ud=%i\n",p,ud);
	printf("Label=%s\n",static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel());
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	//subwc->LFSTK_hideWindow();
	//subwc->mainLoop=false;
	//XSync(subwc->display,true);
	
}

bool buttonCB(void *p,void* ud)
{
	menu->LFSTK_showMenu();
	return(true);
	menu->mainMenuWindow->LFSTK_showWindow(true);
//	subwc->LFSTK_showWindow(true);
	XSetInputFocus(menu->mainMenuWindow->display,menu->mainMenuWindow->window,RevertToParent,CurrentTime);
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
	quit->LFSTK_setMouseCallBack(NULL,buttonCB,NULL);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
//	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit=new LFSTK_buttonClass(wc,"Quit",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	sy+=YSPACING;

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	mainLoop=true;
//	subwindows.clear();

//		LFSTK_toolWindowClass(Display *disp,LFSTK_windowClass *wc,const char *windowtype,int x,int y,int w,int h,const char* name);
	//subwc->LFSTK_setWindowType("_NET_WM_WINDOW_TYPE_MENU");

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_clearWindow(true);

//printf(">>>>>>>>>>>>>>\n");
//	subwc=new LFSTK_toolWindowClass(display,wc,"_NET_WM_WINDOW_TYPE_MENU",0,0,GADGETWIDTH,GADGETHITE*NUMMENUS,"menu window");
//	subwc->topLevel=true;
	menu=new LFSTK_menuClass(wc,DIALOGMIDDLE-HALFGADGETWIDTH,hsy+GADGETHITE,1,1);
//	XTranslateCoordinates( display, wc->window,wc->rootWindow, 0, 0, &x, &y, &dw );
//											subwindows.at(j)->LFSTK_moveWindow(x+xoffset.at(j),y+hsy+yoffset.at(j),true);
//											//menu->mainMenuWindow->LFSTK_moveWindow(x+xoffset.at(j),y+hsy+yoffset.at(j),true);
//printf("<<<<<<<<<<<<<<\n");

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_clearWindow(true);

//	xoffset.clear();
//	yoffset.clear();
//	subwindows.clear();
	
//subwc->LFSTK_clearWindow();
//	subwindows.push_back(menu->mainMenuWindow);
//	xoffset.push_back(DIALOGMIDDLE-HALFGADGETWIDTH+((subwindows.size()-1)*(GADGETWIDTH)));
//	xoffset.push_back(DIALOGMIDDLE-HALFGADGETWIDTH+40);
//	yoffset.push_back(((subwindows.size())*GADGETHITE));


	menuStruct	**mms=new menuStruct*[NUMMENUS];

	for (int j=0; j<NUMMENUS; j++)
		{
			mms[j]=new menuStruct;
			asprintf(&mms[j]->label,"menu %i",j);
			mms[j]->hasSubMenu=false;
			mms[j]->subMenus=NULL;
			mms[j]->userData=(void*)(j+1);
			mms[j]->imageType=NOTHUMB;
		}
	free(mms[12]->label);
	mms[12]->label=strdup("--");
	free(mms[2]->label);
	mms[2]->label=strdup("A really long label that will over flow");

#if 1
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
			mms[2]->subMenus[j]->userData=(void*)((j+1)*100);
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
			mms[6]->subMenus[j]->userData=(void*)((j+1)*10000);
			mms[6]->subMenus[j]->imageType=FILETHUMB;
			//mms[6]->subMenus[j]->imageType=NOTHUMB;
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
			mms[6]->subMenus[4]->subMenus[j]->userData=(void*)((j+1)*100000);
			mms[6]->subMenus[4]->subMenus[j]->imageType=FILETHUMB;
		//	mms[6]->subMenus[4]->subMenus[j]->imageType=NOTHUMB;
			mms[6]->subMenus[4]->subMenus[j]->data.imagePath=strdup("./audio-speakers.png");
		}
#endif
#if 0
	LFSTK_menuItemClass	*label;
//	LFSTK_labelClass	*label;
	sy=0;
//LFSTK_labelClass (LFSTK_windowClass *parentwc, const char *label, int x, int y, unsigned w, unsigned h, int gravity=CENTRE, int bgrav=BUTTONGRAV)

	//this->mainMenuCnt=menucnt;
	//this->mainMenu=menus;
	for(int j=0;j<NUMMENUS;j++)
		{
			//label=new LFSTK_labelClass(subwc,"menu",0,sy,GADGETWIDTH,GADGETHITE);
//			label=new LFSTK_menuItemClass(this->wc,this->mainMenu[j]->label,0,sy,GADGETWIDTH,GADGETHITE,this->mainMenu[j],MENU);
			label=new LFSTK_menuItemClass((LFSTK_toolWindowClass*)subwc,mms[j]->label,0,sy,GADGETWIDTH,GADGETHITE,mms[j],NULL);
			label->LFSTK_setCallBack(NULL,menuCB,(void*)(mms[j]->userData));
			if(mms[j]->imageType==FILETHUMB)
				label->LFSTK_setImageFromPath(mms[j]->data.imagePath,MENU,true);
			if(mms[j]->imageType==CAIROTHUMB)
				label->LFSTK_setImageFromSurface(mms[j]->data.surface,MENU,true);
			sy+=GADGETHITE;
		}
#endif


#if 0
	LFSTK_menuClass *mc=new LFSTK_menuClass(subwc,"Test Menu",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	mc->LFSTK_setCallBack(NULL,menuCB,(void*)0xdeadbeef);
	sy+=YSPACING;


	menuStruct	**mms=new menuStruct*[NUMMENUS];

	for (int j=0; j<NUMMENUS; j++)
		{
			mms[j]=new menuStruct;
			asprintf(&mms[j]->label,"menu %i",j);
			mms[j]->hasSubMenu=false;
			mms[j]->subMenus=NULL;
			mms[j]->userData=(void*)(j+1);
			mms[j]->imageType=NOTHUMB;
		}

	mms[0]->imageType=FILETHUMB;
	mms[0]->data.imagePath="./AspellGUI.png";
	mms[10]->imageType=FILETHUMB;
	mms[10]->data.imagePath="./casper2.JPG";
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
			mms[2]->subMenus[j]->userData=(void*)((j+1)*100);
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
			mms[6]->subMenus[j]->userData=(void*)((j+1)*10000);
			mms[6]->subMenus[j]->imageType=FILETHUMB;
			mms[6]->subMenus[j]->data.imagePath="./computer.png";
		}

	mms[6]->subMenus[4]->hasSubMenu=true;
	mms[6]->subMenus[4]->subMenuCnt=NUMMENUS;

	mms[6]->subMenus[4]->subMenus=new menuStruct*[NUMMENUS];
	for (int j=0; j<NUMMENUS; j++)
		{
			mms[6]->subMenus[4]->subMenus[j]=new menuStruct;
			asprintf(&mms[6]->subMenus[4]->subMenus[j]->label,"sub sub menu 6 - %i",j);
			mms[6]->subMenus[4]->subMenus[j]->hasSubMenu=false;
			mms[6]->subMenus[4]->subMenus[j]->subMenus=NULL;
			mms[6]->subMenus[4]->subMenus[j]->userData=(void*)((j+1)*100000);
			mms[6]->subMenus[4]->subMenus[j]->imageType=FILETHUMB;
			mms[6]->subMenus[4]->subMenus[j]->data.imagePath="./audio-speakers.png";
		}


	mc->LFSTK_addMainMenus(mms,NUMMENUS);
#endif
//	menu->LFSTK_setCallBack(NULL,buttonCB,NULL);
	menu->LFSTK_setMouseCallBack(NULL,menuCB,NULL);
	menu->LFSTK_addMainMenus(mms,NUMMENUS);

//	subwindows.push_back(menu->mainMenuWindow);
//	xoffset.push_back(DIALOGMIDDLE-HALFGADGETWIDTH+((subwindows.size()-1)*(GADGETWIDTH)));
//	xoffset.push_back(DIALOGMIDDLE-HALFGADGETWIDTH+40);
//	yoffset.push_back(((subwindows.size())*GADGETHITE));

printf("wc=%p tw=%p\n",wc,menu->mainMenuWindow);
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


while(0)
{
					XNextEvent(menu->mainMenuWindow->display,&event);
					if(menu->mainMenuWindow->LFSTK_handleWindowEvents(&event)<0)
						mainLoop=false;
					ml=menu->mainMenuWindow->LFSTK_getMappedListener(event.xany.window);
					if(ml!=NULL)
						ml->function(ml->gadget,&event,ml->type);

					switch(event.type)
						{
						case FocusOut:
							printf("foc out>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
							break;
						case ConfigureNotify:
						case Expose:
							//if(event.xany.window==subwc->window)
//								{
//									for(int j=0; j<subwindows.size(); j++)
//										{
//											Window dw;
//											int x;
//											int y;
//											unsigned int w,h,dump;
//											XTranslateCoordinates( display, wc->window,wc->rootWindow, 0, 0, &x, &y, &dw );
//											subwindows.at(j)->LFSTK_moveWindow(x+xoffset.at(j),y+hsy+yoffset.at(j),true);
//											//menu->mainMenuWindow->LFSTK_moveWindow(x+xoffset.at(j),y+hsy+yoffset.at(j),true);
//											XSync(display,false);
//										}
//								}
							break;
						}
				}
				}

#if 0
			for(int j=0; j<subwindows.size(); j++)
				{
					if(XPending(subwindows.at(j)->display))
						{
							XNextEvent(subwindows.at(j)->display,&event);
							mappedListener *ml=subwindows.at(j)->LFSTK_getMappedListener(event.xany.window);

							if(ml!=NULL)
								ml->function(ml->gadget,&event,ml->type);

							subwindows.at(j)->LFSTK_handleWindowEvents(&event);

							switch(event.type)
								{
								case Expose:

									subwindows.at(j)->LFSTK_clearWindow(true);
									break;

								case FocusOut:
									//subwindows.at(j)->LFSTK_hideWindow();
									//subwc->LFSTK_hideWindow();
									printf("focus out\n");
									break;
								}
						}
				}
#endif
		}

	delete wc;
//	for(int j=0; j<NUMMENUS; j++)
//		{
//			if(mms[6]->subMenus[j]->imageType==CAIROTHUMB)
//				cairo_surface_destroy(mms[6]->subMenus[j]->data.surface);
//			if(mms[2]->subMenus[j]->imageType==CAIROTHUMB)
//				cairo_surface_destroy(mms[6]->subMenus[j]->data.surface);
//			
//			free(mms[6]->subMenus[j]->label);
//			free(mms[2]->subMenus[j]->label);
//			delete mms[6]->subMenus[j];
//			delete mms[2]->subMenus[j];
//		}
//	delete[] mms[6]->subMenus;
//	delete[] mms[2]->subMenus;
//	for(int j=0; j<NUMMENUS; j++)
//		{
//			if(mms[j]->imageType==CAIROTHUMB)
//				cairo_surface_destroy(mms[j]->data.surface);
//			free(mms[j]->label);
//			delete mms[j];
//		}
//	delete[] mms;
//	menu->LFSTK_freeMenus(mms,NUMMENUS);
	delete menu;

	XSync(display,true);
	XCloseDisplay(display);
	cairo_debug_reset_static_data();
	return 0;
}