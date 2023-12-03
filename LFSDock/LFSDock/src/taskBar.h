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

struct taskStruct
{
	std::string				taskName="";
	std::string				taskClass[2]={"",""};
	Window					winid=None;
	unsigned long			pid=0;
	std::vector<taskStruct>	tasks;

};

extern LFSTK_listGadgetClass	*taskList;
extern LFSTK_windowClass		*taskWindow;

bool taskSwitcherEnterCB(LFSTK_gadgetClass*p,void* ud);
bool taskSelect(void *object,void* userdata);
bool taskSwitcherExitCB(LFSTK_gadgetClass*p,void* ud);

void updateTaskBar();
bool taskListCB(void* p,void* ud);

#endif
