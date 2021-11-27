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

QString KKEditClass::runPipeAndCapture(QString command)
{
	QString		dump("");
	FILE		*fp=NULL;
	char		line[1024];

	fp=popen(command.toStdString().c_str(), "r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				dump+=line;
			pclose(fp);
		}
	return(dump);
}

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
					dump=this->runPipeAndCapture(QString("hexdump -C %1").arg(fileNames.at(j)));
					QFile		file(fileNames.at(j));
					QFileInfo	fileinfo(file);
					this->newFile(dump,QString("%1.hexdump").arg(fileinfo.fileName()));
				}
		}
}

VISIBLE void KKEditClass::newFile(const QString data,const QString filename)
{
	DocumentClass*	doc;

	sessionBusy=true;
	doc=new DocumentClass(kkedit);
	doc->setPlainText(data);
	doc->tabNumber=qobject_cast<QTabWidget*>(kkedit->mainNotebook)->addTab(doc,"");
	if(filename.compare("")==0)
		doc->setFileName(QString("Untitled-%1").arg(untitledNumber));
	else
		doc->setFileName(filename);
	doc->setTabName(truncateWithElipses(doc->getFileName(),this->prefsMaxTabChars));
	this->mainNotebook->setTabToolTip(doc->tabNumber,doc->getFileName());
	doc->setFilePrefs();
	doc->mimeType="text/plain";
	doc->pageIndex=this->newPageIndex;
	this->pages[this->newPageIndex++]=doc;
	doc->setHiliteLanguage();
	untitledNumber++;
	this->mainNotebook->setCurrentWidget(doc);
	sessionBusy=false;
}

bool KKEditClass::saveFile(void)
{
	DocumentClass	*doc=this->getDocumentForTab(-1);
	QFile			file;
	QFileInfo		fileinfo;
	bool			retval=false;

	if(doc==NULL)
		return(false);

	if(doc->getFilePath().isEmpty()==true)
		{
			fprintf(stderr,"filepath not set\n");
			 QString fileName = QFileDialog::getSaveFileName(this->mainWindow,"Save File",doc->getFileName());
			 if(fileName.isEmpty()==true)
			 	return(false);
			 else
			 	{
					file.setFileName(fileName);
			 		fileinfo.setFile(file);
					retval=file.open(QIODevice::Text | QIODevice::WriteOnly);
					if(retval==true)
						{
					 		doc->setDirPath(fileinfo.canonicalPath());
							doc->setFilePath(fileinfo.canonicalFilePath());
							doc->setFileName(fileinfo.fileName());
							this->mainNotebook->setTabToolTip(doc->tabNumber,doc->getFilePath());
							QTextStream(&file) << doc->toPlainText() << Qt::endl;
							doc->dirty=false;
							doc->setTabName(truncateWithElipses(doc->getFileName(),this->prefsMaxTabChars));
							file.close();
						}
					else
						{
							QMessageBox *msg=new QMessageBox(QMessageBox::Warning,QString("Save File"),QString("Cant save file \"%1\"").arg(doc->getFileName()),QMessageBox::Ok,this->mainWindow,Qt::Dialog);
							msg->exec();
							delete msg;
						}
			 	}
		}
	else
		{
			file.setFileName(doc->getFilePath());
			fileinfo.setFile(file);
			retval=file.open(QIODevice::Text | QIODevice::WriteOnly);
			if(retval==true)
				{
					QTextStream(&file) << doc->toPlainText() << Qt::endl;
					doc->dirty=false;
					doc->setTabName(truncateWithElipses(doc->getFileName(),this->prefsMaxTabChars));
					file.close();
				}
			else
				{
					QMessageBox *msg=new QMessageBox(QMessageBox::Warning,QString("Save File"),QString("Cant save file \"%1\"").arg(doc->getFileName()),QMessageBox::Ok,this->mainWindow,Qt::Dialog);
					msg->exec();
					delete msg;
				}
		}
	return true;
#if 0
	DocumentClass	*page=this->getDocumentForTab(-1);
	FILE			*fd=NULL;

	if(page==NULL)
		return(false);

//	line=page->textCursor().blockNumber();
	if(page->getPathname()!=NULL && data==0)
		{
			fd=fopen(page->getPathname(),"w");
			if (fd!=NULL)
				{
					fputs(page->toPlainText().toLocal8Bit().constData(),fd);
					fclose(fd);
					page->document()->setModified(false);
				}
			else
				{
					QMessageBox *msg=new QMessageBox(QMessageBox::Warning,QString("Save File"),QString("Cant save file \"%1\"").arg(page->getPathname()),QMessageBox::Ok,kkedit->mainWindow,Qt::Dialog);
					msg->exec();
					delete msg;
				}
		}
	else
		{
			if(data!=0)
				{
					saveFilePath=page->getPathname();
					saveFileName=page->getFilename();
					page->setDirname(g_path_get_dirname(page->getPathname()));
				}

			saveFileName=page->getFilename();
			if(getSaveFile()==false)
				return(false);

			fd=fopen(saveFilePath,"w");
			if (fd!=NULL)
				{
					page->setPathname((char*)saveFilePath);
					page->setFilename((char*)saveFileName);
					page->setDirname(g_path_get_dirname(page->getPathname()));
					fputs(page->toPlainText().toLocal8Bit().constData(),fd);
					fclose(fd);
					page->document()->setModified(false);
				}
		}

	saveFileName=NULL;
	saveFilePath=NULL;
#endif
	return(true);
}
