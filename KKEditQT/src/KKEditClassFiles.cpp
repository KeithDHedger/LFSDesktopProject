/*
 *
 * ©K. D. Hedger. Thu 23 Dec 20:39:15 GMT 2021 keithdhedger@gmail.com

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

void KKEditClass::runPipe(QString command)
{
	FILE		*fp=NULL;
	char		line[1024];

	fp=popen(command.toStdString().c_str(), "r");
	if(fp!=NULL)
		pclose(fp);
}

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

void KKEditClass::newFile(const QString data,const QString filename)
{
	DocumentClass	*doc;
	int				tabnum;

	this->sessionBusy=true;
	doc=new DocumentClass(this);
	doc->setPlainText(data);
	tabnum=this->mainNotebook->addTab(doc,"");
	if(filename.compare("")==0)
		doc->setFileName(QString("Untitled-%1").arg(this->untitledNumber));
	else
		doc->setFileName(filename);
	doc->setTabName(this->truncateWithElipses(doc->getFileName(),this->prefsMaxTabChars));
	this->mainNotebook->setTabToolTip(tabnum,doc->getFileName());
	doc->setFilePrefs();
	doc->mimeType="text/plain";
	doc->pageIndex=this->newPageIndex;
	this->pages[this->newPageIndex++]=doc;
	doc->setHiliteLanguage();
	this->untitledNumber++;
	this->mainNotebook->setCurrentWidget(doc);
	this->rebuildTabsMenu();
	this->sessionBusy=false;
	this->setToolbarSensitive();
}

int KKEditClass::askSaveDialog(const QString filename)
{
	QMessageBox msgBox;

	msgBox.setIcon(QMessageBox::Warning);
	msgBox.setText(filename+" has been modified.");
	msgBox.setInformativeText("Do you want to save your changes?");
	msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Cancel);
	return(msgBox.exec());
}

bool KKEditClass::saveFileAs(int tabnum)
{
	DocumentClass	*doc=this->getDocumentForTab(tabnum);
	QFile			file;
	QFileInfo		fileinfo;
	bool			retval=false;
	QString			dialogpath;
	int				calctabnum=this->mainNotebook->indexOf(doc);

	if(doc->getFilePath().isEmpty()==true)
		dialogpath=getenv("HOME") + QString("/") + doc->getFileName();
	else
		dialogpath=doc->getFilePath();

	QString fileName = QFileDialog::getSaveFileName(this->mainWindow,"Save File",dialogpath);

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
					this->mainNotebook->setTabToolTip(calctabnum,doc->getFilePath());
					QTextStream(&file) << doc->toPlainText() << Qt::endl;
					doc->dirty=false;
					doc->setTabName(this->truncateWithElipses(doc->getFileName(),this->prefsMaxTabChars));
					file.close();
					this->recentFiles->addFilePath(doc->getFilePath());
				}
			else
				{
					QMessageBox *msg=new QMessageBox(QMessageBox::Warning,QString("Save File"),QString("Cant save file \"%1\"").arg(doc->getFileName()),QMessageBox::Ok,this->mainWindow,Qt::Dialog);
					msg->exec();
					delete msg;
				}
		}
	this->setToolbarSensitive();
	return(retval);
}

bool KKEditClass::saveFile(int tabnum,bool ask)
{
	DocumentClass	*doc=this->getDocumentForTab(tabnum);
	QFile			file;
	QFileInfo		fileinfo;
	bool			retval=false;

	if(doc==NULL)
		return(false);

	if((doc->dirty==true) && (ask==true))
		{
			int result=this->askSaveDialog(doc->fileName);
			switch(result)
				{
					case QMessageBox::Save:
						break;
					case QMessageBox::Discard:
       					return(true);
						break;
					case QMessageBox::Cancel:
						this->sessionBusy=false;
						return(false);
						break;
				}
		}

	if((doc->getFilePath().isEmpty()==true) && (doc->dirty==true))
		return(this->saveFileAs(tabnum));
	else
		{
			if(doc->dirty==false)
				return(true);
			file.setFileName(doc->getFilePath());
			fileinfo.setFile(file);
			retval=file.open(QIODevice::Text | QIODevice::WriteOnly);
			if(retval==true)
				{
					QTextStream(&file) << doc->toPlainText() << Qt::endl;
					doc->dirty=false;
					doc->setTabName(this->truncateWithElipses(doc->getFileName(),this->prefsMaxTabChars));
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
}

bool KKEditClass::saveAllFiles(void)
{
	this->sessionBusy=true;

	int	numtabs=this->mainNotebook->count();
	for(int loop=0;loop<numtabs;loop++)
		{
			if(this->saveFile(loop)==false)
				{
					this->sessionBusy=false;
					return(false);;
				}
		}
	this->sessionBusy=false;
	return(true);
}

void KKEditClass::newEditor(int what)
{
	QString	command;

	switch(what)
		{
			case NEWADMINEDMENUITEM:
				command=QString("\"%1\" kkedit -m 2>&1 >/dev/null &").arg(this->prefsRootCommand);
				runPipe(command);
				break;
		case NEWEDMENUITEM:
			runPipe("kkedit -m 2>&1 >/dev/null &");
			break;
		case MANPAGEEDMENUITEM:
			if(this->gotManEditor==0)
				runPipe("manpageeditor 2>&1 >/dev/null &");
			break;
		}
}

bool KKEditClass::checkForOpenFile(QString filepath)
{
	DocumentClass	*doc;
	for(int tabs=0;tabs<this->mainNotebook->count();tabs++)
		{
			doc=this->getDocumentForTab(tabs);
			if((doc->filePath!=NULL) && (doc->filePath.compare(filepath)==0))
				{
					this->mainNotebook->setCurrentIndex(tabs);
					return(true);
				}
		}
	return(false);
}
bool KKEditClass::openFile(QString filepath,int linenumber,bool warn)
{
	DocumentClass	*doc=new DocumentClass(this);
	bool			retval=false;
	QFile			file(filepath);
	QFileInfo		fileinfo(file);
	int				tabnum;
	QMimeDatabase	db;
	QMimeType		type;
	QString			content;

	//this->sessionBusy=true;
	if((this->prefsNoOpenduplicate==true) && (this->checkForOpenFile(filepath)==true))
		return(true);

	retval=file.open(QIODevice::Text | QIODevice::ReadOnly);
	if(retval==true)
		{
			content=QString::fromUtf8(file.readAll());
			type=db.mimeTypeForFile(fileinfo.canonicalFilePath());
			doc->mimeType=type.name();
			doc->setPlainText(content);
			doc->setFilePrefs();
			doc->pageIndex=this->newPageIndex;
			this->pages[this->newPageIndex++]=doc;
			tabnum=this->mainNotebook->addTab(doc,doc->getTabName());
			doc->setDirPath(fileinfo.canonicalPath());
			doc->setFilePath(fileinfo.canonicalFilePath());
			doc->setFileName(fileinfo.fileName());
			doc->setTabName(this->truncateWithElipses(doc->getFileName(),this->prefsMaxTabChars));
			this->mainNotebook->setTabToolTip(tabnum,doc->getFilePath());
			this->mainNotebook->setCurrentIndex(tabnum);
			this->gotoLine(linenumber);
			doc->document()->clearUndoRedoStacks(QTextDocument::UndoAndRedoStacks);
			doc->setHiliteLanguage();
#ifndef _USEMINE_
			//doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCpp);
//			QSourceHighliter::Themes theme=(QSourceHighliter::Themes)-1;
//			if(this->prefsSyntaxHilighting==true)
//				theme=(QSourceHighliter::Themes)2;//TODO//get theme from file
//
//			doc->highlighter->setTheme(theme);
#endif
			doc->highlighter->rehighlight();
			doc->dirty=false;

			retval=true;
			file.close();
			this->recentFiles->addFilePath(filepath);
		}

	this->rebuildTabsMenu();
	doc->dirty=false;

	if(this->openFromDialog==false)
		this->switchPage(tabnum);

	this->setToolbarSensitive();
	//this->sessionBusy=false;
	return(retval);
}

QStringList KKEditClass::getNewRecursiveTagList(QString filepath)
{
	QString		sort;
	QString		command;
	QStringList	retval;
	QString		results;

	switch (this->prefsFunctionMenuLayout)
		{
			case 0:
				sort="sort -k 2rb,2rb -k 1b,1b";
				break;
			case 1:
				sort="sort -k 2rb,2rb -k 3n,3n";
				break;
			case 2:
				sort="sort -k 3n";
				break;
			case 4:
				sort="sort -k 2rb,2rb -k 1b,1b";
				break;
			default:
				sort="sort";
				break;
		}

	command=QString("find \"%1\" -maxdepth %2|ctags -L - -x|%3|sed 's@ \\+@ @g'").arg(filepath).arg(this->prefsDepth).arg(sort);
	results=this->runPipeAndCapture(command);
	retval=results.split("\n",Qt::SkipEmptyParts);

	return(retval);
}
