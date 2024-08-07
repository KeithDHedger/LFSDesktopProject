/*
 *
 * ©K. D. Hedger. Sat  6 Feb 13:59:35 GMT 2021 keithdhedger@gmail.com

 * This file (callbacks.cpp) is part of LFSPanel.

 * LFSPanel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSPanel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSPanel.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "globals.h"

bool launcherCB(void *p,void* ud)
{
	launcherList	*launcher=(launcherList*)ud;
	char			*command;
	std::string	correctedcommand;

	if(launcher==NULL)
		return(true);

	correctedcommand=LFSTK_UtilityClass::LFSTK_strReplaceAllStr(launcher->entry.exec,"%u","",true);

	if(launcher->entry.inTerm==false)
		asprintf(&command,"%s &",correctedcommand.c_str());
	else
		asprintf(&command,"%s %s &",prefs.LFSTK_getCString("termcommand"),correctedcommand.c_str());

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
					std::string correctedcommand;

					while(std::getline(stream,line))
						{
							correctedcommand=LFSTK_UtilityClass::LFSTK_strReplaceAllStr(launcher->entry.exec,"%u",line);
							if(launcher->entry.inTerm==false)
								asprintf(&command,"%s &",correctedcommand.c_str());
							else
								asprintf(&command,"%s %s &",prefs.LFSTK_getCString("termcommand"),correctedcommand.c_str());
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
 	Window			sink;
	Window			childwindow;
	int				sinkx;
	int				sinky;
	unsigned int	buttonmask;

	XQueryPointer(apc->display,apc->rootWindow,&sink,&childwindow,&sinkx,&sinky,&sinkx,&sinky,&buttonmask);

	if(buttonmask!=0)
		return(true);

	if(clockButton!=NULL)
		updateClock();

	if(diskButton!=NULL)
		updateDiskStats();

	if(cpuButton!=NULL)
		updateCpuStats();

	if(scwindow!=NULL)
		updateSlider();

	return(true);
}
