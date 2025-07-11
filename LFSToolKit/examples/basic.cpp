#if 0

#(c)keithhedger Fri 28 Jul 20:19:00 BST 2017 kdhedger68713@gmail.com

echo "Compiling, please wait ..."

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full "
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo glib-2.0 ) -llfstoolkit -lImlib2 -o basicexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./basicexample "$@"
retval=$?
echo "Exit code $retval"
rm basicexample
exit $retval
#endif

#include <iostream>
#include <string>
#include <sstream>

#include "../config.h"
#include "../LFSToolKit/lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Basic Button Window"

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_windowClass		*popWindow=NULL;

LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_buttonClass		*inactivebutton=NULL;
LFSTK_buttonClass		*normalbutton=NULL;
LFSTK_buttonClass		*ignoredButton=NULL;
LFSTK_buttonClass		*leftButton=NULL;
LFSTK_buttonClass		*centreButton=NULL;
LFSTK_buttonClass		*rightButton=NULL;
LFSTK_buttonClass		*dropButton=NULL;
LFSTK_labelClass			*poplabel=NULL;

bool						show=true;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	if(ud!=NULL)
		{
			printf(">>>%s<<<\n",(const char*)ud);
			if(strcmp((const char*)ud,"Active")==0)
				{
					show=!show;
					if(show==true)
						inactivebutton->LFSTK_showGadget();
					else
						inactivebutton->LFSTK_hideGadget();
					wc->LFSTK_clearWindow();
				}
		}
	return(true);
}

bool keyCB(void *p,void* ud)
{
	LFSTK_buttonClass	*b=NULL;
	if(p!=NULL)
		{
			b=static_cast<LFSTK_buttonClass*>(p);
			if(b!=NULL)
				DEBUGKEY((XKeyEvent*)b->xEvent)
		}

	if(ud!=NULL)
		{
			printf(">>>%s<<<\n",(const char*)ud);
		}
	return(true);
}

bool mouseCB(void *p,void* ud)
{
	if(ud!=NULL)
		{
			printf(">>>%s<<<\n",(const char*)ud);
			//wc->LFSTK_setKeepBelow(false);
			popWindow->LFSTK_hideWindow();

		}
	return(true);
}

bool moveCB(LFSTK_gadgetClass*p,void* ud)
{
	if(ud!=NULL)
		{
			XEvent			event;
			geometryStruct	geom;
			printf(">>>Mouse In %s<<<\n",(const char*)ud);

			p->LFSTK_getGeomWindowRelative(&geom,apc->rootWindow);	
			//popWindow->LFSTK_moveWindow(geom.x,geom.y-GADGETHITE,true);
			popWindow->LFSTK_moveWindow(geom.x+(geom.w/2)-(poplabel->LFSTK_getTextRealWidth("This is a mouse enter callback")/2),geom.y-GADGETHITE,true);

			popWindow->LFSTK_showWindow();
			popWindow->LFSTK_clearWindow(true);
		}
	return(true);
}

bool exitCB(LFSTK_gadgetClass*p,void* ud)
{
	if(ud!=NULL)
		{
			printf(">>>Mouse Out %s<<<\n",(const char*)ud);
			popWindow->LFSTK_hideWindow();
		}
	return(true);
}

int cnt=1;
bool timerCB(LFSTK_applicationClass *p,void* ud)
{
	printf("Timer callback number %i of 4\n",cnt);
	cnt++;
	if(cnt<5)
		return(true);
	else
		return(false);
}

bool gadgetDrop(void *lwc,propertyStruct *data,void* ud)
{
	LFSTK_buttonClass	*bc=static_cast<LFSTK_buttonClass*>(lwc);
	std::string			c;

	if(data!=NULL)
		{
			if(strcasecmp(data->mimeType,"text/uri-list")==0)
				{
					std::istringstream stream((const char*)data->data);
					std::string line;
					while(std::getline(stream,line))
						{
							c=LFSTK_UtilityClass::LFSTK_strStrip(line);
							std::cerr<<"dropped file '"<<c<<"' on gadget '"<<bc->LFSTK_getLabel()<<"' @x/y "<<data->dropX<<" "<<data->dropY<<std::endl;
						}
				}
			else
				{
					c=LFSTK_UtilityClass::LFSTK_strStrip((const char*)data->data);
					std::cerr<<"dropped text '"<<c<<"' on gadget '"<<bc->LFSTK_getLabel()<<"' @x/y "<<data->dropX<<" "<<data->dropY<<std::endl;
				}
		}
	return(true);
}

int main(int argc, char **argv)
{
	int				sy=BORDER;
	windowInitStruct	*win;

	apc=new LFSTK_applicationClass();

	win=apc->LFSTK_getDefaultWInit();
	win->windowName=BOXLABEL;
	win->windowType=win->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DIALOG"));
	win->level=ABOVEALL;
	apc->LFSTK_addWindow(win,BOXLABEL);
	wc=apc->mainWindow;

	wc->LFSTK_initDnD(false);

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,NorthGravity);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,NorthGravity);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,NorthGravity);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//inactivebutton
	inactivebutton=new LFSTK_buttonClass(wc,"Inactive",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE);
	inactivebutton->LFSTK_setActive(false);
	sy+=YSPACING;

//normal button
	normalbutton=new LFSTK_buttonClass(wc,"Show/Hide",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE);
	normalbutton->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("Active"));
	sy+=YSPACING;

//ignoredButton button
	ignoredButton=new LFSTK_buttonClass(wc,"Ignore CB",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE);
	ignoredButton->LFSTK_setIgnores(false,true);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//left
	leftButton=new LFSTK_buttonClass(wc,"Label Left",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE);
//	leftButton->LFSTK_setGadgetColourPair(NORMALCOLOUR,"#000000","red");
	leftButton->LFSTK_setLabelGravity(LEFT);
	leftButton->LFSTK_setKeyCallBack(NULL,keyCB,USERDATA("Key Left"));
	leftButton->LFSTK_setMouseCallBack(NULL,mouseCB,USERDATA("Left"));
	leftButton->LFSTK_setMouseMoveCallBack(moveCB,exitCB,USERDATA("Left Enter/Exit"));
	sy+=YSPACING;

//centre
	centreButton=new LFSTK_buttonClass(wc,"Label Centre",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE);
	centreButton->LFSTK_setLabelGravity(CENTRE);
	centreButton->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("Centre"));
	sy+=YSPACING;
//rite
	rightButton=new LFSTK_buttonClass(wc,"Label Right",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE);
	rightButton->LFSTK_setLabelGravity(RIGHT);
	rightButton->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("Right"));
	sy+=YSPACING;
//drop gadget
	dropButton=new LFSTK_buttonClass(wc,"Drop Here",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE);
	dropButton->LFSTK_setKeyCallBack(NULL,keyCB,USERDATA("Key Drop"));
	dropButton->LFSTK_setMouseCallBack(NULL,mouseCB,USERDATA("Drop"));
	dropButton->LFSTK_setGadgetDropCallBack(gadgetDrop,NULL);
	dropButton->gadgetAcceptsDnD=true;
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//left image
	//leftButton=new LFSTK_buttonClass(wc,"Left Label+Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE+100);
	leftButton=new LFSTK_buttonClass(wc,"Left Label+Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,(GADGETWIDTH*2));
	leftButton->LFSTK_setImageFromPath("./casper2.JPG",LEFT,true);
	leftButton->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("Left Label+Image"));
	leftButton->LFSTK_setAlpha(1.0);
	//leftButton->LFSTK_setTile("/media/LinuxData/Development/Projects/LFSDesktopProject/LFSToolKit/examples/AspellGUI.png",-1);
	//leftButton->LFSTK_setTile("/home/keithhedger/Desktop/001_wood_coldgrayfence_th.jpg",-1);
	leftButton->LFSTK_setTile("./t2.jpg",-1);
	leftButton->gadgetDetails.geomRelativeToMainWindow=true;
	sy+=YSPACING+(GADGETWIDTH*2);

//centre image
	centreButton=new LFSTK_buttonClass(wc,"Centre Label+Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE);
	centreButton->LFSTK_setImageFromPath("./casper2.JPG",LEFT,true);
	centreButton->LFSTK_setLabelGravity(CENTRE);
	centreButton->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("Centre Label+Image"));
	sy+=YSPACING;
//rite image
	rightButton=new LFSTK_buttonClass(wc,"Right Label+Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE);
	rightButton->LFSTK_setImageFromPath("./casper2.JPG",LEFT,true);
	rightButton->LFSTK_setLabelGravity(RIGHT);
	rightButton->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("Right Label+Image"));
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_setKeepAbove(true);

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	apc->LFSTK_setTimer(1000000,true);
	apc->LFSTK_setTimerCallBack(timerCB,NULL);

	popWindow=new LFSTK_toolWindowClass(apc->display,wc,apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_MENU")),0,0,200,100,"lfstkpopup",apc);

	poplabel=new LFSTK_labelClass(popWindow,"This is a mouse enter callback",0,0,GADGETWIDTH*8,GADGETHITE,WestGravity);
	poplabel->LFSTK_setCairoFontDataParts("sB",20);
	poplabel->LFSTK_setTile(NULL,0);

	popWindow->LFSTK_resizeWindow(poplabel->LFSTK_getTextRealWidth("This is a mouse enter callback"),GADGETHITE);
	popWindow->LFSTK_setWindowColourName(NORMALCOLOUR,"#c0808080");

	int retval=apc->LFSTK_runApp();

	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}
