/*
 *
 * ©K. D. Hedger. Sun 20 Sep 14:41:04 BST 2015 keithdhedger@gmail.com

 * This file (globals.h) is part of LFSPanel.

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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "config.h"
#include <lfstk/LFSTKGlobals.h>

#include "callbacks.h"
#include "clock.h"
#include "disks.h"
#include "cpu.h"
#include "windowlist.h"
#include "slider.h"

#ifndef _GLOBALS_
#define _GLOBALS_

#define WINHELPER DATADIR "/scripts/PanelHelperWinList"
#define BWIDTH 64
#define SPACING 10
#define WINDOWREFRESH 2
#define RCNAME "lfspanel"
#define REFRESHMULTI 4

enum PANELXPOS {PANELLEFT=-1,PANELCENTRE=-2,PANELRIGHT=-3};
enum PANELYPOS {PANELTOP=-1,PANELBOTTOM=-3};
enum PANELSIZE {PANELFULL=-1,PANELSHRINK=-2};
enum PANELGRAVITY {PANELABS=0,PANELNORTH,PANELEAST,PANELSOUTH,PANELWEST};

struct menuEntryStruct
{
	char					*name=NULL;
	char					*exec=NULL;
	char					*icon=NULL;
	bool					inTerm;
};

struct launcherList
{
	launcherList		*next=NULL;
	LFSTK_buttonClass	*bc=NULL;
	char				*icon=NULL;
	menuEntryStruct		entry;
};

enum {NOLAUNCHERS,LAUNCHERINLEFT,LAUNCHERINRITE};

//prefs
extern LFSTK_prefsClass		prefs;

extern int					panelHeight;
extern int					panelWidth;
extern const monitorStruct	*mons;
extern int					onMonitor;
extern int					panelPos;
extern int					panelGravity;

extern int					queueID;
extern msgBuffer			buffer;
extern bool					realMainLoop;

extern const char			*desktopTheme;
extern const char			*panelID;

extern int					refreshRate;

extern int					iconSize;

//panel window
extern LFSTK_applicationClass	*apc;
extern LFSTK_windowClass	*mainwind;
extern int					rightOffset;
extern int					leftOffset;
extern int					launcherSide;

//atoms
extern Atom					WM_STATE;
extern Atom					NET_WM_WINDOW_TYPE_NORMAL;
extern Atom					NET_WM_STATE_HIDDEN;
extern Atom					NET_WM_WINDOW_TYPE_DIALOG;
extern Atom					NET_WM_DESKTOP;
extern Atom					NET_WM_WINDOW_TYPE;
extern Atom					NET_WM_STATE;

void printError(const char *err);
void setSizes(int *x,int *y,int *w,int *h,int *size,int *grav,bool fromleft);
void dropDesktopFile(const char *data,launcherList *launcher);
void sendNotify(const char *message1,const char *message2);

#endif
