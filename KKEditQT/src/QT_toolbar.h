/*
 *
 * ©K. D. Hedger. Sun  9 Jan 13:11:44 GMT 2022 keithdhedger@gmail.com

 * This file (QT_toolbar.h) is part of KKEditQT.

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

#ifndef _QT_TOOLBAR_
#define _QT_TOOLBAR_

class KKEditClass;

#include "KKEditClass.h"

class ToolBarClass : public QToolBar
{
	Q_OBJECT

	public:
		explicit ToolBarClass(KKEditClass *kk,QWidget *parent=0);
		~ToolBarClass();

		KKEditClass	*mainKKEditClass=NULL;

	protected:
//dand
		void		dragEnterEvent(QDragEnterEvent* event);
		void		dropEvent(QDropEvent* event);
		void		dragMoveEvent(QDragMoveEvent *event);

};

#endif
