/*
 *
 * ©K. D. Hedger. Tue 21 Jan 13:07:42 GMT 2025 keithdhedger@gmail.com

 * This file (callbacks.h) is part of LFSTray.

 * LFSTray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSTray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSTray.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CALLBACKS_
#define _CALLBACKS_

extern LFSTray_trayClass	*trayClass;

bool eventCB(LFSTK_applicationClass *p,XEvent *event);

#endif
