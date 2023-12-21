#if 0
#(c)keithhedger Sat 5 Aug 19:18:08 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o calanderexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./calanderexample "$@"
retval=$?
echo "Exit code $retval"
rm calanderexample
exit $retval
#endif

#include <math.h>

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL "Simple Calendar"

struct datesStruct
{
	int			month;
	int			date;
	cairoColor	col;
};

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_buttonClass		*less=NULL;
LFSTK_buttonClass		*more=NULL;
LFSTK_multiLineEditClass	*editbox=NULL;
std::string				thismonth;
int						currentmonth;
int						holdThisMonth=0;
std::vector<std::string>	caldata;
int						dx=0;
int						dy=0;
int						dl=0;
cairoColor				dayhilite={1.0,0.0,1.0,0.06};

//xmas day boxing day new year my birthday midsummer midwinter walpurgis
datesStruct				mydates[]={{11,25,{0.0,0,1,0.05}},{11,26,{0.0,0,1,0.05}},{0,1,{0.0,0,1,0.05}},{6,2,{2.0,0,0,0.05}},{5,21,{0.0,1,0,0.05}},{11,21,{0.0,1,0,0.05}},{3,30,{0,0,0,0.05}},{9,31,{0,0,0,0.05}}};
std::vector<datesStruct>	datesdata;// (mydates, mydates + (sizeof(mydates) / sizeof(datesStruct)) );
const char				*monthNames[]={"January","February","March","April","May","June","July","August","September","October","November","December"};

void setImportantDates(void)
{
	std::string	thisday;

	caldata=apc->globalLib->LFSTK_runAndGet("%s %s","cal",thismonth.c_str());
//do important dates
	for(int j=0;j<datesdata.size();j++)
		{
			if((currentmonth==datesdata.at(j).month) )//&& (std::stoi(thisday)==datesdata.at(j).date)
				{
						thisday=std::to_string(datesdata.at(j).date);
						dl=thisday.length();
						thisday=" "+thisday+" ";
						std::string::size_type found;
						std::string adjstr;
						for(int i=1;i<caldata.size();i++)
							{
								adjstr=" "+caldata.at(i);
								adjstr.insert(adjstr.end()-1,' ');
								found=adjstr.find(thisday);
								if(found!=std::string::npos)
									{
										dy=i-1;
										dx=found;
										if(editbox!=NULL)
											editbox->LFSTK_addHighLights(dx,dy,dl,datesdata.at(j).col);
									}
							}
				}
		}
//	editbox->LFSTK_addHighLights(0,0,32,{1.0,0.0,0,0.1});
//	editbox->LFSTK_addHighLights(0,1,32,{0.0,1.0,0,0.1});
//	editbox->LFSTK_addHighLights(0,2,32,{1.0,0.0,0,0.1});
//	editbox->LFSTK_addHighLights(0,3,32,{0.0,1.0,0,0.1});
//	editbox->LFSTK_addHighLights(0,4,32,{1.0,0.0,0,0.1});
//	editbox->LFSTK_addHighLights(0,5,32,{0.0,1.0,0,0.1});
//	editbox->LFSTK_addHighLights(0,6,32,{1.0,0.0,0,0.1});
//	editbox->LFSTK_addHighLights(0,7,32,{0.0,1.0,0,0.1});
}

void setEditText(void)
{
	std::string	thisday;

	caldata=apc->globalLib->LFSTK_runAndGet("%s %s","cal",thismonth.c_str());
	if(holdThisMonth==currentmonth)
		{
			thisday=apc->globalLib->LFSTK_oneLiner(std::string("%s"),"date +%d");
			dl=thisday.length();
			thisday=" "+thisday+" ";
			std::string::size_type found;
			std::string adjstr;
			for(int j=1;j<caldata.size();j++)
				{
					adjstr=" "+caldata.at(j);
					adjstr.insert(adjstr.end()-1,' ');
					found=adjstr.find(thisday);
					if(found!=std::string::npos)
						{
							dy=j-1;
							dx=found;
							return;
						}
				}
		}
}

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool doPrev(void *p,void* ud)
{
	if(currentmonth==0)
		return(true);
	currentmonth--;	
	thismonth=monthNames[currentmonth];
	editbox->highLights.clear();
	setEditText();
	if(holdThisMonth==currentmonth)
		editbox->LFSTK_addHighLights(dx,dy,dl,dayhilite);
	setImportantDates();
	editbox->LFSTK_setBuffer(caldata.at(0).c_str());
	return(true);
}

bool doNext(void *p,void* ud)
{
	if(currentmonth==11)
		return(true);
	currentmonth++;	
	thismonth=monthNames[currentmonth];
	editbox->highLights.clear();
	setEditText();
	if(holdThisMonth==currentmonth)
		editbox->LFSTK_addHighLights(dx,dy,dl,dayhilite);
	setImportantDates();
	editbox->LFSTK_setBuffer(caldata.at(0).c_str());
	return(true);
}

int main(int argc, char **argv)
{
	int			txtwid;
	int			txthite;
	int			sy=BORDER;
	std::string	thisday;

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKExample");
	wc=apc->mainWindow;

	thisday=apc->globalLib->LFSTK_oneLiner(std::string("%s"),"date +%m");
	currentmonth=std::stoi(thisday)-1;
	thismonth=monthNames[currentmonth];

	for(int j=0;j<sizeof(mydates) / sizeof(datesStruct);j++)
		datesdata.push_back(mydates[j]);
	
	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//line edit
	holdThisMonth=currentmonth;
	setEditText();
	editbox=new LFSTK_multiLineEditClass(wc,caldata.at(0).c_str(),((DIALOGWIDTH-(BORDER+128))/2),sy,10000,10000,BUTTONGRAV);
	txtwid=editbox->LFSTK_getTextWidth(caldata.at(1).c_str())+1;
	txthite=editbox->totalTextHeight;
	editbox->LFSTK_moveGadget((DIALOGWIDTH/2)-(txtwid/2),sy);
	editbox->LFSTK_resizeWindow(txtwid+2,std::floor(txthite)+2);

	editbox->LFSTK_setEditable(false);
	editbox->LFSTK_setIgnores(false,false);
	editbox->LFSTK_addHighLights(dx,dy,dl,dayhilite);
	setImportantDates();
	editbox->LFSTK_setStyle(BEVELNONE);
//	editbox->gadgetDetails.gadgetGeom=editbox->gadgetGeom;
	sy+=YSPACING+txthite;

	less=new LFSTK_buttonClass(wc,"Last Month",BORDER,sy,GADGETWIDTH*2,GADGETHITE);
	less->LFSTK_setMouseCallBack(NULL,doPrev,NULL);

	more=new LFSTK_buttonClass(wc,"Next Month",DIALOGWIDTH-BORDER-GADGETWIDTH-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE);
	more->LFSTK_setMouseCallBack(NULL,doNext,NULL);
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

	wc->acceptOnThis=false;
	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	printf("%s\n",editbox->LFSTK_getCStr());
	delete apc;
	cairo_debug_reset_static_data();
	return(retval);
}
