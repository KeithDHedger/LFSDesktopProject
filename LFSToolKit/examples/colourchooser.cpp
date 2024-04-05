#if 0

#(c)keithhedger Mon 7 Dec 11:04:17 GMT 2020 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

APPNAME=$(basename $0 .cpp)

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo  glib-2.0) -llfstoolkit -lImlib2 -o $APPNAME||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./$APPNAME "$@"

retval=$?
rm $APPNAME
exit $retval

#endif
#include <getopt.h>
#include <sstream>
#include <string>
#include <iomanip>

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Colour Chooser"

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_scrollBarClass		*red=NULL;
LFSTK_scrollBarClass		*blue=NULL;
LFSTK_scrollBarClass		*green=NULL;
LFSTK_scrollBarClass		*alpha=NULL;
LFSTK_lineEditClass		*box=NULL;
LFSTK_lineEditClass		*colour=NULL;
LFSTK_toggleButtonClass	*check=NULL;
bool						lockStep=false;
int						lastred=255;
int						lastgreen=255;
int						lastblue=255;
int						parentWindow=-1;
std::stringstream		colourname;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	if((long)ud==1)
		printf("%s",colour->LFSTK_getCStr());
	return(false);
}

#undef DIALOGWIDTH
#define DIALOGWIDTH 338

bool scrollCB(void *p,void* ud)
{
	long	what=(long)ud;
	int		diff=0;

	if(check->LFSTK_getValue()==true)
		{
			switch(what)
				{
					case 1:
						diff=red->LFSTK_getValue()-lastred;
						green->LFSTK_setValue(green->LFSTK_getValue()+diff,true);
						blue->LFSTK_setValue(blue->LFSTK_getValue()+diff,true);
						break;
					case 2:
						diff=green->LFSTK_getValue()-lastgreen;
						red->LFSTK_setValue(red->LFSTK_getValue()+diff,true);
						blue->LFSTK_setValue(blue->LFSTK_getValue()+diff,true);
						break;
					case 3:
						diff=blue->LFSTK_getValue()-lastblue;
						red->LFSTK_setValue(red->LFSTK_getValue()+diff,true);
						green->LFSTK_setValue(green->LFSTK_getValue()+diff,true);
						break;
					case 4:
						break;
				}
		}

	lastred=red->LFSTK_getValue();
	lastgreen=green->LFSTK_getValue();
	lastblue=blue->LFSTK_getValue();
	colourname.clear();
	colourname.str(std::string());
	colourname << "#"<<std::hex << std::uppercase<<std::setfill('0') << std::setw(2)<<alpha->LFSTK_getValue();
	colourname << std::hex << std::uppercase<<std::setfill('0') << std::setw(2)<<red->LFSTK_getValue();
	colourname << std::hex << std::uppercase<<std::setfill('0') << std::setw(2)<<green->LFSTK_getValue();
	colourname << std::hex << std::uppercase<<std::setfill('0') << std::setw(2)<< blue->LFSTK_getValue();
	box->LFSTK_setGadgetColourPair(NORMALCOLOUR,colourname.str(),"black");
	box->LFSTK_setCursorColourName(colourname.str().c_str());
	box->LFSTK_clearWindow();
	colour->LFSTK_setBuffer(colourname.str().c_str());
	return(true);
}

void setSliders(const char *colour)
{
	colourStruct		colptr;
	colptr=box->LFSTK_setColour(std::string(colour));

	red->LFSTK_setValue(colptr.RGBAColour.r*256,true);
	green->LFSTK_setValue(colptr.RGBAColour.g*256,true);
	blue->LFSTK_setValue(colptr.RGBAColour.b*256,true);
	alpha->LFSTK_setValue(colptr.RGBAColour.a*256,true);
}

bool lineCB(void *p,void *ud)
{
	setSliders(colour->LFSTK_getCStr());
	return(true);
}

int main(int argc, char **argv)
{
	XEvent		event;
	int			sy=BORDER;

	int			c=0;
	int			option_index=0;
	const char	*shortOpts="h?w:";
	option		longOptions[]=
		{
			{"window",1,0,'w'},
			{"help",0,0,'h'},
			{0, 0, 0, 0}
		};

	while(1)
		{
			option_index=0;
			c=getopt_long_only(argc,argv,shortOpts,longOptions,&option_index);
			if (c==-1)
				break;
			switch (c)
				{
					case 'h':
					case '?':
						printf("Usage:\nlfscolourchooser [ARG] - Where ARG is a colour definition either e.g. \"#ff0080\" or \"darkgreen\"\n");
						printf("-?,-h,--help\t\tPrint this help\n");
						printf("-w,--window\t\tSet transient for window\n");
						exit(0);
					case 'w':
						parentWindow=atoi(optarg);
						break;
				}
		}

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKExample");
	wc=apc->mainWindow;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,"R  G  B  A",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,LEFT);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	colourname<<std::endl;
	colourname<<"";
	box=new LFSTK_lineEditClass(wc,"",3*BORDER+2*SCROLLBARWIDTH+GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE*6,BUTTONGRAV);
	box->LFSTK_setIgnores(false,false);

	red=new LFSTK_scrollBarClass(wc,true,BORDER+2,sy,SCROLLBARWIDTH,200+GADGETHITE,BUTTONGRAV);
	red->LFSTK_setScale(0,255);
	red->LFSTK_setPageScroll(16);
	red->reverse=true;
	red->LFSTK_setValue(255);

	green=new LFSTK_scrollBarClass(wc,true,2*BORDER+SCROLLBARWIDTH+6,sy,SCROLLBARWIDTH,200+GADGETHITE,BUTTONGRAV);
	green->LFSTK_setScale(0,255);
	green->LFSTK_setPageScroll(16);
	green->reverse=true;
	green->LFSTK_setValue(255);

	blue=new LFSTK_scrollBarClass(wc,true,3*BORDER+2*SCROLLBARWIDTH+10,sy,SCROLLBARWIDTH,200+GADGETHITE,BUTTONGRAV);
	blue->LFSTK_setScale(0,255);
	blue->LFSTK_setPageScroll(16);
	blue->reverse=true;
	blue->LFSTK_setValue(255);

	alpha=new LFSTK_scrollBarClass(wc,true,4*BORDER+3*SCROLLBARWIDTH+14,sy,SCROLLBARWIDTH,200+GADGETHITE,BUTTONGRAV);
	alpha->LFSTK_setScale(0,255);
	alpha->LFSTK_setPageScroll(16);
	alpha->reverse=true;
	alpha->LFSTK_setValue(255);

	red->LFSTK_setMouseCallBack(NULL,scrollCB,(void*)1);
	green->LFSTK_setMouseCallBack(NULL,scrollCB,(void*)2);
	blue->LFSTK_setMouseCallBack(NULL,scrollCB,(void*)3);
	alpha->LFSTK_setMouseCallBack(NULL,scrollCB,(void*)4);

	box->LFSTK_setGadgetColourPair(NORMALCOLOUR,colourname.str(),"black");
	box->LFSTK_setCursorColourName(colourname.str().c_str());
	box->LFSTK_setActive(false);//TODO//
	sy+=YSPACING;

	check=new LFSTK_toggleButtonClass(wc,"Lock sliders",3*BORDER+2*SCROLLBARWIDTH+GADGETWIDTH,sy+GADGETHITE*6-CHECKBOXSIZE,GADGETWIDTH,CHECKBOXSIZE,NorthWestGravity);
	check->LFSTK_setValue(false);

	colour=new LFSTK_lineEditClass(wc,"FFFFFFFF",3*BORDER+2*SCROLLBARWIDTH+GADGETWIDTH,sy+GADGETHITE*7,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	colour->LFSTK_setKeyCallBack(NULL,lineCB,NULL);
	sy+=200;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"OK",(DIALOGWIDTH/2)-GADGETWIDTH*2,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,(void*)1);
	quit=new LFSTK_buttonClass(wc,"Cancel",(DIALOGWIDTH/2)+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,(void*)2);
	sy+=YSPACING;

	if(argv[optind]!=NULL)
		setSliders(argv[optind]);

	if(parentWindow!=-1)
		{
			wc->LFSTK_setKeepAbove(true);
			wc->LFSTK_setTransientFor(parentWindow);
		}
	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	int retval=apc->LFSTK_runApp();

	delete apc;
	return(retval);
}