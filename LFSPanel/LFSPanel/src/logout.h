/*
 *
 * ©K. D. Hedger. Sun 20 Sep 18:36:35 BST 2015 keithdhedger@gmail.com

 * This file (logout.h) is part of LFSPanel.

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


#ifndef _LOGOUT_
#define _LOGOUT_

#include "globals.h"

enum logoutEntrys {LOGOUT=0,RESTART,SHUTDOWN,NUMLOGOUTENTRYS};

extern LFSTK_buttonClass	*logoutButton;
extern LFSTK_menuClass		*logoutMenu;

int addLogout(int x,int y,int grav,bool fromleft);

#endif
