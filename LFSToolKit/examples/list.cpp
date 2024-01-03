#if 0

#(c)keithhedger Sat 5 Aug 16:52:45 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

if [ ! -e /tmp/biglist ];then
	ls -1 /usr/bin|sort >/tmp/biglist
fi

#ls -1 /home/keithhedger/Wallpapers |sort>/tmp/biglist

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o listexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./listexample "$@"
retval=$?
echo "Exit code $retval"
rm listexample
exit $retval

#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"List Window"

LFSTK_applicationClass		*apc=NULL;
LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_listGadgetClass		*list=NULL;
LFSTK_listGadgetClass		*filelist=NULL;
LFSTK_ExpanderGadgetClass	*multi=NULL;

const char	*lst[]={"item 1","item 2","item 3","item 4","item 5","item 6","7 food","8 water","9 attack","10 defense","11 666","12 999","13 10101010","14 ELP","15 last item",};
const char	*images[]={"./AspellGUI.png",NULL,NULL,NULL,"./audio-speakers.png",NULL,"./casper2.JPG","./computer.png","./computer.png","./computer.png","./computer.png","./computer.png","./computer.png","./audio-speakers.png","./ManPageEditor.png"};
listLabelStruct				**labelLst1=NULL;

bool doQuit(void *p,void* ud)
{
	apc->mainLoop=false;
	return(false);
}

bool select(void *object,void* userdata)
{
	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);
	printf("List item=%i\n",list->LFSTK_getCurrentListItem());
	printf("Selected List item string=%s\n",list->LFSTK_getSelectedLabel());
	if(list->listDataArray->at(list->LFSTK_getCurrentListItem()).imageType==FILETHUMB)
		printf("Image path=%s\n",list->listDataArray->at(list->LFSTK_getCurrentListItem()).imagePath.c_str());
	printf("UserData=%p\n",list->listDataArray->at(list->LFSTK_getCurrentListItem()).userData);
	return(true);
}

int main(int argc, char **argv)
{
	int						sy=0;
	std::vector<hitRect>		hrs;

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

	multi=new LFSTK_ExpanderGadgetClass(wc,"",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE*5);
	multi->stretchX=true;
	multi->stretchY=true;
	multi->lockY=LOCKTOTOP;
	multi->lockX=LOCKTOCENTRE;
	multi->gadgetStretch=STRETCH;

//list
	list=new LFSTK_listGadgetClass(wc,"list",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE*5);
	list->LFSTK_setListItemsColours(GADGETBG,"darkgray","red","blue","green");
	list->LFSTK_setListItemsColours(GADGETFG,"black","white","white","black");
	list->LFSTK_setGadgetColourPair(NORMALCOLOUR,"darkgray","black");

	hrs.push_back({0,0,DIALOGWIDTH-(BORDER*2),GADGETHITE*5,NULL});
	hrs.back().gadget=list;
	hrs.back().gadget->toParent=true;
	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

	listLabelStruct ls;
	for(int j=0;j<15;j++)
		{
			ls.label=lst[j];
			ls.imageType=FILETHUMB;
			//ls.imageType=NOTHUMB;
			if(images[j]!=NULL)
				ls.imagePath=images[j];
			else
				ls.imagePath="";
			ls.userData=USERDATA(j+0x1000);
			list->LFSTK_appendToList(ls);
		}

	list->LFSTK_updateList();
	list->LFSTK_setMouseCallBack(NULL,select,NULL);
	fprintf(stderr,"Max list item width=%i\n",list->LFSTK_getListMaxWidth());

	sy+=GADGETHITE*6;
//TODO//
//file list
	filelist=new LFSTK_listGadgetClass(wc,"list",0-(DIALOGWIDTH/2)+BORDER,0-(sy+GADGETHITE*10),DIALOGWIDTH-(BORDER*2),GADGETHITE*16,SouthGravity);
	filelist->LFSTK_setListFromFile("/tmp/biglist",false);
	filelist->LFSTK_setMouseCallBack(NULL,select,USERDATA(0xdeadbeaf));
//goto end
	if(filelist->LFSTK_findByLabel("zcat",false)==-1)
		printf("Not found\n");
	fprintf(stderr,"Max list item width=%i\n",filelist->LFSTK_getListMaxWidth());
	sy+=GADGETHITE*17;

//line
	multi=new LFSTK_ExpanderGadgetClass(wc,"",0,sy,DIALOGWIDTH,6);
	multi->stretchX=true;
	multi->lockY=LOCKTOBOTTOM;
	multi->gadgetStretch=STRETCH;

	hrs.push_back({0,0,DIALOGWIDTH,2,new LFSTK_buttonClass(wc,"--",0,0,DIALOGWIDTH,2)});
	hrs.back().gadget->gadgetDetails.bevel=BEVELNONE;
	hrs.back().gadget->gadgetDetails.buttonTile=false;

	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

//quit
	multi=new LFSTK_ExpanderGadgetClass(wc,"",DIALOGMIDDLE-HALFGADGETWIDTH,sy+12,GADGETWIDTH,GADGETHITE);
	multi->lockY=LOCKTOBOTTOM;

	hrs.push_back({0,0,GADGETWIDTH,GADGETHITE,new LFSTK_buttonClass(wc,"Quit",0,0,1,1)});
	hrs.back().gadget->LFSTK_setMouseCallBack(NULL,doQuit,NULL);

	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

	sy+=YSPACING;
	sy+=YSPACING/2;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}