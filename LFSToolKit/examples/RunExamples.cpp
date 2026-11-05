#if 0

#(c)keithhedger Sat 13 Oct 17:20:15 BST 2018 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

(
	cd ..
	make ${MAKEFLAGS:--j4} --output-sync
)

APPNAME=$(basename $0 .cpp)
cd "$(dirname "$0")"

g++ -Wstack-protector -Werror -fstack-protector "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo glib-2.0) -llfstoolkit -lImlib2  -Wstack-protector -Werror -fstack-protector -o $APPNAME||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./$APPNAME "$@"
retval=$?
rm $APPNAME
exit $retval

#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL				"Examples"
#define MAKEFLAGS			"-j10  --output-sync"

LFSTK_applicationClass		*apc=NULL;
LFSTK_windowClass			*wc=NULL;
LFSTK_toggleButtonClass		*useDbg=NULL;
LFSTK_toggleButtonClass		*openFile=NULL;
LFSTK_toggleButtonClass		*makeLib=NULL;
LFSTK_ExpanderGadgetClass	*multi=NULL;

const char					*dbg;
int							retValOfCommand=0;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	XSync(apc->display,true);
	return(true);
}


bool buttonCB(void *p,void* ud)
{
	char	*command;
	if(ud!=NULL)
		{
			if(makeLib->LFSTK_getValue()==true)
				system("(pushd ..;make " MAKEFLAGS ";popd)");

			asprintf(&command,"%s %s &",(const char*)ud,dbg);
			retValOfCommand=system(command);
			free(command);
			if(openFile->LFSTK_getValue()==true)
				{
					asprintf(&command,"xdg-open %s",(const char*)ud);
					system(command);
					free(command);
				}
		}
	return(true);
}

bool doDbg(void *p,void* ud)
{
	char	*command;
	if(p!=NULL)
		{
			if(useDbg->LFSTK_getValue()==true)
				dbg="dbg";
			else
				dbg="";
		}
	return(true);
}

int main(int argc, char **argv)
{
	int					sy=0;
	std::vector<hitRect>	hrs;
	bool					useliveupdate=true;
	windowInitStruct		*win;
	dbg="";

	apc=new LFSTK_applicationClass();

	win=apc->LFSTK_getDefaultWInit();
	win->windowName=BOXLABEL;
	win->windowType=win->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DIALOG"));
	win->level=ABOVEALL;
	apc->LFSTK_addWindow(win,BOXLABEL);
	wc=apc->mainWindow;
	wc->LFSTK_setDecorations(false,false,true,false);

//info
	multi=new LFSTK_ExpanderGadgetClass(wc,"",0,0,DIALOGWIDTH,GADGETHITE*4);
	//multi->LFSTK_setUseWindowTile(false);
	//multi->LFSTK_setTile(NULL,0);
	multi->stretchX=true;
	multi->lockY=LOCKTOTOP;
	multi->gadgetStretch=STRETCH;
	multi->liveUpdate=useliveupdate;

	hrs.push_back({0,sy,DIALOGWIDTH,GADGETHITE,NULL});
	hrs.back().gadget=new LFSTK_labelClass(wc,BOXLABEL,0,0,1,1);
	hrs.back().gadget->LFSTK_setLabelGravity(CENTRE);
	hrs.back().gadget->LFSTK_setCairoFontDataParts("sB",20);
	
	sy+=YSPACING;

	hrs.push_back({0,sy,DIALOGWIDTH,GADGETHITE,NULL});
	hrs.back().gadget=new LFSTK_labelClass(wc,COPYRITE,0,0,1,1);
	hrs.back().gadget->LFSTK_setLabelGravity(CENTRE);
	sy+=HALFYSPACING;

	hrs.push_back({0,sy,DIALOGWIDTH,GADGETHITE,NULL});
	hrs.back().gadget=new LFSTK_labelClass(wc,PERSONAL,0,0,1,1);
	hrs.back().gadget->LFSTK_setLabelGravity(CENTRE);
	hrs.back().gadget->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

	multi->LFSTK_setHitRects(hrs);

	hrs.clear();

//options
	multi=new LFSTK_ExpanderGadgetClass(wc,"",0,sy,DIALOGWIDTH,GADGETHITE);
	multi->stretchX=true;
	multi->gadgetStretch=SPACESPREADX;
	multi->liveUpdate=useliveupdate;

	hrs.push_back({0,0,GADGETWIDTH,GADGETHITE,NULL});
	useDbg=new LFSTK_toggleButtonClass(wc,"Use Valgrind",0,0,GADGETWIDTH,GADGETHITE,NorthWestGravity);
	useDbg->LFSTK_setValue(false);
	useDbg->LFSTK_setMouseCallBack(NULL,doDbg,NULL);
	//useDbg->LFSTK_setUseWindowTile(true);
	//useDbg->LFSTK_setTile(NULL,0);
	useDbg->LFSTK_setToggleStyle(TOGGLECHECK);
	hrs.back().gadget=useDbg;

	hrs.push_back({0,0,GADGETWIDTH,GADGETHITE,NULL});
	openFile=new LFSTK_toggleButtonClass(wc,"Open File",0,0,GADGETWIDTH,GADGETHITE,NorthWestGravity);
	openFile->LFSTK_setValue(false);
	//openFile->LFSTK_setUseWindowTile(true);
	//openFile->LFSTK_setTile(NULL,0);
	openFile->LFSTK_setToggleStyle(TOGGLECHECK);
	hrs.back().gadget=openFile;

	hrs.push_back({0,0,GADGETWIDTH,GADGETHITE,NULL});
	makeLib=new LFSTK_toggleButtonClass(wc,"Build Lib",0,0,GADGETWIDTH,GADGETHITE,NorthWestGravity);
	makeLib->LFSTK_setValue(true);
	//makeLib->LFSTK_setUseWindowTile(true);
	//makeLib->LFSTK_setTile(NULL,0);
	makeLib->LFSTK_setToggleStyle(TOGGLECHECK);
	hrs.back().gadget=makeLib;

	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

//line
	sy+=HALFYSPACING+12;
	multi=new LFSTK_ExpanderGadgetClass(wc,"",0,sy,DIALOGWIDTH,GADGETHITE-11);
	multi->stretchX=true;
	multi->lockY=LOCKTOTOP;
	multi->gadgetStretch=STRETCH;
	multi->liveUpdate=useliveupdate;

	hrs.push_back({0,0,DIALOGWIDTH,2,NULL});
	hrs.back().gadget=new LFSTK_buttonClass(wc,"--",0,0,DIALOGWIDTH,2,BUTTONGRAV);
	hrs.back().gadget->gadgetDetails.bevel=BEVELNONE;
	hrs.back().gadget->gadgetDetails.buttonTile=false;
	hrs.back().gadget->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];

	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

	sy+=HALFYSPACING-4;

//run examples - middle bit
	int	internalsy=0;
	multi=new LFSTK_ExpanderGadgetClass(wc,"",GADGETWIDTH+BORDER/2,sy,GADGETWIDTH*3+BORDER*2,GADGETHITE*15);
	multi->stretchX=true;
	multi->stretchY=true;
	multi->gadgetStretch=STRETCH;
	multi->liveUpdate=useliveupdate;
	//multi->LFSTK_setUseWindowTile(false);
	//multi->LFSTK_setTile(NULL,0);

//lineedit
	hrs.push_back({0,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Line Edit",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./lineedit.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);

//multi line
	hrs.push_back({GADGETWIDTH+multi->spacePad,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Multi Line Edit",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./multilineedit.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);

//get icon path
	hrs.push_back({(GADGETWIDTH+multi->spacePad)*2,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Get Icon Path",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./geticonpath.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);

	internalsy+=YSPACING;
//basic buttons
	hrs.push_back({0,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Buttons",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./basic.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);

//toggles
	hrs.push_back({GADGETWIDTH+multi->spacePad,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Toggles",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./toggle.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);

//expander
	hrs.push_back({(GADGETWIDTH+multi->spacePad)*2,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Expander",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./expander.cpp"));
//	hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);
	internalsy+=YSPACING;

//menus
	hrs.push_back({GADGETWIDTH+multi->spacePad,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Menus",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./menus.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);
	internalsy+=YSPACING;

//dialogs
	hrs.push_back({0,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Dialogs",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./dialogs.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);

//fontdialog
	hrs.push_back({(GADGETWIDTH+multi->spacePad)*2,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Font Dialog",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./fontdialog.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);
	internalsy+=YSPACING;

//list
	hrs.push_back({GADGETWIDTH+multi->spacePad,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"List",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./list.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);
	internalsy+=YSPACING;

//window
	hrs.push_back({0,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Window",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./window.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);

//subwindow
	hrs.push_back({(GADGETWIDTH+multi->spacePad)*2,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Sub Window",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./subwindowtest.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);
	internalsy+=YSPACING;

//image
	hrs.push_back({0,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Image",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./image.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);

//image buttons
	hrs.push_back({(GADGETWIDTH+multi->spacePad)*2,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Image Buttons",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./imagebutton.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);
	internalsy+=YSPACING;

//scrollbar
	hrs.push_back({GADGETWIDTH+multi->spacePad,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Scrollbar",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./scrollbar.cpp"));
	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
	//hrs.back().gadget->LFSTK_setTile(NULL,0);
	internalsy+=YSPACING;

//lfsabout
	hrs.push_back({GADGETWIDTH+multi->spacePad,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"LFS About",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./lfsabout.cpp"));
	internalsy+=YSPACING;


//
//	
//	
//Calendar
	hrs.push_back({GADGETWIDTH+multi->spacePad,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Calendar",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./calander.cpp"));
	internalsy+=YSPACING;
//	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
//	//hrs.back().gadget->LFSTK_setTile(NULL,0);
//
//
////hrs.back().gadget->LFSTK_setTile(apc->globalLib->LFSTK_getGlobalString(NORMAL,TYPEBUTTONTILE),-1);
////hrs.back().gadget->LFSTK_setTile("/usr/share/themes/OldBrownWood.lfstk/button.png",-1);
////hrs.back().gadget->LFSTK_setTile("/tmp/xx.png",-1);
////fprintf(stderr,"?=>>%s<<\n",apc->globalLib->LFSTK_getGlobalString(NORMAL,TYPEBUTTONTILE));
//	internalsy+=YSPACING;
//
//run command
	hrs.push_back({GADGETWIDTH+multi->spacePad,internalsy,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Run Command",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA("./lfsruncommand.cpp"));
//	//hrs.back().gadget->LFSTK_setUseWindowTile(true);
//	//hrs.back().gadget->LFSTK_setTile(NULL,0);
////hrs.back().gadget->LFSTK_setTile("/home/keithhedger/Backgrounds/rage.png",-1);
//	internalsy+=YSPACING;
//





	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

	sy+=YSPACING*11;

//bottom bit
	multi=new LFSTK_ExpanderGadgetClass(wc,"",0,sy,DIALOGWIDTH,6);
	multi->stretchX=true;
	multi->lockY=LOCKTOBOTTOM;
	multi->gadgetStretch=STRETCH;
	multi->liveUpdate=useliveupdate;

//line
	hrs.push_back({0,0,DIALOGWIDTH,2,NULL});
	hrs.back().gadget=new LFSTK_buttonClass(wc,"--",0,0,DIALOGWIDTH,2,BUTTONGRAV);
	hrs.back().gadget->gadgetDetails.bevel=BEVELNONE;
	//hrs.back().gadget->gadgetDetails.buttonTile=false;
	hrs.back().gadget->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];

	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

//quit
	multi=new LFSTK_ExpanderGadgetClass(wc,"",DIALOGMIDDLE-HALFGADGETWIDTH,sy+12,GADGETWIDTH,GADGETHITE);
	multi->lockY=LOCKTOBOTTOM;
	multi->liveUpdate=useliveupdate;
	//multi->LFSTK_setUseWindowTile(true);
	//multi->LFSTK_setTile("/home/keithhedger/Backgrounds/bc3.png",-1);

	hrs.push_back({0,0,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Quit",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
//	hrs.back().gadget->LFSTK_setUseWindowTile(true);
//hrs.back().gadget->LFSTK_setTile("/home/keithhedger/Backgrounds/rage.png",-1);
//	hrs.back().gadget->LFSTK_setTile(NULL,0);
//	hrs.back().gadget->useTile=true;
//hrs.back().gadget->isTransparent=false;
//hrs.back().gadget->LFSTK_setAlpha(1.0);
//hrs.back().gadget->LFSTK_setStyle(BEVELOUT);
//hrs.back().gadget->gadgetDetails.buttonTile=true;
	internalsy+=YSPACING;

	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

	sy+=YSPACING+14;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	//apc->LFSTK_setTimer(1,true);
	int retval=apc->LFSTK_runApp();
	delete apc;
	return(retval);

}