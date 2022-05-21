/*
 *
 * ©K. D. Hedger. Tue 31 Jul 13:32:11 BST 2018 keithdhedger@gmail.com

 * This file (prefs.h) is part of LFSWManager.

 * LFSWManager is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSWManager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWManager.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef _PREFS_
#define _PREFS_

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/Xdbe.h>

#include "wind.h"

extern args				wmPrefs[];
extern args				themeRC[];

void saveVarsToFile(const char* filepath,args* dataptr,const char *fs);
bool loadVarsFromFile(char* filepath,args* dataptr,const char *fs);

#endif
