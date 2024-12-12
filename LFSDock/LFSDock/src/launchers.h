/*
 *
 * ©K. D. Hedger. Wed 28 Oct 19:47:48 GMT 2015 keithdhedger@gmail.com

 * This file (launchers.h) is part of LFSPanel.

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

#ifndef _LAUNCHERS_
#define _LAUNCHERS_

#include "globals.h"
#include "callbacks.h"

extern std::vector<launcherDataStruct>	launchersArray;
extern LFSTK_findClass					*findlaunchers;
extern LFSTK_gadgetClass					*currentLauncher;
extern LFSTK_windowClass					*launcherContextWC;
extern const char						*contextThemeIconData[];

bool		launcherCB(void *p,void* ud);
int		addLaunchers(int x,int y,int grav);
bool		launcherExitCB(LFSTK_gadgetClass* p,void* ud);

#endif
