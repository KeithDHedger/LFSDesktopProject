/*
 *
 * ©K. D. Hedger. Thu 23 Dec 20:38:31 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClassCallbacks.cpp) is part of KKEditQT.

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

void KKEditClass::doSessionsMenuItems(void)
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	QFile			file;
	bool			retval;
	DocumentClass	*doc;
	bookMarkStruct	bm;
	QString			sessionname;
	QString			tempstr;
	MenuItemClass	*menuItemSink;
	unsigned int	sessionnumber=mc->getMenuID();

	if(sender()->objectName().compare(SAVESESSIONMENUNAME)==0)
		{
			if(sessionnumber==CURRENTSESSION)
				{
					if(this->currentSessionNumber==0xdeadbeef)
						return;
					sessionnumber=this->currentSessionNumber;
				}

			sessionname=QString("Session-%1").arg(sessionnumber);
			file.setFileName(QString("%1/Session-%2").arg(this->sessionFolder).arg(sessionnumber));
//get sesion name
			retval=file.open(QIODevice::Text | QIODevice::ReadOnly);
			if(retval==true)
				{
					QTextStream	in(&file);
					tempstr=in.readLine();//sessionname
					file.close();
					if(tempstr.isEmpty()==false)
						{
							sessionname=tempstr;
						}
				}
	
			retval=file.open(QIODevice::Text | QIODevice::WriteOnly);
			if(retval==true)
				{
					int		linenumber=10;
					bool	visible=true;
					bool	ok;

					if(mc->getMenuID()!=CURRENTSESSION)
						{
							QString	text=QInputDialog::getText(this->mainWindow,"Session Name","Enter Session Name",QLineEdit::Normal,sessionname,&ok);
							if ((ok==true) && (!text.isEmpty()))
								{
									sessionname=(text);
									mc->setText(sessionname);
									this->sessionNames[mc->getMenuID()]=sessionname;
								}
						}
					else
						{
							sessionname=this->sessionNames.value(sessionnumber);
						}

					this->restoreSessionsMenu->clear();
					this->restoreSessionsMenu->addAction(this->restoreDefaultSessionMenuItem);
					this->restoreSessionsMenu->addSeparator();
					this->restoreSessionMenuItemsList.clear();
					for(int j=1;j<MAXSESSIONS;j++)
						{
							menuItemSink=this->makeMenuItemClass(RESTORESESSIONSMENU,this->sessionNames.value(j),0,NULL,RESTORESESSIONMENUNAME,j);
							this->restoreSessionMenuItemsList.append(menuItemSink);
						}

					QTextStream(&file) << sessionname << Qt::endl;
					for(int j=0;j<this->mainNotebook->count();j++)
						{
							doc=this->getDocumentForTab(j);
							linenumber=doc->getCurrentLineNumber();

							QTextStream(&file) << linenumber << " " << visible << " " << doc->getFilePath() << Qt::endl;
							foreach(bookMarkStruct bm, this->bookMarks)
								{
									if(doc->pageIndex==bm.docIndex)
										QTextStream(&file) << bm.line << " " << bm.bmLabel << Qt::endl;
								}
							QTextStream(&file) << "-1 endmarks" << Qt::endl;
						}
					file.close();
				}
			return;
		}

	if(sender()->objectName().compare(RESTORESESSIONMENUNAME)==0)
		{
			sessionnumber=mc->getMenuID();
			if(sessionnumber==CURRENTSESSION)
				return;
			this->closeAllTabs();
			this->sessionBusy=true;
			file.setFileName(QString("%1/Session-%2").arg(this->sessionFolder).arg(sessionnumber));
			retval=file.open(QIODevice::Text | QIODevice::ReadOnly);
			if(retval==true)
				{
					this->showBarberPole("Restore Session","Please Wait","Cancel",QString("%1/session").arg(this->tmpFolderName));
					int		linenumber=999;
					int		visible=666;
					int		mainline;
					QTextStream	in(&file);
					QString line;
					line=in.readLine();//sessionname
					while(in.atEnd()==false)
						{
							in >> mainline;
							in >> visible;
							line=in.readLine().trimmed();
							linenumber=-1;
							this->runPipe(QString("echo \"Opening %1 ...\">\"%2/session\" &").arg(line.trimmed()).arg(this->tmpFolderName));
							this->openFile(line);
							do
								{
									in >> linenumber;
									line=in.readLine().trimmed();
									if(linenumber>0)
										{
											this->gotoLine(linenumber);
											this->handleBMMenu(this->mainNotebook->currentWidget(),TOGGLEBOOKMARKMENUITEM);
										}
								}
							while(linenumber!=-1);
							this->gotoLine(mainline);
						}
					this->currentSessionNumber=sessionnumber;
					file.close();
					this->runPipe(QString("echo quit>\"%1/session\"").arg(this->tmpFolderName));
				}
		}

	this->sessionBusy=false;
	this->switchPage(this->mainNotebook->count()-1);
	this->setToolbarSensitive();
}

void KKEditClass::doSelectTab()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	QTabBar			*bar=this->mainNotebook->tabBar();

	bar->setTabVisible(mc->getMenuID(),true);
	bar->setCurrentIndex(mc->getMenuID());
}

void KKEditClass::doBookmarkMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*document=this->getDocumentForTab(-1);

	switch(mc->getMenuID())
		{
			case REMOVEALLBOOKMARKSMENUITEM:
				this->rebuildBookMarkMenu();
				break;
			case TOGGLEBOOKMARKMENUITEM:
				this->handleBMMenu(this->mainNotebook->currentWidget(),TOGGLEBOOKMARKMENUITEM);
				break;
			default:
				this->handleBMMenu(this->mainNotebook->currentWidget(),mc->getMenuID());
				break;
		}
}

void KKEditClass::doToolsMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*document=this->getDocumentForTab(-1);
	QFile			file;
	QStringList		sl;
	int				holdindex;

	switch(mc->getMenuID())
		{
			 case MANAGETOOLSMENUITEM:
			 	this->toolsWindow->show();
			 	break;

			 default:
			 	sl=this->verifyTool(mc->getMenuString());
			 	if(sl.isEmpty()==true)
			 		return;

				if(sl.at(TOOL_COMMAND).section(TOOLCOMMAND,1,1).trimmed().isEmpty()==false)
					{
						QString str=sl.at(TOOL_COMMAND).section(TOOLCOMMAND,1,1).trimmed();
						if(document!=NULL)
							{
								//%d doc folder
								setenv("KKEDIT_CURRENTDIR",document->getDirPath().toStdString().c_str(),1);
								str.replace("%d",document->getDirPath());
								//%f doc filepath
								setenv("KKEDIT_CURRENTFILE",document->getFilePath().toStdString().c_str(),1);
								str.replace("%f",document->getFilePath());
								//%t selected text
										setenv("KKEDIT_SELECTION",document->textCursor().selectedText().replace(QRegularExpression("\u2029|\\r\\n|\\r"),"\n").toStdString().c_str(),1);
										str.replace("%t",document->textCursor().selectedText());
										//%m
										setenv("KKEDIT_MIMETYPE",document->mimeType.toStdString().c_str(),1);
										str.replace("%m",document->mimeType);
									}

								//%h html file
								setenv("KKEDIT_HTMLFILE",this->htmlFile.toStdString().c_str(),1);
								str.replace("%h",this->htmlFile);
								//%i
								setenv("KKEDIT_DATADIR","TODO",1);
								str.replace("%i","TODO");

								int trm=sl.indexOf(QRegularExpression(QString(".*%1.*").arg(TOOLRUNINTERM)));//todo//
								
								DEBUGSTR( "prefsTerminalCommand=" << this->prefsTerminalCommand << " str=" << str << "<<" << sl.at(trm).section(TOOLRUNINTERM,1,1).trimmed() )
								if(sl.at(trm).section(TOOLRUNINTERM,1,1).trimmed().toInt()==1)
									{
										str=this->prefsTerminalCommand + " " + str;
										str=QString("cd %1;%2").arg(this->toolsFolder).arg(str);
										runPipe(str);
										return;
									}
								
								if(((sl.at(TOOL_FLAGS).section(TOOLFLAGS,1,1).trimmed().toInt() & TOOL_PASTE_OP)==TOOL_PASTE_OP) && (document!=NULL))
									{
										str=QString("cd %1;%2").arg(this->toolsFolder).arg(str);
										document->textCursor().removeSelectedText();
										document->textCursor().insertText(runPipeAndCapture(str));
										return;
									}

								if(((sl.at(TOOL_FLAGS).section(TOOLFLAGS,1,1).trimmed().toInt() & TOOL_REPLACE_OP)==TOOL_REPLACE_OP) && (document!=NULL))
									{
										str=QString("cd %1;%2").arg(this->toolsFolder).arg(str);
										document->setPlainText(runPipeAndCapture(str));
										return;
									}

								if((sl.at(TOOL_FLAGS).section(TOOLFLAGS,1,1).trimmed().toInt() & TOOL_VIEW_OP)==TOOL_VIEW_OP)//TODO//
									{
										str=QString("cd %1;%2").arg(this->toolsFolder).arg(str);
										str=runPipeAndCapture(str);
										QMessageBox mes(QMessageBox::Information,"Tool Output",str,QMessageBox::Ok);
										mes.exec();
										return;
									}

								if((sl.at(TOOL_FLAGS).section(TOOLFLAGS,1,1).trimmed().toInt() & TOOL_ASYNC)==TOOL_ASYNC)
									{
										str=QString("cd %1;%2").arg(this->toolsFolder).arg(str);
										str=runPipeAndCapture(str);
										return;
									}
	
								if((sl.at(TOOL_FLAGS).section(TOOLFLAGS,1,1).trimmed().toInt() & TOOL_SHOW_DOC)==TOOL_SHOW_DOC)
									{
										str=QString("cd %1;%2").arg(this->toolsFolder).arg(str);
										DEBUGSTR( str )
										runPipe(str);
										this->showWebPage(sl.at(TOOL_NAME).section(TOOLNAME,1,1).trimmed(),"file://" + this->htmlFile);
										return;
									}

								DEBUGSTR( str )
								runPipe(str);
							}
			 	break;
		}
}

void KKEditClass::doHelpMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*document=this->getDocumentForTab(-1);

	switch(mc->getMenuID())
		{
			 case ABOUTMENUITEM:
			 	{
					QString			content;
					QFile			licencefile(DATADIR "/docs/gpl-3.0.txt");
					bool			retval;
					AboutBoxClass	*about=new AboutBoxClass(this->mainWindow,DATADIR "/pixmaps/" PACKAGE ".png");

					retval=licencefile.open(QIODevice::Text | QIODevice::ReadOnly);
					if(retval==true)
						{
							content=QString::fromUtf8(licencefile.readAll());
							licencefile.close();
							about->setLicence(content);
						}
					about->setAuthors(defaultauthors);
					about->runAbout();
				}
			 	break;

			 case HELPMENUITEM:
			 	this->showWebPage("Help For " PACKAGE,"file://" DATADIR "/help/index.html");
			 	break;
			 case ONLINEHELPMENUITEM:
			 	this->showWebPage("Online Help For " PACKAGE,"https://keithdhedger.github.io/KKEdit/"); //TODO//online help
			 	break;
			 case GETPLUGSMENUITEM:
				DEBUGSTR("TODO get plugins ...")
			 	break;
		}
}

void KKEditClass::doNavMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*document=this->getDocumentForTab(-1);

	switch(mc->getMenuID())
		{
			case GOBACKMENUITEM:
				this->history->goBack();
				break;
			case GOFORWARDMENUITEM:
				this->history->goForward();
				break;
			case GOTODEFINEMENUITEM:
				this->goToDefinition("");
				break;
			case OPENINCLUDEMENUITEM:
				this->findFile();
				break;
			case GOTOLINEMENUITEM:
				this->showLineEntry();
				break;
			case SEARCHFORDEFINEMENUITEM:
				this->functionSearchDialog();
				break;
			case SEARCHGTKDOCS:
				this->searchAPIDocs("",0);
				break;
			case SEARCHQT5DOCS:
				this->searchAPIDocs("",1);
				break;
		}
}

void KKEditClass::doViewMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*document=this->getDocumentForTab(-1);

	switch(mc->getMenuID())
		{
			case DOCSMENUITEM:
				this->showDocs();
				break;
			case TOGGLETOOLBARMENUITEM:
				this->toolbarVisible=!this->toolbarVisible;
				if(this->toolbarVisible)
					{
						this->toggleToolBarMenuItem->setText("Hide Tool Bar");
						this->toolBar->show();
					}
				else
					{
						this->toggleToolBarMenuItem->setText("Show Tool Bar");
						this->toolBar->hide();
					}
				break;
			case TOGGLETOOLWINDOWMENUITEM:
				this->toolWindowVisible=!this->toolWindowVisible;
				if(this->toolWindowVisible)
					this->toggleToolWindowMenuItem->setText("Hide Tool Output");
				else
					this->toggleToolWindowMenuItem->setText("Show Tool Output");
				break;
			case TOGGLESTATUSBARMENUITEM:
				this->statusBarVisible=!this->statusBarVisible;
				if(this->statusBarVisible)
					{
						this->toggleStatusBarMenuItem->setText("Hide Status Bar");
						this->statusBar->show();
					}
				else
					{
						this->toggleStatusBarMenuItem->setText("Show Status Bar");
						this->statusBar->hide();
					}
				break;
			case TOGGLEDOCVIEWMENUITEM:
#ifdef _BUILDDOCVIEWER_
				this->docviewerVisible=!this->docviewerVisible;
				if(this->docviewerVisible)
					{
						this->toggleDocViewMenuItem->setText("Hide Docviewer");
						this->docView->show();
					}
				else
					{
						this->toggleDocViewMenuItem->setText("Show Docviewer");
						this->docView->hide();
					}
#endif
				break;
			case TOGGLELINENUMBERSMENUITEM:
				this->lineNumbersVisible=!this->lineNumbersVisible;
				this->resetAllFilePrefs();
				break;
			case TOGGLELINEWRAPMENUITEM:
				this->wrapLine=!this->wrapLine;
				this->resetAllFilePrefs();
				break;
			case TOGGLEHILITELINEMENUITEM:
				this->hiliteCurrentLine=!this->hiliteCurrentLine;
				this->resetAllFilePrefs();
				break;
			case TOGGLEWHITESPACEMENUITEM:
				this->showWhiteSpace=!this->showWhiteSpace;
				this->resetAllFilePrefs();
				break;
			case TOGGLESYNTAXHILITINGMENUITEM:
				this->showHighLighting=!this->showHighLighting;
				this->resetAllFilePrefs();
				break;
			case TOGGLECOMPLETIONSMENUITEM:
				this->showCompletions=!this->showCompletions;
				break;
		}
}

void KKEditClass::doEditMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*document=this->getDocumentForTab(-1);

	switch(mc->getMenuID())
		{
			case UNDOMENUITEM:
				if(document!=NULL)
					document->undo();
				break;
			case REDOMENUITEM:
				if(document!=NULL)
					document->redo();
				break;
			case UNDOALLMENUITEM:
				if(document!=NULL)
					while(document->document()->availableUndoSteps()>0)
						document->undo();
				break;
			case REDOALLMENUITEM:
				if(document!=NULL)
					{
						while(document->document()->availableRedoSteps()>0)
							document->redo();
						document->redo();
					}
				break;
			case CUTMENUITEM:
				if(document!=NULL)
					document->cut();
				break;
			case COPYMENUITEM:
				if(document!=NULL)
					document->copy();
				break;
			case PASTEMENUITEM:
				if(document!=NULL)
					document->paste();
				break;
			case FINDMENUITEM:
				this->findReplaceDialog->show();
			case FINDNEXTMENUITEM:
				doFindReplace(FINDNEXT);
				break;
			case SORTTABSMENUITEM:
				this->sortTabs();
				break;
			case SHOWALLTABSMENUITEM:
				for(int j=0;j<this->mainNotebook->count();j++)
					this->mainNotebook->setTabVisible(j,true);
				break;
			case PREFSMENUITEM:
				this->doPrefs();
				break;
			case PLUGPREFSMENUITEM:
				//doPlugPrefs(NULL,0);
				break;
		}
}

void KKEditClass::doFileMenuItems()
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	bool			retval;

	switch(mc->getMenuID())
		{
//file menu
			case NEWMENUITEM:
				this->newFile();
				break;
			case OPENMENUITEM:
				this->openFileDialog();
				break;
			case HEXDUMPMENUITEM:
				this->openAsHexDump();
				break;
			case NEWADMINEDMENUITEM:
				this->newEditor(NEWADMINEDMENUITEM);
				break;
			case NEWEDMENUITEM:
				this->newEditor(NEWEDMENUITEM);
				break;
			case
				MANPAGEEDMENUITEM:
				this->newEditor(MANPAGEEDMENUITEM);
				break;
			case BUILDDOCSMENUITEM:
				this->buildDocs();
				break;
			case SAVEMENUITEM:
				this->saveFile(-1,false);
				this->setToolbarSensitive();
				break;
			case SAVEASMENUITEM:
				this->saveFileAs(-1);
				break;
			case SAVEALLMENUITEM:
				this->saveAllFiles();
				break;
			case SAVESESSIONMENUITEM:
				//saveSession(NULL,SAVESESSIONMENUITEM);
				break;
			case RESTORESESSIONMENUITEM:
				//restoreSession(NULL,RESTORESESSIONMENUITEM);
				break;
			case PRINTMENUITEM:
				//printFile(NULL,PRINTMENUITEM);
				break;
			case CLOSEMENUITEM:
				this->closingAllTabs=false;
				retval=this->closeTab(-1);
				break;
			case CLOSEALLMENUITEM:
				this->closeAllTabs();
				break;
			case REVERTMENUITEM:
				this->reloadDocument();
				break;
			case QUITMENUITEM:
				this->shutDownApp();
				break;
		}
}

void KKEditClass::doTimer(void)
{
	int			retcode=0;
	msgStruct	buffer;

#ifdef _BUILDDOCVIEWER_
	this->setDocMenu();
#endif

	while(retcode!=-1)
		{
			buffer.mText[0]=0;
			retcode=msgrcv(this->queueID,&buffer,MAXMSGSIZE,MSGANY,IPC_NOWAIT);
			buffer.mText[retcode]=0;
			if(retcode!=-1)
				{
					switch(buffer.mType)
						{
							case OPENFILE:
								this->openFile(buffer.mText);
								break;
							case SAVEFILE:
								break;
							case QUITAPP:
								this->shutDownApp();
								break;
							case ACTIVATEAPP://TODO//minimized
								this->application->setActiveWindow(this->mainWindow);
								this->mainWindow->activateWindow();
								break;
							case OPENSESSION:
								for(int j=0;j<this->restoreSessionMenuItemsList.count();j++)
									{
										if(QString(buffer.mText).compare(this->restoreSessionMenuItemsList.at(j)->text())==0)
											emit this->restoreSessionMenuItemsList.at(j)->triggered();
									}
								break;
						}
				}
		}
}

void KKEditClass::doDoubleClickPrefs(QListWidgetItem *item)
{
	const char		*data="NOSXCPURFG9ADLsEQBW";
	int				row=listWidget->currentRow();
	QListWidgetItem	*titem=this->listWidget->takeItem(row);

	for(unsigned int j=0;j<strlen(data);j++)
		{
			if(item->type()==data[j])
				{
					this->tool[j]->setEnabled(true);
					break;
				}
		}
	delete titem;
}

void KKEditClass::setPreferences(void)
{
	this->prefsIndent=qobject_cast<QCheckBox*>(this->prefsWidgets[AUTOINDENT])->checkState();
	this->prefsLineWrap=qobject_cast<QCheckBox*>(this->prefsWidgets[WRAP])->checkState();
	this->prefsShowLineNumbers=qobject_cast<QCheckBox*>(this->prefsWidgets[SHOWNUMS])->checkState();
	this->prefsHighLightline=qobject_cast<QCheckBox*>(this->prefsWidgets[HIGHLIGHT])->checkState();
	this->prefsDepth=qobject_cast<QCheckBox*>(this->prefsWidgets[MAXFUNCDEPTH])->checkState();
	this->prefsTabWidth=qobject_cast<QSpinBox*>(this->prefsIntWidgets[TABWIDTH])->value();
	this->prefsMaxTabChars=qobject_cast<QSpinBox*>(this->prefsIntWidgets[MAXTABCHARS])->value();
	this->prefsMaxFuncChars=qobject_cast<QSpinBox*>(this->prefsIntWidgets[MENUWIDTH])->value();
	this->prefsFunctionMenuLayout=qobject_cast<QComboBox*>(this->prefsOtherWidgets[FUNCTIONCOMBO])->currentIndex();
	this->prefsSyntaxHilighting=qobject_cast<QCheckBox*>(this->prefsWidgets[SYNTAXHILITE])->checkState();
	this->prefsAutoShowCompletions=qobject_cast<QCheckBox*>(this->prefsWidgets[AUTOSHOW])->checkState();
	this->prefsNoOpenduplicate=qobject_cast<QCheckBox*>(this->prefsWidgets[NODUPLICATE])->checkState();
	this->recentFiles->maxFiles=qobject_cast<QSpinBox*>(this->prefsIntWidgets[MAXRECENTS])->value();
	this->recentFiles->updateRecents();

//term command
	this->prefsTerminalCommand=qobject_cast<QLineEdit*>(prefsOtherWidgets[PREFSTERMCOMMAND])->text();
//root command
	this->prefsRootCommand=qobject_cast<QLineEdit*>(prefsOtherWidgets[PREFSROOTCOMMAND])->text();
//qt doc dir
	this->prefsQtDocDir=qobject_cast<QLineEdit*>(prefsOtherWidgets[PREFSQTDOCDIR])->text();

	this->prefsWindow->hide();

	this->prefsToolBarLayout="";
	for (int j=0;j<this->listWidget->count();j++)
		this->prefsToolBarLayout+=this->listWidget->item(j)->type();

	this->setUpToolBar();
//show line numbers
	this->lineNumbersVisible=this->prefsShowLineNumbers;
	this->toggleLineNumbersMenuItem->setChecked(this->lineNumbersVisible);
//wrap lines
	this->wrapLine=this->prefsLineWrap;
	this->toggleWrapLineMenuItem->setChecked(this->wrapLine);
//hilite current line
	this->hiliteCurrentLine=this->prefsHighLightline;
	this->toggleHiliteCurrentLineMenuItem->setChecked(this->hiliteCurrentLine);
//syntax hiliting
	this->showHighLighting=this->prefsSyntaxHilighting;
	this->toggleSyntaxHilightingMenuItem->setChecked(this->showHighLighting);
//completions
	this->showCompletions=this->prefsAutoShowCompletions;
	this->toggleCompletionsMenuItem->setChecked(this->showCompletions);

	this->resetAllFilePrefs();
	this->writeExitData();
	this->setAppShortcuts();
	switchPage(this->mainNotebook->currentIndex());
}

void KKEditClass::setBMColour(void)
{
	QColor	tmpcolour;
	tmpcolour=QColorDialog::getColor(QColor(this->prefsBookmarkHiLiteColor),0,"Select Color",QColorDialog::ShowAlphaChannel);
	if(tmpcolour.isValid())
		{
			this->prefsOtherWidgets[BMHIGHLIGHTCOLOUR]->setProperty("palette",QPalette(tmpcolour));
			this->prefsOtherWidgets[BMHIGHLIGHTCOLOUR]->setProperty("autoFillBackground",true);
			this->prefsOtherWidgets[BMHIGHLIGHTCOLOUR]->setProperty("text",tmpcolour.name());
			this->prefsBookmarkHiLiteColor=tmpcolour;
		}
}

void KKEditClass::setLineColour(void)
{
	QColor	tmpcolour;
	tmpcolour=QColorDialog::getColor(QColor(this->prefsHiLiteLineColor),0,"Select Color",QColorDialog::ShowAlphaChannel);
	if(tmpcolour.isValid())
		{
			this->prefsOtherWidgets[CURRENTLINECOLOUR]->setProperty("palette",QPalette(tmpcolour));
			this->prefsOtherWidgets[CURRENTLINECOLOUR]->setProperty("autoFillBackground",true);
			this->prefsOtherWidgets[CURRENTLINECOLOUR]->setProperty("text",tmpcolour.name());
			this->prefsHiLiteLineColor=tmpcolour;
		}
}

void KKEditClass::setFont(void)
{
	bool	ok;
	QFont	tmpfont;

	tmpfont=QFontDialog::getFont(&ok,this->prefsDocumentFont,this->mainWindow);
	if(ok==true)
		{
			this->prefsDocumentFont=tmpfont;
			qobject_cast<QLabel*>(this->prefsOtherWidgets[PREFSCURRENTFONT])->setText(QString("%1 %2").arg(tmpfont.family()).arg(tmpfont.pointSize()));
		}
}

void KKEditClass::addToToolBar(void)
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());

	this->prefsToolBarLayout=(char*)qobject_cast<MenuItemClass*>(mc)->objectName().constData();
	this->populateStore();
}

void KKEditClass::doTabBarContextMenu(void)
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	bool			retval;
	QClipboard		*clipboard=this->application->clipboard();
	DocumentClass	*doc;

	doc=this->getDocumentForTab((mc->getMenuID() & 0xff));
	switch(mc->getMenuID() & 0xfff00)
		{
			case COPYFOLDERPATH:
				clipboard->setText(doc->getDirPath());
				break;
			case COPYFILEPATH:
				clipboard->setText(doc->getFilePath());
				break;
			case COPYFILENAME:
				clipboard->setText(doc->getFileName());
				break;
			case SPELLCHECKDOC:
				fprintf(stderr,"SPELLCHECKDOC\n");
				this->checkDoc(doc);
				break;
			case OPENFROMHERE:
				this->openFile(QString("%1/%2").arg(doc->getDirPath()).arg(mc->text()));
				break;
			case HIDETAB:
				this->mainNotebook->setTabVisible(mc->getMenuID() & 0xff,false);
				break;
			case LOCKCONTENTS:
				doc->setReadOnly(!doc->isReadOnly());
				if(doc->isReadOnly()==true)
						doc->setTabName(QString("+%1").arg(doc->getTabName()));
				else
					if(doc->dirty==true)
						doc->setTabName(this->truncateWithElipses(doc->getFileName(),this->prefsMaxTabChars)+"*");
					else
						doc->setTabName(this->truncateWithElipses(doc->getFileName(),this->prefsMaxTabChars));
				break;
		}
	this->setToolbarSensitive();
}

void KKEditClass::doTabBarContextMenuSetHilite(void)
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	DocumentClass	*doc;
	int				themenum=2;

	doc=this->getDocumentForTab((mc->getMenuID() & 0xff));
	if(doc==NULL)
		return;

	this->sessionBusy=true;
	switch(mc->getMenuID() & 0xfff00)
		{
			case NONESRCCODE:
				themenum=-1;
				break;
			case CPPSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCpp);
				break;
			case CSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeC);
				break;
			case BASHSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeBash);
				break;
			case PYTHONSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodePython);
				break;
			case GOSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeGo);
				break;
			case LUASRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeLua);
				break;
			case YAMLSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeYAML);
				break;
			case PHPSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodePHP);
				break;
			case XMLSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeXML);
				break;
			case CSSSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeCSS);
				break;
			case JSSRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeJs);
				break;
			case MAKESRCCODE:
				doc->highlighter->setCurrentLanguage(QSourceHighlite::QSourceHighliter::CodeMake);
				break;
		}
	doc->highlighter->setTheme((QSourceHighliter::Themes)themenum);
	this->sessionBusy=false;
}

void KKEditClass::doOddMenuItems(void)
{
	MenuItemClass	*mc=qobject_cast<MenuItemClass*>(sender());
	bool			retval;
	QClipboard		*clipboard=this->application->clipboard();
	DocumentClass	*doc=this->getDocumentForTab(-1);

		switch(mc->getMenuID())
			{
				case SPELLCHECKMENUITEM:
#ifdef _ASPELL_
					if(doc->textCursor().selectedText().isEmpty()==false)
						{
							this->returnWord=false;
							this->setUpSpellGUI(doc->textCursor().selectedText(),doc);
						}
#endif
					break;
			}
}

void KKEditClass::doOddButtons(void)
{
	DocumentClass	*doc=this->getDocumentForTab(-1);
	bool			saveas=false;

	switch(sender()->objectName().toInt())
		{
			case TOOLSSAVEAS:
				saveas=true;
			case TOOLSSAVE:
				{
					int		flags=0;
					QFile	file;
					bool	retval;
					QPushButton	*savebutton;
	
					if(saveas==false)
						file.setFileName(this->toolSelect->currentData().toString());
					else
						file.setFileName(QString("%1/%2").arg(this->toolsFolder).arg(this->toolsWindow->findChild<QLineEdit*>(TOOLNAME)->text()));
					retval=file.open(QIODevice::Text | QIODevice::WriteOnly);
					if(retval==true)
						{

							this->showBarberPole("Rebuild Tools Menu","Rebuilding tools menu, please wait ...","Cancel",QString("%1/tools").arg(this->tmpFolderName));

							DEBUGSTR("Saving : " << this->toolSelect->currentData().toString())

							QTextStream(&file) << TOOLALWAYSINPOPUP << "\t" << this->toolsWindow->findChild<QCheckBox*>(TOOLALWAYSINPOPUP)->isChecked() << Qt::endl;
							QTextStream(&file) << TOOLCLEAROP << "\t" << this->toolsWindow->findChild<QCheckBox*>(TOOLCLEAROP)->isChecked() << Qt::endl;
							QTextStream(&file) << TOOLCOMMAND << "\t" << this->toolsWindow->findChild<QLineEdit*>(TOOLCOMMAND)->text() << Qt::endl;
							QTextStream(&file) << TOOLCOMMENT << "\t" << this->toolsWindow->findChild<QLineEdit*>(TOOLCOMMENT)->text() << Qt::endl;

							if(this->toolsWindow->findChild<QRadioButton*>(TOOLPASTEOUT)->isChecked()==true)
								flags=TOOL_PASTE_OP;
							if(this->toolsWindow->findChild<QRadioButton*>(TOOLREPLACEALL)->isChecked()==true)
								flags=TOOL_REPLACE_OP;
							if(this->toolsWindow->findChild<QRadioButton*>(TOOLVIEWOUT)->isChecked()==true)
								flags=TOOL_VIEW_OP;
							if(this->toolsWindow->findChild<QCheckBox*>(TOOLRUNSYNC)->isChecked()==false)
								flags|=TOOL_ASYNC;
							if(this->toolsWindow->findChild<QCheckBox*>(TOOLSHOWDOC)->isChecked()==true)
								flags|=TOOL_SHOW_DOC;

							QTextStream(&file) << TOOLFLAGS << "\t" << flags << Qt::endl;
							QTextStream(&file) << TOOLSHOWINPOPUP << "\t" << this->toolsWindow->findChild<QCheckBox*>(TOOLSHOWINPOPUP)->isChecked() << Qt::endl;
							QTextStream(&file) << TOOLRUNINTERM << "\t" << this->toolsWindow->findChild<QCheckBox*>(TOOLRUNINTERM)->isChecked() << Qt::endl;
							QTextStream(&file) << TOOLNAME << "\t" << this->toolsWindow->findChild<QLineEdit*>(TOOLNAME)->text() << Qt::endl;
							QTextStream(&file) << TOOLRUNASROOT << "\t" << this->toolsWindow->findChild<QCheckBox*>(TOOLRUNASROOT)->isChecked() << Qt::endl;
							QTextStream(&file) << TOOLKEY << "\t" << this->toolsWindow->findChild<QLineEdit*>(TOOLKEY)->text() << Qt::endl;
							QTextStream(&file) << TOOLUSEPOLE << "\t" << this->toolsWindow->findChild<QCheckBox*>(TOOLUSEPOLE)->isChecked() << Qt::endl;

							file.close();

							flags=this->toolSelect->currentIndex();
			 				this->rebuildToolsMenu();
							if(saveas==true)
								flags=this->toolSelect->findText(this->toolsWindow->findChild<QLineEdit*>(TOOLNAME)->text());
			 				this->toolSelect->setCurrentIndex(flags);
							this->runPipe(QString("echo quit>\"%1/tools\"").arg(this->tmpFolderName));

							savebutton=this->toolsWindow->findChild<QPushButton*>(QString("%1").arg(TOOLSSAVE));
							savebutton->setEnabled(true);
						}
				}
				break;
			case TOOLSDELETE:
				{
					QFile		file;
					file.setFileName(this->toolSelect->currentData().toString());
					QFileInfo	fileinfo(file);
					DEBUGSTR("Deleting : " << fileinfo.fileName())
					if(this->yesNoDialog("Deleting "+fileinfo.fileName(),"This is not undoable, continue?")!=QMessageBox::Yes)
						return;

					this->showBarberPole("Rebuild Tools Menu","Rebuilding tools menu, please wait ...","Cancel",QString("%1/tools").arg(this->tmpFolderName));
					file.remove();
					this->rebuildToolsMenu();
					this->runPipe(QString("echo quit>\"%1/tools\"").arg(this->tmpFolderName));
				}
				break;
			case TOOLSCANCEL:
				this->toolsWindow->hide();
				break;
#ifdef _ASPELL_
			case APPLYWORDBUTTON:
				fprintf(stderr,"APPLYWORD\n");
				this->spellCheckGUI->hide();
				if(this->returnWord==false)
					{
						doc->textCursor().removeSelectedText();
						doc->textCursor().insertText(this->wordDropBox->currentText());
					}
				else
					this->goodWord=this->wordDropBox->currentText();
				this->returnWord=false;
				break;
			case IGNOREWORDBUTTON:
				fprintf(stderr,"IGNOREWORDBUTTON\n");
				if(this->returnWord==false)
					aspell_speller_add_to_session(this->spellChecker,doc->textCursor().selectedText().toStdString().c_str(),-1);
				else
					aspell_speller_add_to_session(this->spellChecker,this->badWord.toStdString().c_str(),-1);
				this->spellCheckGUI->hide();
				this->returnWord=false;
				break;
			case ADDWORDBUTTON:
				fprintf(stderr,"ADDWORDBUTTON\n");
				aspell_speller_add_to_personal(this->spellChecker,this->badWord.toStdString().c_str(),-1);
				aspell_speller_save_all_word_lists(this->spellChecker);
				this->spellCheckGUI->hide();
				break;
			case CANCELSPELLCHECK:
				fprintf(stderr,"CANCELSPELLCHECK\n");
				this->cancelCheck=true;
				this->spellCheckGUI->hide();
				break;
#endif
			case CANCELPREFS:
				this->prefsWindow->hide();
				this->readConfigs();
				this->resetAllFilePrefs();
				this->switchPage(this->mainNotebook->currentIndex());
				break;
			case DOLIVESEARCH:
				fprintf(stderr,"DOLIVESEARCH\n");
				break;
			case DOAPISEARCH:
				fprintf(stderr,"DOAPISEARCH\n");
				this->goToDefinition(this->findDefWidget->text());
				break;
			case DOQT5SEARCH:
				fprintf(stderr,"DOQT5SEARCH\n");
				this->searchAPIDocs(this->findQtApiWidget->text(),1);
				break; 
			case DOGTKSEARCH:
				fprintf(stderr,"DOGTKSEARCH gtkwidget QLineEdit\n");
				this->searchAPIDocs(this->findGtkApiWidget->text(),0);
				break;
			case DOCVIEWERGOHOME:
				this->showWebPage("",this->htmlURI);
				break;
			default:
				break;
		}
}



