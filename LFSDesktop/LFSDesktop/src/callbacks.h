/*
 *
 * ©K. D. Hedger. Mon 25 Jan 11:39:44 GMT 2021 keithdhedger@gmail.com

 * This file (callbacks.h) is part of LFSDesktop.

 * LFSDesktop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSDesktop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDesktop.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef _CALLBACKS_
#define _CALLBACKS_

#include "globals.h"

bool windowDrop(LFSTK_windowClass *lwc,void* ud);
bool doDiskMenuSelect(void *p,void* ud);
bool doDeskItemMenuSelect(void *p,void* ud);
bool mouseUpCB(void *p,void* ud);
bool timerCB(LFSTK_applicationClass *p,void* ud);

#endif
