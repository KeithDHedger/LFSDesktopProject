/*
 *
 * ©K. D. Hedger. Sun 19 Nov 19:08:31 GMT 2023 keithdhedger@gmail.com

 * This file (globals.cpp) is part of LFSDock.

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

//prefs
LFSTK_prefsClass	prefs;
std::string				configDir;
std::string				launchersDir;
std::string				configFile;
LFSTK_windowClass		*popWindow=NULL;
LFSTK_labelClass			*popLabel=NULL;
launcherList				*ll=NULL;
int						iconSize=16;
int						posMultiplier=1;

int						panelSize=2;
const monitorStruct		*mons=NULL;
int						onMonitor=0;
int						panelGravity=PANELNORTH;
const char				*panelTextColour="";
const char				*panelBGColour="";
int						extraSpace=16;

int						queueID;
msgBuffer				buffer;

const char				*desktopTheme=NULL;
bool						realMainLoop=true;

int						refreshRate=1;

//panel window
LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*mainwind=NULL;
int						launcherSide=NOLAUNCHERS;

//atoms
Atom						WM_STATE=None;
Atom						NET_WM_WINDOW_TYPE_NORMAL=None;
Atom						NET_WM_STATE_HIDDEN=None;
Atom						NET_WM_WINDOW_TYPE_DIALOG=None;
Atom						NET_WM_DESKTOP=None;
Atom						NET_WM_WINDOW_TYPE=None;
Atom						NET_WM_STATE=None;
Atom						NET_WM_NAME=None;
Atom						UTF8_STRING=None;
Atom						NET_CURRENT_DESKTOP=None;

const char				*possibleError="Unknown";

void sendNotify(const char *name,const char *message)//TODO//could be better
{
#ifdef _GOTNOTIFYSEND_
	std::string	com=std::string("notify-send -u low -t 2000 -i stock_dialog-info \"" + std::string(name) + " " + std::string(message) + " ...\" &");
	system(com.c_str());
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
					asprintf(&command,"mkdir -p '%s/launchers-DOCK';cp -nP '%s' '%s/launchers-DOCK'",apc->configDir.c_str(),cleanstr,apc->configDir.c_str());
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

void setGadgetDetails(LFSTK_gadgetClass *gadget)
{
	gadget->LFSTK_setAlpha(1.0);
	gadget->LFSTK_setTile(NULL,0);
	gadget->gadgetDetails.bevel=BEVELNONE;
	gadget->LFSTK_setGadgetColours(GADGETBG,"#00000000","#00000000","#00000000","#00000000");
	gadget->	LFSTK_setGadgetColourPair(NORMALCOLOUR,"#00000000",panelTextColour);
}
