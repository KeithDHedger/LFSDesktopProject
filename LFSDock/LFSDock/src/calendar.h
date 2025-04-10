/*
 *
 * ©K. D. Hedger. Wed 20 Dec 11:29:40 GMT 2023 keithdhedger@gmail.com

 * This file (calendar.h) is part of LFSDock.

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

#ifndef _CALENDAR_
#define _CALENDAR_

#include <math.h>
#include <iostream>
#include <fstream>

#include "globals.h"
#include "callbacks.h"

struct datesStruct
{
	int			month;
	int			date;
	cairoColor	col;
};

extern LFSTK_windowClass			*calWindow;
extern LFSTK_toggleButtonClass	*calendarButton;
extern LFSTK_multiLineEditClass	*editbox;
extern bool						calWindowVisible;
extern bool						calendarIsUp;
extern std::vector<datesStruct>	datesData;

int addCalendar(int x,int y,int grav);
bool calExitCB(LFSTK_gadgetClass*p,void* ud);

#endif
