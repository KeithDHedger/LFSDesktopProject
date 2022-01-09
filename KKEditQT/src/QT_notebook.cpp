/*
 *
 * ©K. D. Hedger. Sun  9 Jan 14:05:01 GMT 2022 keithdhedger@gmail.com

 * This file (QT_notebook.cpp) is part of KKEditQT.

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

 
#include "QT_toolbar.h"

NoteBookClass::~NoteBookClass()
{
}

NoteBookClass::NoteBookClass(KKEditClass *kk,QWidget *parent): QTabWidget(parent)
{
	this->mainKKEditClass=kk;
	this->setAcceptDrops(true);
}

void NoteBookClass::dragMoveEvent(QDragMoveEvent *event)
{
	int tabIndex=this->mainKKEditClass->tabBar->tabAt(event->pos());
	this->setCurrentIndex(tabIndex);

	if((event->mimeData()->hasUrls()==true) || (event->mimeData()->hasText()))
		event->accept();
	else
		QTabWidget::dragMoveEvent(event);
}

void NoteBookClass::dragEnterEvent(QDragEnterEvent* event)
{
	if((event->mimeData()->hasUrls()==true) || (event->mimeData()->hasText()))
		event->accept();
	else
		QTabWidget::dragEnterEvent(event);
}

void NoteBookClass::dropEvent(QDropEvent* event)
{
 	if(event->mimeData()->hasUrls())
		{
			if(event->mimeData()->urls().isEmpty()==false)
				{
					for(int j=0;j<event->mimeData()->urls().count();j++)
						this->mainKKEditClass->openFile(event->mimeData()->urls().at(j).toLocalFile());
				}
			event->accept();
			return;
		}

	QTabWidget::dropEvent(event);
}
