/*
 *
 * ©K. D. Hedger. Sun 20 Sep 14:41:10 BST 2015 keithdhedger@gmail.com

 * This file (globals.cpp) is part of LFSPanel.

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

//prefs
LFSTK_prefsClass	prefs;

int					panelHeight=16;
int					panelWidth=-1;
const monitorStruct	*mons=NULL;
int					onMonitor=0;
int					panelPos=PANELCENTRE;
int					panelGravity=PANELNORTH;

int					queueID;
msgBuffer			buffer;

const char			*desktopTheme=NULL;
const char			*panelID="";
bool				realMainLoop=true;

int					refreshRate=1;

int					iconSize=16;

//panel window
LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*mainwind=NULL;
int						rightOffset=0;
int						leftOffset=0;
int						launcherSide=NOLAUNCHERS;

//atoms
Atom				WM_STATE=None;
Atom				NET_WM_WINDOW_TYPE_NORMAL=None;
Atom				NET_WM_STATE_HIDDEN=None;
Atom				NET_WM_WINDOW_TYPE_DIALOG=None;
Atom				NET_WM_DESKTOP=None;
Atom				NET_WM_WINDOW_TYPE=None;
Atom				NET_WM_STATE=None;

void setSizes(int *x,int *y,int *w,int *h,int *size,int *grav,bool fromleft)
{
	*w=panelHeight;
	*h=panelHeight;
	*size=(*w)-12;

	switch(*grav)
		{
			case PANELNORTH:
			case PANELSOUTH:
				if(fromleft==true)
					{
						*grav=NorthWestGravity;
						*x=*x;
					}
				else
					{
						*grav=NorthEastGravity;
						*x=*x-*w+1;
					}
				*y=0;
				break;

			case PANELEAST:
			case PANELWEST:
				if(fromleft==true)
					{
						*grav=NorthWestGravity;
						*y=*x;
					}
				else
					{
						*grav=SouthWestGravity;
						*y=*x-*h+1;
					}
				*x=0;
				break;
		}
}

void sendNotify(const char *name,const char *message)//TODO//could be better
{
#ifdef _GOTNOTIFYSEND_
	char	*command;
	asprintf(&command,"notify-send -u low -t 2000 -i stock_dialog-info \"%s\" \"%s ...\"",name,message);
	system(command);
	free(command);
#endif
}

void dropDesktopFile(const char *data,launcherList *launcher)
{
	char			*cleanstr;
	char			*command=NULL;
	char			*ptr;
	std::istringstream stream(data);
	std::string line;
	while(std::getline(stream,line))
		{
			cleanstr=apc->globalLib->LFSTK_cleanString((const char*)line.c_str());
			if((strrchr(cleanstr,'.')!=NULL) && (strcmp(strrchr(cleanstr,'.'),".desktop")==0))
				{
					asprintf(&command,"mkdir -p '%s/launchers-%s';cp -nP '%s' '%s/launchers-%s'",apc->configDir,panelID,cleanstr,apc->configDir,panelID);
					ptr=strrchr(cleanstr,'/');
					sendNotify("Adding launcher ",++ptr);
					system(command);
					free(command);
					apc->exitValue=0;
					apc->mainLoop=false;
					free(cleanstr);
					return;
				}

			if(launcher!=NULL)
				{
					if(launcher->entry.inTerm==false)
						asprintf(&command,"%s \"%s\" &",launcher->entry.exec,cleanstr);
					else
						asprintf(&command,"%s %s \"%s\" &",prefs.LFSTK_getCString("termcommand"),launcher->entry.exec,cleanstr);
						sendNotify("Running ",launcher->entry.exec);
						system(command);
						free(cleanstr);
						free(command);
				}
		}
}

