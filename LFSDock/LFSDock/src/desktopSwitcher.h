/*
 *
 * ©K. D. Hedger. Sun 19 Nov 19:12:44 GMT 2023 keithdhedger@gmail.com

 * This file (desktopSwitcher.h) is part of LFSDock.

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

#ifndef _DESKTOPSWITCHER_
#define _DESKTOPSWITCHER_

#include "globals.h"

extern LFSTK_buttonClass *switchButton;
//extern LFSTK_toggleButtonClass *switchButton;


void		sendPropNotifyMessage(Window win,Atom msg);

int		addDesktopSwitcer(int x,int y,int grav);
bool		desktopSelect(void *object,void* userdata);
void		updateDeskSwitcher(void);

#endif
