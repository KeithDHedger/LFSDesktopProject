#if 0

#(c)keithhedger Fri 28 Jul 20:19:00 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o windowexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./windowexample "$@"
retval=$?
echo "Exit code $retval"
rm windowexample
exit $retval
#endif

#include "../config.h"
#include "../LFSToolKit/lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Expander Gadget"

LFSTK_applicationClass		*apc=NULL;
LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_ExpanderGadgetClass	*multi=NULL;

bool mouseparentCB(void *p,void* ud)
{
	LFSTK_ExpanderGadgetClass	*gadg=NULL;
	gadg=static_cast<LFSTK_ExpanderGadgetClass*>(p);

	for(int j=0;j<gadg->hitRects.size();j++)
		{
			if(gadg->hitRects.at(j).gadget->window==gadg->xEvent->xbutton.subwindow)
				{
					if(j==0)
						gadg->gadgetStretch=STRETCH;
					if(j==2)
						gadg->gadgetStretch=MOVE;
					printf("Gadget No. %i Label=\"%s\" found\n",j,gadg->hitRects.at(j).gadget->LFSTK_getLabel());
				}
		}
	return(true);
}

bool mouseCB(void *p,void* ud)
{
	printf("Normal gadget callback\n");
	return(true);
}

int main(int argc, char **argv)
{
	int						sy=0;
	std::vector<hitRect>	hrs;
	
	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKExample");
	wc=apc->mainWindow;

	multi=new LFSTK_ExpanderGadgetClass(wc,"",0,0,DIALOGWIDTH,GADGETHITE*3);
	multi->stretchX=false;
	multi->stretchY=false;
	multi->lockY=LOCKTOTOP;
	multi->lockX=LOCKTOCENTRE;
	multi->gadgetStretch=MOVE;

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

	multi=new LFSTK_ExpanderGadgetClass(wc,"",DIALOGMIDDLE-GADGETWIDTH*2,sy,GADGETWIDTH*4,GADGETHITE);
	multi->LFSTK_setMouseCallBack(NULL,mouseparentCB,NULL);
	multi->stretchX=true;
	multi->stretchY=false;
	multi->lockY=LOCKTOBOTTOM;
	multi->lockX=LOCKTOCENTRE;
	multi->gadgetStretch=STRETCH;

//vertical
//	hrs.push_back({0,0,GADGETWIDTH*2,GADGETHITE,NULL});
//	hrs.back().gadget=new LFSTK_buttonClass(wc,"Normal",0,0,10,10);
//	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,mouseCB,(void*)multi);
//
//	hrs.push_back({0,GADGETHITE*3,GADGETWIDTH*2,GADGETHITE,NULL});
//	hrs.back().gadget=new LFSTK_labelClass(wc,"Label Gadget",0,0,1,1);
//	hrs.back().gadget->toParent=true;
//
//	hrs.push_back({0,GADGETHITE*2,GADGETWIDTH*2,GADGETHITE,NULL});
//	hrs.back().gadget=new LFSTK_buttonClass(wc,"Button Gadget",0,0,1,1);
//	hrs.back().gadget->toParent=true;

//horizontal
	hrs.push_back({0,0,GADGETWIDTH,GADGETHITE,NULL});
	hrs.back().gadget=new LFSTK_buttonClass(wc,"Stretch",0,0,10,10);
	hrs.back().gadget->toParent=true;

	hrs.push_back({(GADGETWIDTH*2)-HALFGADGETWIDTH,0,GADGETWIDTH,GADGETHITE,NULL});
	hrs.back().gadget=new LFSTK_labelClass(wc,"LockToBottom",0,0,1,1);
	hrs.back().gadget->toParent=true;

	hrs.push_back({(GADGETWIDTH*3)-1,0,GADGETWIDTH,GADGETHITE,NULL});
	hrs.back().gadget=new LFSTK_buttonClass(wc,"Move",0,0,10,10);
	hrs.back().gadget->toParent=true;

//	hrs.push_back({(GADGETWIDTH*3)-1,0,GADGETWIDTH,GADGETHITE,NULL});
//	hrs.back().gadget=new LFSTK_buttonClass(wc,"Move2",0,0,1,1);
//	hrs.back().gadget->toParent=true;

	multi->LFSTK_setHitRects(hrs);

	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	fprintf(stderr,"%s\n",wc->globalLib->LFSTK_getGlobalString(-1,TYPEMONOFONT));
	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	delete apc;	
	cairo_debug_reset_static_data();
	return(retval);
}