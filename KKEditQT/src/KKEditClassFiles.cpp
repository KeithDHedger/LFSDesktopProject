/*
 *
 * ©K. D. Hedger. Fri 26 Nov 17:34:26 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClassFiles.cpp) is part of KKEditQT.

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

void KKEditClass::openAsHexDump(void)
{
	QStringList fileNames;
	QString		dump;
	FILE		*fp=NULL;
	char		line[1024];
	QString		command;

	fileNames=QFileDialog::getOpenFileNames(this->mainWindow,"Open File","","",0);
	if(fileNames.count())
		{
			for (int j=0;j<fileNames.size();j++)
				{
					command=QString("hexdump -C %1").arg(fileNames.at(j));
					dump="";
					fp=popen(command.toStdString().c_str(), "r");
					if(fp!=NULL)
						{
							QFile		file(fileNames.at(j));
							QFileInfo	fileinfo(file);
							while(fgets(line,1024,fp))
								dump+=line;
							pclose(fp);
							this->newFile(dump,QString("%1.hexdump").arg(fileinfo.fileName()));
						}
			}
		}
}
