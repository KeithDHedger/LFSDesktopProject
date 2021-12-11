/*
 *
 * ©K. D. Hedger. Sun 31 Oct 12:19:03 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClass.cpp) is part of KKEdit.

 * KKEdit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * KKEdit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKEdit.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "KKEditClass.h"

KKEditClass::KKEditClass(QApplication *app)
{
	this->application=app;
}

KKEditClass::~KKEditClass()
{
	char*	command=NULL;

	for(int j=0;j<SHORTCUTSCOUNT;j++)
		delete this->appShortcuts[j];

	asprintf(&command,"rm -rf %s",this->tmpFolderName.c_str());
	system(command);
	debugFree(&command,"doShutdown command");
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
						this->lineNumberWidget=new QLineEdit;
						reinterpret_cast<QLineEdit*>(this->lineNumberWidget)->setValidator(new QIntValidator);
						this->lineNumberWidget->setToolTip("Go To Line");
						this->lineNumberWidget->setMaximumWidth(48);
						QObject::connect(((QLineEdit*)this->lineNumberWidget),&QLineEdit::textChanged,jumpToLineFromBar);
						this->toolBar->addWidget(this->lineNumberWidget);
						break;
//find in gtkdoc
					case 'A':
						this->findApiWidget=new QLineEdit;
						this->findApiWidget->setToolTip("Find API In Gtk Docs");
						QObject::connect(((QLineEdit*)this->findApiWidget),&QLineEdit::returnPressed,docSearchFromBar);
						this->toolBar->addWidget(this->findApiWidget);
						break;

//find in qt5doc
					case 'Q':
						this->findQtApiWidget=new QLineEdit;
						this->findQtApiWidget->setToolTip("Find API In Qt5 Docs");
						QObject::connect(((QLineEdit*)this->findQtApiWidget),&QLineEdit::returnPressed,qt5DocSearchFromBar);
						this->toolBar->addWidget(this->findQtApiWidget);
						break;
//find in function def
					case 'D':
						this->findDefWidget=new QLineEdit;
						this->findDefWidget->setToolTip("Search For Define");
						QObject::connect(((QLineEdit*)this->findDefWidget),&QLineEdit::returnPressed,defSearchFromBar);
						this->toolBar->addWidget(this->findDefWidget);
						break;
//livesearch
					case 'L':
						this->liveSearchWidget=new QLineEdit;
						this->liveSearchWidget->setToolTip("Live Search");
						QObject::connect(((QLineEdit*)this->liveSearchWidget),&QLineEdit::textChanged,doLiveSearch);
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

void KKEditClass::buildDocViewer(void)
{
#ifdef _BUILDDOCVIEWER_
	QVBoxLayout	*docvlayout=new QVBoxLayout;
	QHBoxLayout	*dochlayout=new QHBoxLayout;
	QWidget		*widget;

	this->docView=new QMainWindow(mainWindow);
	this->docView->setGeometry(this->docWindowX,this->docWindowY,this->docWindowWidth,this->docWindowHeight);
	
	widget=new QWidget;
	widget->setLayout(docvlayout);
	qobject_cast<QMainWindow*>(this->docView)->setCentralWidget(widget);

	this->webView=new QWebView(widget);
    qobject_cast<QWebView*>(this->webView)->load(QUrl("file://" DATADIR "/help/help.en.html"));
    docvlayout->addWidget(this->webView);

	widget=new QPushButton(QIcon::fromTheme("go-previous"),"Back");
	dochlayout->addWidget(widget);
	QObject::connect((QPushButton*)widget,&QPushButton::clicked,webKitGoBack);

	dochlayout->addStretch(1);

	widget=new QPushButton(QIcon::fromTheme("go-home"),"Home");
	dochlayout->addWidget(widget);
	QObject::connect((QPushButton*)widget,&QPushButton::clicked,webKitGoHome);

	dochlayout->addStretch(1);

	widget=new QPushButton(QIcon::fromTheme("edit-find"),"Find");
	dochlayout->addWidget(widget);

	widget=new QLineEdit;
	dochlayout->addWidget(widget);

	widget=new QPushButton(QIcon::fromTheme("go-down"),"Down");
	dochlayout->addWidget(widget);
	
	widget=new QPushButton(QIcon::fromTheme("go-up"),"Up");
	dochlayout->addWidget(widget);

	dochlayout->addStretch(1);

	widget=new QPushButton(QIcon::fromTheme("go-next"),"Forward");
	dochlayout->addWidget(widget);
	QObject::connect((QPushButton*)widget,&QPushButton::clicked,webKitGoForward);

	widget=new QWidget;
	widget->setLayout(dochlayout);
	docvlayout->addWidget(widget);

	this->docView->hide();
#endif
}

void KKEditClass::switchPage(int index)
{
	char*			functions=NULL;
	char*			lineptr;
	DocumentClass	*doc=NULL;
	int				linenum;
	char			tmpstr[1024];
	QString			correctedstr;
	char*			typenames[50]= {NULL,};
	char*			newstr=NULL;
	bool			flag;
	int				numtypes=0;
	QMenu			*whattypemenu;
	QMenu			*typesubmenus[50]= {NULL,};
	bool			onefunc=false;
	MenuItemClass	*menuitem;

	if(this->sessionBusy==true)
		return;

	doc=qobject_cast<DocumentClass*>(this->mainNotebook->widget(index));
	if(doc==0)
		return;
	if(doc==NULL)
		return;

	doc->setStatusBarText();
	qobject_cast<QMenu*>(this->funcMenu)->clear();
	getRecursiveTagList((char*)doc->getFilePath().toStdString().c_str(),&functions);

	lineptr=functions;
	while (lineptr!=NULL)
		{
			tmpstr[0]=0;
			sscanf (lineptr,"%*s %*s %i %[^\n]s",&linenum,tmpstr);
			correctedstr=this->truncateWithElipses(tmpstr,this->prefsMaxFuncChars);
			sprintf(tmpstr,"%s",correctedstr.toStdString().c_str());

			if(strlen(tmpstr)>0)
				{
					if(this->prefsFunctionMenuLayout==4)
						{
							newstr=NULL;
							newstr=globalSlice->sliceBetween(lineptr,(char*)" ",(char*)" ");
							if(newstr!=NULL)
								{
									flag=false;
									for(int j=0; j<numtypes; j++)
										{
											if (strcmp(newstr,typenames[j])==0)
												{
													whattypemenu=typesubmenus[j];
													flag=true;
													break;
												}
										}

									if(flag==false)
										{
											typenames[numtypes]=strdup(newstr);
											debugFree(&newstr,"switchPage newstr");
											if(typenames[numtypes][strlen(typenames[numtypes])-1]=='s')
												asprintf(&newstr,"%s's",typenames[numtypes]);
											else
												asprintf(&newstr,"%ss",typenames[numtypes]);
											newstr[0]=toupper(newstr[0]);
											typesubmenus[numtypes]=new QMenu(newstr);
											qobject_cast<QMenu*>(this->funcMenu)->addMenu(qobject_cast<QMenu*>(typesubmenus[numtypes]));
											whattypemenu=typesubmenus[numtypes];
											numtypes++;
										}

									debugFree(&newstr,"switchPage newstr");

									onefunc=true;
									menuitem=new MenuItemClass(tmpstr);
									menuitem->setMenuID(linenum);
									menuitem->mainKKEditClass=this;
									qobject_cast<QMenu*>(whattypemenu)->addAction(menuitem);
									QObject::connect(menuitem,SIGNAL(triggered()),menuitem,SLOT(menuClickedGotoLine()));
							}
						}
					else
						{
							onefunc=true;
							menuitem=new MenuItemClass(tmpstr);
							menuitem->setMenuID(linenum);
							menuitem->mainKKEditClass=this;
							qobject_cast<QMenu*>(this->funcMenu)->addAction(menuitem);
							QObject::connect(menuitem,SIGNAL(triggered()),menuitem,SLOT(menuClickedGotoLine()));
						}
				}

			lineptr=strchr(lineptr,'\n');
			if (lineptr!=NULL)
				lineptr++;
		}
	this->rebuildTabsMenu();
	this->funcMenu->setEnabled(onefunc);
}

void KKEditClass::rebuildBookMarkMenu()
{
	MenuItemClass	*menuItemSink;

	this->bookMarkMenu->clear();
	menuItemSink=this->makeMenuItemClass(BOOKNARKSMENU,"Remove All Bookmarks",0,"window-close",REMOVEALLBOOKMARKS,REMOVEALLBOOKMARKSMENUITEM);
	menuItemSink=this->makeMenuItemClass(BOOKNARKSMENU,"Toggle Bookmark",QKeySequence::fromString("Ctrl+T"),DATADIR"/pixmaps/BookMark.png",TOGGLEBOOKMARK,TOGGLEBOOKMARKMENUITE);

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
			case TOGGLEBMCLICKED:
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
					bms.docIndex=doc->pageIndex;
					bms.line=cursor.blockNumber()+1;
					bms.bmKey=this->bookMarksIndex;
					bms.menu=this->makeMenuItemClass(BOOKNARKSMENU,bms.bmLabel,0,NULL,"NOTNEEDED",this->bookMarksIndex);
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
	asprintf(&filename,"%s/" KKEDITFOLDER "/tools",getenv("HOME"));
	g_mkdir_with_parents(filename,493);
	debugFree(&filename,"init filename");

	this->readConfigs();

	if((this->queueID=msgget(this->sessionID,IPC_CREAT|0660))==-1)
		fprintf(stderr,"Can't create message queue, scripting wont work :( ...\n");

	this->checkMessages=new QTimer();
//linle a symlink.
//					 vv from object      vv signal 	  	   vv to object   vv func slot must inherit QObject
	QObject::connect(this->checkMessages,SIGNAL(timeout()),this,SLOT(doTimer()));
	this->checkMessages->start(this->prefsMsgTimer);

	//globalSlice=new StringSlice;
//
//
//	
//	loadKeybindings();
//
//	tmpStyleName=strdup(styleName);
//	tmpHighlightColour=highlightColour;
//

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

	asprintf(&htmlFile,"%s/Docview-%s.html",this->tmpFolderName.c_str(),globalSlice->randomName(6));
	asprintf(&htmlURI,"file://%s/Docview-%s.html",this->tmpFolderName.c_str(),globalSlice->randomName(6));

	if(this->forceDefaultGeom==false)
		r=this->prefs.value("app/geometry",QVariant(QRect(50,50,1024,768))).value<QRect>();
	this->mainWindow->setGeometry(r);


this->appShortcuts[HIDETABSHORTCUT]=new QShortcut(this->mainWindow);
//this->appShortcuts[HIDETABSHORTCUT].setKey(Qt::CTRL + Qt::Key_H);
this->appShortcuts[HIDETABSHORTCUT]->setKey(QKeySequence("Ctrl+H"));
//this->appShortcuts->id=0xaaee;
//this->appShortcuts->setKey(Qt::CTRL + Qt::Key_K);
this->appShortcuts[HIDETABSHORTCUT]->setObjectName(QString("%1").arg(HIDETABSHORTCUT));
			QObject::connect(this->appShortcuts[HIDETABSHORTCUT],SIGNAL(activated()),this,SLOT(doAppShortCuts()));
//QShortcut *sh=new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_K), this, SLOT(doAppShortCuts()));
	this->mainWindow->show();
return;
	refreshMainWindow();

	buildFindReplace();

	for(unsigned int j=0; j<g_slist_length(findList); j++)
		{
			reinterpret_cast<QComboBox*>(findDropBox)->addItem((const char*)g_slist_nth_data(findList,j));
		}
	reinterpret_cast<QComboBox*>(findDropBox)->setCurrentIndex(g_slist_length(findList)-1);

	for(unsigned int j=0; j<g_slist_length(findList); j++)
		{
			reinterpret_cast<QComboBox*>(replaceDropBox)->addItem((const char*)g_slist_nth_data(replaceList,j));
		}
	reinterpret_cast<QComboBox*>(replaceDropBox)->setCurrentIndex(g_slist_length(findList)-1);

//TODO//

#ifdef _BUILDDOCVIEWER_
	buildDocViewer();
#endif


//TODO//
	setSensitive();
}

void KKEditClass::doAppShortCuts(void)
{
	QShortcut	*sc=qobject_cast<QShortcut*>(sender());

	switch(sc->objectName().toInt())
		{
			case HIDETABSHORTCUT:
				this->mainNotebook->setTabVisible(this->mainNotebook->currentIndex(),false);
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
	this->prefsRootCommand=this->prefs.value("editor/rootcommand",GTKSUPATH).toString();

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
//					if(cnt==(HIDETAB-COPYFOLDERPATH)/0x100)//TODO//
//						{
//						fprintf(stderr,"here\n");
//							menuitem->setShortcut(QKeySequence("Ctrl+H"));
//						}
//					
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

	this->prefs.setValue("editor/funcsort",this->prefsFunctionMenuLayout);
	this->prefs.setValue("editor/prefsdepth",this->prefsDepth);
	this->prefs.setValue("editor/toolbarlayout",this->prefsToolBarLayout);
	this->prefs.setValue("editor/maxtabchars",this->prefsMaxTabChars);
	this->prefs.setValue("editor/maxfuncchars",this->prefsMaxFuncChars);
	this->prefs.setValue("editor/terminalcommand",this->prefsTerminalCommand);
	this->prefs.setValue("editor/rootcommand",this->prefsRootCommand);
	this->prefs.setValue("editor/toolbarlayout",this->prefsToolBarLayout);

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
}

void KKEditClass::findFile(void)
{
	DocumentClass	*document=this->getDocumentForTab(-1);
	char			*selection;
	StringSlice		slice;
	char			*filename=NULL;
	char			*filepath=NULL;
	char			*command;
	char			buffer[2048];
	char			*searchdir=NULL;
	FILE			*fp;

	if(document==NULL)
		return;

	selection=strdup(document->textCursor().block().text().toUtf8().constData());
	if(selection[0]!='#')
		return;

	filename=slice.sliceBetween(selection,(char*)"include ",NULL);

	if(slice.getResult()==0)
		{
			if(filename[0]=='<')
				searchdir=strdup("/usr/include");
			else
				searchdir=strdup(document->getDirPath().toStdString().c_str());//TODO//

			asprintf(&filepath,"%s/%s",searchdir,slice.sliceLen(filename,1,strlen(filename)-2));
			if(kkedit->openFile(filepath,0,false)==false)
				{
					asprintf(&command,"find \"%s\" -name \"%s\"",searchdir,basename(filepath));
					fp=popen(command, "r");
					if(fp!=NULL)
						{
							while(fgets(buffer,1024,fp))
								{
									buffer[strlen(buffer)-1]=0;
									kkedit->openFile(buffer,0,false);
								}
							pclose(fp);
						}
					debugFree(&command,"findFile command");
				}
			debugFree(&filepath,"findFile filepath");
			debugFree(&searchdir,"searchdir filepath");
		}

	free(selection);
}

void KKEditClass::showBarberPole(QString windowtitle,QString bodylabel,QString cancellabel,QString controlfiile)
{
}

void KKEditClass::buildDocs(void)
{
	DocumentClass	*doc=kkedit->getDocumentForTab(-1);
	struct stat		sb;
	FILE			*fp;
	char			line[4096];
	char			opdata[4096];
	QString			pipecom;

	if(doc==NULL)
		return;

	pipecom=QString("KKEditQTProgressBar \"Building Docs\" \"Please Wait ...\" \"\" \"%1/progress\" &").arg(this->tmpFolderName.c_str());
	this->runPipe(QString("KKEditQTProgressBar \"Building Docs\" \"Please Wait ...\" \"\" \"%1/progress\" &").arg(this->tmpFolderName.c_str()));

	QDir::setCurrent(doc->getDirPath());
	stat("Doxyfile",&sb);
	if(!S_ISREG(sb.st_mode))
		system("cp " DATADIR "/docs/Doxyfile .");

	if(thePage!=NULL)
		debugFree(&thePage,"doDoxy thePage");

	asprintf(&thePage,"file://%s/html/index.html",doc->getDirPath().toStdString().c_str());
	fp=popen("doxygen Doxyfile","r");
	while(fgets(line,4095,fp))
		{
			line[strlen(line)-1]=0;
			snprintf(opdata,4095,"echo -n \"%s\" >\"%s/progress\"",line,this->tmpFolderName.c_str());
			system(opdata);
		}
	pclose(fp);

	showDocView(USEURI,thePage,"Doxygen Documentation");

	this->runPipe(QString("echo quit>\"%1/progress\"").arg(this->tmpFolderName.c_str()));
}

void KKEditClass::showDocs(void)
{
	DocumentClass	*doc=kkedit->getDocumentForTab(-1);
	QFileInfo		fileinfo=QString("%1/html/index.html").arg(doc->getDirPath());

	if(fileinfo.exists()==false)
		this->buildDocs();
	else
		{
			debugFree(&thePage,"doDoxy thePage");
			asprintf(&thePage,"file://%s/html/index.html",doc->getDirPath().toStdString().c_str());
			showDocView(USEURI,thePage,"Doxygen Documentation");
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
		this->rebuildTabsMenu();

	this->sessionBusy=false;
	return(true);
}

void KKEditClass::shutDownApp()
{
	if(this->forcedMultInst==false)
		this->writeExitData();
//TODO
#ifdef _ASPELL_
	delete_aspell_config(this->aspellConfig);
	delete_aspell_speller(this->spellChecker);
#endif
	//if(onExitSaveSession)
	//	saveSession(NULL,0);
	//if(doSaveAll(widget,(uPtr)true)==false)
	//	return;
	//g_list_foreach(globalPlugins->plugins,releasePlugs,NULL);

	if(this->saveAllFiles()==true)
		QApplication::quit();
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



