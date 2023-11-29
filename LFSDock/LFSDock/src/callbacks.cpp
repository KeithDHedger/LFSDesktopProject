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

/*
bool windowAllMenuCB(void *p,void* ud)
{
	Window			winid=0;
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();

	winid=(Window)(ud);
	possibleError="Can't activate window";
	sendClientMessage(winid,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
	return(true);
}
*/
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

bool launcherCB(void *p,void* ud)
{
	launcherList		*launcher=(launcherList*)ud;
	Window			win=None;
	std::string		ex=launcher->entry.exec;
	std::size_t		found;
	std::string		command;
	std::string		args;
	std::string		str;
	std::string		whch;

	if(launcher==NULL)
		return(true);

	if(p!=NULL)
		{
			win=getWindowByClass(launcher->entry.name);
			if(win!=None)
				{
					sendClientMessage(win,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
					return(true);
				}
			win=getWindowByPID(launcher->pid);
			if(win!=None)
				{
					sendClientMessage(win,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
					return(true);
				}
		}


	found=ex.find(std::string(" "));
	if(found!=std::string::npos)
		{
			command=ex.substr(0,found);
			args=ex.substr(found,-1);
		}
	else
		{
			command=ex;
			args="";
		}

	whch=apc->globalLib->LFSTK_oneLiner(std::string("which '%s'"),command.c_str());

	sendNotify("Launching ",launcher->entry.name);

	if(launcher->entry.inTerm==false)
		str=apc->globalLib->LFSTK_oneLiner(std::string("exec %s %s &\necho $!"),whch.c_str(),args.c_str());
	else
		str=apc->globalLib->LFSTK_oneLiner(std::string("exec %s %s %s &\necho $!"),prefs.LFSTK_getCString("termcommand"),whch.c_str(),args.c_str());
	launcher->pid=std::stoul(str,nullptr,0);

	return(true);
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

	if(switchButton!=NULL)
		updateSwitcher();

	if((contextWindow->isVisible==false) && (contextWindow->popupFromGadget!=NULL))
		{
			exitCB(contextWindow->popupFromGadget,(void*)1);
			contextWindow->popupFromGadget=NULL;
		}
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

bool exitCB(LFSTK_gadgetClass*p,void* ud)
{
	if(ud!=NULL)
		{
			launcherList		*ll;
			ll=(launcherList*)ud;
			geometryStruct	geom2;
			int				adj;

			adj=extraSpace*posMultiplier;
			p->LFSTK_getGeom(&geom2);	
			p->LFSTK_moveGadget(geom2.x,geom2.y+adj);
			popWindow->LFSTK_hideWindow();
		}
	return(true);
}

bool enterCB(LFSTK_gadgetClass*p,void* ud)
{
	if(ud!=NULL)
		{
			geometryStruct	geom;
			launcherList		*ll;
			int				width;
			int				adj;

			adj=extraSpace*posMultiplier;
			ll=(launcherList*)ud;

//			printf(">>>Mouse In %s<<<\n",ll->entry.name);
			p->LFSTK_getGeom(&geom);	
			p->LFSTK_moveGadget(geom.x,geom.y-adj);
			popLabel->LFSTK_setLabel(ll->entry.name);
			popLabel->LFSTK_setFontString(prefs.LFSTK_getCString(prefs.LFSTK_hashFromKey("font")),true);
			width=popLabel->LFSTK_getTextRealWidth(ll->entry.name);
			popWindow->LFSTK_resizeWindow(width,GADGETHITE);
			p->LFSTK_getGeomWindowRelative(&geom,apc->rootWindow);	

			if(posMultiplier==1)
				popWindow->LFSTK_moveWindow(geom.x-(width/2)+(geom.w/2),geom.y-(GADGETHITE),true);
			else
				popWindow->LFSTK_moveWindow(geom.x-(width/2)+(geom.w/2),iconSize+extraSpace,true);
			popWindow->LFSTK_showWindow();
			popWindow->LFSTK_clearWindow(true);
		}
	return(true);
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
			//printf("BUTTON=%u\n",bn);
			//fprintf(stderr,"userdata=%p\n",lwc->popupFromGadget->userData);
			launcherList *ll=(launcherList*)lwc->popupFromGadget->userData;
			fprintf(stderr,"lpath=%s\n",ll->desktopFilePath.c_str());
			switch(bn-1)
				{
					case BUTTONLAUNCH:
						printf("BUTTONLAUNCH=%u\n",bn);
						launcherCB(NULL,lwc->popupFromGadget->userData);
						break;
					case BUTTONREMOVE:
						printf("BUTTONREMOVE=%u\n",bn);
						unlink(ll->desktopFilePath.c_str());
						apc->exitValue=0;
						apc->mainLoop=false;
						break;
				}
			
			exitCB(lwc->popupFromGadget,ud);
			lwc->popupFromGadget=NULL;
		}
	return(true);
}