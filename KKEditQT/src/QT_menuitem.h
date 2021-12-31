/*
 *
 * ©K. D. Hedger. Thu 23 Dec 20:36:49 GMT 2021 keithdhedger@gmail.com

 * This file (QT_menuitem.h) is part of KKEditQT.

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

#ifndef _QT_MENUITEM_
#define _QT_MENUITEM_

#include "kkedit-includes.h"

//typedef void (*menuCallbackVoid)(Widget* widget,long data);
//typedef bool (*menuCallbackBool)(Widget* widget,long data);


class MenuItemClass : public QAction
{
	Q_OBJECT

	public:
		explicit MenuItemClass(const QString &text, QWidget *parent = 0);
		~MenuItemClass();
		KKEditClass			*mainKKEditClass=NULL;

		void				setMenuID(unsigned int id);
		unsigned int 		getMenuID(void);
		void				setMenuString(QString str);
		QString				getMenuString(void);
		//void				setCallBackVoid(menuCallbackVoid func);
		//void				setCallBackBool(menuCallbackBool func);

	public slots:
		void				menuClickedGotoLine();

	private:
		int					menuID;
		QString				menuString;
		//menuCallbackVoid	callbackVoid;
		//menuCallbackBool	callbackBool;

	private slots:
		void				menuClickedVoid(void);
		bool				menuClickedBool(void);
};

#endif

