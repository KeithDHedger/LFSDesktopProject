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
int					cnt=0;
int					cnt2=0;
int					root_x_return=-1,root_y_return=-1;
int					rxdiff=-1;
int					rydiff=-1;
int					diFF=ICONSPACE-1;

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
//TODO//
//			if(strcasecmp(data->mimeType,"text/plain")==0)
//				{
//	int		xx=(long unsigned int)ud;
//	char		*command=NULL;
//				fprintf(stderr,"\n\n");
//					std::istringstream stream((const char*)data->data);
//					std::string line;
//					while(std::getline(stream,line))
//						{
//						launcherDataStruct lds=launchersArray.at((unsigned long)ud);
//					std::cout<<"line="<<line<<" launcher num="<<(unsigned long)ud<<std::endl;
//							if(lds.inTerm==false)
//								asprintf(&command,"%s \"%s\" &",lds.exec.c_str(),line.c_str());
//							else
//								asprintf(&command,"%s %s \"%s\" &",prefs.LFSTK_getCString("termcommand"),lds.exec.c_str(),line.c_str());
//							//sendNotify(lds.name.c_str(),line.c_str());
//							fprintf(stderr,"command=>>%s<<\n",command);
//							std::cout<<"name="<<lds.name;
//							std::cout<<"\nexec="<<lds.exec<<" "<<line;
//							std::cout<<std::endl;
//							system(command);
//							freeAndNull(&command);
//						}
//				}
		}
	return(true);
}

bool timerCB(LFSTK_applicationClass *p,void* ud)
{
	readMsg();

	if(clockButton!=NULL)
		updateClock();

	if(scwindow!=NULL)
		updateSlider();

	if((gotLaunchers==true) && (currentLauncher!=NULL))
		cnt++;

	if((useTaskBar==true) && (currentTask!=NULL))
		cnt2++;

	if((gotLaunchers==true) && (launcherContextWC->isVisible==false) && (launcherContextWC->popupFromGadget!=NULL))
		{
			launcherExitCB(launcherContextWC->popupFromGadget,(void*)1);
			launcherContextWC->popupFromGadget=NULL;
			currentLauncher=NULL;;
		}

	if((cnt>10) && (currentLauncher!=NULL))
		{
			cnt=0;
			launcherExitCB(currentLauncher,(void*)1);
			launcherContextWC->popupFromGadget=NULL;
			currentLauncher=NULL;;
		}		

	if((useTaskBar==true) && (cnt2>10) && (currentTask!=NULL))
		{
			cnt2=0;
			taskSwitcherExitCB(currentTask,(void*)1);
			currentTask=NULL;;
		}

	if((popActionWindow!=NULL) && (popActionWindow->isVisible==true) && ((inSomeWindow==false) && (popActionWindow->inWindow==false)))
		popActionListExitCB(NULL,(void*)1);

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
			switch(dockGravity)
				{
					case PANELNORTH:
						winc->LFSTK_moveWindow((geom.x+(geom.w/2))+(list->pad/2)-(list->LFSTK_getListMaxWidth()/2),geom.y+geom.h-extraSpace,true);
						break;
					case PANELSOUTH:
						winc->LFSTK_moveWindow((geom.x+(geom.w/2))+(list->pad/2)-(list->LFSTK_getListMaxWidth()/2),geom.y-wingeom->h+extraSpace,true);
						break;
				}
			winc->LFSTK_showWindow(true);
			winc->LFSTK_setKeepAbove(true);
			winc->LFSTK_redrawAllGadgets();
			apc->windows->at(apc->LFSTK_findWindow(winc)).showing=true;
		}
	else
		{
			winc->LFSTK_hideWindow();
			apc->windows->at(apc->LFSTK_findWindow(winc)).showing=false;
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
	return(true);
}
