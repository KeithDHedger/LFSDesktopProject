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

const char	*lst2[]={"0002.jpg","108540.jpg","108577.jpg","162192-stained trees.jpg","17282.jpg","173367.jpg","176971.jpg","186301.jpg","2000 AD.jpg","307494.jpg","311514.jpg","311917.jpg","312592.jpg","314441.jpg","314447.jpg","322616.jpg","547283.jpg","552086.jpg","5532792-country-wallpaper.jpg","72177-gnome_xmas_1280.jpg","a1.jpg","a1.JPG","ACHR1046-christmas-eve_900px.jpg","Ankor1BW.JPG","Ankor2BW.JPG","ankorold2.JPG","ankorold.JPG","background1.jpg","background2.jpg","background3.jpg","background5.jpg","backgroundred.jpg","Bala_Lake.jpg","Between 2 Worlds.jpg","black_mosaic_by_seikq.jpg","BW stained trees.jpg","CaptainAmerica_1_CoverVariantMeindering.jpg","captain-america-2-winter-soldier-concept-art1.jpg","dsc_0001.jpg","DSC_0004.JPG","DSC_0096.JPG","DSC_0287.JPG","DSC_0303.JPG","dsc_0309.jpg","dsc_0309.JPG","DSC_0592.JPG","DSC_0612.JPG","f490278553.jpg","f490278769.jpg","f490279073.jpg","f490330137.jpg","f490331017.jpg","f490331465.jpg","f490331761.jpg","f490331897.jpg","f490332177.jpg","f490332393.jpg","f490332681.jpg","f490373121.jpg","f490405889.jpg","f490406121.jpg","f490441097.jpg","f490441609.jpg","f490455041.jpg","FearBrown.jpg","fearwallbrown.jpg","fearwall.jpg","fog_by_nucuBW.jpg","fog_by_nucu.jpg","fogyellow.jpg","free-autumn-desktop-wallpaper-8.jpg","Free-Fall-Desktop-Wallpaper.jpg","gothic-graveyard.jpg","groo2k-1103277364.jpg","halloween-graveyard.gif","haunting.gif","House of the Long Shadows.jpg","Imagine II 1440X900 By Sakasa.jpg","IMG_2558.JPG","ispy.jpg","katehhouse2000a.gif","myfear.png","NautilusBackground2.png","nmsmith-1165244098.jpg","paintintherain.jpg","Parquet_widescreen.jpg","pat.png","red.jpg","scratchborn.jpg","stockvault-heilstatten-g113042.jpg","t1.JPG","thumb-350-288861.jpg","visitors.jpg","vladstudio_christmas_santa_flying_2_1280x1024.jpg","vladstudio_christmas_train_1280x1024.jpg","wheretheheartis.jpg","wide_linux_autumn_desktop_autumn_linux_background-other.jpg","wood125.jpg","wood125.JPG","xmas1.jpg","xmas2.jpg","xmas3.jpg","xmas4.jpg","xmas6.jpg"};

const char	*images2[]={"/home/keithhedger/Wallpapers/0002.jpg","/home/keithhedger/Wallpapers/108540.jpg","/home/keithhedger/Wallpapers/108577.jpg","/home/keithhedger/Wallpapers/162192-stained trees.jpg","/home/keithhedger/Wallpapers/17282.jpg","/home/keithhedger/Wallpapers/173367.jpg","/home/keithhedger/Wallpapers/176971.jpg","/home/keithhedger/Wallpapers/186301.jpg","/home/keithhedger/Wallpapers/2000 AD.jpg","/home/keithhedger/Wallpapers/307494.jpg","/home/keithhedger/Wallpapers/311514.jpg","/home/keithhedger/Wallpapers/311917.jpg","/home/keithhedger/Wallpapers/312592.jpg","/home/keithhedger/Wallpapers/314441.jpg","/home/keithhedger/Wallpapers/314447.jpg","/home/keithhedger/Wallpapers/322616.jpg","/home/keithhedger/Wallpapers/547283.jpg","/home/keithhedger/Wallpapers/552086.jpg","/home/keithhedger/Wallpapers/5532792-country-wallpaper.jpg","/home/keithhedger/Wallpapers/72177-gnome_xmas_1280.jpg","/home/keithhedger/Wallpapers/a1.jpg","/home/keithhedger/Wallpapers/a1.JPG","/home/keithhedger/Wallpapers/ACHR1046-christmas-eve_900px.jpg","/home/keithhedger/Wallpapers/Ankor1BW.JPG","/home/keithhedger/Wallpapers/Ankor2BW.JPG","/home/keithhedger/Wallpapers/ankorold2.JPG","/home/keithhedger/Wallpapers/ankorold.JPG","/home/keithhedger/Wallpapers/background1.jpg","/home/keithhedger/Wallpapers/background2.jpg","/home/keithhedger/Wallpapers/background3.jpg","/home/keithhedger/Wallpapers/background5.jpg","/home/keithhedger/Wallpapers/backgroundred.jpg","/home/keithhedger/Wallpapers/Bala_Lake.jpg","/home/keithhedger/Wallpapers/Between 2 Worlds.jpg","/home/keithhedger/Wallpapers/black_mosaic_by_seikq.jpg","/home/keithhedger/Wallpapers/BW stained trees.jpg","/home/keithhedger/Wallpapers/CaptainAmerica_1_CoverVariantMeindering.jpg","/home/keithhedger/Wallpapers/captain-america-2-winter-soldier-concept-art1.jpg","/home/keithhedger/Wallpapers/dsc_0001.jpg","/home/keithhedger/Wallpapers/DSC_0004.JPG","/home/keithhedger/Wallpapers/DSC_0096.JPG","/home/keithhedger/Wallpapers/DSC_0287.JPG","/home/keithhedger/Wallpapers/DSC_0303.JPG","/home/keithhedger/Wallpapers/dsc_0309.jpg","/home/keithhedger/Wallpapers/dsc_0309.JPG","/home/keithhedger/Wallpapers/DSC_0592.JPG","/home/keithhedger/Wallpapers/DSC_0612.JPG","/home/keithhedger/Wallpapers/f490278553.jpg","/home/keithhedger/Wallpapers/f490278769.jpg","/home/keithhedger/Wallpapers/f490279073.jpg","/home/keithhedger/Wallpapers/f490330137.jpg","/home/keithhedger/Wallpapers/f490331017.jpg","/home/keithhedger/Wallpapers/f490331465.jpg","/home/keithhedger/Wallpapers/f490331761.jpg","/home/keithhedger/Wallpapers/f490331897.jpg","/home/keithhedger/Wallpapers/f490332177.jpg","/home/keithhedger/Wallpapers/f490332393.jpg","/home/keithhedger/Wallpapers/f490332681.jpg","/home/keithhedger/Wallpapers/f490373121.jpg","/home/keithhedger/Wallpapers/f490405889.jpg","/home/keithhedger/Wallpapers/f490406121.jpg","/home/keithhedger/Wallpapers/f490441097.jpg","/home/keithhedger/Wallpapers/f490441609.jpg","/home/keithhedger/Wallpapers/f490455041.jpg","/home/keithhedger/Wallpapers/FearBrown.jpg","/home/keithhedger/Wallpapers/fearwallbrown.jpg","/home/keithhedger/Wallpapers/fearwall.jpg","/home/keithhedger/Wallpapers/fog_by_nucuBW.jpg","/home/keithhedger/Wallpapers/fog_by_nucu.jpg","/home/keithhedger/Wallpapers/fogyellow.jpg","/home/keithhedger/Wallpapers/free-autumn-desktop-wallpaper-8.jpg","/home/keithhedger/Wallpapers/Free-Fall-Desktop-Wallpaper.jpg","/home/keithhedger/Wallpapers/gothic-graveyard.jpg","/home/keithhedger/Wallpapers/groo2k-1103277364.jpg","/home/keithhedger/Wallpapers/halloween-graveyard.gif","/home/keithhedger/Wallpapers/haunting.gif","/home/keithhedger/Wallpapers/House of the Long Shadows.jpg","/home/keithhedger/Wallpapers/Imagine II 1440X900 By Sakasa.jpg","/home/keithhedger/Wallpapers/IMG_2558.JPG","/home/keithhedger/Wallpapers/ispy.jpg","/home/keithhedger/Wallpapers/katehhouse2000a.gif","/home/keithhedger/Wallpapers/myfear.png","/home/keithhedger/Wallpapers/NautilusBackground2.png","/home/keithhedger/Wallpapers/nmsmith-1165244098.jpg","/home/keithhedger/Wallpapers/paintintherain.jpg","/home/keithhedger/Wallpapers/Parquet_widescreen.jpg","/home/keithhedger/Wallpapers/pat.png","/home/keithhedger/Wallpapers/red.jpg","/home/keithhedger/Wallpapers/scratchborn.jpg","/home/keithhedger/Wallpapers/stockvault-heilstatten-g113042.jpg","/home/keithhedger/Wallpapers/t1.JPG","/home/keithhedger/Wallpapers/thumb-350-288861.jpg","/home/keithhedger/Wallpapers/visitors.jpg","/home/keithhedger/Wallpapers/vladstudio_christmas_santa_flying_2_1280x1024.jpg","/home/keithhedger/Wallpapers/vladstudio_christmas_train_1280x1024.jpg","/home/keithhedger/Wallpapers/wheretheheartis.jpg","/home/keithhedger/Wallpapers/wide_linux_autumn_desktop_autumn_linux_background-other.jpg","/home/keithhedger/Wallpapers/wood125.jpg","/home/keithhedger/Wallpapers/wood125.JPG","/home/keithhedger/Wallpapers/xmas1.jpg","/home/keithhedger/Wallpapers/xmas2.jpg","/home/keithhedger/Wallpapers/xmas3.jpg","/home/keithhedger/Wallpapers/xmas4.jpg","/home/keithhedger/Wallpapers/xmas6.jpg"};




bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

bool select(void *object,void* ud)
{
	printf("List item=%i\n",static_cast<LFSTK_listGadgetClass*>(object)->LFSTK_getCurrentListItem());
	printf("List item 2 string=%s\n",static_cast<LFSTK_listGadgetClass*>(object)->LFSTK_getListString(2));
	printf("Selected List item string=%s\n",static_cast<LFSTK_listGadgetClass*>(object)->LFSTK_getListString(-1));
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
	list=new LFSTK_listGadgetClass(wc,"list",BORDER,sy,DIALOGWIDTH-(BORDER*3),GADGETHITE*5,BUTTONGRAV,NULL,0);
	list->LFSTK_setImageList((char**)&images,15);
	list->LFSTK_setList((char**)&lst,15);
	list->LFSTK_setCallBack(NULL,select,NULL);
	sy+=GADGETHITE*6;

//file list
	filelist=new LFSTK_listGadgetClass(wc,"list",BORDER,sy,DIALOGWIDTH-(BORDER*3),GADGETHITE*5,BUTTONGRAV,NULL,0);
	//filelist->LFSTK_setListFromFile("/etc/fstab",false);
	filelist->LFSTK_setListFromFile("/tmp/biglist",false);
	//filelist->LFSTK_setImageList((char**)&images2,104);
	//filelist->LFSTK_setList((char**)&lst2,104);
//	filelist->LFSTK_setListFromFile("/home/keithhedger/Wallpapers",false);
	filelist->LFSTK_setCallBack(NULL,select,NULL);

	sy+=GADGETHITE*6;

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