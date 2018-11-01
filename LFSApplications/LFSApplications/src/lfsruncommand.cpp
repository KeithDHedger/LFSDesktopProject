/*
 *
 * ©K. D. Hedger. Thu 24 Aug 13:29:34 BST 2017 keithdhedger@gmail.com

 * This file (lfsruncommand.cpp) is part of LFSApplications.

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
#include "lfstk/LFSTKGlobals.h"

#undef DIALOGWIDTH
#define DIALOGWIDTH		800
#define LISTHITE		GADGETHITE * 8
#define MAXHISTORY		40

LFSTK_windowClass		*wc=NULL;
LFSTK_listGadgetClass	*list=NULL;
LFSTK_lineEditClass		*le=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;

const char				*commandToRun=NULL;
char					*commandfile;

bool					mainLoop=true;
Display					*display;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	return(false);
}

bool doExecute(void *object,void* ud)
{
	char	*data;

	asprintf(&data,"%s &",le->LFSTK_getCStr());
	if(strlen(data)>2)
		{
			system(data);
			free(data);

			asprintf(&data,"echo \"%s\" >> %s",le->LFSTK_getCStr(),commandfile);
			system(data);
			free(data);
			asprintf(&data,"tail -n %i %s|sort -u -o %s.tmp; mv %s.tmp %s",MAXHISTORY,commandfile,commandfile,commandfile,commandfile);
			system(data);
		}
	free(data);
	mainLoop=false;
	return(true);
}

bool select(void *object,void* ud)
{
	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);

	commandToRun=list->labelData[list->LFSTK_getCurrentListItem()]->label;
	le->LFSTK_setBuffer(commandToRun);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent				event;
	int 				sy=0;
	LFSTK_windowClass	*wc;
	LFSTK_buttonClass	*quit;
	LFSTK_buttonClass	*run;
	LFSTK_labelClass	*label;

	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"Run Command",false);
	display=wc->display;
	//wc->LFSTK_initDnD();

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

	list=list=new LFSTK_listGadgetClass(wc,"",BORDER,sy,DIALOGWIDTH-(BORDER*2)-LGAP,LISTHITE,NorthWestGravity,NULL,0);

	asprintf(&commandfile,"%s/%s",wc->configDir,"command.hist");
	list->LFSTK_setListFromFile(commandfile,false);
	list->LFSTK_setCallBack(NULL,select,NULL);
	sy+=LISTHITE+8;

//command
	le=new LFSTK_lineEditClass(wc,"",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE,BUTTONGRAV);
	sy+=GADGETHITE+8;
//le->labelGravity=CENTRE;
	quit=new LFSTK_buttonClass(wc,"Quit",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);

	run=new LFSTK_buttonClass(wc,"Execute",DIALOGWIDTH-BORDER-GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,NorthEastGravity);
	run->LFSTK_setCallBack(NULL,doExecute,NULL);
	sy+=GADGETHITE+BORDER;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

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

	free(commandfile);
	delete wc;
	XCloseDisplay(display);
	
	return 0;
}
