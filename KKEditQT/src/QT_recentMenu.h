/*
 *
 * ©K. D. Hedger. Sun 13 Feb 13:33:22 GMT 2022 keithdhedger@gmail.com

 * This file (QT_recentMenu.h) is part of KKEditQT.

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


#ifndef _QT_RECENTMENU_
#define _QT_RECENTMENU_

#include "kkedit-includes.h"

class RecentMenuClass : public QObject
{
	Q_OBJECT

	public:
		RecentMenuClass(KKEditClass *kk);
		~RecentMenuClass();

		void		updateRecents(void);
		void		addFilePath(QString path);

		QMenu		*recentMenu;
		int			maxFiles=12;

	private:		
		QString		recentFileList;
		KKEditClass *mainKKEditClass;

	public slots:
		void		menuClicked(QAction *action);

};

#endif
