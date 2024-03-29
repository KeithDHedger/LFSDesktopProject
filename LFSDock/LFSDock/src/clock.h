/*
 *
 * ©K. D. Hedger. Sun 19 Nov 19:09:41 GMT 2023 keithdhedger@gmail.com

 * This file (clock.h) is part of LFSDock.

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

#ifndef _CLOCK_
#define _CLOCK_

extern LFSTK_buttonClass	*clockButton;

int addClock(int x,int y,int grav);
void updateClock(void);

#endif
