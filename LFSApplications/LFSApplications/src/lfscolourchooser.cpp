/*
 *
 * ©K. D. Hedger. Tue  8 Dec 14:14:46 GMT 2020 keithdhedger@gmail.com

 * This file (lfscolourchooser.cpp) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <sstream>
#include <string>
#include <iomanip>

#include <lfstk/LFSTKGlobals.h>

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
LFSTK_toggleButtonClass	*includeAlpha=NULL;

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
	long		what=(long)ud;
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
	if(includeAlpha->LFSTK_getValue()==true)
		colourname << "#"<<std::hex << std::uppercase<<std::setfill('0') << std::setw(2)<<alpha->LFSTK_getValue();
	else
		colourname << "#";
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

bool doAlpha(void *p,void *ud)
{
	alpha->LFSTK_setActive(includeAlpha->LFSTK_getValue());	
	alpha->LFSTK_clearWindow();
	return(true);
}

int main(int argc, char **argv)
{
	XEvent			event;
	int				sy=BORDER;
	LFSTK_prefsClass	cliprefs("lfspanelprefs",VERSION);
	option			longOptions[]={{"window",1,0,'w'},{0, 0, 0, 0}};

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL);
	wc=apc->mainWindow;
	
	cliprefs.prefsMap=
		{
			{LFSTK_UtilityClass::LFSTK_hashFromKey("window"),{TYPEINT,"window","Set transient for window ARG","",false,0}}
		};
	if(cliprefs.LFSTK_argsToPrefs(argc,argv,longOptions,true)==false)
		return(1);
	parentWindow=cliprefs.LFSTK_getInt("window");

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

	includeAlpha=new LFSTK_toggleButtonClass(wc,"Include Alpha",3*BORDER+2*SCROLLBARWIDTH+GADGETWIDTH+GADGETWIDTH,sy+GADGETHITE*6-CHECKBOXSIZE,GADGETWIDTH,CHECKBOXSIZE,NorthWestGravity);
	includeAlpha->LFSTK_setValue(true);
	includeAlpha->LFSTK_setMouseCallBack(NULL,doAlpha,NULL);

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