#if 0

#(c)keithhedger Fri 4 Aug 20:10:13 BST 2017 kdhedger68713@gmail.com

echo "Compiling, please wait ..."

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo  glib-2.0) -llfstoolkit -lImlib2 -o imagebuttonexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./imagebuttonexample "$@"
retval=$?
echo "Exit code $retval"
rm imagebuttonexample
exit $retval
#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Image Buttons"

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass		*imagebuttonL=NULL;
LFSTK_buttonClass		*imagebuttonC=NULL;
LFSTK_buttonClass		*imagebuttonR=NULL;
LFSTK_toggleButtonClass	*imagebuttonT=NULL;
LFSTK_buttonClass		*imagebuttonTOP=NULL;
LFSTK_buttonClass		*imagebuttonBOTTOM=NULL;
LFSTK_buttonClass		*imagebuttonNA=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;

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
			printf(">>>%p<<<\n",(const char*)ud);
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

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//image button toolbutton
	imagebuttonT=new LFSTK_toggleButtonClass(wc,"Toggle/Tool Button",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*2,BUTTONGRAV);
	imagebuttonT->LFSTK_setToggleStyle(TOGGLENORMAL);
	imagebuttonT->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA(0xdeadbeef0));
	imagebuttonT->LFSTK_setImageFromPath("./AspellGUI.png",TOOLBAR,true);
	sy+=YSPACING*2;

//image button left
	imagebuttonL=new LFSTK_buttonClass(wc,"Left Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*2,BUTTONGRAV);
imagebuttonL->flip=1;

	imagebuttonL->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA(0xdeadbeef1));
	imagebuttonL->LFSTK_setImageFromPath("./casper2.JPG",LEFT,true);
	imagebuttonL->LFSTK_setLabelGravity(CENTRE);
	imagebuttonL->drawLabelBG=true;
	imagebuttonL->autoLabelBGColour=true;
	sy+=YSPACING*2;

//image button centre
	imagebuttonC=new LFSTK_buttonClass(wc,"Centre Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*2,BUTTONGRAV);
	//imagebuttonC->LFSTK_setMouseCallBack(buttonCB,NULL,USERDATA(0xdeadbeef2));
	imagebuttonC->LFSTK_setImageFromPath("./casper2.JPG",CENTRE,true);
	imagebuttonC->LFSTK_setFontString("sans:size=16:bold",true);
	imagebuttonC->forceCentreLabel=true;
	imagebuttonC->drawLabelBG=true;
	imagebuttonC->autoLabelBGColour=true;
	imagebuttonC->userStringData="Centre Image";
	imagebuttonC->userData=USERDATA(0xdeadbeef2);

	imagebuttonC->LFSTK_connectCB(ButtonPress,[](std::string msg)
		{
			fprintf(stderr,"got ButtonPress 4000:>>%s<<\n",msg.c_str());
			fprintf(stderr,"gadget->currentButton=%i\n",imagebuttonC->currentButton);
			fprintf(stderr,"gadget->userData=%p\n",imagebuttonC->userData);
			fprintf(stderr,"gadget->userStringData=>>%s<<\n",imagebuttonC->userStringData.c_str());
			fprintf(stderr,"gadget->event=%p\n",imagebuttonC->xEvent);
			if(imagebuttonC->xEvent!=NULL)
				{
					fprintf(stderr,"gadget->event->type=%p\n",imagebuttonC->xEvent->type);
				}
			//qDebug()<<QString("got 4:%1").arg(msg)<<cc.cnt;
		});

//	imagebuttonC->connectCBByName(27,[](std::string msg)
//		{
//			fprintf(stderr,"got 8000:%s\n",msg.c_str());
//			//qDebug()<<QString("got 4:%1").arg(msg)<<cc.cnt;
//		});





//imagebuttonC->LFSTK_setLabelBGColour(1,0,0,1.0);
	sy+=YSPACING*2;

//image button right
	imagebuttonR=new LFSTK_buttonClass(wc,"Right Image (flipped)",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*2,BUTTONGRAV);
//imagebuttonR->flip=2;
	imagebuttonR->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA(0xdeadbeef3));
	imagebuttonR->LFSTK_setImageFromPath("./casper2.JPG",RIGHT,true);
	imagebuttonR->LFSTK_setLabelGravity(CENTRE);
	imagebuttonR->gadgetDetails.reserveSpace=4;
imagebuttonR->flip=0;
	sy+=YSPACING*2;

//image button top
	imagebuttonTOP=new LFSTK_buttonClass(wc,"Top Image",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*4,BUTTONGRAV);
	imagebuttonTOP->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA(0xdeadbeef4));
	imagebuttonTOP->LFSTK_setImageFromPath("./casper2.JPG",TOOLBAR,true);
	imagebuttonTOP->LFSTK_setImageGravity(TOP);
	imagebuttonTOP->LFSTK_setLabelGravity(BOTTOM);
	imagebuttonTOP->gadgetDetails.reserveSpace=4;
	sy+=YSPACING*2+GADGETHITE*2;
imagebuttonTOP->flip=2;

//image button bottom
	imagebuttonBOTTOM=new LFSTK_buttonClass(wc,"Bottom Image (flipped)",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*4,BUTTONGRAV);
imagebuttonBOTTOM->flip=3;
	imagebuttonBOTTOM->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA(0xdeadbeef5));
	imagebuttonBOTTOM->LFSTK_setImageFromPath("./casper2.JPG",TOOLBAR,true);
	imagebuttonBOTTOM->LFSTK_setImageGravity(BOTTOM);
	imagebuttonBOTTOM->LFSTK_setLabelGravity(TOP);
	imagebuttonBOTTOM->gadgetDetails.reserveSpace=4;
	sy+=YSPACING*2+GADGETHITE*2;

//image button menu
	imagebuttonR=new LFSTK_buttonClass(wc,"Menu Image Button",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	imagebuttonR->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA(0xdeadbeef6));
	imagebuttonR->LFSTK_setImageFromPath("./casper2.JPG",MENU,true);
	sy+=YSPACING;

//image button no alpha
	imagebuttonNA=new LFSTK_buttonClass(wc,"Image No Alpha",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*2,BUTTONGRAV);
	imagebuttonNA->LFSTK_setMouseCallBack(NULL,buttonCB,USERDATA(0xdeadbeef7));
	imagebuttonNA->LFSTK_setImageFromPath("./casper2.JPG",LEFT,true);
	imagebuttonNA->LFSTK_setLabelGravity(CENTRE);
	imagebuttonNA->LFSTK_setAlpha(255);
	sy+=YSPACING*2;

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

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}