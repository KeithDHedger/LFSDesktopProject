/*
 *
 * ©K. D. Hedger. Sun  9 Jan 14:04:55 GMT 2022 keithdhedger@gmail.com

 * This file (QT_notebook.h) is part of KKEditQT.

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

#ifndef _QT_NOTEBOOK_
#define _QT_NOTEBOOK_

class KKEditClass;

#include "KKEditClass.h"

class NoteBookClass : public QTabWidget
{
	Q_OBJECT

	public:
		explicit NoteBookClass(KKEditClass *kk,QWidget *parent=0);
		~NoteBookClass();

		KKEditClass	*mainKKEditClass=NULL;

	protected:
//dand
		void		dragEnterEvent(QDragEnterEvent* event);
		void		dropEvent(QDropEvent* event);
		void		dragMoveEvent(QDragMoveEvent *event);

};


#endif
