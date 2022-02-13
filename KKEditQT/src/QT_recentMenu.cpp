/*
 *
 * ©K. D. Hedger. Sun 13 Feb 13:33:15 GMT 2022 keithdhedger@gmail.com

 * This file (QT_recentMenu.cpp) is part of KKEditQT.

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

#include "QT_recentMenu.h"

RecentMenuClass::~RecentMenuClass()
{
	this->mainKKEditClass->runPipe(QString("tail -n%1 %2 > %3.bak").arg(this->maxFiles).arg(this->recentFileList).arg(this->recentFileList));
	this->mainKKEditClass->runPipe(QString("mv %1.bak %2").arg(this->recentFileList).arg(this->recentFileList));
}

RecentMenuClass::RecentMenuClass(KKEditClass *kk)
{
	QIcon	itemicon=QIcon::fromTheme("document-open");

	this->recentMenu=new QMenu("Open Recent");
	QObject::connect(this->recentMenu,SIGNAL(triggered(QAction*)),this,SLOT(menuClicked(QAction*)));
	this->recentMenu->setIcon(itemicon);

	this->mainKKEditClass=kk;
	this->recentFileList=QString("%1/recentfiles").arg(kk->homeDataFolder);
}

void RecentMenuClass::addFilePath(QString path)
{
	QFile	file;
	bool	retval;

	file.setFileName(this->recentFileList);
	retval=file.open(QIODevice::Text | QIODevice::Append);
	if(retval==true)
		{
			QTextStream(&file) << path << Qt::endl;
			file.close();
			this->updateRecents();
		}
}

void RecentMenuClass::updateRecents(void)
{
	QFile			file;
	MenuItemClass	*mc;
	bool			retval;
	int				diff;
	int				cnt=0;

	this->recentMenu->clear();
	file.setFileName(this->recentFileList);

	retval=file.open(QIODevice::Text | QIODevice::ReadOnly);
	if(retval==true)
		{
			QTextStream	in(&file);
			QString line;
			while(in.atEnd()==false)
				{
					line=in.readLine().trimmed();
					this->recentMenu->addAction(line);
				}
			file.close();
		}

	QList<QAction*>	acts=this->recentMenu->actions(); 
	if(acts.count()>this->maxFiles)
		{
			diff=acts.count()-this->maxFiles;
			for(int j=0;j<diff;j++)
				this->recentMenu->removeAction(acts.at(cnt++));
		}
}

void RecentMenuClass::menuClicked(QAction *action)
{
	this->mainKKEditClass->openFile(action->text());
}
