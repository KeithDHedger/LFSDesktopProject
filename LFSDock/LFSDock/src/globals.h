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

#include "config.h"
#include <lfstk/LFSTKGlobals.h>

#include "callbacks.h"
#include "clock.h"
#include "slider.h"
#include "desktopSwitcher.h"

#ifndef _GLOBALS_
#define _GLOBALS_

enum PANELXPOS {PANELLEFT=-1,PANELCENTRE=-2,PANELRIGHT=-3};
enum PANELSIZE {PANELFULL=-1,PANELSHRINK=-2};
enum PANELGRAVITY {PANELNORTH=1,PANELSOUTH=2};

struct menuEntryStruct
{
	char					*name=NULL;
	char					*exec=NULL;
	char					*icon=NULL;
	bool					inTerm;
};

struct launcherList
{
	launcherList			*next=NULL;
	LFSTK_buttonClass	*bc=NULL;
	char					*icon=NULL;
	menuEntryStruct		entry;
};

enum {NOLAUNCHERS,LAUNCHERINLEFT};

//prefs
extern LFSTK_prefsClass			prefs;
extern std::string				configDir;
extern std::string				launchersDir;
extern std::string				configFile;
extern LFSTK_windowClass			*popWindow;
extern LFSTK_labelClass			*popLabel;
extern launcherList				*ll;
extern int						iconSize;
extern int						posMultiplier;

extern int						panelSize;
extern const monitorStruct		*mons;
extern int						onMonitor;
extern int						panelGravity;
extern const char				*panelTextColour;
extern const char				*panelBGColour;

extern int						extraSpace;

extern int						queueID;
extern msgBuffer					buffer;
extern bool						realMainLoop;

extern const char				*desktopTheme;
extern int						refreshRate;


//panel window
extern LFSTK_applicationClass	*apc;
extern LFSTK_windowClass			*mainwind;
extern int						launcherSide;

//atoms
extern Atom						WM_STATE;
extern Atom						NET_WM_WINDOW_TYPE_NORMAL;
extern Atom						NET_WM_STATE_HIDDEN;
extern Atom						NET_WM_WINDOW_TYPE_DIALOG;
extern Atom						NET_WM_DESKTOP;
extern Atom						NET_WM_WINDOW_TYPE;
extern Atom						NET_WM_STATE;
extern Atom						NET_WM_NAME;
extern Atom						UTF8_STRING;
extern Atom						NET_CURRENT_DESKTOP;

extern const char				*possibleError;

void printError(const char *err);
void dropDesktopFile(const char *data,launcherList *launcher);
void sendNotify(const char *message1,const char *message2);
void setGadgetDetails(LFSTK_gadgetClass *gadget);

#endif
