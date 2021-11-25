/*
 *
 * ©K. D. Hedger. Mon 15 Nov 16:33:52 GMT 2021 keithdhedger@gmail.com

 * This file (SingleInstanceClass.h) is part of KKEdit.

 * KKEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEdit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SINGLEINSTANCECLASS_
#define _SINGLEINSTANCECLASS_

#include <QtWidgets>
#include <QApplication>
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <sys/types.h>
#include <signal.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MAXMSGSIZE 1024
#define MSGKEY 0x8000
#define ALLOK 0
#define UNKNOWNARG 1
#define NOMAKEQUEUE 2
#define NOSENDMSG 3
#define WAITMSG 0

#define MSGANY 0
#define MSGSEND 2
#define MSGRECEIVE 1

class SingleInstanceClass
{
	public:
		SingleInstanceClass(QApplication *app,bool forcem=false);
		~SingleInstanceClass();

		long			getSIWorkSpace(void);
		void*			getX11Prop(Window w,Atom prop,Atom type,int fmt,unsigned long *rcountp);
		bool			getRunning(void);

		QApplication	*app;
		int				workspace=-1;
		bool			isOnX11=true;
	private:
		bool			deleteComfiles=false;
		QFile			fileMsg;
		QFile			filePID;
		bool			usingMulti=false;
};


#endif
