/*
 *
 * ©K. D. Hedger. Thu 23 Dec 20:38:05 GMT 2021 keithdhedger@gmail.com

 * This file (SingleInstanceClass.h) is part of KKEditQT.

 * KKEditQT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEditQT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEditQT.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SINGLEINSTANCECLASS_
#define _SINGLEINSTANCECLASS_

#include "kkedit-includes.h"

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

class SingleInstanceClass
{
	public:
		SingleInstanceClass(QApplication *app,bool forcem);
		~SingleInstanceClass();

		long			getSIWorkSpace(void);
		void*			getX11Prop(Window w,Atom prop,Atom type,int fmt,unsigned long *rcountp);
		bool			getRunning(void);

		QApplication	*app;
		int				workspace=-1;
		bool			isOnX11=true;
		int				queueID=-1;

	private:
		bool			deleteComfiles=false;
		QFile			fileMsg;
		QFile			filePID;
		bool			usingMulti=false;
};


#endif
