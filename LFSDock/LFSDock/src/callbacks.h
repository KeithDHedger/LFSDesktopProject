/*
 *
 * ©K. D. Hedger. Sat  6 Feb 13:59:29 GMT 2021 keithdhedger@gmail.com

 * This file (callbacks.h) is part of LFSPanel.

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

#ifndef _CALLBACKS_
#define _CALLBACKS_

bool launcherCB(void *p,void* ud);
bool gadgetDrop(void *lwc,propertyStruct *data,void* ud);
bool timerCB(LFSTK_applicationClass *p,void* ud);
void readMsg(void);
bool exitCB(LFSTK_gadgetClass*p,void* ud);
bool moveCB(LFSTK_gadgetClass*p,void* ud);
#endif
