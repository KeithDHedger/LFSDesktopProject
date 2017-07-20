/*
 *
 * ©K. D. Hedger. Thu 20 Jul 13:19:52 BST 2017 kdhedger68713@gmail.com

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

#include <stdio.h>
#include <unistd.h>

#include <lfstk/LFSTKGlobals.h>

#define		WINWIDTH 800
#define		WINHITE 600
#define		ICONBUTTON 48
#define		MIDWIN WINWIDTH/2
#define		BUTTONWITDH 64
#define		BUTTONHITE 24
#define		YSPACING BUTTONHITE
#define		BORDER 8
#define		LISTHITE BUTTONHITE * 8
#define		MAXHISTORY "20"

#define		MYEMAIL "keithhedger@keithhedger.darktech.org"
#define		MYWEBSITE "http://keithhedger.darktech.org/"

LFSTK_windowClass		*wc=NULL;
LFSTK_listGadgetClass	*list=NULL;
LFSTK_lineEditClass		*le=NULL;

const char				*commandToRun=NULL;
	char				*commandfile;

bool					mainLoop=true;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	return(false);
}

bool doExecute(void *object,void* ud)
{
	char	*data;

	system(le->LFSTK_getBuffer()->c_str());
	mainLoop=false;

	asprintf(&data,"echo %s >> %s",le->LFSTK_getBuffer()->c_str(),commandfile);
	system(data);
	free(data);
	asprintf(&data,"tail -n " MAXHISTORY " %s|sort -u -o %s.tmp; mv %s.tmp %s",commandfile,commandfile,commandfile,commandfile);
	system(data);
	free(data);
	return(true);
}


bool select(void *object,void* ud)
{
	commandToRun=static_cast<LFSTK_listGadgetClass*>(object)->LFSTK_getListString(static_cast<LFSTK_listGadgetClass*>(object)->LFSTK_getCurrentListItem());
	le->LFSTK_setBuffer(commandToRun);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent				event;

	LFSTK_windowClass	*wc;
	LFSTK_buttonClass	*quit;
	LFSTK_buttonClass	*run;
	LFSTK_labelClass	*label;

	int 				sy=BORDER;

	wc=new LFSTK_windowClass(0,0,WINWIDTH,WINHITE,"About LFS Desktop",false);
	list=list=new LFSTK_listGadgetClass(wc,"list",BORDER,sy,WINWIDTH-32,LISTHITE,NorthWestGravity,NULL,0);

	asprintf(&commandfile,"%s/%s",wc->configDir,"command.hist");
	list->LFSTK_setListFromFile(commandfile,false);
	list->LFSTK_setCallBack(NULL,select,NULL);
	sy+=LISTHITE+8;

//command
	le=new LFSTK_lineEditClass(wc,list->LFSTK_getListString(list->LFSTK_getCurrentListItem()),BORDER,sy,WINWIDTH-BORDER,BUTTONHITE,NorthWestGravity);
	sy+=BUTTONHITE+8;

	quit=new LFSTK_buttonClass(wc,"Quit",BORDER,sy,BUTTONWITDH,BUTTONHITE,NorthWestGravity);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);

	run=new LFSTK_buttonClass(wc,"Execute",WINWIDTH-BORDER-BUTTONWITDH,sy,BUTTONWITDH,BUTTONHITE,NorthEastGravity);
	run->LFSTK_setCallBack(NULL,doExecute,NULL);
	sy+=BUTTONHITE+BORDER;

	wc->LFSTK_resizeWindow(WINWIDTH,sy);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);
			switch(event.type)
				{
					case LeaveNotify:
						break;
					case Expose:
						wc->LFSTK_clearWindow();
						list->LFSTK_clearWindow();
						break;

					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						list->LFSTK_clearWindow();
						break;

					case ClientMessage:
						if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
							mainLoop=false;
				}
		}

	free(commandfile);
	delete wc;
	
	return 0;
}