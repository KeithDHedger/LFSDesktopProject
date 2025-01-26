/*
 *
 * ©K. D. Hedger. Tue 21 Jan 15:58:25 GMT 2025 keithdhedger@gmail.com

 * This file (embedClass.h) is part of LFSTray.

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

#ifndef _EMBEDCLASS_
#define _EMBEDCLASS_

#include "globals.h"

class LFSTray_embedClass
{
	public:
		LFSTray_embedClass(LFSTray_trayClass *trayclass);
		~LFSTray_embedClass(void);

		LFSTray_trayClass	*tray=NULL;
		int					nextIconX=0;
		int					nextIconY=0;
		sysIcons				currentIcon;;

		void		removeIcon(Window wid);
		void		refreshIcons(void);
		void		embedWindow(Window wid);
		bool		addIcon(Window wid);
		void		nextPosition(void);
		Pixmap	makePixmap(void);
};

#endif
