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

	widget=new QPushButton(QIcon::fromTheme("go-previous",QIcon("go-previous")),"Back");
	dochlayout->addWidget(widget);
	QObject::connect((QPushButton*)widget,&QPushButton::clicked,webKitGoBack);

	dochlayout->addStretch(1);

	widget=new QPushButton(QIcon::fromTheme("go-home",QIcon("go-home")),"Home");
	dochlayout->addWidget(widget);
	QObject::connect((QPushButton*)widget,&QPushButton::clicked,webKitGoHome);

	dochlayout->addStretch(1);

	widget=new QPushButton(QIcon::fromTheme("edit-find",QIcon("edit-find")),"Find");
	dochlayout->addWidget(widget);

	widget=new QLineEdit;
	dochlayout->addWidget(widget);

	widget=new QPushButton(QIcon::fromTheme("go-down",QIcon("go-down")),"Down");
	dochlayout->addWidget(widget);
	widget=new QPushButton(QIcon::fromTheme("go-up",QIcon("go-up")),"Up");
	dochlayout->addWidget(widget);

	dochlayout->addStretch(1);

	widget=new QPushButton(QIcon::fromTheme("go-next",QIcon("go-next")),"Forward");
	dochlayout->addWidget(widget);
	QObject::connect((QPushButton*)widget,&QPushButton::clicked,webKitGoForward);

	widget=new QWidget;
	widget->setLayout(dochlayout);
	docvlayout->addWidget(widget);

	this->docView->hide();
#endif
}


//void switchTab(int thispage)//TODO//move ?
//{
//fprintf(stderr,"here\n");
//}

//void KKEditClass::buildMainGui(void)
//{
//	MenuItemClass	*menuItemSink;
//
//	this->mainWindow=new QMainWindow;
////	this->mainWindowVBox=new QVBoxLayout();//TODO//HMMmmmmmm
////	this->mainWindowVBox->setContentsMargins(2,2,2,2);
////	this->mainWindow->setLayout(this->mainWindowVBox);
//
//	this->mainNotebook=new QTabWidget;
//	
//	qobject_cast<QTabWidget*>(this->mainNotebook)->setDocumentMode(true);
//	qobject_cast<QTabWidget*>(this->mainNotebook)->setTabsClosable(true);
//	qobject_cast<QTabWidget*>(this->mainNotebook)->setMovable(true);
//	
//	QObject::connect(qobject_cast<QTabWidget*>(this->mainNotebook),&QTabWidget::currentChanged,switchPage);
//	//connect(this,SIGNAL(&QTabWidget::currentChanged(int)),this,SLOT(switchTab(int)));
//	QObject::connect(qobject_cast<QTabWidget*>(this->mainNotebook),&QTabWidget::tabCloseRequested,closeTabQT);
//////	QObject::connect(qobject_cast<QWidget*>(mainNotebook),&QWidget::keyPressEvent,keyp);
//
//	this->menuBar=new QMenuBar;
//	this->toolBar=new QToolBar;
//
//
////file menu
//	this->fileMenu=new QMenu("&File");
//	this->menuBar->addMenu(this->fileMenu);
//
////new
//	this->newMenuItem=this->makeMenuItemClass(FILEMENU,"New",QKeySequence::New,"document-new",NEWMENUNAME,NEWMENUITEM);
//
////open
//	this->openMenuItem=this->makeMenuItemClass(FILEMENU,"Open",QKeySequence::Open,"document-open",OPENMENUNAME,OPENMENUITEM);
//
////open as hexdump
//	menuItemSink=this->makeMenuItemClass(FILEMENU,"Open As Hexdump",0,"document-open",HEXDUMPMENUNAME,HEXDUMPMENUITEM);
//
//	this->fileMenu->addSeparator();
//
////extras
//	menuItemSink=this->makeMenuItemClass(FILEMENU,"New Admin Editor",0,DATADIR"/pixmaps/ROOTKKEdit.png",NEWADMINMENUNAME,NEWADMINEDMENUITEM);
//	menuItemSink=this->makeMenuItemClass(FILEMENU,"New Editor",0,DATADIR"/pixmaps/MenuKKEdit.png",NEWEDITORMENUNAME,NEWEDMENUITEM);
//	
//	if(gotManEditor==0)
//		menuItemSink=this->makeMenuItemClass(FILEMENU,"Manpage Editor",0,DATADIR"/pixmaps/ManPageEditor.png",MANEDITORMENUNAME,MANPAGEEDMENUITEM);
//
////doxy
//	if(gotDoxygen==0)
//		menuItemSink=this->makeMenuItemClass(FILEMENU,"Build Documentation",0,"edit-copy",DOXYBUILDMENUNAME,BUILDDOCSMENUITEM);
//
//	this->fileMenu->addSeparator();
//
////recent menu
////TODO//
////	makeMenuItem(this->fileMenu,"Recent Files",0,NULL,RECENTMENUNAME,&newFileWrap,NULL,1000);
//	this->fileMenu->addSeparator();
//
////save
//	this->saveMenuItem=this->makeMenuItemClass(FILEMENU,"Save",QKeySequence::Save,"document-save",SAVEMENUNAME,SAVEMENUITEM);
//
////savas
//	this->saveAsMenuItem=this->makeMenuItemClass(FILEMENU,"Save As",QKeySequence("Ctrl+Shift+S"),"document-save-as",SAVEASMENUNAME,SAVEASMENUITEM);
//
////save all
//	this->saveAllMenuItem=this->makeMenuItemClass(FILEMENU,"Save All",0,"document-save",SAVEALLMENUNAME,SAVEALLMENUITEM);
//
//	this->fileMenu->addSeparator();
//
////save session
//	this->saveSessionMenuItem=this->makeMenuItemClass(FILEMENU,"Save Session",0,"document-save",SAVESESSIONMENUNAME,SAVESESSIONMENUITEM);
//
////restore session
//	this->restoreSessionMenuItem=this->makeMenuItemClass(FILEMENU,"Save Session",0,"document-save",RESTORESESSIONMENUNAME,RESTORESESSIONMENUITEM);
//
////printfile
//	this->printMenuItem=this->makeMenuItemClass(FILEMENU,"Print",QKeySequence::Print,"document-print",PRINTMENUNAME,PRINTMENUITEM);
//
//	this->fileMenu->addSeparator();
//
////close
//	this->closeMenuItem=this->makeMenuItemClass(FILEMENU,"Close",QKeySequence::Close,"window-close",CLOSEMENUNAME,CLOSEMENUITEM);
//
////close-all
//	this->closeAllMenuItem=this->makeMenuItemClass(FILEMENU,"Close All",0,"window-close",CLOSEALLMENUNAME,CLOSEALLMENUITEM);
//
//	this->fileMenu->addSeparator();
//
////reload file
//	this->revertMenuItem=this->makeMenuItemClass(FILEMENU,"Revert",0,"document-revert",REVERTMENUNAME,REVERTMENUITEM);
//
//	this->fileMenu->addSeparator();
//
////quit
//	menuItemSink=this->makeMenuItemClass(FILEMENU,"Quit",QKeySequence::Quit,"application-exit",QUITMENUNAME,QUITMENUITEM);
//
////edit menu
//	this->editMenu=new QMenu("&Edit");
//	this->menuBar->addMenu(this->editMenu);
//
////undo
//	this->undoMenuItem=this->makeMenuItemClass(EDITMENU,"Undo",QKeySequence::Undo,"edit-undo",UNDOMENUNAME,UNDOMENUITEM);
//
////redo
//	this->redoMenuItem=this->makeMenuItemClass(EDITMENU,"Redo",QKeySequence::Redo,"edit-redo",REDOMENUNAME,REDOMENUITEM);
//
////undoall
//	this->undoAllMenuItem=this->makeMenuItemClass(EDITMENU,"Undo All",0,"edit-undo",UNDOALLMENUNAME,UNDOALLMENUITEM);
//
////redoall
//	this->redoAllMenuItem=this->makeMenuItemClass(EDITMENU,"Redo All",0,"edit-redo",REDOALLMENUNAME,REDOALLMENUITEM);
//
//	this->editMenu->addSeparator();
//
////cut
//	this->cutMenuItem=this->makeMenuItemClass(EDITMENU,"Cut",QKeySequence::Cut,"edit-cut",CUTMENUNAME,CUTMENUITEM);
////copy
//	this->copyMenuItem=this->makeMenuItemClass(EDITMENU,"Copy",QKeySequence::Copy,"edit-copy",COPYMENUNAME,COPYMENUITEM);
////paste
//	this->pasteMenuItem=this->makeMenuItemClass(EDITMENU,"Paste",QKeySequence::Paste,"edit-paste",PASTEMENUNAME,PASTEMENUITEM);
//
//	this->editMenu->addSeparator();
//
////find
//	this->findMenuItem=this->makeMenuItemClass(EDITMENU,"Find",QKeySequence::Find,"edit-find",FINDMENUNAME,FINDMENUITEM);
////find next
//	this->findNextMenuItem=this->makeMenuItemClass(EDITMENU,"Find Next",QKeySequence("Ctrl+G"),"go-next",FINDNEXTMENUNAME,FINDNEXTMENUITEM);
//
//	this->editMenu->addSeparator();
//
////sort tabs
//	this->sortTabsMenuItem=this->makeMenuItemClass(EDITMENU,"Sort Tabs",0,"view-sort-ascending",SORTTABSMENUNAME,SORTTABSMENUITEM);
////show all tabs
//	this->showAllTabsMenuItem=this->makeMenuItemClass(EDITMENU,"Show All Tabs",0,"list-add",SHOWALLTABSMENUNAME,SHOWALLTABSMENUITEM);
////select tab
//	this->selectTabMenuItem=this->makeMenuItemClass(EDITMENU,"Select Tab",0,"accessories-text-editor",SELECTTABMENUNAME,SELECTTABMENUITEM);
//
//	this->editMenu->addSeparator();
//
////prefs
//	menuItemSink=this->makeMenuItemClass(EDITMENU,"Preferences",0,"preferences-desktop",PREFSMENUNAME,PREFSMENUITEM);
////plugs
//	menuItemSink=this->makeMenuItemClass(EDITMENU,"Plugin Prefs",0,"preferences-system",PLUGPREFSMENUNAME,PLUGPREFSMENUITEM);
//
////////////////////////////////////////////
////view menu
//	this->viewMenu=new QMenu("&View");
//	this->menuBar->addMenu(this->viewMenu);
//
////show docs
//	menuItemSink=this->makeMenuItemClass(VIEWMENU,"Show Documentaion",0,NULL,SHOWDOCSMENUNAME,DOCSMENUITEM);
//
////toggle toolbar bar
//	if(this->toolbarVisible)//TODO//
//		toggleToolBarMenuItem=this->makeMenuItemClass(VIEWMENU,"Hide Tool Bar",0,NULL,SHOWTOOLBARMENUNAME,TOGGLETOOLBARMENUITEM);
//	else
//		toggleToolBarMenuItem=this->makeMenuItemClass(VIEWMENU,"Show Tool Bar",0,NULL,SHOWTOOLBARMENUNAME,TOGGLETOOLBARMENUITEM);
//
////tooloutput
//	if(this->toolWindowVisible)
//		this->toggleToolWindowMenuItem=this->makeMenuItemClass(VIEWMENU,"Hide Tool Output",0,NULL,SHOWTOOLOUTMENUNAME,TOGGLETOOLWINDOWMENUITEM);
//	else
//		this->toggleToolWindowMenuItem=this->makeMenuItemClass(VIEWMENU,"Show Tool Output",0,NULL,SHOWTOOLOUTMENUNAME,TOGGLETOOLWINDOWMENUITEM);
//
////toggle statusbar
//	if(this->statusBarVisible)
//		this->toggleStatusBarMenuItem=this->makeMenuItemClass(VIEWMENU,"Hide Status Bar",0,NULL,SHOWSTATUSMENUNAME,TOGGLESTATUSBARMENUITEM);
//	else
//		this->toggleStatusBarMenuItem=this->makeMenuItemClass(VIEWMENU,"Show Status Bar",0,NULL,SHOWTOOLOUTMENUNAME,TOGGLESTATUSBARMENUITEM);
//
//#ifdef _BUILDDOCVIEWER_
////toggle docviewer
//	this->toggleDocViewMenuItem=this->makeMenuItemClass(VIEWMENU,"Show Docviewer",0,NULL,SHOWDOCVIEWERMENUNAME,TOGGLEDOCVIEWMENUITEM);
//#endif
//
//	this->viewMenu->addSeparator();
//
////toggle line numbers
//	this->lineNumbersVisible=this->prefsShowLineNumbers;
//	this->toggleLineNumbersMenuItem=this->makeMenuItemClass(VIEWMENU,"Show Line Numbers",0,NULL,"NOTNEEDED",TOGGLELINENUMBERSMENUITEM);
//	this->toggleLineNumbersMenuItem->setCheckable(true);
//	this->toggleLineNumbersMenuItem->setChecked(this->prefsShowLineNumbers);
////toggle wrap line
//	this->wrapLine=this->prefsLineWrap;
//	this->toggleWrapLineMenuItem=this->makeMenuItemClass(VIEWMENU,"Wrap Lines",0,NULL,"NOTNEEDED",TOGGLELINEWRAPMENUITEM);
//	this->toggleWrapLineMenuItem->setCheckable(true);
//	this->toggleWrapLineMenuItem->setChecked(this->prefsLineWrap);
////toggle hilit current line
//	this->hiliteCurrentLine=this->prefsHighLightline;
//	this->toggleHiliteCurrentLineMenuItem=this->makeMenuItemClass(VIEWMENU,"Highlight Current Line",0,NULL,"NOTNEEDED",TOGGLEHILITELINEMENUITEM);
//	this->toggleHiliteCurrentLineMenuItem->setCheckable(true);
//	this->toggleHiliteCurrentLineMenuItem->setChecked(this->hiliteCurrentLine);
////show hiliting
//	this->showHighLighting=this->prefsSyntaxHilighting;
//	this->toggleSyntaxHilightingMenuItem=this->makeMenuItemClass(VIEWMENU,"Syntax Highlighting",0,NULL,"NOTNEEDED",TOGGLESYNTAXHILITINGMENUITEM);
//	this->toggleSyntaxHilightingMenuItem->setCheckable(true);
//	this->toggleSyntaxHilightingMenuItem->setChecked(this->showHighLighting);
////toggle whitespace
//	this->toggleWhiteSpaceMenuItem=this->makeMenuItemClass(VIEWMENU,"Show Whitespace",0,NULL,"NOTNEEDED",TOGGLEWHITESPACEMENUITEM);
//	this->toggleWhiteSpaceMenuItem->setCheckable(true);
//	this->toggleWhiteSpaceMenuItem->setChecked(false);
////QCompleter
////toggle completions
//	this->showCompletions=this->prefsAutoShowCompletions;
//	this->toggleCompletionsMenuItem=this->makeMenuItemClass(VIEWMENU,"Auto Show Completions",0,NULL,"NOTNEEDED",TOGGLECOMPLETIONSMENUITEM);
//	this->toggleCompletionsMenuItem->setCheckable(true);
//	this->toggleCompletionsMenuItem->setChecked(this->showCompletions);
//
////////////////////////////////////////////
//
////navigation menu
//	this->navMenu=new QMenu("&Navigation");
//	this->menuBar->addMenu(this->navMenu);
//
////goto define
//	this->goToDefineMenu=reinterpret_cast<QWidget*>(makeMenuItem(this->navMenu,"Go To Definition",QKeySequence::fromString("Ctrl+D"),"dialog-question",GOTODEFMENUNAME,&goToDefinition,NULL,0));
//
////open include
//	makeMenuItem(this->navMenu,"Open Include File",QKeySequence::fromString("Ctrl+I"),"document-open",OPENINCLUDEMENUNAME,&findFile,NULL,0);
////goto line number
//	makeMenuItem(this->navMenu,"Go To Line",0,"go-down",GOTOLINEMENUNAME,&jumpToLine,NULL,0);
////find define
//	makeMenuItem(this->navMenu,"Search For Define",0,"edit-find",SEARCHFORDEFMENUNAME,&functionSearch,NULL,0);
////find gtkdoc
//	makeMenuItem(this->navMenu,"Search In Gtk Docs",0,"edit-find",SEARCHGTKMENUNAME,&searchGtkDocs,NULL,0);
////find qt5
//	makeMenuItem(this->navMenu,"Search In Qt5 Docs",0,"edit-find",SEARCHQT5MENUNAME,&searchQT5Docs,NULL,0);
////goto doxy docs
//	if(gotDoxygen==0)
//		makeMenuItem(this->navMenu,"Find In Documentation",0,"edit-find",SEARCHDOXYMENUNAME,&doxyDocs,NULL,0);
//////go back
//	this->goBackMenu=this->makeMenuItemClass(NAVMENU,"Back",0,"go-previous","NOTNEEDED",GOBACKMENUITEM);
//////go forward
//	this->goFowardMenu=this->makeMenuItemClass(NAVMENU,"Forward",0,"go-next","NOTNEEDED",GOFORWARDMENUITEM);
//
//	qobject_cast<QMenu*>(this->navMenu)->addSeparator();
////////////////////////////////////////////
//
////function menu
//	this->funcMenu=new QMenu("Fun&ctions");
//	this->menuBar->addMenu(this->funcMenu);
//
/////////////////////////////////////////////
//
////newbookmarks
//	this->bookMarkMenu=new QMenu("&Bookmarks");
//	this->menuBar->addMenu(this->bookMarkMenu);
//	this->rebuildBookMarkMenu();
//
/////////////////////////////////////////////
//
////external tools	
//	this->toolsMenu=new QMenu("&Tools");
//	this->menuBar->addMenu(this->toolsMenu);
//	buildTools();
//
/////////////////////////////////////////////
//
////help
//	this->helpMenu=new QMenu("&Help");
//	this->menuBar->addMenu(this->helpMenu);
//
////about
//	makeMenuItem(this->helpMenu,"&About",0,"help-about",ABOUTMENUNAME,&doAbout,NULL,0);
////help
//	makeMenuItem(this->helpMenu,"&Help",0,"help-contents",HELPMENUNAME,&openHelp,NULL,0);
//
////	menuitem=makeMenuItem(GTK_STOCK_HELP,menu,(void*)openHelp,0,HELPMENUNAME,STOCKMENU,NULL,NULL);
////get plugins
//	makeMenuItem(this->helpMenu,"&Get Plugins",0,DATADIR"/pixmaps/KKEditPlugMenu.png",GETPLUGSMENUNAME,&getPlugins,NULL,0);
////	menuitem=makeMenuItem(DATADIR"/pixmaps/KKEditPlugMenu.png",menu,(void*)getPlugins,0,GETPLUGSMENUNAME,PIXMAPMENU,"Get Plugins",NULL);
//
//
//	//this->mainWindow
//	//this->mainWindowVBox->addWidget(menuBar);
//	//this->mainWindowVBox->addWidget(toolBar);
//	//this->mainWindowVBox->addWidget(mainNotebook);
//	this->setUpToolBar();
//	//qobject_cast<QMainWindow*>(this->mainWindow)->setStatusBar(statusbar);
//
//
//	this->mainWindow->setMenuBar(qobject_cast<QMenuBar*>(this->menuBar));
//	this->mainWindow->addToolBar(this->toolBar);
//	this->mainWindow->setCentralWidget(this->mainNotebook);
//
//	this->statusText=new QLabel;	
//	this->statusText->setText("Line 0\tCol 0");
// 	this->statusBar=this->mainWindow->statusBar();
// 	this->mainWindow->show();
// 	//fprintf(stderr,">>%s<<\n",this->statusText->text().toStdString().c_str());
// 	this->statusBar->addWidget(this->statusText);
//	//statbar=reinterpret_cast<QMainWindow*>(this->mainWindow)->statusBar();
//	//statbar=qobject_cast<QMainWindow*>(this->mainWindow)->statusBar();
// 	//reinterpret_cast<QMainWindow*>(this->mainWindow)->statusBar()->showMessage("Ready", 2000);
//}

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
									sessionBusy=true;
									QTextBlock			block=doc->document()->findBlockByLineNumber(value.line);
									QTextBlockFormat	bf=block.blockFormat();

									bf.clearBackground();
									cursor.setBlockFormat(bf);
									sessionBusy=false;
								
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
				qobject_cast<QTabWidget*>(this->mainNotebook)->setCurrentWidget(doc);
				this->gotoLine(bms.line);
				break;
		}
}

void KKEditClass::initApp(int argc,char** argv)
{
	char	tmpfoldertemplate[]="/tmp/KKEditQT-XXXXXX";
	int		exitstatus;
	char*	filename;

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

//#ifdef _ASPELL_
//	AspellCanHaveError*	possible_err;
//	spellChecker=NULL;
//	aspellConfig=NULL;
//	aspellConfig=new_aspell_config();
//	possible_err=new_aspell_speller(aspellConfig);
//
//	if(aspell_error_number(possible_err)!= 0)
//		puts(aspell_error_message(possible_err));
//	else
//		spellChecker=to_aspell_speller(possible_err);
//#endif
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

	asprintf(&htmlFile,"%s/Docview-%s.html",this->tmpFolderName.c_str(),globalSlice->randomName(6));
	asprintf(&htmlURI,"file://%s/Docview-%s.html",this->tmpFolderName.c_str(),globalSlice->randomName(6));

	if(this->forceDefaultGeom==false)
		{
			QRect r=this->prefs.value("editor/geom",QVariant(QRect(50,50,1024,768))).value<QRect>();
			this->mainWindow->setGeometry(r);
		}
	else
		{
			QRect r(0,0,1024,768);
			this->mainWindow->setGeometry(r);
		}

	fprintf(stderr,"this->currentWorkSpace=%i\n",this->currentWorkSpace);
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

bool KKEditClass::openFile(std::string filepath,int linenumber,bool warn)
{
	DocumentClass	*doc=new DocumentClass(this);
	bool			retval=false;
	QFile			file(QString::fromStdString(filepath));
	QFileInfo		fileinfo(file);

	busyFlag=true;
	sessionBusy=true;

	retval=file.open(QIODevice::Text | QIODevice::ReadOnly);
	if(retval==true)
		{
			QString			content=QString::fromUtf8(file.readAll());
			QMimeDatabase	db;
			QMimeType		type=db.mimeTypeForFile(fileinfo.canonicalFilePath());
			fprintf(stderr,">>%s<<\n",type.name().toStdString().c_str());
			doc->mimeType=type.name();
			doc->setPlainText(content);
			doc->setFilePrefs();
			doc->pageIndex=this->newPageIndex;
			this->pages[this->newPageIndex++]=doc;
			doc->tabNumber=qobject_cast<QTabWidget*>(kkedit->mainNotebook)->addTab(doc,doc->getTabName());
			doc->setDirPath(fileinfo.canonicalPath());
			doc->setFilePath(fileinfo.canonicalFilePath());
			doc->setFileName(fileinfo.fileName());
			doc->setTabName(truncateWithElipses(doc->getFileName(),this->prefsMaxTabChars));
			qobject_cast<QTabWidget*>(kkedit->mainNotebook)->setTabToolTip(doc->tabNumber,doc->getFilePath());
			qobject_cast<QTabWidget*>(kkedit->mainNotebook)->setCurrentIndex(doc->tabNumber);
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
		}
	busyFlag=false;
	sessionBusy=false;
	doc->dirty=false;
	if(doc->tabNumber==0)
		switchPage(0);
	return(retval);
}

bool KKEditClass::openFileDialog(void)
{
	QStringList fileNames;

	fileNames=QFileDialog::getOpenFileNames(this->mainWindow,"Open File","","",0);
	if (fileNames.count())
		{
			for (int j=0;j<fileNames.size();j++)
				this->openFile(fileNames.at(j).toUtf8().constData(),0,true);
		}
	return(true);
}

void KKEditClass::writeExitData(void)
{
//editor
	if(this->forceDefaultGeom==false)
		this->prefs.setValue("editor/geom",this->mainWindow->geometry());
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
