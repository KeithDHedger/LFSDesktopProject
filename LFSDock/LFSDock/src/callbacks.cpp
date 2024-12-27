/*
 *
 * ©K. D. Hedger. Sun 19 Nov 19:09:02 GMT 2023 keithdhedger@gmail.com

 * This file (callbacks.cpp) is part of LFSDock.

 * LFSDock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSDock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDock.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "globals.h"

LFSTK_windowClass	*taskContextWindow=NULL;
int					root_x_return=-1,root_y_return=-1;
int					rxdiff=-1;
int					rydiff=-1;
int					diFF=ICONSPACE-1;

bool exitPopList(LFSTK_gadgetClass*p,void* ud)
{
	Window				dump;
	int					rdump;
	int					win_x_return1=-1;
	int					win_y_return1=-1;
	unsigned int			mdump;
	const geometryStruct	*geom;
	pointStruct			pt;

	XQueryPointer(apc->display,apc->rootWindow,&dump,&dump,&rdump,&rdump,&win_x_return1,&win_y_return1,&mdump);

	geom=popActionWindow->LFSTK_getWindowGeom();
	pt={win_x_return1,win_y_return1};

	if(apc->globalLib->LFSTK_pointInRect(&pt,(geometryStruct*)geom)==false)
		{
			showhidetActionList(NULL,popActionWindow,popActionList);
			for(int j=0;j<tasks.size();j++)
				{
					if(moveGadget==true)
							setGadgetPosition(taskbuttons[j],false);
				}
			dockWindow->LFSTK_redrawAllGadgets();
		}
	return(true);
}

void sendClientMessage(Window win,const char *msg,unsigned long data0,unsigned long data1,unsigned long data2,unsigned long data3,unsigned long data4)
{
	XEvent	event;
	long		mask=SubstructureRedirectMask|SubstructureNotifyMask;

	event.xclient.type=ClientMessage;
	event.xclient.serial=0;
	event.xclient.send_event=True;
	event.xclient.message_type=XInternAtom(dockWindow->app->display,msg,False);
	event.xclient.window=win;
	event.xclient.format=32;
	event.xclient.data.l[0]=data0;
	event.xclient.data.l[1]=data1;
	event.xclient.data.l[2]=data2;
	event.xclient.data.l[3]=data3;
	event.xclient.data.l[4]=data4;
	XSendEvent(dockWindow->app->display,dockWindow->app->rootWindow,False,mask,&event);
}

bool gadgetDrop(void *lwc,propertyStruct *data,void* ud)
{
	if(data!=NULL)
		{
			if(strcasecmp(data->mimeType,"text/uri-list")==0)
				{
					dropDesktopFile((const char*)data->data,ud);
					return(true);
				}
		}
	return(true);
}

bool timerCB(LFSTK_applicationClass *p,void* ud)
{
//	if(dockIsHidden==false)
//		return(true);

	readMsg();

	if(clockButton!=NULL)
		updateClock();

	if(scwindow!=NULL)
		updateSlider();

	if((popActionWindow!=NULL) && (popActionWindow->isVisible==true) && ((inSomeWindow==false) && (popActionWindow->inWindow==false)))
		exitPopList(NULL,NULL);

	if(useTaskBar==true)
		updateTaskBar();

	if(switchButton!=NULL)
		updateDeskSwitcher();

	return(true);
}

void readMsg(void)
{
	int		retcode;

	buffer.mText[0]=0;
	retcode=msgrcv(queueID,&buffer,MAX_MSG_SIZE,DOCK_MSG,IPC_NOWAIT);

	if(strcmp(buffer.mText,"quitdock")==0)
		{
			apc->mainLoop=false;
			realMainLoop=false;
		}
	buffer.mText[0]=0;
}

void showhidetActionList(LFSTK_gadgetClass *bc,LFSTK_windowClass *winc,LFSTK_listGadgetClass *list)
{
	geometryStruct		geom;
	unsigned long		d;
	const geometryStruct	*wingeom;

	if(bc!=NULL && bc->gadgetType==BUTTONGADGET)
		{
			wingeom=winc->LFSTK_getWindowGeom();
			d=(unsigned long)bc->userData;
			bc->LFSTK_getGeomWindowRelative(&geom,apc->rootWindow);
			if(dockGravity==PANELSOUTH)
				winc->LFSTK_moveWindow((geom.x+(geom.w/2))+(list->pad/2)-(list->LFSTK_getListMaxWidth()/2),geom.y-wingeom->h+extraSpace,true);
			else
				winc->LFSTK_moveWindow((geom.x+(geom.w/2))+(list->pad/2)-(list->LFSTK_getListMaxWidth()/2),geom.y+geom.h-extraSpace,true);

			winc->LFSTK_showWindow(true);
			winc->LFSTK_setKeepAbove(true);
			winc->LFSTK_redrawAllGadgets();
			//dockWindow->ignoreContext=true;//TODO//
			dockWindow->LFSTK_setContextWindow(NULL);
			apc->windows->at(apc->LFSTK_findWindow(winc)).showing=true;
		}
	else
		{
			winc->LFSTK_hideWindow();
			apc->windows->at(apc->LFSTK_findWindow(winc)).showing=false;
			dockWindow->LFSTK_setContextWindow(mainContextWindow);
		}
}

bool checkInBorder(LFSTK_gadgetClass *gadg)
{
	Window			root_return;
	Window			child_return;
	int				win_x_return1=-1;
	int				win_y_return1=-1;
	int				root_x_return1=-1;
	int				root_y_return1=-1;
	unsigned int		mask_return;
	int				rxdeltadiffx;
	int				rxdeltadiffy;

	XQueryPointer(apc->display,apc->rootWindow,&root_return,&child_return,&root_x_return1,&root_y_return1,&win_x_return1,&win_y_return1,&mask_return);

	rxdeltadiffx=abs(root_x_return-root_x_return1);
	rxdeltadiffy=abs(root_y_return-root_y_return1);

	if((rxdeltadiffx<diFF) && (rxdeltadiffy<diFF))
		{
			return(false);
		}

	rxdiff=rxdeltadiffx;
	rydiff=rxdeltadiffy;
	root_x_return=root_x_return1;
	root_y_return=root_y_return1;

	return(true);
}

void setGadgetPosition(LFSTK_gadgetClass *gadg,bool active)
{
	geometryStruct	geom;

	gadg->LFSTK_getGeom(&geom);

	if(active==true)
		gadg->LFSTK_moveGadget(geom.x,activeY);
	else
		gadg->LFSTK_moveGadget(geom.x,normalY);
}

bool popActionListEnterCB(LFSTK_gadgetClass*p,void* ud)
{
	return(true);
}

bool popActionListExitCB(LFSTK_gadgetClass*p,void* ud)
{
	showhidetActionList(NULL,popActionWindow,popActionList);
	inSomeWindow=true;
	return(true);
}

bool popActionWindowSelect(void *object,void* userdata)//TODO//
{
	infoDataStruct			ls;
	unsigned long			wud=0;

	for(int j=0;j<tasks.size();j++)
		{
			if(moveGadget==true)
				setGadgetPosition(taskbuttons[j],false);
		}

	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);
	ls=list->listDataArray->at(list->LFSTK_getCurrentListItem());

	LFSTK_gadgetClass *bc=static_cast<LFSTK_gadgetClass*>(ls.userData);;

	wud=(unsigned long)popActionWindow->userData;
	switch(wud)
		{
			case TASKSWITCHER:
				{
					LFSTK_gadgetClass *bc=static_cast<LFSTK_gadgetClass*>(object);;
					taskSelect(bc,(void*)ls.userData);
				}
				break;
			case DESKTOPSWITCHER:
				desktopSelect(NULL,NULL);
				break;
		}

	dockWindow->LFSTK_redrawAllGadgets();
	return(true);
}

bool hideCB(void* p,void* ud)
{
	LFSTK_buttonClass	*bc=static_cast<LFSTK_buttonClass*>(p);

	if(bc!=NULL)
		{
			iconWindow->LFSTK_hideWindow();
			dockIsHidden=false;
			if(useTaskBar==true)
				{
					oldwidth=0;
					updateTaskBar(true);
				}
			else
				{
					resizeDock(holdpsize,iconWidth+extraSpace);
				}
			if(calendarButton!=NULL)
				{
					calendarButton->LFSTK_setValue(false);
					calExitCB(calendarButton,NULL);
				}
			if(volumeButton!=NULL)
				{
					volumeButton->LFSTK_setValue(false);
					volExitCB(volumeButton,NULL);
				}
		}
	return(true);
}