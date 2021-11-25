/*
 *
 * ©K. D. Hedger. Sun 21 Nov 13:26:59 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClassGUIs.cpp) is part of KKEdit.

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

void KKEditClass::doPrefs(Widget* widget,uPtr data)
{
//TODO//HMMMmmmmmm
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
	makePrefsCheck(AUTOSAVE,"Auto Save/Restore Session",onExitSaveSession,0,posy);
//	g_signal_connect(G_OBJECT(prefsWidgets[AUTOSAVE]),"toggled",G_CALLBACK(setPrefs),(void*)prefsWidgets[AUTOSAVE]);

//no duplicates
	posy++;
	makePrefsCheck(NODUPLICATE,"Don't Open Duplicate File",noDuplicates,0,posy);
//turn off warnings
	posy++;
	makePrefsCheck(NOWARN,"Don't Warn On File Change",noWarnings,0,posy);

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
	makePrefsDial(COMPLETIONSIZE,"Completion Minimum Word Size:",autoShowMinChars,2,20,posy);

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

//show keybindings dialog
//TODO//
	posy++;
	hbox=new QHBoxLayout;
    button = new QPushButton("Customize Keyboard Shortcuts");
	hbox->addStretch(1);
	hbox->addWidget(button);
	hbox->setStretch(1,2);
	hbox->addStretch(1);
	table->addLayout(hbox,posy,0,1,-1,Qt::AlignVCenter);
	
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
	prefsOtherWidgets[PREFSBROWSERCOMMAND]=new QLineEdit(browserCommand);
	table->addWidget(widgetlabel,posy,0,Qt::AlignVCenter);
	table->addWidget(prefsOtherWidgets[PREFSBROWSERCOMMAND],posy,1,1,-1,Qt::AlignVCenter);
	space=new QSpacerItem(0,0,QSizePolicy::Maximum,QSizePolicy::Maximum);

//find replace history max
	posy++;
	makePrefsDial(MAXHISTORY,"Max Find/Replace History:",maxFRHistory,0,MAXTEXTWIDTH,posy);
//max tab label width
	posy++;
	makePrefsDial(MAXTABCHARS,"Max Characters In Tab:",this->prefsMaxTabChars,5,MAXTEXTWIDTH,posy);
//max function strings
	posy++;
	makePrefsDial(MENUWIDTH,"Max Characters In Function Defs:",this->prefsMaxFuncChars,5,MAXTEXTWIDTH,posy);
//max bookmark strings
	posy++;
	makePrefsDial(MAXBMWIDTH,"Max Characters In Bookmarks:",maxBMChars,5,MAXTEXTWIDTH,posy);

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

	makePrefsCheck(BEKIND,"I have donated",nagScreen,-1,-1);
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
	QObject::connect(((QPushButton*)button),&QPushButton::clicked,cancelPrefs);
	hbox->addWidget(button);
	hbox->addStretch(1);
	mainvbox->addLayout(hbox);

	this->prefsWindow->setLayout(mainvbox);

	this->prefsWindow->setWindowModality(Qt::WindowModal);
}

void KKEditClass::addIcon(const char* icon,const char* data,int toolnumber,const char* tooltip)
{
/*
	MenuItemClass	*menuitem=new MenuItemClass(name);
	QIcon			itemicon=QIcon::fromTheme(iconname,QIcon(iconname));

	menuitem->setMenuID(userdata);
	menuitem->setIcon(itemicon);
	menuitem->setShortcut(key);
	menuitem->setObjectName(objectname);

*/
	QIcon qicon;
	MenuItemClass* menuitem=new MenuItemClass(icon);
	qicon=QIcon::fromTheme(icon,QIcon(icon));
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

	icon=QIcon::fromTheme(name,QIcon(name));
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
	vlayout->addWidget(this->findDropBox);

//replace
	label=new QLabel("Replace With");
	label->setAlignment(Qt::AlignCenter);
	vlayout->addWidget(label);

	this->replaceDropBox=new QComboBox;
	reinterpret_cast<QComboBox*>(this->replaceDropBox)->setEditable(true);
	reinterpret_cast<QComboBox*>(this->replaceDropBox)->setCompleter(0);
	vlayout->addWidget(replaceDropBox);

//switches 1st row
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,0,0,0);
	hbox=new QWidget;
	hbox->setLayout(hlayout);

//case
	this->frSwitches[FRCASE]=new QCheckBox("Case insensitive");
	reinterpret_cast<QCheckBox*>(this->frSwitches[FRCASE])->setChecked(this->insensitiveSearch);
	QObject::connect((QCheckBox*)this->frSwitches[FRCASE],&QCheckBox::stateChanged,doSearchPrefs);
	hlayout->addWidget(this->frSwitches[FRCASE]);
//use regex
	this->frSwitches[FRUSEREGEX]=new QCheckBox("Use Regex");
	reinterpret_cast<QCheckBox*>(this->frSwitches[FRUSEREGEX])->setChecked(this->useRegex);
	QObject::connect((QCheckBox*)this->frSwitches[FRUSEREGEX],&QCheckBox::stateChanged,doSearchPrefs);
	hlayout->addWidget(this->frSwitches[FRUSEREGEX]);

	vlayout->addWidget(hbox);

//switches 2nd row
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,0,0,0);
	hbox=new QWidget;
	hbox->setLayout(hlayout);
//wrap
	this->frSwitches[FRWRAP]=new QCheckBox("Wrap");
	reinterpret_cast<QCheckBox*>(this->frSwitches[FRWRAP])->setChecked(this->wrapSearch);
	QObject::connect((QCheckBox*)this->frSwitches[FRWRAP],&QCheckBox::stateChanged,doSearchPrefs);
	hlayout->addWidget(this->frSwitches[FRWRAP]);

//all files
	this->frSwitches[FRALLFILES]=new QCheckBox("All Files");
	reinterpret_cast<QCheckBox*>(this->frSwitches[FRALLFILES])->setChecked(this->findInAllFiles);
	QObject::connect((QCheckBox*)this->frSwitches[FRALLFILES],&QCheckBox::stateChanged,doSearchPrefs);
	hlayout->addWidget(this->frSwitches[FRALLFILES]);
//hilite all
	this->frSwitches[FRHIGHLIGHTALL]=new QCheckBox("Highlight All");
	reinterpret_cast<QCheckBox*>(this->frSwitches[FRHIGHLIGHTALL])->setChecked(this->hightlightAll);
	QObject::connect((QCheckBox*)this->frSwitches[FRHIGHLIGHTALL],&QCheckBox::stateChanged,doSearchPrefs);
	hlayout->addWidget(this->frSwitches[FRHIGHLIGHTALL]);
//rep all
	this->frSwitches[FRREPLACEALL]=new QCheckBox("Replace All");
	reinterpret_cast<QCheckBox*>(this->frSwitches[FRREPLACEALL])->setChecked(this->replaceAll);
	QObject::connect((QCheckBox*)this->frSwitches[FRREPLACEALL],&QCheckBox::stateChanged,doSearchPrefs);
	hlayout->addWidget(this->frSwitches[FRREPLACEALL]);

	vlayout->addWidget(hbox);

//switches 3rd row
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,4,0,4);
	hbox=new QWidget;
	hbox->setLayout(hlayout);

	button=new QPushButton("Forward");
	QObject::connect((QPushButton*)button,&QPushButton::clicked,doFindForwardWrap);
	icon=QIcon::fromTheme("go-next",QIcon("go-next"));
	button->setIcon(icon);
	hlayout->addWidget(button);

	button=new QPushButton("Back");
	QObject::connect((QPushButton*)button,&QPushButton::clicked,doFindBackWrap);
	icon=QIcon::fromTheme("go-previous",QIcon("go-previous"));
	button->setIcon(icon);
	hlayout->addWidget(button);

	if(replaceAll==false)
		this->frReplace=new QPushButton("Replace");
	else
		this->frReplace=new QPushButton("Replace All");
	QObject::connect((QPushButton*)frReplace,&QPushButton::clicked,doReplaceWrap);
	icon=QIcon::fromTheme("edit-find-replace",QIcon("edit-find-replace"));
	reinterpret_cast<QPushButton*>(this->frReplace)->setIcon(icon);
	hlayout->addWidget(this->frReplace);

	vlayout->addWidget(hbox);

	this->findReplaceDialog->setLayout(vlayout);
}

void KKEditClass::buildMainGui(void)
{
	MenuItemClass	*menuItemSink;

	this->mainWindow=new QMainWindow;
//	this->mainWindowVBox=new QVBoxLayout();//TODO//HMMmmmmmm
//	this->mainWindowVBox->setContentsMargins(2,2,2,2);
//	this->mainWindow->setLayout(this->mainWindowVBox);

	this->mainNotebook=new QTabWidget;
	
	qobject_cast<QTabWidget*>(this->mainNotebook)->setDocumentMode(true);
	qobject_cast<QTabWidget*>(this->mainNotebook)->setTabsClosable(true);
	qobject_cast<QTabWidget*>(this->mainNotebook)->setMovable(true);
	
	QObject::connect(qobject_cast<QTabWidget*>(this->mainNotebook),&QTabWidget::currentChanged,switchPage);//TODO//
	//connect(this,SIGNAL(&QTabWidget::currentChanged(int)),this,SLOT(switchTab(int)));
	QObject::connect(qobject_cast<QTabWidget*>(this->mainNotebook),&QTabWidget::tabCloseRequested,closeTabQT);
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
	this->selectTabMenuItem=this->makeMenuItemClass(EDITMENU,"Select Tab",0,"accessories-text-editor",SELECTTABMENUNAME,SELECTTABMENUITEM);

	this->editMenu->addSeparator();

//prefs
	menuItemSink=this->makeMenuItemClass(EDITMENU,"Preferences",0,"preferences-desktop",PREFSMENUNAME,PREFSMENUITEM);
//plugs
	menuItemSink=this->makeMenuItemClass(EDITMENU,"Plugin Prefs",0,"preferences-system",PLUGPREFSMENUNAME,PLUGPREFSMENUITEM);

//////////////////////////////////////////
//view menu
	this->viewMenu=new QMenu("&View");
	this->menuBar->addMenu(this->viewMenu);

//show docs
	menuItemSink=this->makeMenuItemClass(VIEWMENU,"Show Documentaion",0,NULL,SHOWDOCSMENUNAME,DOCSMENUITEM);

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

//////////////////////////////////////////

//navigation menu
	this->navMenu=new QMenu("&Navigation");
	this->menuBar->addMenu(this->navMenu);

//goto define
	goToDefineMenuItem=this->makeMenuItemClass(NAVMENU,"Go To Definition",QKeySequence::fromString("Ctrl+D"),"dialog-question",GOTODEFMENUNAME,GOTODEFINEMENUITEM);
//open include
	menuItemSink=this->makeMenuItemClass(NAVMENU,"Open Include File",QKeySequence::fromString("Ctrl+I"),"document-open",GOTODEFMENUNAME,OPENINCLUDEMENUITEM);
//goto line number
	menuItemSink=this->makeMenuItemClass(NAVMENU,"Go To Line",0,"go-down",GOTOLINEMENUNAME,GOTOLINEMENUITEM);
	//makeMenuItem(this->navMenu,"Go To Line",0,"go-down",GOTOLINEMENUNAME,&jumpToLine,NULL,0);
//find define
	makeMenuItem(this->navMenu,"Search For Define",0,"edit-find",SEARCHFORDEFMENUNAME,&functionSearch,NULL,0);
//find gtkdoc
	makeMenuItem(this->navMenu,"Search In Gtk Docs",0,"edit-find",SEARCHGTKMENUNAME,&searchGtkDocs,NULL,0);
//find qt5
	makeMenuItem(this->navMenu,"Search In Qt5 Docs",0,"edit-find",SEARCHQT5MENUNAME,&searchQT5Docs,NULL,0);
//goto doxy docs
	if(gotDoxygen==0)
		makeMenuItem(this->navMenu,"Find In Documentation",0,"edit-find",SEARCHDOXYMENUNAME,&doxyDocs,NULL,0);
////go back
	this->goBackMenu=this->makeMenuItemClass(NAVMENU,"Back",0,"go-previous","NOTNEEDED",GOBACKMENUITEM);
////go forward
	this->goFowardMenu=this->makeMenuItemClass(NAVMENU,"Forward",0,"go-next","NOTNEEDED",GOFORWARDMENUITEM);

	qobject_cast<QMenu*>(this->navMenu)->addSeparator();
//////////////////////////////////////////

//function menu
	this->funcMenu=new QMenu("Fun&ctions");
	this->menuBar->addMenu(this->funcMenu);

///////////////////////////////////////////

//newbookmarks
	this->bookMarkMenu=new QMenu("&Bookmarks");
	this->menuBar->addMenu(this->bookMarkMenu);
	this->rebuildBookMarkMenu();

///////////////////////////////////////////

//external tools	
	this->toolsMenu=new QMenu("&Tools");
	this->menuBar->addMenu(this->toolsMenu);
	buildTools();

///////////////////////////////////////////

//help
	this->helpMenu=new QMenu("&Help");
	this->menuBar->addMenu(this->helpMenu);

//about
	makeMenuItem(this->helpMenu,"&About",0,"help-about",ABOUTMENUNAME,&doAbout,NULL,0);
//help
	makeMenuItem(this->helpMenu,"&Help",0,"help-contents",HELPMENUNAME,&openHelp,NULL,0);

//	menuitem=makeMenuItem(GTK_STOCK_HELP,menu,(void*)openHelp,0,HELPMENUNAME,STOCKMENU,NULL,NULL);
//get plugins
	makeMenuItem(this->helpMenu,"&Get Plugins",0,DATADIR"/pixmaps/KKEditQTPlugMenu.png",GETPLUGSMENUNAME,&getPlugins,NULL,0);
//	menuitem=makeMenuItem(DATADIR"/pixmaps/KKEditPlugMenu.png",menu,(void*)getPlugins,0,GETPLUGSMENUNAME,PIXMAPMENU,"Get Plugins",NULL);


	//this->mainWindow
	//this->mainWindowVBox->addWidget(menuBar);
	//this->mainWindowVBox->addWidget(toolBar);
	//this->mainWindowVBox->addWidget(mainNotebook);
	this->setUpToolBar();
	//qobject_cast<QMainWindow*>(this->mainWindow)->setStatusBar(statusbar);


	this->mainWindow->setMenuBar(qobject_cast<QMenuBar*>(this->menuBar));
	this->mainWindow->addToolBar(this->toolBar);
	this->mainWindow->setCentralWidget(this->mainNotebook);

	this->statusText=new QLabel;	
	this->statusText->setText("Line 0\tCol 0");
 	this->statusBar=this->mainWindow->statusBar();
 	this->mainWindow->show();
 	//fprintf(stderr,">>%s<<\n",this->statusText->text().toStdString().c_str());
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

