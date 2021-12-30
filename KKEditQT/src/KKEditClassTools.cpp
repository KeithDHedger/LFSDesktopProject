/*
 *
 * ©K. D. Hedger. Thu 30 Dec 16:36:29 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClassTools.cpp) is part of KKEditQT.

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

#include "KKEditClass.h"

void KKEditClass::rebuildToolsMenu(void)
{
	QFile		file;
	QStringList	sl;
	QString		toolsdir=toolsFolder;
	QDir		dir(toolsdir);
	QStringList	flist=dir.entryList(QDir::Files);

	this->toolSelect->clear();
	this->toolSelect->addItem("New Tool");

	for(int k=0; k<flist.count(); k++)
		{
			file.setFileName(toolsdir + "/" + flist.at(k));
			if(file.open(QIODevice::Text | QIODevice::ReadOnly))
				{
					QString line;
					QTextStream	in(&file);
					sl=QTextStream(&file).readAll().split("\n",Qt::SkipEmptyParts);

					if((sl.count()>2) && (sl.at(0).startsWith("name")) && (sl.at(1).startsWith("command")) && (sl.at(2).startsWith("comment")))
						{
							this->toolSelect->addItem(flist.at(k));
							//QTextStream(stdout) << flist.at(k) << Qt::endl;
							//for( QStringList::Iterator it = sl.begin(); it != sl.end(); ++it)
							//	QTextStream(stdout) << ">>" << *it << "<<<" << "\n";
						}
					file.close();

				}
		}
}

//void KKEditClass::buildTools(void)
//{
//}
