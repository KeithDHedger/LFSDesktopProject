/*
 *
 * ©K. D. Hedger. Sun 19 Nov 19:08:47 GMT 2023 keithdhedger@gmail.com

 * This file (globals.h) is part of LFSDock.

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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "config.h"
#include <lfstk/LFSTKGlobals.h>

#include "callbacks.h"
#include "launchers.h"
#include "clock.h"
#include "slider.h"
#include "desktopSwitcher.h"
#include "taskBar.h"
#include "calendar.h"

#ifndef _GLOBALS_
#define _GLOBALS_

enum PANELGRAVITY {PANELNORTH=1,PANELSOUTH=2};
enum WHATSORT {LAUNCHER=100,DESKTOPSWITCHER,TASKSWITCHER};

struct desktopFileStruct
{
	char					*name=NULL;
	char					*exec=NULL;
	char					*icon=NULL;
	bool					inTerm;
};

struct launcherDataStruct
{
	std::string			name;
	std::string			exec;
	std::string			path;
	bool					inTerm;
	unsigned long		pid=0;
};

struct taskStruct
{
	std::string				taskName="";
	std::string				taskClass[2]={"",""};
	Window					winid=None;
	unsigned long			pid=0;
	std::vector<taskStruct>	tasks;
};

//prefs
extern LFSTK_prefsClass			prefs;
extern std::string				configDir;
extern std::string				launchersDir;
extern std::string				configFile;
extern std::string				whatDock;

extern int						iconWidth;
extern int						iconHeight;

extern int						normalY;
extern int						activeY;
extern int						extraSpace;
extern int						deskCount;

extern int						dockSize;
extern const monitorStruct		*mons;
extern int						onMonitor;
extern int						dockGravity;
extern std::string				dockTextColour;
extern std::string				dockBGColour;


extern int						queueID;
extern msgBuffer					buffer;
extern bool						realMainLoop;

extern std::string				desktopTheme;
extern int						refreshRate;
extern bool						useMicros;

extern LFSTK_buttonClass			*taskbuttons[20];
extern int						windowWidth;
extern GKeyFile					*kf;
extern LFSTK_findClass			*gFind;
extern bool						useTaskBar;
extern bool						gotLaunchers;


//dock window
extern LFSTK_applicationClass	*apc;
extern LFSTK_windowClass			*dockWindow;
extern LFSTK_windowClass			*popActionWindow;
extern LFSTK_listGadgetClass		*popActionList;
extern bool						inSomeWindow;
extern int						popActionWindowYOffset;

//atoms
extern Atom						WM_STATE;
extern Atom						NET_WM_WINDOW_TYPE_NORMAL;
extern Atom						NET_WM_STATE_HIDDEN;
extern Atom						NET_WM_WINDOW_TYPE_DIALOG;
extern Atom						NET_WM_WINDOW_TYPE;
extern Atom						NET_WM_STATE;
extern Atom						NET_WM_NAME;
extern Atom						UTF8_STRING;
extern Atom						NET_CURRENT_DESKTOP;
extern Atom						WM_CLASS;
extern Atom						NET_WM_PID;
extern Atom						NET_NUMBER_OF_DESKTOPS;
extern Atom						NET_ACTIVE_WINDOW;
extern Atom						NET_WM_DESKTOP;

extern const char				*possibleError;

void printError(const char *err);
void dropDesktopFile(const char *data,void *launcher);

void sendNotify(const char *message1,const char *message2);
void setGadgetDetails(LFSTK_gadgetClass *gadget);
bool hasProp(Window win,Atom atom);
Window getWindowByClass(std::string name);
Window getWindowByPID(unsigned long pid);

Window doTreeWalkForClass(Window wind,std::string namecheck);
Window doTreeWalkForPID(Window wind,unsigned long pid);
void moveDock(int extra);
std::string getWindowName(Window winid);

void setGadgetPosition(LFSTK_gadgetClass *gadg,bool active);

#endif
