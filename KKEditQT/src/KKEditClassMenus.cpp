/*
 *
 * ©K. D. Hedger. Thu 18 Nov 11:47:07 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClassMenus.cpp) is part of KKEdit.

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

#include "KKEditClass.h"

MenuItemClass* KKEditClass::makeMenuItemClass(int mainmenu,const QString name,const QKeySequence key,const QString iconname,const QString objectname,int userdata)
{
	MenuItemClass	*menuitem=new MenuItemClass(name);
	QIcon			itemicon=QIcon::fromTheme(iconname,QIcon(iconname));

	menuitem->setMenuID(userdata);
	menuitem->setIcon(itemicon);
	if(key!=0)
		menuitem->setShortcut(key);
	menuitem->setObjectName(objectname);

	switch(mainmenu)
		{
			case FILEMENU:
				this->fileMenu->addAction(menuitem);
				QObject::connect(menuitem,SIGNAL(triggered()),this,SLOT(doFileMenuItems()));
				break;
			case EDITMENU:
				this->editMenu->addAction(menuitem);
				QObject::connect(menuitem,SIGNAL(triggered()),this,SLOT(doEditMenuItems()));
				break;
			case VIEWMENU:
				this->viewMenu->addAction(menuitem);
				QObject::connect(menuitem,SIGNAL(triggered()),this,SLOT(doViewMenuItems()));
				break;
			case NAVMENU:
				this->navMenu->addAction(menuitem);
				QObject::connect(menuitem,SIGNAL(triggered()),this,SLOT(doNavMenuItems()));
				break;
			case BOOKNARKSMENU:
				this->bookMarkMenu->addAction(menuitem);
				QObject::connect(menuitem,SIGNAL(triggered()),this,SLOT(doBookmarkMenuItems()));
				break;
			case HELPMENU:
				this->helpMenu->addAction(menuitem);
				QObject::connect(menuitem,SIGNAL(triggered()),this,SLOT(doHelpMenuItems()));
				break;
			case TOOLSMENU:
				this->toolsMenu->addAction(menuitem);
				QObject::connect(menuitem,SIGNAL(triggered()),this,SLOT(doToolsMenuItems()));
				break;
			//case TABCONTEXTMENU:
			//	break;
		}

	return(menuitem);
}
