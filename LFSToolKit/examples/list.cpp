#if 0

#©keithhedger Sat 5 Aug 16:52:45 BST 2017 kdhedger68713@gmail.com

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

#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"List Window"

LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_listGadgetClass		*list=NULL;
LFSTK_listGadgetClass		*filelist=NULL;

bool						mainLoop=true;
Display						*display;

const char	*lst[]={"item 1","item 2","item 3","item 4","item 5","item 6","7 food","8 water","9 attack","10 defense","11 666","12 999","13 10101010","14 ELP","15 last item",};
const char	*images[]={"./AspellGUI.png",NULL,NULL,NULL,"./audio-speakers.png",NULL,"./casper2.JPG","./computer.png","./computer.png","./computer.png","./computer.png","./computer.png","./computer.png","./audio-speakers.png","./ManPageEditor.png"};
listLabelStruct				**labelLst1=NULL;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

bool select(void *object,void* userdata)
{
	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);

	printf("List item=%i\n",list->LFSTK_getCurrentListItem());
	printf("Selected List item string=%s\n",list->LFSTK_getSelectedLabel());
	if(list->listDataArray->at(list->LFSTK_getCurrentListItem()).imageType==FILETHUMB)
		printf("Image path=%s\n",list->listDataArray->at(list->LFSTK_getCurrentListItem()).data.imagePath);
	printf("UserData=%p\n",list->listDataArray->at(list->LFSTK_getCurrentListItem()).userData);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
		
	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"List Example",false);
	display=wc->display;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//list
	list=new LFSTK_listGadgetClass(wc,"list",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE*5,BUTTONGRAV,NULL,0);
	listLabelStruct ls;
	for(int j=0;j<15;j++)
		{
			ls.label=strdup((char*)lst[j]);
			ls.imageType=FILETHUMB;
			if(images[j]!=NULL)
				ls.data.imagePath=strdup(images[j]);
			else
				ls.data.imagePath=NULL;
			ls.userData=(void*)(long)j+0x1000;
			list->LFSTK_appendToList(ls);
		}

	list->LFSTK_updateList();
	list->LFSTK_setMouseCallBack(NULL,select,NULL);
	sy+=GADGETHITE*6;

//file list
	filelist=new LFSTK_listGadgetClass(wc,"list",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE*16,BUTTONGRAV,NULL,0);
	filelist->LFSTK_setListFromFile("/tmp/biglist",false);
	filelist->LFSTK_setMouseCallBack(NULL,select,(void*)0xdeadbeaf);
//goto end
//	filelist->LFSTK_selectByIndex(1000000);
	if(filelist->LFSTK_findByLabel("zcat",false)==-1)
		printf("Not found\n");
	sy+=GADGETHITE*17;

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
	wc->LFSTK_showWindow();

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
		}

	delete wc;
	XCloseDisplay(display);
	cairo_debug_reset_static_data();
	return 0;
}