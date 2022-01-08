/*
 *
 * ©K. D. Hedger. Thu 23 Dec 20:39:55 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClass.cpp) is part of KKEditQT.

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

KKEditClass::KKEditClass(QApplication *app)
{
	this->application=app;
//	this->history=new HistoryClass;
}

KKEditClass::~KKEditClass()
{
	char*	command=NULL;

	for(int j=0;j<SHORTCUTSCOUNT;j++)
		delete this->appShortcuts[j];

//	delete this->history;

	asprintf(&command,"rm -rf %s",this->tmpFolderName.toStdString().c_str());
	system(command);
	if (command!=NULL) free(command);command=NULL;
}

void KKEditClass::doSearchFromBar(const QString txt)
{
	fprintf(stderr,"void KKEditClass::doSearchFromBar(const QString txt)=%s\n",txt.toStdString().c_str());
	switch(sender()->objectName().toInt())
		{
			case DOLINEBOX:
				//fprintf(stderr,"DOLIVESEARCH\n");
				this->gotoLine(txt.toInt());
				break;
		}
}

void KKEditClass::setUpToolBar(void)
{
	this->toolBar->clear();
	for(int j=0;j<this->prefsToolBarLayout.length();j++)
		{
			switch(this->prefsToolBarLayout.at(j).toLatin1())
				{
//new
					case 'N':
						this->toolBar->addAction(qobject_cast<QAction*>(this->newMenuItem));
						break;
//open+recent
					case 'O':
						this->toolBar->addAction(qobject_cast<QAction*>(this->openMenuItem));
						break;
//save
					case 'S':
						this->toolBar->addAction(qobject_cast<QAction*>(this->saveMenuItem));
						break;

					case 's':
						this->toolBar->addSeparator();
						break;
//cut
					case 'X':
						this->toolBar->addAction(qobject_cast<QAction*>(this->cutMenuItem));
						break;
//copy
					case 'C':
						this->toolBar->addAction(qobject_cast<QAction*>(this->copyMenuItem));
						break;
//paste
					case 'P':
						this->toolBar->addAction(qobject_cast<QAction*>(this->pasteMenuItem));
						break;
//undo
					case 'U':
						this->toolBar->addAction(qobject_cast<QAction*>(this->undoMenuItem));
						break;
//redo
					case 'R':
						this->toolBar->addAction(qobject_cast<QAction*>(this->redoMenuItem));
						break;
//find
					case 'F':
						this->toolBar->addAction(qobject_cast<QAction*>(this->findMenuItem));
						break;
//navigation
					case 'G':
						this->toolBar->addAction(qobject_cast<QAction*>(this->goToDefineMenuItem));
						break;
//go back
					case 'B':
						this->toolBar->addAction(qobject_cast<QAction*>(this->goBackMenu));
						break;
//go foward
					case 'W':
						this->toolBar->addAction(qobject_cast<QAction*>(this->goFowardMenu));
						break;

					case '9':
						if(this->lineNumberWidget==NULL)
							{
								this->lineNumberWidget=new QLineEdit;
								qobject_cast<QLineEdit*>(this->lineNumberWidget)->setValidator(new QIntValidator);
								this->lineNumberWidget->setObjectName(QString("%1").arg(DOLINEBOX));
								this->lineNumberWidget->setToolTip("Go To Line");
								this->lineNumberWidget->setMaximumWidth(48);
								QObject::connect(this->lineNumberWidget,SIGNAL(textEdited(const QString)),this,SLOT(doSearchFromBar(const QString)));
							}
						this->toolBar->addWidget(this->lineNumberWidget);
						break;
//find in gtkdoc
					case 'A':
						if(this->findGtkApiWidget==NULL)
							{
								this->findGtkApiWidget=new QLineEdit;
								this->findGtkApiWidget->setObjectName(QString("%1").arg(DOGTKSEARCH));
								this->findGtkApiWidget->setToolTip("Find API In Gtk Docs");
								QObject::connect(this->findGtkApiWidget,SIGNAL(returnPressed()),this,SLOT(doOddButtons()));
							}
						this->toolBar->addWidget(this->findGtkApiWidget);
						break;

//find in qt5doc
					case 'Q':
						if(this->findQtApiWidget==NULL)
							this->findQtApiWidget=new QLineEdit;
						this->findQtApiWidget->setObjectName(QString("%1").arg(DOQT5SEARCH));
						this->findQtApiWidget->setToolTip("Find API In Qt5 Docs");
						QObject::connect(this->findQtApiWidget,SIGNAL(returnPressed()),this,SLOT(doOddButtons()));
						this->toolBar->addWidget(this->findQtApiWidget);
						break;
//find in function def
					case 'D':
						if(this->findDefWidget==NULL)
							this->findDefWidget=new QLineEdit;
						this->findDefWidget->setObjectName(QString("%1").arg(DOAPISEARCH));
						this->findDefWidget->setToolTip("Search For Define");
						QObject::connect(this->findDefWidget,SIGNAL(returnPressed()),this,SLOT(doOddButtons()));
						this->toolBar->addWidget(this->findDefWidget);
						break;
//livesearch
					case 'L':
						if(this->liveSearchWidget==NULL)
							this->liveSearchWidget=new QLineEdit;
						this->liveSearchWidget->setToolTip("Live Search");
						//QObject::connect(((QLineEdit*)this->liveSearchWidget),&QLineEdit::textChanged,doLiveSearch);
						this->toolBar->addWidget(this->liveSearchWidget);
						break;

//expander
					case 'E':
						{
							QHBoxLayout *hbox=new QHBoxLayout;
							QWidget		*widg=new QWidget;
    						hbox->addStretch(1);
    						widg->setLayout(hbox);
    						this->toolBar->addWidget(widg);
    					}
						break;
				}
		}
}

void KKEditClass::switchPage(int index)
{
	DocumentClass			*doc=NULL;
	MenuItemClass			*menuitem;
	int						linenumber;
	QString					label="";
	QString					entrytype="";
	QStringList				sl;
	QHash<QString,QMenu*>	menus;

	if(this->sessionBusy==true)
		return;

	doc=qobject_cast<DocumentClass*>(this->mainNotebook->widget(index));
	if(doc==0)
		return;
	if(doc==NULL)
		return;

	menus.clear();
	sl.clear();
	doc->setStatusBarText();
	this->funcMenu->clear();

	sl=this->getNewRecursiveTagList(doc->getFilePath());
	if(sl.isEmpty()==true)
		{
			this->funcMenu->setEnabled(false);
			return;
		}

	for(int j=0;j<sl.count();j++)
		{
			linenumber=sl.at(j).section(" ",2,2).toInt();
			label=sl.at(j).section(" ",4);
			entrytype=sl.at(j).section(" ",1,1);

			if(entrytype.isEmpty()==false)
				{
					if(this->prefsFunctionMenuLayout==4)
						{
							entrytype=entrytype.left(1).toUpper()+entrytype.mid(1) +"s";
							menuitem=new MenuItemClass(this->truncateWithElipses(label,this->prefsMaxFuncChars));
						}
					else
						menuitem=new MenuItemClass(this->truncateWithElipses(entrytype.toUpper() + " " +label,this->prefsMaxFuncChars));
				
					menuitem->setMenuID(linenumber);
					menuitem->mainKKEditClass=this;
					QObject::connect(menuitem,SIGNAL(triggered()),menuitem,SLOT(menuClickedGotoLine()));			
					if(this->prefsFunctionMenuLayout==4)
						{
							if(menus.contains(entrytype)==false)
								{
									menus[entrytype]=new QMenu(entrytype);
									this->funcMenu->addMenu(menus.value(entrytype));
									menus.value(entrytype)->setStyleSheet("QMenu { menu-scrollable: true ;}");
								}
							menus.value(entrytype)->addAction(menuitem);
						}
					else
						{
							this->funcMenu->addAction(menuitem);
						}
				}
		}

	this->rebuildTabsMenu();
	this->funcMenu->setEnabled(true);
}

void KKEditClass::rebuildBookMarkMenu()
{
	MenuItemClass	*menuItemSink;

	this->bookMarkMenu->clear();
	menuItemSink=this->makeMenuItemClass(BOOKMARKSMENU,"Remove All Bookmarks",0,"window-close",REMOVEALLBOOKMARKS,REMOVEALLBOOKMARKSMENUITEM);
	menuItemSink=this->makeMenuItemClass(BOOKMARKSMENU,"Toggle Bookmark",QKeySequence::fromString("Ctrl+T"),DATADIR"/pixmaps/BookMark.png",TOGGLEBOOKMARK,TOGGLEBOOKMARKMENUITEM);

	this->bookMarkMenu->addSeparator();
	this->bookMarks.clear();
}

void KKEditClass::handleBMMenu(QWidget *widget,int what)
{
	DocumentClass	*doc=this->pages.value(qobject_cast<DocumentClass*>(widget)->pageIndex);
	QTextCursor		cursor=doc->textCursor();
	bookMarkStruct	bms;
	switch(what)
		{
			case TOGGLEBOOKMARKMENUITEM:
				{
					foreach(bookMarkStruct value,this->bookMarks)
						{
							if((value.bmLabel.compare(cursor.block().text().simplified())==0) && (value.docIndex==doc->pageIndex))
								{
									this->sessionBusy=true;
									QTextBlock			block=doc->document()->findBlockByLineNumber(value.line);
									QTextBlockFormat	bf=block.blockFormat();

									bf.clearBackground();
									cursor.setBlockFormat(bf);
									this->sessionBusy=false;
								
									this->bookMarkMenu->removeAction(value.menu);
									this->bookMarks.remove(value.bmKey);
									return;
								}
						}

					bms.bmLabel=cursor.block().text().simplified();
					bms.line=cursor.blockNumber()+1;
					if(bms.bmLabel.isEmpty()==true)
						bms.bmLabel=doc->getFileName() + QString(" Line %1").arg(bms.line); 
					bms.docIndex=doc->pageIndex;
					bms.bmKey=this->bookMarksIndex;
					bms.menu=this->makeMenuItemClass(BOOKMARKSMENU,bms.bmLabel,0,NULL,"NOTNEEDED",this->bookMarksIndex);
					this->bookMarks[this->bookMarksIndex++]=bms;
				}
				break;

			default:
				bms=this->bookMarks.value(what);
				doc=this->pages.value(bms.docIndex);
				this->mainNotebook->setCurrentWidget(doc);
				this->gotoLine(bms.line);
				break;
		}
}

void KKEditClass::initApp(int argc,char** argv)
{
	char	tmpfoldertemplate[]="/tmp/KKEditQT-XXXXXX";
	int		exitstatus;
	char*	filename;
	QRect	r(0,0,1024,768);
	QDir	tdir;
	QString	tstr;
	QFile	file;

	this->homeFolder=QString("%1").arg(tdir.homePath());
	this->homeDataFolder=QString("%1/%2").arg(this->homeFolder).arg(KKEDITFOLDER);
	this->sessionFolder=QString("%1/%2/%3").arg(this->homeFolder).arg(KKEDITFOLDER).arg("sesssions");
	this->toolsFolder=QString("%1/%2/%3").arg(this->homeFolder).arg(KKEDITFOLDER).arg("tools");

	tdir.mkpath(this->sessionFolder);
	for(int j=0;j<MAXSESSIONS;j++)
		{
			tstr=QString("touch '%1/Session-%2'").arg(this->sessionFolder).arg(j);
			system(tstr.toStdString().c_str());
			file.setFileName(QString("%1/Session-%2").arg(this->sessionFolder).arg(j));
			if(file.open(QIODevice::Text | QIODevice::ReadOnly))
				{
					tstr=QTextStream(&file).readLine();
					file.close();
					if(tstr.isEmpty()==true)
						{
							if(file.open(QIODevice::Text | QIODevice::WriteOnly))
								{
									if(j>0)
										{
											QTextStream(&file) << "New Session-" << j <<Qt::endl;
											this->sessionNames[j]=QString("New Session-%1").arg(j);
										}
									else
										{
											QTextStream(&file) << "Default Session" <<Qt::endl;
											this->sessionNames[0]=QString("Default Session");
										}
									file.close();
								}
						}
					else
						this->sessionNames[j]=tstr;
				}
		}
DEBUGSTR( ">>" << this->homeFolder << "<<" )
DEBUGSTR( ">>" << this->homeDataFolder << "<<" )
DEBUGSTR( ">>" << this->sessionFolder << "<<" )

	//
	this->tmpFolderName=mkdtemp(tmpfoldertemplate);
//	this->prefsTerminalCommand="xterm -e ";
//	this->prefsRootCommand=GTKSUPATH;

	exitstatus=system("which manpageeditor 2>&1 >/dev/null");
	this->gotManEditor=WEXITSTATUS(exitstatus);
	exitstatus=system("which doxygen 2>&1 >/dev/null");
	this->gotDoxygen=WEXITSTATUS(exitstatus);
//	if(getuid()!=0)
//		styleName="classic";
//	else
//		styleName="Root Source";
	//this->highlightColour="#808080";
//	asprintf(&filename,"%s/" KKEDITFOLDER "/tools",getenv("HOME"));
//	g_mkdir_with_parents(filename,493);
//	if (filename!=NULL) free(filename);filename=NULL;

	this->mainWindow=new QMainWindow;
	for(int j=0;j<SHORTCUTSCOUNT;j++)
		this->appShortcuts[j]=new QShortcut(this->mainWindow);

	this->setAppShortcuts();

	this->readConfigs();

	if((this->queueID=msgget(this->sessionID,IPC_CREAT|0660))==-1)
		fprintf(stderr,"Can't create message queue, scripting wont work :( ...\n");

	this->checkMessages=new QTimer();
//linle a symlink.
//					 vv from object      vv signal 	  	   vv to object   vv func slot must inherit QObject
	QObject::connect(this->checkMessages,SIGNAL(timeout()),this,SLOT(doTimer()));
	this->checkMessages->start(this->prefsMsgTimer);

	this->buildMainGui();
	this->buildPrefsWindow();
//TODO//
//	if(onExitSaveSession==true)
//		restoreSession(NULL,(void*)restoreBookmarks);
#ifdef _BUILDDOCVIEWER_
	this->buildDocViewer();
#endif

	buildFindReplace();
#ifdef _ASPELL_
	AspellCanHaveError	*possible_err;
	this->aspellConfig=new_aspell_config();
	possible_err=new_aspell_speller(this->aspellConfig);

	if(aspell_error_number(possible_err)!= 0)
		puts(aspell_error_message(possible_err));
	else
		spellChecker=to_aspell_speller(possible_err);

	this->spellCheckMenuItem=new MenuItemClass("Spell Check");
	QIcon	itemicon=QIcon::fromTheme("tools-check-spelling");
	this->spellCheckMenuItem->setMenuID(SPELLCHECKMENUITEM);
	this->spellCheckMenuItem->setIcon(itemicon);
	QObject::connect(this->spellCheckMenuItem,SIGNAL(triggered()),this,SLOT(doOddMenuItems()));
	this->buildSpellCheckerGUI();
#endif

	this->htmlFile=QString("%1/Docview-%2.html").arg(this->tmpFolderName).arg(this->randomName(6));
	//this->htmlURI=QString("file://%1/Docview-%2.html").arg(this->tmpFolderName).arg(this->randomName(6));
	this->htmlURI="file://"+this->htmlFile;

	if(this->forceDefaultGeom==false)
		r=this->prefs.value("app/geometry",QVariant(QRect(50,50,1024,768))).value<QRect>();
	this->mainWindow->setGeometry(r);

	this->setToolbarSensitive();
	this->mainWindow->show();
return;
//	refreshMainWindow();
//
//	buildFindReplace();
//
//	for(unsigned int j=0; j<g_slist_length(findList); j++)
//		{
//			reinterpret_cast<QComboBox*>(findDropBox)->addItem((const char*)g_slist_nth_data(findList,j));
//		}
//	reinterpret_cast<QComboBox*>(findDropBox)->setCurrentIndex(g_slist_length(findList)-1);
//
//	for(unsigned int j=0; j<g_slist_length(findList); j++)
//		{
//			reinterpret_cast<QComboBox*>(replaceDropBox)->addItem((const char*)g_slist_nth_data(replaceList,j));
//		}
//	reinterpret_cast<QComboBox*>(replaceDropBox)->setCurrentIndex(g_slist_length(findList)-1);
//
////TODO//
//
//#ifdef _BUILDDOCVIEWER_
//	buildDocViewer();
//#endif
//
//
////TODO//

}

QString KKEditClass::randomName(int len)
{
	QString	retval="";

	const char alphanum[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	for(int i=0;i<len;++i)
		retval+=alphanum[rand()%(sizeof(alphanum)-1)];
   return(retval);
}

void KKEditClass::doAppShortCuts(void)
{
	QShortcut		*sc=qobject_cast<QShortcut*>(sender());
	DocumentClass	*doc=this->getDocumentForTab(-1);
	QString			txt;
	QTextCursor		cursor;
	int				anc;

	if(doc==NULL)
		return;

	cursor=doc->textCursor();
	switch(sc->objectName().toInt())
		{
			case HIDETABSHORTCUT:
				this->mainNotebook->setTabVisible(this->mainNotebook->currentIndex(),false);
				break;
			case DELETELINESHORTCUT:
				cursor.select(QTextCursor::LineUnderCursor);
				cursor.removeSelectedText();
				cursor.deleteChar();
				break;
			case DELETETOEOLSHORTCUT:
				cursor.movePosition(QTextCursor::EndOfLine,QTextCursor::KeepAnchor);
				cursor.removeSelectedText();
				break;
			case DELETETOSOLSHORTCUT:
				cursor.movePosition(QTextCursor::StartOfLine,QTextCursor::KeepAnchor);
				cursor.removeSelectedText();
				break;
			case SELECTWORDSHORTCUT:
				cursor.select(QTextCursor::WordUnderCursor);
				doc->setTextCursor(cursor);
				break;
			case DELETEWORDSHORTCUT:
				cursor.select(QTextCursor::WordUnderCursor);
				cursor.removeSelectedText();
				break;
			case DUPLICATELINESHORTCUT:
				anc=cursor.anchor();
				cursor.select(QTextCursor::BlockUnderCursor);
				txt=cursor.selectedText();
				if(txt.isEmpty()==true)
					txt='\n';
				cursor.movePosition(QTextCursor::EndOfLine,QTextCursor::MoveAnchor);
				cursor.insertText(txt);
				cursor.setPosition(anc);
				doc->setTextCursor(cursor);
				emit doc->cursorPositionChanged();
				break;
			case SELECTLINESHORTCUT:
				cursor.select(QTextCursor::LineUnderCursor);
				doc->setTextCursor(cursor);
				break;
			case MOVELINEUPSHORTCUT:
				anc=cursor.positionInBlock();
				cursor.select(QTextCursor::LineUnderCursor);
				txt=cursor.selectedText();
				cursor.removeSelectedText();
				cursor.deleteChar();
				cursor.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor);
				cursor.insertText(QString("%1%2").arg(txt).arg('\n'));
				cursor.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor);
				cursor.setPosition(cursor.anchor()+anc);
				doc->setTextCursor(cursor);
				emit doc->cursorPositionChanged();
				break;
			case MOVELINEDOWNSHORTCUT:
				anc=cursor.positionInBlock();
				cursor.select(QTextCursor::LineUnderCursor);
				txt=cursor.selectedText();
				cursor.removeSelectedText();
				cursor.deleteChar();
				cursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor);
				cursor.insertText(QString("%1%2").arg(txt).arg('\n'));
				cursor.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor);
				cursor.setPosition(cursor.anchor()+anc);
				doc->setTextCursor(cursor);
				emit doc->cursorPositionChanged();
				break;
			case MOVESELECTIONUPSHORTCUT:
				txt=cursor.selectedText();
				cursor.removeSelectedText();
				cursor.deleteChar();
				cursor.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor);
				anc=cursor.anchor();
				cursor.insertText(QString("%1%2").arg(txt).arg('\n'));
				cursor.setPosition(anc);
				cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor,txt.length());
				doc->setTextCursor(cursor);
				emit doc->cursorPositionChanged();
				break;
			case MOVESELECTIONDOWNSHORTCUT:
				txt=cursor.selectedText();
				cursor.removeSelectedText();
				cursor.deleteChar();
				cursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor);
				anc=cursor.anchor();
				cursor.insertText(QString("%1%2").arg(txt).arg('\n'));
				cursor.setPosition(anc);
				cursor.movePosition(QTextCursor::NextCharacter,QTextCursor::KeepAnchor,txt.length());
				doc->setTextCursor(cursor);
				emit doc->cursorPositionChanged();
				break;
		}
}

void KKEditClass::readConfigs(void)
{
//editor
	this->prefsFunctionMenuLayout=this->prefs.value("editor/funcsort",4).toInt();
	this->prefsDepth=this->prefs.value("editor/prefsdepth",1).toInt();
	this->prefsToolBarLayout=this->prefs.value("editor/toolbarlayout","NSOsURsBWsFGsE9ELEDEE").toString();
	this->prefsMaxTabChars=this->prefs.value("editor/maxtabchars",20).toInt();
	this->prefsMaxFuncChars=this->prefs.value("editor/maxfuncchars",64).toInt();
	this->prefsTerminalCommand=this->prefs.value("editor/terminalcommand","xterm -e").toString();
	this->prefsRootCommand=this->prefs.value("editor/rootcommand","gtksu -- ").toString();
	this->prefsQtDocDir=this->prefs.value("editor/qtdocdir","/usr/share/doc/qt5").toString();

//document
	this->prefsHighLightline=this->prefs.value("document/highlightline",QVariant(bool(true))).value<bool>();
	this->prefsShowLineNumbers=this->prefs.value("document/showlinenumbers",QVariant(bool(true))).value<bool>();
	this->prefsDocumentFont=this->prefs.value("document/font",QVariant(QFont("Monospace",10))).value<QFont>();
	this->prefsSyntaxHilighting=this->prefs.value("document/syntaxhilighting",QVariant(bool(true))).value<bool>();
	this->prefsTabWidth=this->prefs.value("document/tabwidth",4).toInt();
	this->prefsLineWrap=this->prefs.value("document/wrap",QVariant(bool(true))).value<bool>();
	this->prefsIndent=this->prefs.value("document/indent",QVariant(bool(true))).value<bool>();
	this->prefsAutoShowCompletions=this->prefs.value("document/autoshowcompletions",QVariant(bool(true))).value<bool>();

//theme
	this->prefsHiLiteLineColor=this->prefs.value("theme/hilitelinecol",QVariant(QColor(0xff,0xff,0xff,0x40))).value<QColor>();
	this->prefsBookmarkHiLiteColor=this->prefs.value("theme/bmhilitecol",QVariant(QColor(0,0,0,0x40))).value<QColor>();

//application
	this->prefsMsgTimer=this->prefs.value("app/msgtimer",1000).toInt();
	this->prefsUseSingle=this->prefs.value("app/usesingle",QVariant(bool(true))).value<bool>();
	this->findList=this->prefs.value("app/findlist").toStringList();
	this->replaceList=this->prefs.value("app/replacelist").toStringList();
	this->defaultShortCutsList=this->prefs.value("app/shortcuts",QVariant(QStringList({"Ctrl+H","Ctrl+Y","Ctrl+?","Ctrl+K","Ctrl+Shift+H","Ctrl+D","Ctrl+Shift+D","Ctrl+L","Ctrl+M","Ctrl+Shift+M","Ctrl+@","Ctrl+'"}))).toStringList();

	this->setAppShortcuts();	
}

void KKEditClass::tabContextMenu(const QPoint &pt)
{
	MenuItemClass	*menuitem;
	MenuItemClass	*menuitem1;
	int				tabIndex;
	int				srccnt=0;

	if(pt.isNull())
		return;

	tabIndex=this->tabBar->tabAt(pt);

	if (tabIndex!=-1)
		{
			QMenu	menu(this->tabBar);
			QMenu	srcmenu(this->tabContextMenuItems[(SRCHILTIE-COPYFOLDERPATH)/0x100].label);
			QMenu	filemenu(this->tabContextMenuItems[(OPENFROMHERE-COPYFOLDERPATH)/0x100].label);
			for(int cnt=0;cnt<TABCONTEXTMENUCNT;cnt++)
				{
					if(cnt==(SRCHILTIE-COPYFOLDERPATH)/0x100)
						{
							menu.addMenu(&srcmenu);
							while(this->srcMenuNames[srccnt]!=NULL)
								{
									menuitem1=new MenuItemClass(this->srcMenuNames[srccnt]);
									menuitem1->setMenuID(((srccnt)*0x100)+tabIndex);
									srcmenu.addAction(menuitem1);
									QObject::connect(menuitem1,SIGNAL(triggered()),this,SLOT(doTabBarContextMenuSetHilite()));
									srccnt++;
								}
							continue;
						}

					if(cnt==(OPENFROMHERE-COPYFOLDERPATH)/0x100)
						{
							menu.addMenu(&filemenu);
							DocumentClass	*doc=this->getDocumentForTab(tabIndex);
							QDir			dir(doc->getDirPath());
							QStringList		flist=dir.entryList(QDir::Files);
							for(int k=0;k<flist.count();k++)
								{
										menuitem1=new MenuItemClass(flist.at(k));
										menuitem1->setMenuID(OPENFROMHERE+tabIndex);
										filemenu.addAction(menuitem1);
										QObject::connect(menuitem1,SIGNAL(triggered()),this,SLOT(doTabBarContextMenu()));
								}
							continue;
						}
			
					menuitem=new MenuItemClass(this->tabContextMenuItems[cnt].label);
					menuitem->setMenuID(this->tabContextMenuItems[cnt].what+tabIndex);
					menu.addAction(menuitem);
					QIcon itemicon=QIcon::fromTheme(this->tabContextMenuItems[cnt].icon);
					menuitem->setIcon(itemicon);
					QObject::connect(menuitem,SIGNAL(triggered()),this,SLOT(doTabBarContextMenu()));
				}
			menu.exec(this->tabBar->mapToGlobal(pt));
		}
}

void KKEditClass::writeExitData(void)
{
//editor
	if(this->forceDefaultGeom==false)
		this->prefs.setValue("app/geometry",this->mainWindow->geometry());

#ifdef _BUILDDOCVIEWER_
	this->prefs.setValue("app/viewergeometry",this->docView->geometry());
#endif

	this->prefs.setValue("editor/funcsort",this->prefsFunctionMenuLayout);
	this->prefs.setValue("editor/prefsdepth",this->prefsDepth);
	this->prefs.setValue("editor/toolbarlayout",this->prefsToolBarLayout);
	this->prefs.setValue("editor/maxtabchars",this->prefsMaxTabChars);
	this->prefs.setValue("editor/maxfuncchars",this->prefsMaxFuncChars);
	this->prefs.setValue("editor/terminalcommand",this->prefsTerminalCommand);
	this->prefs.setValue("editor/rootcommand",this->prefsRootCommand);
	this->prefs.setValue("editor/toolbarlayout",this->prefsToolBarLayout);
	this->prefs.setValue("editor/qtdocdir",this->prefsQtDocDir);

//document
	this->prefs.setValue("document/indent",this->prefsIndent);
	this->prefs.setValue("document/wrap",this->prefsLineWrap);
	this->prefs.setValue("document/tabwidth",this->prefsTabWidth);
	this->prefs.setValue("document/syntaxhilighting",this->prefsSyntaxHilighting);
	this->prefs.setValue("document/font",this->prefsDocumentFont);
	this->prefs.setValue("document/showlinenumbers",this->prefsShowLineNumbers);
	this->prefs.setValue("document/highlightline",this->prefsHighLightline);
	this->prefs.setValue("document/autoshowcompletions",this->prefsAutoShowCompletions);

//theme
	this->prefs.setValue("theme/hilitelinecol",this->prefsHiLiteLineColor);
	this->prefs.setValue("theme/bmhilitecol",this->prefsBookmarkHiLiteColor);

//application
	this->prefs.setValue("app/msgtimer",this->prefsMsgTimer);
	this->prefs.setValue("app/usesingle",this->prefsUseSingle);
	this->prefs.setValue("app/shortcuts",this->defaultShortCutsList);
	this->prefs.setValue("app/findlist",this->findList);
	this->prefs.setValue("app/replacelist",this->replaceList);
}

void KKEditClass::findFile(void)
{
	DocumentClass	*document=this->getDocumentForTab(-1);
	QString			selection;
	QString			filename;
	QString			results;
	QStringList		retval;

	if(document==NULL)
		return;

	selection=document->textCursor().block().text().trimmed();

	if((selection.isEmpty()==true) || (selection.startsWith('#')==false))
		return;

	filename=selection.replace(QRegExp("#.*include\\s*[\"<](.*)[\">]"),"\\1").trimmed();

	if(this->openFile(QString("%1/%2").arg(document->getDirPath()).arg(filename))==true)
		return;

	results=this->runPipeAndCapture(QString("find \"%1\" -iname \"%2\"").arg(document->getDirPath()).arg(filename));
	if(results.isEmpty()==false)
		{
			retval=results.split("\n",Qt::SkipEmptyParts);
			for(int j=0;j<retval.count();j++)
				this->openFile(retval.at(j));
		}

	results=this->runPipeAndCapture(QString("find \"/usr/include\" -iname \"%1\"").arg(filename));
	if(results.isEmpty()==true)
		return;

	retval=results.split("\n",Qt::SkipEmptyParts);
	for(int j=0;j<retval.count();j++)
		this->openFile(retval.at(j));
}

void KKEditClass::showBarberPole(QString windowtitle,QString bodylabel,QString cancellabel,QString controlfile)
{
	QString	pipecom;

	pipecom=QString("KKEditQTProgressBar \"%1\" \"%2\" \"%3\" \"%4\" &").arg(windowtitle).arg(bodylabel).arg(cancellabel).arg(controlfile);
	this->runPipe(pipecom);
//DEBUGSTR(pipecom)
}

void KKEditClass::buildDocs(void)
{
	DocumentClass	*doc=this->getDocumentForTab(-1);
	struct stat		sb;
	FILE			*fp;
	char			line[4096];
	char			opdata[4096];
	QString			pipecom;
	QFileInfo		fileinfo;
	if(doc==NULL)
		return;

	pipecom=QString("KKEditQTProgressBar \"Building Docs\" \"Please Wait ...\" \"\" \"%1/progress\" &").arg(this->tmpFolderName);
	this->runPipe(QString("KKEditQTProgressBar \"Building Docs\" \"Please Wait ...\" \"\" \"%1/progress\" &").arg(this->tmpFolderName));

	QDir::setCurrent(doc->getDirPath());
	stat("Doxyfile",&sb);
	if(!S_ISREG(sb.st_mode))
		system("cp " DATADIR "/docs/Doxyfile .");

	fileinfo=QString("%1/html/index.html").arg(doc->getDirPath());
	fp=popen("doxygen Doxyfile","r");
	while(fgets(line,4095,fp))
		{
			line[strlen(line)-1]=0;
			this->runPipe(QString("echo -n \"%1\" >\"%2/progress\"").arg(line).arg(this->tmpFolderName));
		}
	pclose(fp);

	QString com=QString("/bin/echo '<meta http-equiv=\"refresh\" content=\"0; URL='file://%1'\" />' > %2").arg(fileinfo.absoluteFilePath()).arg(this->htmlFile);
	system(com.toStdString().c_str());
	this->docView->setWindowTitle("Doxygen Documentation");
	this->webView->load(QUrl("file://" + this->htmlFile));
	this->docviewerVisible=true;
	this->toggleDocViewMenuItem->setText("Hide Docviewer");
	this->docView->show();

	this->runPipe(QString("echo quit>\"%1/progress\"").arg(this->tmpFolderName));
}

void KKEditClass::showDocs(void)
{
	DocumentClass	*doc=this->getDocumentForTab(-1);
	QFileInfo		fileinfo=QString("%1/html/index.html").arg(doc->getDirPath());

	if(fileinfo.exists()==false)
		this->buildDocs();
	else
		{
			QString com=QString("/bin/echo '<meta http-equiv=\"refresh\" content=\"0; URL='file://%1'\" />' > %2").arg(fileinfo.absoluteFilePath()).arg(this->htmlFile);
			system(com.toStdString().c_str());
			this->docView->setWindowTitle("Doxygen Documentation");
			this->webView->load(QUrl("file://" + this->htmlFile));
			this->docviewerVisible=true;
			this->toggleDocViewMenuItem->setText("Hide Docviewer");
			this->docView->show();
		}
}

void KKEditClass::closeAllTabs(void)
{
printf("void KKEditClass::closeAllTabs(void)\n");
	bool retval;
	this->sessionBusy=true;
	int	numtabs=(this->mainNotebook)->count();
	for(int loop=0; loop<numtabs; loop++)
		{
			closingAllTabs=true;
			retval=this->closeTab(0);
			if(retval==false)
				{
					this->sessionBusy=false;
					return;
				}
		}

	this->rebuildBookMarkMenu();
	this->rebuildTabsMenu();
	this->currentSessionNumber=0xdeadbeef;
	this->closingAllTabs=false;
	this->setToolbarSensitive();
	this->sessionBusy=false;
}

bool KKEditClass::closeTab(int index)
{
	DocumentClass	*doc=NULL;
	int				thispage=index;

	this->sessionBusy=true;

	if(this->closingAllTabs==true)
		thispage=0;
	else
		{
			if(index==-1)
				thispage=this->mainNotebook->currentIndex();
			else
				thispage=index;
		}

	doc=qobject_cast<DocumentClass*>(this->mainNotebook->widget(thispage));
	if(doc!=0)
		{
			if(doc->dirty==true)
				{
					int result=this->askSaveDialog(doc->fileName);
					switch(result)
						{
							case QMessageBox::Save:
								this->saveFile(thispage,false);
       // Save was clicked
								break;
							case QMessageBox::Discard:
       // Don't Save was clicked
								break;
							case QMessageBox::Cancel:
       // Cancel was clicked
								this->sessionBusy=false;
								this->rebuildTabsMenu();
								return(false);
								break;
						}
				}

			this->mainNotebook->removeTab(thispage);
			delete doc;
		}

	if(this->closingAllTabs==false)
		{
			this->rebuildTabsMenu();
			this->setToolbarSensitive();
		}

	this->sessionBusy=false;
	return(true);
}

void KKEditClass::shutDownApp()
{
	if(this->forcedMultInst==false)
		this->writeExitData();
//TODO
	//if(onExitSaveSession)
	//	saveSession(NULL,0);
	//if(doSaveAll(widget,(uPtr)true)==false)
	//	return;
	//g_list_foreach(globalPlugins->plugins,releasePlugs,NULL);

	if(this->saveAllFiles()==true)
		{
#ifdef _ASPELL_
			delete_aspell_config(this->aspellConfig);
			delete_aspell_speller(this->spellChecker);
#endif
			QApplication::quit();
		}
}

QString KKEditClass::truncateWithElipses(const QString str,unsigned int maxlen)
{
	QString newlabel;
	if(str.length()>maxlen)
		newlabel=str.left((maxlen-3)/2)+"..."+str.right((maxlen-3)/2);
	else
		newlabel=str;

	return(newlabel);
}

void KKEditClass::setAppShortcuts(void)
{
	for(int j=0;j<SHORTCUTSCOUNT;j++)
		{
			if(this->appShortcuts[j]!=NULL)
				{
					delete this->appShortcuts[j];
					this->appShortcuts[j]=new QShortcut(this->mainWindow);
				}
			this->appShortcuts[j]->setKey(QKeySequence(this->defaultShortCutsList.at(j)));
			this->appShortcuts[j]->setObjectName(QString("%1").arg(j));
			QObject::connect(this->appShortcuts[j],SIGNAL(activated()),this,SLOT(doAppShortCuts()));
		}
}

void KKEditClass::setToolbarSensitive(void)
{
	DocumentClass	*doc=this->getDocumentForTab(-1);
	bool			override;
	bool			gotdoc=true;
	bool			hasselection=false;

	if(doc==NULL)
		{
			override=false;
			gotdoc=false;
		}
	else
		{
			override=doc->dirty;
			hasselection=doc->textCursor().hasSelection();
		}

//	if(	this->currentSessionNumber==0xdeadbeef)
	if(this->mainNotebook->count()==0)
		{
			this->saveSessionsMenu->setEnabled(false);
			this->currentSessionNumber=0xdeadbeef;
		}
	else
		this->saveSessionsMenu->setEnabled(true);

	if(this->currentSessionNumber==0xdeadbeef)
		this->saveCurrentSessionMenuItem->setEnabled(false);
	else
		this->saveCurrentSessionMenuItem->setEnabled(true);
		
	this->restoreDefaultSessionMenuItem->setEnabled(false);//TODO//

	for(int j=0;j<this->prefsToolBarLayout.length();j++)
		{
			switch(this->prefsToolBarLayout.at(j).toLatin1())
				{
//new
					case 'N':
						this->newMenuItem->setEnabled(true);
						break;
//open+recent
					case 'O':
						this->newMenuItem->setEnabled(true);
						break;
//save
					case 'S':
						this->saveMenuItem->setEnabled(override);
						break;

//cut
					case 'X':
						this->cutMenuItem->setEnabled((gotdoc==true) && ((hasselection) & (!doc->isReadOnly())));
						break;
//copy
					case 'C':
						this->copyMenuItem->setEnabled(hasselection);
						break;
//paste
					case 'P':
						this->pasteMenuItem->setEnabled((gotdoc==true) && (doc->canPaste()));
						break;
//undo
					case 'U':
						this->undoMenuItem->setEnabled((gotdoc==true) && (doc->gotUndo));
						this->undoAllMenuItem->setEnabled((gotdoc==true) && (doc->gotUndo));
						break;
//redo
					case 'R':
						this->redoMenuItem->setEnabled((gotdoc==true) && (doc->gotRedo));
						this->redoAllMenuItem->setEnabled((gotdoc==true) && (doc->gotRedo));
						break;
//find
					case 'F':
						this->findMenuItem->setEnabled(true);
						break;
//navigation
					case 'G':
						this->goToDefineMenuItem->setEnabled(hasselection);
						break;
//go back
					case 'B':
						this->goBackMenu->setEnabled(true);
						break;
//go foward
					case 'W':
						this->goFowardMenu->setEnabled(true);
						break;

					case '9':
						this->lineNumberWidget->setEnabled(gotdoc);
						this->goToLineDialogMenuItem->setEnabled(gotdoc);
						break;
//find in gtkdoc
					case 'A':
						this->findGtkApiWidget->setEnabled(true);
						break;

//find in qt5doc
					case 'Q':
						this->findQtApiWidget->setEnabled(true);
						break;
//find in function def
					case 'D':
						this->findDefWidget->setEnabled(true);
						break;
//livesearch
					case 'L':
						this->liveSearchWidget->setEnabled(gotdoc);
						break;
				}
		}
}

void KKEditClass::debugSignalSlot(int what)
{
}


