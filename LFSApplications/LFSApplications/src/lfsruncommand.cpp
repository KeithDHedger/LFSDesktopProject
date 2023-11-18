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
#include <lfstk/LFSTKGlobals.h>

#undef DIALOGWIDTH
#define DIALOGWIDTH		800
#define LISTHITE		GADGETHITE * 8
#define MAXHISTORY		60

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_listGadgetClass	*list=NULL;
LFSTK_lineEditClass		*le=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;

const char				*commandToRun=NULL;
char					*commandfile;
int						maxHistory=MAXHISTORY;

bool doQuit(void *p,void* ud)
{
	apc->mainLoop=false;
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
			asprintf(&data,"tail -n %i %s|sort -u -o %s.tmp; mv %s.tmp %s",maxHistory,commandfile,commandfile,commandfile,commandfile);
			system(data);
		}
	free(data);
	apc->mainLoop=(bool)ud;
	return(true);
}

bool select(void *object,void* ud)
{
	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);

	commandToRun=list->LFSTK_getSelectedLabel();
	le->LFSTK_setBuffer(commandToRun);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent				event;
	int 				sy=0;
	LFSTK_buttonClass	*quit;
	LFSTK_buttonClass	*execute;
	LFSTK_buttonClass	*run;
	char				*thist=NULL;

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,"Run Application");
	wc=apc->mainWindow;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

	list=list=new LFSTK_listGadgetClass(wc,"",BORDER,sy,DIALOGWIDTH-(BORDER*2)-LGAP,LISTHITE);

	thist=wc->globalLib->LFSTK_oneLiner("head -n1 '%s/%s'",apc->configDir.c_str(),"command.max");

	maxHistory=MAXHISTORY;
	if((thist!=NULL) && (thist[0]!=0))
		maxHistory=atoi(thist);
	free(thist);
	thist=getenv("MAXLFSRUNHIST");
	if(thist!=NULL)
		maxHistory=atoi(thist);
	if(argv[1]!=NULL)
		maxHistory=atoi(argv[1]);

	asprintf(&commandfile,"%s/%s",apc->configDir.c_str(),"command.hist");
	list->LFSTK_setListFromFile(commandfile,false);
	list->LFSTK_setMouseCallBack(NULL,select,NULL);
	sy+=LISTHITE+8;

//command
	le=new LFSTK_lineEditClass(wc,"",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE);
	sy+=GADGETHITE+8;
	quit=new LFSTK_buttonClass(wc,"Quit",BORDER,sy,GADGETWIDTH,GADGETHITE);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);

	execute=new LFSTK_buttonClass(wc,"Run",BORDER+((DIALOGWIDTH-(BORDER*2))/2)-(GADGETWIDTH/2),sy,GADGETWIDTH,GADGETHITE);
	execute->LFSTK_setMouseCallBack(NULL,doExecute,(void*)true);

	run=new LFSTK_buttonClass(wc,"Run And Quit",DIALOGWIDTH-BORDER-GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE);
	run->LFSTK_setMouseCallBack(NULL,doExecute,(void*)false);
	sy+=GADGETHITE+BORDER;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

	int retval=apc->LFSTK_runApp();

	free(commandfile);
	delete apc;
	return(retval);
}
