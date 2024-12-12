/*
 *
 * ©K. D. Hedger. Tue 28 Nov 11:35:30 GMT 2023 keithdhedger@gmail.com

 * This file (taskBar.h) is part of LFSDock.

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

#ifndef _TASKBAR_
#define _TASKBAR_

#include "globals.h"
#include "callbacks.h"

extern std::vector<taskStruct>	holdtasks;
extern std::vector<taskStruct>	filltasks;
extern std::vector<taskStruct>	tasks;
extern int						oldwidth;

bool taskSwitcherEnterCB(LFSTK_gadgetClass*p,void* ud);
bool taskSwitcherExitCB(LFSTK_gadgetClass*p,void* ud);
bool taskSelect(void *object,void* userdata);
bool taskListCB(void* p,void* ud);
bool taskListCBDown(void* p,void* ud);

void updateTaskBar(bool force=false);

#endif
