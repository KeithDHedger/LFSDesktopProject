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

bool launcherCB(void *p,void* ud)
{
	launcherList	*launcher=(launcherList*)ud;
	char			*command;

	if(launcher==NULL)
		return(true);

	if(launcher->entry.inTerm==false)
		asprintf(&command,"%s &",launcher->entry.exec);
	else
		asprintf(&command,"%s %s &",prefs.LFSTK_getCString("termcommand"),launcher->entry.exec);

	sendNotify("Launching ",launcher->entry.name);
#ifdef _ENABLEDEBUG_
	DEBUGFUNC("%s",command);
#else
	system(command);
#endif
	free(command);
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
