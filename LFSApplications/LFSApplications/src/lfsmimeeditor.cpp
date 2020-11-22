/*
 *
 * ©K. D. Hedger. Sun 22 Nov 18:38:48 GMT 2020 keithdhedger@gmail.com

 * This file (lfsmimeeditor.cpp) is part of LFSApplications.

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

#include <lfstk/LFSTKGlobals.h>

#define LISTHITE		GADGETHITE * 16

LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*cancel=NULL;
LFSTK_buttonClass		*apply=NULL;
LFSTK_buttonClass		*insert=NULL;
LFSTK_buttonClass		*delentry=NULL;
LFSTK_listGadgetClass	*list=NULL;
LFSTK_lineEditClass		*editline=NULL;

bool					mainLoop=true;
Display					*display;
char					*mimeTypesFile=NULL;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

bool doApply(void *p,void* ud)
{
	char	*command;

	asprintf(&command,"sed -i '%is@.*@%s@' \"%s\"",list->LFSTK_getCurrentListItem()+1,editline->LFSTK_getCStr(),mimeTypesFile);
	system(command);
	free(command);
	asprintf(&command,"awk -i inplace '/^$/ {print; next} {if ($1 in a) next; a[$1]=$0; print}' \"%s\"",mimeTypesFile);
	system(command);
	free(command);
	list->LFSTK_setListFromFile(mimeTypesFile,true);
	return(true);
}

bool doInsert(void *p,void* ud)
{
	char	*command;

	asprintf(&command,"sed -i '%iG' \"%s\"",list->LFSTK_getCurrentListItem()+1,mimeTypesFile);
	system(command);
	list->LFSTK_setListFromFile(mimeTypesFile,true);
	free(command);
	return(true);
}

bool doDelete(void *p,void* ud)
{
	char	*command;
	int holdnum=list->currentItem;

	if(holdnum>=list->listCnt)
		return(true);
	if(list->LFSTK_getSelectedLabel()[0]=='[')
		return(true);

	asprintf(&command,"sed -i '%id' \"%s\"",list->LFSTK_getCurrentListItem()+1,mimeTypesFile);
	printf(">>%s<<\n",command);
	system(command);
	list->LFSTK_setListFromFile(mimeTypesFile,true);
	list->currentItem=holdnum;
	free(command);
	return(true);
}

bool select(void *object,void* ud)
{
	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);

	if(list->LFSTK_getSelectedLabel()[0]=='[')
		return(true);
	editline->LFSTK_setBuffer(list->LFSTK_getSelectedLabel());
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
		
	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"Mime Type Editor",false);
	display=wc->display;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;
	
//mime type list
	list=list=new LFSTK_listGadgetClass(wc,"",BORDER,sy,DIALOGWIDTH-(BORDER*2)-LGAP,LISTHITE,NorthWestGravity,NULL,0);
	asprintf(&mimeTypesFile,"%s/.config/mimeapps.list",getenv("HOME"));

	list->LFSTK_setListFromFile(mimeTypesFile,true);
	list->LFSTK_setCallBack(NULL,select,NULL);
	sy+=LISTHITE+8;

//command
	editline=new LFSTK_lineEditClass(wc,"",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//cancel/quit
	cancel=new LFSTK_buttonClass(wc,"Cancel",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	cancel->LFSTK_setCallBack(NULL,doQuit,NULL);

	insert=new LFSTK_buttonClass(wc,"Insert",DIALOGMIDDLE-BORDER-GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	insert->LFSTK_setCallBack(NULL,doInsert,NULL);

	delentry=new LFSTK_buttonClass(wc,"Remove",DIALOGMIDDLE+BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	delentry->LFSTK_setCallBack(NULL,doDelete,NULL);

	apply=new LFSTK_buttonClass(wc,"Apply",DIALOGWIDTH-GADGETWIDTH-BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	apply->LFSTK_setCallBack(NULL,doApply,NULL);
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

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
	free(mimeTypesFile);
	return 0;
}