/*
 *
 * ©K. D. Hedger. Sat  4 Dec 16:39:14 GMT 2021 keithdhedger@gmail.com

 * This file (lfsabout.cpp) is part of LFSApplications.

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
#include <lfstk/LFSTKGlobals.h>

#define WINDOWWIDTH		320
#define WINDOWHITE		10
#define WINDOWMIDDLE	WINDOWWIDTH/2
#define SPACING			24
#define HALFSPACING		SPACING/2
#define BOXLABEL		"LFS About"

enum {LNAME,LDESCRIPTION,LCOPYRITE,LEMAIL,LWEBSITE,LGPL,LNOMORELABELS};

const char	*labelTexts[]={"LFS Desktop Project","Low resource/dependency desktop for LFS",COPYRITE,PERSONAL,MYWEBSITE,"Released under GPLv3"};

#define IMAGESIZE		64

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_imageClass		*tux;
LFSTK_labelClass		*labels[LNOMORELABELS];

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

int main(int argc, char **argv)
{
	XEvent			event;
	int				sy=BORDER;
	int				dw=WINDOWWIDTH;
	windowInitStruct	*win;

	apc=new LFSTK_applicationClass();

	win=apc->LFSTK_getDefaultWInit();
	win->windowName=BOXLABEL;
	win->windowType=win->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DIALOG"));
	win->level=ABOVEALL;

	apc->LFSTK_addWindow(win,BOXLABEL,"LFSAbout");
	wc=apc->mainWindow;
	wc->LFSTK_setDecorations(false,false,false,true);
	
	tux=new LFSTK_imageClass(wc,NULL,WINDOWMIDDLE-(IMAGESIZE/2),sy,IMAGESIZE,IMAGESIZE,BUTTONGRAV,true);
	tux->LFSTK_setImageFromPath("/usr/share/pixmaps/LFSTux.png",AUTO,true);
	tux->LFSTK_clearWindow();
	sy+=SPACING*3;

	for(int j=LNAME;j<LNOMORELABELS;j++)
		{
			labels[j]=new LFSTK_labelClass(wc,labelTexts[j],0,sy,WINDOWWIDTH,GADGETHITE);
			sy+=SPACING;
		}
	labels[LNAME]->LFSTK_setCairoFontDataParts("sB",18);
	labels[LDESCRIPTION]->LFSTK_setCairoFontDataParts("s",14);

	for(int j=LNAME;j<LNOMORELABELS;j++)
		{
			if(labels[j]->LFSTK_getTextWidth(labelTexts[j])>dw)
				dw=labels[j]->LFSTK_getTextWidth(labelTexts[j])+(BORDER);
		}

	for(int j=LNAME;j<LNOMORELABELS;j++)
		labels[j]->LFSTK_resizeWindow(dw,GADGETHITE);

	tux->LFSTK_moveGadget((dw/2)-(IMAGESIZE/2),BORDER);

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,dw,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=SPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Close",(dw/2)-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	sy+=SPACING+16;

	wc->LFSTK_resizeWindow(dw,sy,true);
	int retval=apc->LFSTK_runApp();

	delete apc;
	return(retval);
}