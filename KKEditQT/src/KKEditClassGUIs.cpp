/*
 *
 * ©K. D. Hedger. Thu 23 Dec 20:39:44 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClassGUIs.cpp) is part of KKEditQT.

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

void KKEditClass::makePrefsDial(int widgnum,const QString label,int value,int minvalue,int maxvalue,int posy)
{
	QLabel*	widgetlabel;
	prefsIntWidgets[widgnum]=new QSpinBox;
	prefsIntWidgets[widgnum]->setProperty("maximum",maxvalue);
	prefsIntWidgets[widgnum]->setProperty("minimum",minvalue);
	prefsIntWidgets[widgnum]->setProperty("value",value);
	widgetlabel=new QLabel(label);
	this->table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	this->table->addWidget(prefsIntWidgets[widgnum],posy,1,1,-1,Qt::AlignVCenter);
}

void KKEditClass::makePrefsCheck(int widgnum,const QString label,bool onoff,int posx,int posy)
{
	prefsWidgets[widgnum]=new QCheckBox(label);
	prefsWidgets[widgnum]->setProperty("checked", onoff);
	if(posx!=-1)
		this->table->addWidget(prefsWidgets[widgnum],posy,posx,Qt::AlignTop);
}

void KKEditClass::doPrefs(void)
{
	this->resetKeyCombo();
	this->prefsWindow->setWindowModality(Qt::ApplicationModal);
	this->prefsWindow->raise();
	this->prefsWindow->setWindowState(Qt::WindowActive);
	this->prefsWindow->show();
}

void KKEditClass::buildPrefsWindow(void)
{
	QVBoxLayout			*mainvbox=new QVBoxLayout();
	QHBoxLayout			*hbox=new QHBoxLayout;
	QTabWidget			*prefsnotebook=new QTabWidget;
	QWidget				*button;
	QWidget				*tab;
	QLabel				*widgetlabel;
	int					posy;

	listWidget=new QListWidget;
	fromHBox=new QToolBar;

	this->prefsWindow=new QDialog(this->mainWindow);
	this->prefsWindow->setWindowTitle("Preferences");

	mainvbox->setContentsMargins(0,0,0,16);
	hbox->setContentsMargins(0,0,0,0);

	this->populateDnD();
	this->populateStore();

	QObject::connect(this->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(doDoubleClickPrefs(QListWidgetItem*)));

	this->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	this->listWidget->setDragEnabled(true);
	this->listWidget->setDragDropMode(QAbstractItemView::InternalMove);
	this->listWidget->viewport()->setAcceptDrops(true);
	this->listWidget->setFlow(QListView::LeftToRight);
	this->listWidget->setGridSize(QSize(32,32));
	this->listWidget->setSizePolicy (QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored));
	this->listWidget->setMinimumHeight(48);
	this->listWidget->setMinimumWidth(32*(strlen(this->prefsToolBarLayout.toStdString().c_str()))+4);//TODO//

	mainvbox->addWidget(this->listWidget,2);
	mainvbox->addWidget(this->fromHBox);
//pages
//page1
	this->table=new QGridLayout;
	tab=new QWidget();

//appearence 1
//prefsIndent
	posy=0;
	makePrefsCheck(AUTOINDENT,"Auto Indent Lines",this->prefsIndent,0,posy);
//linenumbers
	posy++;
	makePrefsCheck(SHOWNUMS,"Show Line Numbers",this->prefsShowLineNumbers,0,posy);
//wraplines
	posy++;
	makePrefsCheck(WRAP,"Wrap Lines",this->prefsLineWrap,0,posy);
//highlite
	posy++;
	makePrefsCheck(HIGHLIGHT,"Highlight Current Line",this->prefsHighLightline,0,posy);
//no syntax colour
	posy++;
	makePrefsCheck(SYNTAXHILITE,"Syntax Highlighting",this->prefsSyntaxHilighting,0,posy);
//single instance
	posy++;
	makePrefsCheck(USESINGLE,"Use Single Instance",this->prefsUseSingle,0,posy);

//auto save session
	posy++;
	makePrefsCheck(AUTOSAVE,"Auto Save/Restore Session",this->onExitSaveSession,0,posy);
//no duplicates
	posy++;
	makePrefsCheck(NODUPLICATE,"Don't Open Duplicate File",this->noDuplicates,0,posy);
//turn off warnings
	posy++;
	makePrefsCheck(NOWARN,"Don't Warn On File Change",this->noWarnings,0,posy);

//autoshow completion
	posy++;
	makePrefsCheck(AUTOSHOW,"Auto show Completions",this->prefsAutoShowCompletions,0,posy);

	tab->setLayout(table);
	prefsnotebook->addTab(tab,"General Appearance");

//page2
	table=new QGridLayout;
	tab=new QWidget();

//tabwidth
	posy=0;
	makePrefsDial(TABWIDTH,"Tab width:",this->prefsTabWidth,2,64,posy);

//TODO//
//style
	posy++;
	prefsOtherWidgets[THEMECOMBO]=new QComboBox;
	widgetlabel=new QLabel("Theme:");
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[THEMECOMBO],posy,1,posy,-1,Qt::AlignVCenter);

//shortcuts
	posy++;
	prefsOtherWidgets[SHORTCUTSCOMBO]=new QComboBox;
	this->resetKeyCombo();
    QObject::connect(prefsOtherWidgets[SHORTCUTSCOMBO],SIGNAL(activated(int)),this,SLOT(buildGetKeyShortCut(int)));

	widgetlabel=new QLabel("Set Keyboard Shortcuts:");
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[SHORTCUTSCOMBO],posy,1,posy-1,-1,Qt::AlignBaseline);

//font button
	posy++;
	widgetlabel = new QLabel("Font:");
	prefsOtherWidgets[PREFSCURRENTFONT]=new QLabel(QString("%1 %2").arg(this->prefsDocumentFont.family()).arg(this->prefsDocumentFont.pointSize()));
	int frameStyle1 = QFrame::Sunken|QFrame::Panel;//TODO//
	qobject_cast<QLabel*>(prefsOtherWidgets[PREFSCURRENTFONT])->setFrameStyle(frameStyle1);
    QPushButton *fontbutton = new QPushButton("Set Font");
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[PREFSCURRENTFONT],posy,1,Qt::AlignVCenter);
	table->addWidget(fontbutton,posy,2,Qt::AlignVCenter);
	QObject::connect(fontbutton,SIGNAL(clicked()),this,SLOT(setFont()));

//current linecol
	int frameStyle=QFrame::Raised | QFrame::Panel;
	posy++;
    widgetlabel = new QLabel("Highlight Current Line Colour:");
	prefsOtherWidgets[CURRENTLINECOLOUR]=new QLabel("LINE");
    qobject_cast<QLabel*>(prefsOtherWidgets[CURRENTLINECOLOUR])->setFrameStyle(frameStyle);
    QPushButton *colorButton = new QPushButton("Set Colour");
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[CURRENTLINECOLOUR],posy,1,Qt::AlignVCenter);
	table->addWidget(colorButton,posy,2,Qt::AlignVCenter);

	QColor colour=QColor(this->prefsHiLiteLineColor);
	prefsOtherWidgets[CURRENTLINECOLOUR]->setProperty("palette",QPalette(colour));
	prefsOtherWidgets[CURRENTLINECOLOUR]->setProperty("autoFillBackground",true);
	QObject::connect(colorButton,SIGNAL(clicked()),this,SLOT(setLineColour()));

//bm highlight colour
	posy++;
    widgetlabel = new QLabel("Bookmark Highlight Colour:");
	prefsOtherWidgets[BMHIGHLIGHTCOLOUR]=new QLabel("BOOKMARK");
    qobject_cast<QLabel*>(prefsOtherWidgets[BMHIGHLIGHTCOLOUR])->setFrameStyle(frameStyle);
    QPushButton *colorButton1 = new QPushButton("Set Colour");
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[BMHIGHLIGHTCOLOUR],posy,1,Qt::AlignVCenter);
	table->addWidget(colorButton1,posy,2,Qt::AlignVCenter);

	QColor colour1=QColor(this->prefsBookmarkHiLiteColor);
	prefsOtherWidgets[BMHIGHLIGHTCOLOUR]->setProperty("palette",QPalette(colour1));
	prefsOtherWidgets[BMHIGHLIGHTCOLOUR]->setProperty("autoFillBackground",true);
	QObject::connect(colorButton1,SIGNAL(clicked()),this,SLOT(setBMColour()));

//autoshow completion
	posy++;
	makePrefsDial(COMPLETIONSIZE,"Completion Minimum Word Size:",this->autoShowMinChars,2,20,posy);

//sort functions
	posy++;
	prefsOtherWidgets[FUNCTIONCOMBO]=new QComboBox;
	qobject_cast<QComboBox*>(prefsOtherWidgets[FUNCTIONCOMBO])->addItem("Display functions etc in menu by type and alphabetically");
	qobject_cast<QComboBox*>(prefsOtherWidgets[FUNCTIONCOMBO])->addItem("Display functions etc in menu by type and file position");
	qobject_cast<QComboBox*>(prefsOtherWidgets[FUNCTIONCOMBO])->addItem("Display functions etc in menu by file position");
	qobject_cast<QComboBox*>(prefsOtherWidgets[FUNCTIONCOMBO])->addItem("Display functions etc in menu alphabetically");
	qobject_cast<QComboBox*>(prefsOtherWidgets[FUNCTIONCOMBO])->addItem("Display functions etc in menu in categorised format");
	prefsOtherWidgets[FUNCTIONCOMBO]->setProperty("currentIndex",this->prefsFunctionMenuLayout);

	hbox=new QHBoxLayout;
    hbox->addStretch(1);
	hbox->addWidget(prefsOtherWidgets[FUNCTIONCOMBO]);
	hbox->setStretch(1,2);
	hbox->addStretch(1);
	table->addLayout(hbox,posy,0,1,-1);

	QSpacerItem *space=new QSpacerItem(0,0,QSizePolicy::Maximum,QSizePolicy::Maximum);
	posy++;
	table->addItem(space,posy,0,100,-1);

	table->setColumnStretch(1,1);
	tab->setLayout(table);

	prefsnotebook->addTab(tab,"Text Style");

//page 3
//admin
	table=new QGridLayout;
	tab=new QWidget();
//function search prefsDepth
	posy=0;
	makePrefsDial(MAXFUNCDEPTH,"Tag File Search Depth:",this->prefsDepth,0,20,posy);

//terminalcommand
	posy++;
    widgetlabel=new QLabel("Terminal Command:");
	prefsOtherWidgets[PREFSTERMCOMMAND]=new QLineEdit(this->prefsTerminalCommand);
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[PREFSTERMCOMMAND],posy,1,1,-1,Qt::AlignVCenter);

//root command
	posy++;
    widgetlabel=new QLabel("Run As Root Command:");
	prefsOtherWidgets[PREFSROOTCOMMAND]=new QLineEdit(this->prefsRootCommand);
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[PREFSROOTCOMMAND],posy,1,1,-1,Qt::AlignVCenter);

//set default browser
	posy++;
    widgetlabel=new QLabel("Default Browser:");
	prefsOtherWidgets[PREFSBROWSERCOMMAND]=new QLineEdit(this->browserCommand);
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[PREFSBROWSERCOMMAND],posy,1,1,-1,Qt::AlignVCenter);
	space=new QSpacerItem(0,0,QSizePolicy::Maximum,QSizePolicy::Maximum);

//find replace history max
	posy++;
	makePrefsDial(MAXHISTORY,"Max Find/Replace History:",this->maxFRHistory,0,MAXTEXTWIDTH,posy);
//max tab label width
	posy++;
	makePrefsDial(MAXTABCHARS,"Max Characters In Tab:",this->prefsMaxTabChars,5,MAXTEXTWIDTH,posy);
//max function strings
	posy++;
	makePrefsDial(MENUWIDTH,"Max Characters In Function Defs:",this->prefsMaxFuncChars,5,MAXTEXTWIDTH,posy);
//max bookmark strings
	posy++;
	makePrefsDial(MAXBMWIDTH,"Max Characters In Bookmarks:",this->maxBMChars,5,MAXTEXTWIDTH,posy);

//use global plug menu
	posy++;
	makePrefsCheck(GLOBALPLUGMENU,"Use Global Plugins Menu ( Requires Restart )",this->prefsUseGlobalPlugMenu,0,posy);
//end admin

	posy++;
	table->addItem(space,posy,0,100,-1);

	table->setColumnStretch(1,1);

	tab->setLayout(this->table);
	prefsnotebook->addTab(tab,"Administration");

	mainvbox->addWidget(prefsnotebook);
//nag
	posy++;
    widgetlabel=new QLabel("<b>Be Kind To Poor Programmers</b>");

	hbox=new QHBoxLayout;
    hbox->addStretch(1);
	hbox->addWidget(widgetlabel);
	hbox->addStretch(1);
	mainvbox->addLayout(hbox,0);

//TODO//
	makePrefsCheck(BEKIND,"I have donated",this->prefsNagScreen,-1,-1);
	hbox=new QHBoxLayout;
    hbox->addStretch(1);
	hbox->addWidget(prefsWidgets[BEKIND]);
	hbox->addStretch(1);
	mainvbox->addLayout(hbox,0);

    widgetlabel=new QLabel("I have really donated some some money to the author.\nMy conscience is clear and my Karma is squeaky clean :)");
	hbox=new QHBoxLayout;
    hbox->addStretch(1);
	hbox->addWidget(widgetlabel);
	hbox->addStretch(1);
	mainvbox->addLayout(hbox,0);
 
    widgetlabel=new QLabel;
    frameStyle=QFrame::Sunken | QFrame::HLine;
	widgetlabel->setFrameStyle(frameStyle);
	mainvbox->addWidget(widgetlabel);

//buttons
	hbox=new QHBoxLayout;
	hbox->addStretch(1);
	button=new QPushButton("Save Prefs");
	//QObject::connect(((QPushButton*)button),&QPushButton::clicked,setPrefs);
	//	QObject::connect(this->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(doDoubleClickPrefs(QListWidgetItem*)));
	QObject::connect(button,SIGNAL(clicked()),this,SLOT(setPreferences()));
	hbox->addWidget(button);
	hbox->addStretch(1);
	button=new QPushButton("Restore Prefs");
	button->setObjectName(QString("%1").arg(CANCELPREFS));
	QObject::connect(button,SIGNAL(clicked()),this,SLOT(doOddButtons()));
	//QObject::connect(((QPushButton*)button),&QPushButton::clicked,cancelPrefs);
	hbox->addWidget(button);
	hbox->addStretch(1);
	mainvbox->addLayout(hbox);

	this->prefsWindow->setLayout(mainvbox);

	this->prefsWindow->setWindowModality(Qt::WindowModal);
}

void KKEditClass::addIcon(const char* icon,const char* data,int toolnumber,const char* tooltip)
{
	QIcon qicon;
	MenuItemClass* menuitem=new MenuItemClass(icon);
	qicon=QIcon::fromTheme(icon);
	menuitem->setObjectName(data);
	menuitem->setIcon(qicon);
	QObject::connect(menuitem,SIGNAL(triggered()),this,SLOT(addToToolBar()));
	menuitem->setMenuID(toolnumber);
	menuitem->setToolTip(tooltip);

	this->fromHBox->addAction(menuitem);
	this->tool[toolnumber]=menuitem;
}

void KKEditClass::populateDnD(void)
{
	this->addIcon("document-new","N",0,"New File");
	this->addIcon("document-open","O",1,"Open File");
	this->addIcon("document-save","S",2,"Save File");
	this->addIcon("edit-cut","X",3,"Cut");
	this->addIcon("edit-copy","C",4,"Copy");
	this->addIcon("edit-paste","P",5,"Paste");
	this->addIcon("edit-undo","U",6,"Undo");
	this->addIcon("edit-redo","R",7,"Redo");
	this->addIcon("edit-find","F",8,"Find");
	this->addIcon("dialog-question","G",9,"Go To Definition");
	this->addIcon("go-previous","B",17,"Go Back");
	this->addIcon("go-next","W",18,"Go Forward");
	this->addIcon(DATADIR"/pixmaps/num.png","9",10,"Go To Line");
	this->addIcon(DATADIR"/pixmaps/api.png","A",11,"Find API In Gtk Docs");
	this->addIcon(DATADIR"/pixmaps/qtapi.png","Q",16,"Find API In Qt5 Docs");
	this->addIcon(DATADIR"/pixmaps/finddef.png","D",12,"Search For Define");
	this->addIcon(DATADIR"/pixmaps/live.png","L",13,"Live Search");
	this->addIcon(DATADIR"/pixmaps/sep.png","s",14,"Separator");
	this->addIcon(DATADIR"/pixmaps/expand.png","E",15,"Expander");
}

void KKEditClass::addIconToList(const char* name,int type)
{
	QIcon icon;
	QListWidgetItem *iconw;

	icon=QIcon::fromTheme(name);
	iconw=new QListWidgetItem(icon,"",0,type);
	this->listWidget->addItem(iconw);
}

void KKEditClass::populateStore(void)
{
	for(int j=0;j<this->prefsToolBarLayout.length();j++)
		{
			switch(this->prefsToolBarLayout.at(j).toLatin1())
				{
//new
					case 'N':
						this->addIconToList("document-new",'N');
						this->tool[0]->setEnabled(false);
						break;

//open+recent
					case 'O':
						this->addIconToList("document-open",'O');
						this->tool[1]->setEnabled(false);
						break;
//save
					case 'S':
						this->addIconToList("document-save",'S');
						this->tool[2]->setEnabled(false);
						break;
//cut
					case 'X':
						this->addIconToList("edit-cut",'X');
						this->tool[3]->setEnabled(false);
						break;
//copy
					case 'C':
						this->addIconToList("edit-copy",'C');
						this->tool[4]->setEnabled(false);
						break;
//paste
					case 'P':
						this->addIconToList("edit-paste",'P');
						this->tool[5]->setEnabled(false);
						break;
//undo
					case 'U':
						this->addIconToList("edit-undo",'U');
						this->tool[6]->setEnabled(false);
						break;
//redo
					case 'R':
						this->addIconToList("edit-redo",'R');
						this->tool[7]->setEnabled(false);
						break;
//find
					case 'F':
						this->addIconToList("edit-find",'F');
						this->tool[8]->setEnabled(false);
						break;
//navigation
					case 'G':
						this->addIconToList("dialog-question",'G');
						this->tool[9]->setEnabled(false);
						break;
//go back
					case 'B':
						this->addIconToList("go-previous",'B');
						this->tool[17]->setEnabled(false);
						break;
//go forward
					case 'W':
						this->addIconToList("go-next",'W');
						this->tool[18]->setEnabled(false);
						break;
//go to line
					case '9':
						this->addIconToList(DATADIR"/pixmaps/num.png",'9');
						this->tool[10]->setEnabled(false);
						break;
//find api gtk
					case 'A':
						this->addIconToList(DATADIR"/pixmaps/api.png",'A');
						this->tool[11]->setEnabled(false);
						break;
//find api qt5
					case 'Q':
						this->addIconToList(DATADIR"/pixmaps/qtapi.png",'Q');
						this->tool[16]->setEnabled(false);
						break;
//find def
					case 'D':
						this->addIconToList(DATADIR"/pixmaps/finddef.png",'D');
						this->tool[12]->setEnabled(false);
						break;
//live search
					case 'L':
						this->addIconToList(DATADIR"/pixmaps/live.png",'L');
						this->tool[13]->setEnabled(false);
						break;
//seperator
					case 's':
						this->addIconToList(DATADIR"/pixmaps/sep.png",'s');
						break;
//expander
					case 'E':
						this->addIconToList(DATADIR"/pixmaps/expand.png",'E');
						break;
				}
		}
}

void KKEditClass::buildFindReplace(void)
{
	QVBoxLayout*	vlayout=new QVBoxLayout;
	QWidget*		hbox;
	QHBoxLayout*	hlayout;
	QLabel			*label;
	QPushButton		*button;
	QIcon			icon;

	this->findReplaceDialog=new QDialog(this->mainWindow);
	this->findReplaceDialog->setWindowTitle("Find/Replace");
	vlayout->setContentsMargins(4,0,4,0);

//find
	label=new QLabel("Find");
	label->setAlignment(Qt::AlignCenter);
	vlayout->addWidget(label);

	this->findDropBox=new QComboBox;
	reinterpret_cast<QComboBox*>(this->findDropBox)->setEditable(true);
	reinterpret_cast<QComboBox*>(this->findDropBox)->setCompleter(0);
	this->findDropBox->addItems(this->findList);
	vlayout->addWidget(this->findDropBox);

//replace
	label=new QLabel("Replace With");
	label->setAlignment(Qt::AlignCenter);
	vlayout->addWidget(label);

	this->replaceDropBox=new QComboBox;
	reinterpret_cast<QComboBox*>(this->replaceDropBox)->setEditable(true);
	reinterpret_cast<QComboBox*>(this->replaceDropBox)->setCompleter(0);
	this->replaceDropBox->addItems(this->replaceList);
	vlayout->addWidget(replaceDropBox);

//switches 1st row
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,0,0,0);
	hbox=new QWidget;
	hbox->setLayout(hlayout);

//case
	this->frSwitches[FRCASE]=new QCheckBox("Case insensitive");
	this->frSwitches[FRCASE]->setChecked(this->insensitiveSearch);
	hlayout->addWidget(this->frSwitches[FRCASE]);
//use regex
	this->frSwitches[FRUSEREGEX]=new QCheckBox("Use Regex");
	this->frSwitches[FRUSEREGEX]->setChecked(this->useRegex);
	hlayout->addWidget(this->frSwitches[FRUSEREGEX]);

	vlayout->addWidget(hbox);

//switches 2nd row
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,0,0,0);
	hbox=new QWidget;
	hbox->setLayout(hlayout);
//wrap
	this->frSwitches[FRWRAP]=new QCheckBox("Wrap");
	this->frSwitches[FRWRAP]->setChecked(this->wrapSearch);
	hlayout->addWidget(this->frSwitches[FRWRAP]);

//all files
	this->frSwitches[FRALLFILES]=new QCheckBox("All Files");
	this->frSwitches[FRALLFILES]->setChecked(this->findInAllFiles);
	hlayout->addWidget(this->frSwitches[FRALLFILES]);
//hilite all
	this->frSwitches[FRHIGHLIGHTALL]=new QCheckBox("Highlight All");
	this->frSwitches[FRHIGHLIGHTALL]->setChecked(this->hightlightAll);
	hlayout->addWidget(this->frSwitches[FRHIGHLIGHTALL]);
//rep all
	this->frSwitches[FRREPLACEALL]=new QCheckBox("Replace All");
	this->frSwitches[FRREPLACEALL]->setChecked(this->replaceAll);
	QObject::connect(this->frSwitches[FRREPLACEALL],SIGNAL(stateChanged(int)),this,SLOT(setSearchPrefs(int)));
	hlayout->addWidget(this->frSwitches[FRREPLACEALL]);

	vlayout->addWidget(hbox);

//switches 3rd row
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,4,0,4);
	hbox=new QWidget;
	hbox->setLayout(hlayout);

	button=new QPushButton("Forward");
	button->setObjectName(FINDNEXTOBJECTNAME);
	QObject::connect(button,SIGNAL(clicked()),this,SLOT(doFindButton()));
	icon=QIcon::fromTheme("go-next");
	button->setIcon(icon);
	hlayout->addWidget(button);

	button=new QPushButton("Back");
	button->setObjectName(FINDPREVOBJECTNAME);
	QObject::connect(button,SIGNAL(clicked()),this,SLOT(doFindButton()));
	icon=QIcon::fromTheme("go-previous");
	button->setIcon(icon);
	hlayout->addWidget(button);

	if(replaceAll==false)
		this->frReplace=new QPushButton("Replace");
	else
		this->frReplace=new QPushButton("Replace All");
	frReplace->setObjectName(FINDREPLACEOBJECTNAME);
	QObject::connect(frReplace,SIGNAL(clicked()),this,SLOT(doFindButton()));
	icon=QIcon::fromTheme("edit-find-replace");
	this->frReplace->setIcon(icon);
	hlayout->addWidget(this->frReplace);

	vlayout->addWidget(hbox);

	this->findReplaceDialog->setLayout(vlayout);
}

void KKEditClass::buildMainGui(void)
{
	MenuItemClass	*menuItemSink;

	//this->mainWindow=new QMainWindow;
//	this->mainWindowVBox=new QVBoxLayout();//TODO//HMMmmmmmm
//	this->mainWindowVBox->setContentsMargins(2,2,2,2);
//	this->mainWindow->setLayout(this->mainWindowVBox);

	this->mainNotebook=new QTabWidget;

//contextMenuEvent
	this->tabBar=this->mainNotebook->tabBar();
	this->tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
	QObject::connect(this->tabBar,SIGNAL(customContextMenuRequested(const QPoint &)),SLOT(tabContextMenu(const QPoint &)));

	this->mainNotebook->setDocumentMode(true);
	this->mainNotebook->setTabsClosable(true);
	this->mainNotebook->setMovable(true);
	
	//QObject::connect(this->mainNotebook,&QTabWidget::currentChanged(int),switchPage(int));//TODO//
	QObject::connect(this->mainNotebook,SIGNAL(currentChanged(int)),this,SLOT(switchPage(int)));
	QObject::connect(this->mainNotebook,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
	//QObject::connect(this->mainNotebook,&QTabWidget::tabCloseRequested,closeTabQT);
////	QObject::connect(qobject_cast<QWidget*>(mainNotebook),&QWidget::keyPressEvent,keyp);

	this->menuBar=new QMenuBar;
	this->toolBar=new QToolBar;


//file menu
	this->fileMenu=new QMenu("&File");
	this->menuBar->addMenu(this->fileMenu);

//new
	this->newMenuItem=this->makeMenuItemClass(FILEMENU,"New",QKeySequence::New,"document-new",NEWMENUNAME,NEWMENUITEM);

//open
	this->openMenuItem=this->makeMenuItemClass(FILEMENU,"Open",QKeySequence::Open,"document-open",OPENMENUNAME,OPENMENUITEM);

//open as hexdump
	menuItemSink=this->makeMenuItemClass(FILEMENU,"Open As Hexdump",0,"document-open",HEXDUMPMENUNAME,HEXDUMPMENUITEM);

	this->fileMenu->addSeparator();

//extras
	menuItemSink=this->makeMenuItemClass(FILEMENU,"New Admin Editor",0,DATADIR"/pixmaps/ROOTKKEditQT.png",NEWADMINMENUNAME,NEWADMINEDMENUITEM);
	menuItemSink=this->makeMenuItemClass(FILEMENU,"New Editor",0,DATADIR"/pixmaps/MenuKKEditQT.png",NEWEDITORMENUNAME,NEWEDMENUITEM);
	
	if(gotManEditor==0)
		menuItemSink=this->makeMenuItemClass(FILEMENU,"Manpage Editor",0,DATADIR"/pixmaps/ManPageEditor.png",MANEDITORMENUNAME,MANPAGEEDMENUITEM);

//doxy
	if(gotDoxygen==0)
		menuItemSink=this->makeMenuItemClass(FILEMENU,"Build Documentation",0,"edit-copy",DOXYBUILDMENUNAME,BUILDDOCSMENUITEM);

	this->fileMenu->addSeparator();

//recent menu
//TODO//
//	makeMenuItem(this->fileMenu,"Recent Files",0,NULL,RECENTMENUNAME,&newFileWrap,NULL,1000);
	this->fileMenu->addSeparator();

//save
	this->saveMenuItem=this->makeMenuItemClass(FILEMENU,"Save",QKeySequence::Save,"document-save",SAVEMENUNAME,SAVEMENUITEM);

//savas
	this->saveAsMenuItem=this->makeMenuItemClass(FILEMENU,"Save As",QKeySequence("Ctrl+Shift+S"),"document-save-as",SAVEASMENUNAME,SAVEASMENUITEM);

//save all
	this->saveAllMenuItem=this->makeMenuItemClass(FILEMENU,"Save All",0,"document-save",SAVEALLMENUNAME,SAVEALLMENUITEM);

	this->fileMenu->addSeparator();

//save session
	this->saveSessionMenuItem=this->makeMenuItemClass(FILEMENU,"Save Session",0,"document-save",SAVESESSIONMENUNAME,SAVESESSIONMENUITEM);

//restore session
	this->restoreSessionMenuItem=this->makeMenuItemClass(FILEMENU,"Save Session",0,"document-save",RESTORESESSIONMENUNAME,RESTORESESSIONMENUITEM);

//printfile
	this->printMenuItem=this->makeMenuItemClass(FILEMENU,"Print",QKeySequence::Print,"document-print",PRINTMENUNAME,PRINTMENUITEM);

	this->fileMenu->addSeparator();

//close
	this->closeMenuItem=this->makeMenuItemClass(FILEMENU,"Close",QKeySequence::Close,"window-close",CLOSEMENUNAME,CLOSEMENUITEM);
	//this->closeMenuItem=this->makeMenuItemClass(FILEMENU,"Close",QKeySequence::Close,"window-close",CLOSEMENUNAME,CLOSEMENUITEM);

//close-all
	this->closeAllMenuItem=this->makeMenuItemClass(FILEMENU,"Close All",0,"window-close",CLOSEALLMENUNAME,CLOSEALLMENUITEM);

	this->fileMenu->addSeparator();

//reload file
	this->revertMenuItem=this->makeMenuItemClass(FILEMENU,"Revert",0,"document-revert",REVERTMENUNAME,REVERTMENUITEM);

	this->fileMenu->addSeparator();

//quit
	menuItemSink=this->makeMenuItemClass(FILEMENU,"Quit",QKeySequence::Quit,"application-exit",QUITMENUNAME,QUITMENUITEM);

//edit menu
	this->editMenu=new QMenu("&Edit");
	this->menuBar->addMenu(this->editMenu);

//undo
	this->undoMenuItem=this->makeMenuItemClass(EDITMENU,"Undo",QKeySequence::Undo,"edit-undo",UNDOMENUNAME,UNDOMENUITEM);

//redo
	this->redoMenuItem=this->makeMenuItemClass(EDITMENU,"Redo",QKeySequence::Redo,"edit-redo",REDOMENUNAME,REDOMENUITEM);

//undoall
	this->undoAllMenuItem=this->makeMenuItemClass(EDITMENU,"Undo All",0,"edit-undo",UNDOALLMENUNAME,UNDOALLMENUITEM);

//redoall
	this->redoAllMenuItem=this->makeMenuItemClass(EDITMENU,"Redo All",0,"edit-redo",REDOALLMENUNAME,REDOALLMENUITEM);

	this->editMenu->addSeparator();

//cut
	this->cutMenuItem=this->makeMenuItemClass(EDITMENU,"Cut",QKeySequence::Cut,"edit-cut",CUTMENUNAME,CUTMENUITEM);
//copy
	this->copyMenuItem=this->makeMenuItemClass(EDITMENU,"Copy",QKeySequence::Copy,"edit-copy",COPYMENUNAME,COPYMENUITEM);
//paste
	this->pasteMenuItem=this->makeMenuItemClass(EDITMENU,"Paste",QKeySequence::Paste,"edit-paste",PASTEMENUNAME,PASTEMENUITEM);

	this->editMenu->addSeparator();

//find
	this->findMenuItem=this->makeMenuItemClass(EDITMENU,"Find",QKeySequence::Find,"edit-find",FINDMENUNAME,FINDMENUITEM);
//find next
	this->findNextMenuItem=this->makeMenuItemClass(EDITMENU,"Find Next",QKeySequence("Ctrl+G"),"go-next",FINDNEXTMENUNAME,FINDNEXTMENUITEM);

	this->editMenu->addSeparator();

//sort tabs
	this->sortTabsMenuItem=this->makeMenuItemClass(EDITMENU,"Sort Tabs",0,"view-sort-ascending",SORTTABSMENUNAME,SORTTABSMENUITEM);
//show all tabs
	this->showAllTabsMenuItem=this->makeMenuItemClass(EDITMENU,"Show All Tabs",0,"list-add",SHOWALLTABSMENUNAME,SHOWALLTABSMENUITEM);
//select tab
	this->selectTabMenu=new QMenu("Select Tab");
	this->editMenu->addMenu(this->selectTabMenu);

	this->editMenu->addSeparator();

//prefs
	menuItemSink=this->makeMenuItemClass(EDITMENU,"Preferences",0,"preferences-desktop",PREFSMENUNAME,PREFSMENUITEM);
//plugs
	menuItemSink=this->makeMenuItemClass(EDITMENU,"Plugin Prefs",0,"preferences-system",PLUGPREFSMENUNAME,PLUGPREFSMENUITEM);

//view menu
	this->viewMenu=new QMenu("&View");
	this->menuBar->addMenu(this->viewMenu);

//show docs
	menuItemSink=this->makeMenuItemClass(VIEWMENU,"Show Documentation",0,NULL,SHOWDOCSMENUNAME,DOCSMENUITEM);

//toggle toolbar bar
	if(this->toolbarVisible)//TODO//
		toggleToolBarMenuItem=this->makeMenuItemClass(VIEWMENU,"Hide Tool Bar",0,NULL,SHOWTOOLBARMENUNAME,TOGGLETOOLBARMENUITEM);
	else
		toggleToolBarMenuItem=this->makeMenuItemClass(VIEWMENU,"Show Tool Bar",0,NULL,SHOWTOOLBARMENUNAME,TOGGLETOOLBARMENUITEM);

//tooloutput
	if(this->toolWindowVisible)
		this->toggleToolWindowMenuItem=this->makeMenuItemClass(VIEWMENU,"Hide Tool Output",0,NULL,SHOWTOOLOUTMENUNAME,TOGGLETOOLWINDOWMENUITEM);
	else
		this->toggleToolWindowMenuItem=this->makeMenuItemClass(VIEWMENU,"Show Tool Output",0,NULL,SHOWTOOLOUTMENUNAME,TOGGLETOOLWINDOWMENUITEM);

//toggle statusbar
	if(this->statusBarVisible)
		this->toggleStatusBarMenuItem=this->makeMenuItemClass(VIEWMENU,"Hide Status Bar",0,NULL,SHOWSTATUSMENUNAME,TOGGLESTATUSBARMENUITEM);
	else
		this->toggleStatusBarMenuItem=this->makeMenuItemClass(VIEWMENU,"Show Status Bar",0,NULL,SHOWTOOLOUTMENUNAME,TOGGLESTATUSBARMENUITEM);

#ifdef _BUILDDOCVIEWER_
//toggle docviewer
	this->toggleDocViewMenuItem=this->makeMenuItemClass(VIEWMENU,"Show Docviewer",0,NULL,SHOWDOCVIEWERMENUNAME,TOGGLEDOCVIEWMENUITEM);
#endif

	this->viewMenu->addSeparator();

//toggle line numbers
	this->lineNumbersVisible=this->prefsShowLineNumbers;
	this->toggleLineNumbersMenuItem=this->makeMenuItemClass(VIEWMENU,"Show Line Numbers",0,NULL,"NOTNEEDED",TOGGLELINENUMBERSMENUITEM);
	this->toggleLineNumbersMenuItem->setCheckable(true);
	this->toggleLineNumbersMenuItem->setChecked(this->prefsShowLineNumbers);
//toggle wrap line
	this->wrapLine=this->prefsLineWrap;
	this->toggleWrapLineMenuItem=this->makeMenuItemClass(VIEWMENU,"Wrap Lines",0,NULL,"NOTNEEDED",TOGGLELINEWRAPMENUITEM);
	this->toggleWrapLineMenuItem->setCheckable(true);
	this->toggleWrapLineMenuItem->setChecked(this->prefsLineWrap);
//toggle hilit current line
	this->hiliteCurrentLine=this->prefsHighLightline;
	this->toggleHiliteCurrentLineMenuItem=this->makeMenuItemClass(VIEWMENU,"Highlight Current Line",0,NULL,"NOTNEEDED",TOGGLEHILITELINEMENUITEM);
	this->toggleHiliteCurrentLineMenuItem->setCheckable(true);
	this->toggleHiliteCurrentLineMenuItem->setChecked(this->hiliteCurrentLine);
//show hiliting
	this->showHighLighting=this->prefsSyntaxHilighting;
	this->toggleSyntaxHilightingMenuItem=this->makeMenuItemClass(VIEWMENU,"Syntax Highlighting",0,NULL,"NOTNEEDED",TOGGLESYNTAXHILITINGMENUITEM);
	this->toggleSyntaxHilightingMenuItem->setCheckable(true);
	this->toggleSyntaxHilightingMenuItem->setChecked(this->showHighLighting);
//toggle whitespace
	this->toggleWhiteSpaceMenuItem=this->makeMenuItemClass(VIEWMENU,"Show Whitespace",0,NULL,"NOTNEEDED",TOGGLEWHITESPACEMENUITEM);
	this->toggleWhiteSpaceMenuItem->setCheckable(true);
	this->toggleWhiteSpaceMenuItem->setChecked(false);
//QCompleter
//toggle completions
	this->showCompletions=this->prefsAutoShowCompletions;
	this->toggleCompletionsMenuItem=this->makeMenuItemClass(VIEWMENU,"Auto Show Completions",0,NULL,"NOTNEEDED",TOGGLECOMPLETIONSMENUITEM);
	this->toggleCompletionsMenuItem->setCheckable(true);
	this->toggleCompletionsMenuItem->setChecked(this->showCompletions);

//navigation menu
	this->navMenu=new QMenu("&Navigation");
	this->menuBar->addMenu(this->navMenu);

//goto define
	goToDefineMenuItem=this->makeMenuItemClass(NAVMENU,"Go To Definition",0,"dialog-question",GOTODEFMENUNAME,GOTODEFINEMENUITEM);
//open include
	menuItemSink=this->makeMenuItemClass(NAVMENU,"Open Include File",QKeySequence::fromString("Ctrl+I"),"document-open",GOTODEFMENUNAME,OPENINCLUDEMENUITEM);
//goto line number
	goToLineDialogMenuItem=this->makeMenuItemClass(NAVMENU,"Go To Line",0,"go-down",GOTOLINEMENUNAME,GOTOLINEMENUITEM);
//find define
	menuItemSink=this->makeMenuItemClass(NAVMENU,"Search For Define",0,"edit-find",SEARCHFORDEFMENUNAME,SEARCHFORDEFINEMENUITEM);
//find gtkdoc
	menuItemSink=this->makeMenuItemClass(NAVMENU,"Search In Gtk Docs",0,"edit-find",SEARCHGTKMENUNAME,SEARCHGTKDOCS);
//find qt5
	menuItemSink=this->makeMenuItemClass(NAVMENU,"Search In Qt5 Docs",0,"edit-find",SEARCHQT5MENUNAME,SEARCHQT5DOCS);
//goto doxy docs
	if(gotDoxygen==0)
		menuItemSink=this->makeMenuItemClass(NAVMENU,"Find In Documentation",0,"edit-find",SEARCHDOXYMENUNAME,SEARCHDOXYDOCS);
//go back
	this->goBackMenu=this->makeMenuItemClass(NAVMENU,"Back",0,"go-previous","NOTNEEDED",GOBACKMENUITEM);
//go forward
	this->goFowardMenu=this->makeMenuItemClass(NAVMENU,"Forward",0,"go-next","NOTNEEDED",GOFORWARDMENUITEM);

	this->navMenu->addSeparator();

//function menu
	this->funcMenu=new QMenu("Fun&ctions");
	this->menuBar->addMenu(this->funcMenu);

//newbookmarks
	this->bookMarkMenu=new QMenu("&Bookmarks");
	this->menuBar->addMenu(this->bookMarkMenu);
	this->rebuildBookMarkMenu();

//external tools	
	this->toolsMenu=new QMenu("&Tools");
	this->menuBar->addMenu(this->toolsMenu);
	this->buildTools();

//TODO//
//plugin menu

//help
	this->helpMenu=new QMenu("&Help");
	this->menuBar->addMenu(this->helpMenu);
//
//about
	menuItemSink=this->makeMenuItemClass(HELPMENU,"About",0,"help-about",ABOUTMENUNAME,ABOUTMENUITEM);
//help
	menuItemSink=this->makeMenuItemClass(HELPMENU,"Help",0,"help-contents",HELPMENUNAME,HELPMENUITEM);
	menuItemSink=this->makeMenuItemClass(HELPMENU,"Online Help",0,"help-contents",ONLINEHELPMENUNAME,ONLINEHELPMENUITEM);
//get plugins
	menuItemSink=this->makeMenuItemClass(HELPMENU,"Get Plugins",0,DATADIR"/pixmaps/KKEditQTPlugMenu.png",GETPLUGSMENUNAME,GETPLUGSMENUITEM);

	this->setUpToolBar();
	this->mainWindow->setMenuBar(qobject_cast<QMenuBar*>(this->menuBar));
	this->mainWindow->addToolBar(this->toolBar);
	this->mainWindow->setCentralWidget(this->mainNotebook);

	this->statusText=new QLabel;	
	this->statusText->setText("Line 0\tCol 0");
 	this->statusBar=this->mainWindow->statusBar();
 	this->statusBar->addWidget(this->statusText);
}

void KKEditClass::showLineEntry(void)
{
	bool	ok;

	QString	text=QInputDialog::getText(this->mainWindow,"Go To Line","Enter Line Number",QLineEdit::Normal,"0",&ok);

	if ((ok==true) && (!text.isEmpty()))
		{
			this->gotoLine(text.toUInt());
		}
}

void KKEditClass::buildTools(void)
{
	MenuItemClass	*menuItemSink;
//TODO//
printf("TODO buildTools\n");
//	this->toolsMenu=new QMenu("&Tools");
//	this->menuBar->addMenu(this->toolsMenu);
//	this->toolsMenu->addSeparator();
	menuItemSink=this->makeMenuItemClass(TOOLSMENU,"Manage External Tools",0,"accessories-text-editor","NOTNEEDED",MANAGETOOLSMENUITEM);
	this->toolsMenu->addSeparator();
//	menuitem=gtk_image_menu_item_new_with_label("Manage External Tools");
//	image=gtk_image_new_from_stock(GTK_STOCK_EDIT,GTK_ICON_SIZE_MENU);
//	gtk_image_menu_item_set_image((GtkImageMenuItem *)menuitem,image);
//	gtk_menu_shell_append(GTK_MENU_SHELL(menu),menuitem);
//	g_signal_connect(G_OBJECT(menuitem),"activate",G_CALLBACK(doMakeTool),NULL);

}

void KKEditClass::sortTabs(void)
{
	QTabBar	*bar=this->mainNotebook->tabBar();
	bool	flag=true;
	DocumentClass	*doc1;
	DocumentClass	*doc2;

	while(flag==true)
		{
			flag=false;
			for(int j=0;j<bar->count()-1;j++)
				{
					doc1=this->getDocumentForTab(j);
					doc2=this->getDocumentForTab(j+1);
					if(doc1->getFileName().compare(doc2->getFileName())>0)
						{
							flag=true;
							bar->moveTab(j+1,j);
						}
				}
		}
	rebuildTabsMenu();
}

void KKEditClass::rebuildTabsMenu(void)
{
	DocumentClass	*doc;
	MenuItemClass	*menuitem;
	QTabBar			*bar=this->mainNotebook->tabBar();

	this->selectTabMenu->clear();
	for(int j=0;j<bar->count();j++)
		{
			doc=this->getDocumentForTab(j);
			menuitem=new MenuItemClass(doc->getFileName());
			menuitem->setMenuID(j);
			this->selectTabMenu->addAction(menuitem);
			QObject::connect(menuitem,SIGNAL(triggered()),this,SLOT(doSelectTab()));
		}
}

void KKEditClass::buildSpellCheckerGUI(void)
{
#ifdef _ASPELL_
	QVBoxLayout*	vlayout=new QVBoxLayout;
	QWidget*		hbox;
	QHBoxLayout*	hlayout;
	QLabel			*label;
	QPushButton		*button;
	QIcon			icon;

	this->spellCheckGUI=new QDialog(this->mainWindow);
	this->spellCheckGUI->setWindowTitle("Spell Check Word");
	vlayout->setContentsMargins(4,0,4,0);

//find
	this->infoLabel=new QLabel("Change word to:");
	this->infoLabel->setAlignment(Qt::AlignCenter);
	vlayout->addWidget(this->infoLabel);

	this->wordDropBox=new QComboBox;
	this->wordDropBox->setEditable(true);
	//reinterpret_cast<QComboBox*>(this->findDropBox)->setCompleter(0);
	vlayout->addWidget(this->wordDropBox);

//switches 3rd row
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,4,0,4);
	hbox=new QWidget;
	hbox->setLayout(hlayout);

	button=new QPushButton("Apply");
	button->setObjectName(QString("%1").arg(APPLYWORDBUTTON));
	QObject::connect(button,SIGNAL(clicked()),this,SLOT(doOddButtons()));
	hlayout->addWidget(button);

	button=new QPushButton("Ignore");
	button->setObjectName(QString("%1").arg(IGNOREWORDBUTTON));
	QObject::connect(button,SIGNAL(clicked()),this,SLOT(doOddButtons()));
	hlayout->addWidget(button);

	button=new QPushButton("Add");
	button->setObjectName(QString("%1").arg(ADDWORDBUTTON));
	QObject::connect(button,SIGNAL(clicked()),this,SLOT(doOddButtons()));
	hlayout->addWidget(button);

	button=new QPushButton("Cancel");
	button->setObjectName(QString("%1").arg(CANCELSPELLCHECK));
	QObject::connect(button,SIGNAL(clicked()),this,SLOT(doOddButtons()));
	hlayout->addWidget(button);

	vlayout->addWidget(hbox);

	this->spellCheckGUI->setLayout(vlayout);
#endif
}

bool KKEditClass::openFileDialog(void)
{
	QStringList fileNames;

	fileNames=QFileDialog::getOpenFileNames(this->mainWindow,"Open File","","",0);
	if (fileNames.count())
		{
			this->openFromDialog=true;
			for (int j=0;j<fileNames.size();j++)
				this->openFile(fileNames.at(j).toUtf8().constData(),0,true);
		}
	this->openFromDialog=false;
	switchPage(this->mainNotebook->currentIndex());
	return(true);
}

void KKEditClass::resetKeyCombo(void)
{
	qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->clear();
	qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->addItem(QString("Hide tab - %1").arg(this->defaultShortCutsList.at(HIDETABSHORTCUT)));
	qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->addItem(QString("Delete current line - %1").arg(this->defaultShortCutsList.at(DELETELINESHORTCUT)));
	qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->addItem(QString("Delete to EOL - %1").arg(this->defaultShortCutsList.at(DELETETOEOLSHORTCUT)));
	qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->addItem(QString("Delete to SOL - %1").arg(this->defaultShortCutsList.at(DELETETOSOLSHORTCUT)));
	qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->addItem(QString("Select word under cursor - %1").arg(this->defaultShortCutsList.at(SELECTWORDSHORTCUT)));
	qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->addItem(QString("Delete word under cursor - %1").arg(this->defaultShortCutsList.at(DELETEWORDSHORTCUT)));
	qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->addItem(QString("Duplicate line - %1").arg(this->defaultShortCutsList.at(DUPLICATELINESHORTCUT)));
	qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->addItem(QString("Select current line - %1").arg(this->defaultShortCutsList.at(SELECTLINESHORTCUT)));
	qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->addItem(QString("Move line up - %1").arg(this->defaultShortCutsList.at(MOVELINEUPSHORTCUT)));
	qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->addItem(QString("Move line down - %1").arg(this->defaultShortCutsList.at(MOVELINEDOWNSHORTCUT)));
	qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->addItem(QString("Move Selection up - %1").arg(this->defaultShortCutsList.at(MOVESELECTIONUPSHORTCUT)));
	qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->addItem(QString("Move Selection down - %1").arg(this->defaultShortCutsList.at(MOVESELECTIONDOWNSHORTCUT)));
}

void KKEditClass::buildGetKeyShortCut(int index)
{
	bool	ok;

	QString	text=QInputDialog::getText(this->mainWindow,"Keyboard Shortcut","Enter Key Combo",QLineEdit::Normal,this->defaultShortCutsList.at(index),&ok);
	if ((ok==true) && (!text.isEmpty()))
		{
			this->defaultShortCutsList[index]=text;
			this->resetKeyCombo();
			qobject_cast<QComboBox*>(prefsOtherWidgets[SHORTCUTSCOMBO])->setCurrentIndex(index);
			this->setAppShortcuts();
		}
}

void KKEditClass::buildDocViewer(void)
{
#ifdef _BUILDDOCVIEWER_
	QVBoxLayout	*docvlayout=new QVBoxLayout;
	QHBoxLayout	*dochlayout=new QHBoxLayout;
	QWidget		*widget;
	QPushButton	*button;
	QRect		r;

	this->docView=new QMainWindow(mainWindow);

//TODO//close event
//QObject::connect((QWidget*)this->docView,&QWidget::closeEvent,[=]() {fprintf(stderr,">>>>>\n");});

	r=this->prefs.value("app/viewergeometry",QVariant(QRect(100,100,800,600))).value<QRect>();
	this->docView->setGeometry(r);
	
	widget=new QWidget;
	widget->setLayout(docvlayout);
	this->docView->setCentralWidget(widget);

	this->webView=new QWebView(widget);
	this->webView->load(QUrl("file://" DATADIR "/help/index.html"));

	docvlayout->addWidget(this->webView);

	button=new QPushButton(QIcon::fromTheme("go-previous"),"Back");
	QObject::connect(button,&QPushButton::clicked,[=]() {this->webView->page()->triggerAction(QWebPage::Back);});
	dochlayout->addWidget(button);

	dochlayout->addStretch(1);

	button=new QPushButton(QIcon::fromTheme("go-home"),"Home");
	dochlayout->addWidget(button);
	button->setObjectName(QString("%1").arg(DOCVIEWERGOHOME));
	QObject::connect(button,SIGNAL(clicked()),this,SLOT(doOddButtons()));

	dochlayout->addStretch(1);

	button=new QPushButton(QIcon::fromTheme("edit-find"),"Find");
	QObject::connect(button,&QPushButton::clicked,[=]() {QTextStream(stderr) << "find" << Qt::endl;});
	dochlayout->addWidget(button);

	widget=new QLineEdit;
	dochlayout->addWidget(widget);

	button=new QPushButton(QIcon::fromTheme("go-down"),"Down");
	QObject::connect(button,&QPushButton::clicked,[=]() {QTextStream(stderr) << "Down" << Qt::endl;});
	dochlayout->addWidget(button);
	
	button=new QPushButton(QIcon::fromTheme("go-up"),"Up");
	QObject::connect(button,&QPushButton::clicked,[=]() {QTextStream(stderr) << "Up" << Qt::endl;});
	dochlayout->addWidget(button);

	dochlayout->addStretch(1);

	button=new QPushButton(QIcon::fromTheme("go-next"),"Forward");
	dochlayout->addWidget(button);
	QObject::connect(button,&QPushButton::clicked,[=]() {this->webView->page()->triggerAction(QWebPage::Forward);});

	widget=new QWidget;
	widget->setLayout(dochlayout);
	docvlayout->addWidget(widget);

	this->docView->hide();
#endif
}
