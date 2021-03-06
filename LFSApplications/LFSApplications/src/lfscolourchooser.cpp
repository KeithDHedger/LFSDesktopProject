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
#include <getopt.h>

#include <lfstk/LFSTKGlobals.h>

#define BOXLABEL			"Colour Chooser"

LFSTK_applicationClass		*apc=NULL;
LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_scrollBarClass		*red=NULL;
LFSTK_scrollBarClass		*blue=NULL;
LFSTK_scrollBarClass		*green=NULL;
LFSTK_lineEditClass			*box=NULL;
LFSTK_lineEditClass			*colour=NULL;
LFSTK_toggleButtonClass		*check=NULL;
char						*colourname=NULL;
bool						lockStep=false;
int							lastred=255;
int							lastgreen=255;
int							lastblue=255;
int							parentWindow=-1;

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
				}
		}
	free(colourname);

	lastred=red->LFSTK_getValue();
	lastgreen=green->LFSTK_getValue();
	lastblue=blue->LFSTK_getValue();

	asprintf(&colourname,"#%02X%02X%02X",red->LFSTK_getValue(),green->LFSTK_getValue(),blue->LFSTK_getValue());
	box->LFSTK_setColourName(NORMALCOLOUR,colourname);
	box->LFSTK_setCursorColourName(colourname);
	box->LFSTK_clearWindow();
	colour->LFSTK_setBuffer(colourname);
	return(true);
}

void setSliders(const char *colour)
{
	colourStruct	colptr;
	colptr.name=NULL;

	wc->globalLib->LFSTK_setColourFromName(apc->display,apc->cm,&colptr,colour);
	red->LFSTK_setValue(colptr.RGBAColour.r*256,true);
	green->LFSTK_setValue(colptr.RGBAColour.g*256,true);
	blue->LFSTK_setValue(colptr.RGBAColour.b*256,true);
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
	apc->LFSTK_addWindow(NULL,BOXLABEL);
	wc=apc->mainWindow;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,"R  G  B",BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,LEFT);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	asprintf(&colourname,"#ffff00");
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

	red->LFSTK_setMouseCallBack(NULL,scrollCB,(void*)1);
	green->LFSTK_setMouseCallBack(NULL,scrollCB,(void*)2);
	blue->LFSTK_setMouseCallBack(NULL,scrollCB,(void*)3);

	box->LFSTK_setColourName(NORMALCOLOUR,colourname);
	box->LFSTK_setCursorColourName(colourname);
	box->LFSTK_setActive(false);//TODO//
	sy+=YSPACING;

	check=new LFSTK_toggleButtonClass(wc,"Lock sliders",3*BORDER+2*SCROLLBARWIDTH+GADGETWIDTH,sy+GADGETHITE*6-CHECKBOXSIZE,GADGETWIDTH,CHECKBOXSIZE,NorthWestGravity);
	check->LFSTK_setValue(false);

	colour=new LFSTK_lineEditClass(wc,"",3*BORDER+2*SCROLLBARWIDTH+GADGETWIDTH,sy+GADGETHITE*7,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);

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