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

LFSTK_windowClass	*contextWindow;

void sendClientMessage(Window win,const char *msg,unsigned long data0,unsigned long data1,unsigned long data2,unsigned long data3,unsigned long data4)
{
	XEvent	event;
	long		mask=SubstructureRedirectMask|SubstructureNotifyMask;

	event.xclient.type=ClientMessage;
	event.xclient.serial=0;
	event.xclient.send_event=True;
	event.xclient.message_type=XInternAtom(mainwind->app->display,msg,False);
	event.xclient.window=win;
	event.xclient.format=32;
	event.xclient.data.l[0]=data0;
	event.xclient.data.l[1]=data1;
	event.xclient.data.l[2]=data2;
	event.xclient.data.l[3]=data3;
	event.xclient.data.l[4]=data4;
	XSendEvent(mainwind->app->display,mainwind->app->rootWindow,False,mask,&event);
}

bool gadgetDrop(void *lwc,propertyStruct *data,void* ud)
{
	launcherList	*launcher=(launcherList*)ud;
	char			*command=NULL;

	if(data!=NULL)
		{
			if(strcasecmp(data->mimeType,"text/uri-list")==0)
				{
					dropDesktopFile((const char*)data->data,launcher);
					return(true);
				}

			if(strcasecmp(data->mimeType,"text/plain")==0)
				{
					std::istringstream stream((const char*)data->data);
					std::string line;
					while(std::getline(stream,line))
						{
							if(launcher->entry.inTerm==false)
								asprintf(&command,"%s \"%s\" &",launcher->entry.exec,line.c_str());
							else
								asprintf(&command,"%s %s \"%s\" &",prefs.LFSTK_getCString("termcommand"),launcher->entry.exec,line.c_str());
							sendNotify(launcher->entry.name,line.c_str());
							system(command);
							free(command);
						}
				}
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

	if(switchButton!=NULL)//TODO//
		updateSwitcher();

	if((contextWindow->isVisible==false) && (contextWindow->popupFromGadget!=NULL))
		{
			launcherExitCB(contextWindow->popupFromGadget,(void*)1);
			contextWindow->popupFromGadget=NULL;
		}

	if(useTaskBar==true)
		updateTaskBar();

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

bool contextCB(void *p,void* ud)
{
	LFSTK_windowClass	*lwc=static_cast<LFSTK_gadgetClass*>(p)->wc;
	int					winnum;
	long	 unsigned		bn=(long	 unsigned)ud;
	if(p!=NULL)
		{
			winnum=lwc->app->LFSTK_findWindow(lwc);
			lwc->app->windows->at(winnum).loopFlag=false;
			launcherList *ll=(launcherList*)lwc->popupFromGadget->userData;
			switch(bn-1)
				{
					case BUTTONLAUNCH:
						launcherCB(NULL,lwc->popupFromGadget->userData);
						break;
					case BUTTONREMOVE:
						unlink(ll->desktopFilePath.c_str());
						apc->exitValue=0;
						apc->mainLoop=false;
						break;
				}
			
			launcherExitCB(lwc->popupFromGadget,ud);
			lwc->popupFromGadget=NULL;
		}
	return(true);
}

void showhidetActionList(LFSTK_buttonClass *bc,LFSTK_windowClass *winc,LFSTK_listGadgetClass *list)
{
	geometryStruct	geom;
	unsigned long	d;

	if(bc!=NULL)
		{	
			d=(unsigned long)bc->userData;
			bc->LFSTK_getGeomWindowRelative(&geom,apc->rootWindow);
			switch(panelGravity)
				{
					case PANELNORTH:
						winc->LFSTK_moveWindow(geom.x+(geom.w/2)-(list->LFSTK_getListMaxWidth()/2),geom.y+geom.h,true);
						break;
					case PANELSOUTH:
						winc->LFSTK_moveWindow(geom.x+(geom.w/2)-(list->LFSTK_getListMaxWidth()/2),geom.y-(GADGETHITE*(filltasks.at(d).tasks.size()+1)),true);
						break;
				}
			winc->LFSTK_showWindow(true);
			winc->LFSTK_redrawAllGadgets();
			apc->windows->at(apc->LFSTK_findWindow(winc)).showing=true;
		}
	else
		{
			winc->LFSTK_hideWindow();
			apc->windows->at(apc->LFSTK_findWindow(winc)).showing=false;
		}
}