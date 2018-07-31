/*
 *
 * ©K. D. Hedger. Mon 26 Oct 13:09:21 GMT 2015 keithdhedger@gmail.com

 * This file (appmenu.h) is part of LFSWManager.

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

#ifndef _APPMENU_
#define _APPMENU_

#include <lfstk/LFSTKGlobals.h>

//#include "globals.h"

#define MAXCATS 14
#define MAXENTRYS 500

struct menuEntryStruct
{
	char					*name;
	char					*exec;
	bool					inTerm;
};

extern LFSTK_windowClass	*appWindow;
extern LFSTK_menuButtonClass	*catButtons[MAXCATS];
void addAppmenu(void);
void refreshMenu(void);

#endif
