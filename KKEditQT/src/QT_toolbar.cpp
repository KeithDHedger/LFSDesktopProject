/*
 *
 * ©K. D. Hedger. Sun  9 Jan 13:11:32 GMT 2022 keithdhedger@gmail.com

 * This file (QT_toolbar.cpp) is part of KKEditQT.

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

ToolBarClass::~ToolBarClass()
{
}

ToolBarClass::ToolBarClass(KKEditClass *kk,QWidget *parent): QToolBar(parent)
{
	this->mainKKEditClass=kk;
	this->setAcceptDrops(true);
}

void ToolBarClass::dragMoveEvent(QDragMoveEvent *event)
{
	if((event->mimeData()->hasUrls()==true))
		event->accept();
	else
		QToolBar::dragMoveEvent(event);
}

void ToolBarClass::dragEnterEvent(QDragEnterEvent* event)
{
	if((event->mimeData()->hasUrls()==true))
		event->accept();
    else
		QToolBar::dragEnterEvent(event);
}

void ToolBarClass::dropEvent(QDropEvent* event)
{
 	if (event->mimeData()->hasUrls())
		{
			if(event->mimeData()->urls().isEmpty()==false)
				{
					for(int j=0;j<event->mimeData()->urls().count();j++)
						this->mainKKEditClass->openFile(event->mimeData()->urls().at(j).toLocalFile());
				}
			event->accept();
		}
	QToolBar::dropEvent(event);
}
