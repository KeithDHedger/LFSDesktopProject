#if 0

#(c)keithhedger Sat 5 Aug 19:18:08 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o lineeditexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./lineeditexample "$@"
retval=$?
echo "Exit code $retval"
rm lineeditexample
exit $retval
#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL		"Line Edit"

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool doDropped(void *p,propertyStruct *data,void* ud)
{
	LFSTK_gadgetClass	*gadg=NULL;

	gadg=static_cast<LFSTK_gadgetClass*>(p);
	printf("dropped %s on gadget %p @x/y %i %i\n",data->data,gadg,data->dropX,data->dropY);
	return(true);
}

bool doKeyUp(void *p,void* ud)
{
	LFSTK_lineEditClass	*gadg=NULL;
	gadg=static_cast<LFSTK_lineEditClass*>(p);

	if(gadg->LFSTK_getKey()!=XK_VoidSymbol)
		{
			printf(">>%s<<\n",gadg->LFSTK_getCStr());
			printf("Keycode=0x%x\n",gadg->LFSTK_getKey());
			printf("Keysym=XK_%s\n",gadg->LFSTK_getKeySym().c_str());
			printf("Modifier=0x%x\n",gadg->LFSTK_getModifier());
		}
	return(true);
}

bool doMouseUp(void *p,void* ud)
{
	LFSTK_lineEditClass	*gadg=NULL;
	gadg=static_cast<LFSTK_lineEditClass*>(p);
	printf("button=%X state=%X\n",gadg->mouseEvent->button,gadg->mouseEvent->state & ControlMask);
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
	int							sy=BORDER;
	std::vector<hitRect>			hrs;
	LFSTK_ExpanderGadgetClass	*multi=NULL;

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKExample");
	wc=apc->mainWindow;

	multi=new LFSTK_ExpanderGadgetClass(wc,"",0,0,DIALOGWIDTH,GADGETHITE*3);
	multi->gadgetStretch=SPACESPREADY;

	hrs.push_back({0,0,DIALOGWIDTH,GADGETHITE,new LFSTK_labelClass(wc,BOXLABEL,0,0,1,1)});
	hrs.back().gadget->LFSTK_setCairoFontDataParts("sB",20);
	hrs.push_back({0,0,DIALOGWIDTH,GADGETHITE,new LFSTK_labelClass(wc,COPYRITE,0,0,1,1)});
	hrs.push_back({0,0,DIALOGWIDTH,GADGETHITE,new LFSTK_labelClass(wc,PERSONAL,0,0,1,1)});
	hrs.back().gadget->LFSTK_setCairoFontDataParts("B");
	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

	sy+=YSPACING*2;

//line edit
	multi=new LFSTK_ExpanderGadgetClass(wc,"",0,sy,DIALOGWIDTH,GADGETHITE);
	multi->stretchX=true;
	multi->gadgetStretch=STRETCH;
	multi->gadgetAcceptsDnD=true;

	hrs.push_back({BORDER,0,DIALOGWIDTH-BORDER*2,GADGETHITE,new LFSTK_lineEditClass(wc,"Basic line editing class for LFSToolKit.",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,doMouseUp,NULL);
	hrs.back().gadget->LFSTK_setKeyCallBack(NULL,doKeyUp,USERDATA(12345));
	hrs.back().gadget->LFSTK_setGadgetDropCallBack(doDropped);


	//hrs.back().gadget->LFSTK_setAlpha(0.2);
	//hrs.back().gadget->LFSTK_setGadgetColourPair(NORMALCOLOUR,"#20ff00ff","#20000000");
	//hrs.back().gadget->LFSTK_setColourName(NORMALCOLOUR,"#e0ffffff");
	//printf(">>>>hrs.back().gadget=%p\n",hrs.back().gadget);
	static_cast<LFSTK_lineEditClass*>(hrs.back().gadget)->LFSTK_setCallbackOnReturn(false);

	multi->LFSTK_setHitRects(hrs);
	hrs.clear();
//	multi->LFSTK_setGadgetDropCallBack(doDropped,USERDATA(0xdeadbeef));

	sy+=YSPACING;

//bottom bit
	multi=new LFSTK_ExpanderGadgetClass(wc,"",0,sy,DIALOGWIDTH,6);
	multi->stretchX=true;
	multi->lockY=LOCKTOBOTTOM;
	multi->gadgetStretch=STRETCH;

//line
	hrs.push_back({0,0,DIALOGWIDTH,2,NULL});
	hrs.back().gadget=new LFSTK_buttonClass(wc,"--",0,0,DIALOGWIDTH,2,BUTTONGRAV);
	hrs.back().gadget->gadgetDetails.bevel=BEVELNONE;
	hrs.back().gadget->gadgetDetails.buttonTile=false;
	hrs.back().gadget->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];

	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

//quit
	multi=new LFSTK_ExpanderGadgetClass(wc,"",DIALOGMIDDLE-HALFGADGETWIDTH,sy+12,GADGETWIDTH,GADGETHITE);
	multi->lockY=LOCKTOBOTTOM;

	hrs.push_back({0,0,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Quit",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,doQuit,NULL);

	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

	sy+=YSPACING+16;
	wc->LFSTK_setWindowDropCallBack(windowDrop,(void*)0xdeadbeef);

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();

	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}
