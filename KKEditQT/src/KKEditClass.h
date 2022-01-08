/*
 *
 * ©K. D. Hedger. Thu 23 Dec 20:40:06 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClass.h) is part of KKEditQT.

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

#ifndef _KKEDITCLASS_
#define _KKEDITCLASS_

#define STATUSBARTIMEOUT 5000

enum {FILEMENU=0x4000,EDITMENU,VIEWMENU,NAVMENU,FUNCTIONSMENU,BOOKMARKSMENU,TOOLSMENU,PLUGINSMENU,HELPMENU,SAVESESSIONSMENU,RESTORESESSIONSMENU,CURRENTSESSION,NOMENU};

//file
enum {NEWMENUITEM=0x8000,OPENMENUITEM,HEXDUMPMENUITEM,NEWADMINEDMENUITEM,NEWEDMENUITEM,MANPAGEEDMENUITEM,BUILDDOCSMENUITEM,SAVEMENUITEM,SAVEASMENUITEM,SAVEALLMENUITEM,SAVESESSIONMENUITEM,RESTORESESSIONMENUITEM,PRINTMENUITEM,CLOSEMENUITEM,CLOSEALLMENUITEM,REVERTMENUITEM,QUITMENUITEM};
//edit
enum {UNDOMENUITEM=0x9000,REDOMENUITEM,UNDOALLMENUITEM,REDOALLMENUITEM,EDSEP1,CUTMENUITEM,COPYMENUITEM,PASTEMENUITEM,EDSEP2,FINDMENUITEM,FINDNEXTMENUITEM,EDSEP3,SORTTABSMENUITEM,SHOWALLTABSMENUITEM,SELECTTABMENUITEM,EDSEP4,PREFSMENUITEM,PLUGPREFSMENUITEM};
//view
enum {DOCSMENUITEM=0xa000,TOGGLETOOLBARMENUITEM,TOGGLETOOLWINDOWMENUITEM,TOGGLESTATUSBARMENUITEM,TOGGLEDOCVIEWMENUITEM,TOGGLELINENUMBERSMENUITEM,TOGGLELINEWRAPMENUITEM,TOGGLEHILITELINEMENUITEM,TOGGLEWHITESPACEMENUITEM,TOGGLESYNTAXHILITINGMENUITEM,TOGGLECOMPLETIONSMENUITEM};
//nav
enum {GOTODEFINEMENUITEM=0xb000,OPENINCLUDEMENUITEM,GOTOLINEMENUITEM,SEARCHFORDEFINEMENUITEM,SEARCHGTKDOCS,SEARCHQT5DOCS,SEARCHDOXYDOCS,GOBACKMENUITEM,GOFORWARDMENUITEM};
//func
//bms
enum {REMOVEALLBOOKMARKSMENUITEM=0xc000,TOGGLEBOOKMARKMENUITEM};

//tools
enum {MANAGETOOLSMENUITEM=0xd000,TOOLNUMBER};
//plugins
//help
enum {ABOUTMENUITEM=0xe000,HELPMENUITEM,ONLINEHELPMENUITEM,GETPLUGSMENUITEM};

enum	{AUTOINDENT=0,SHOWNUMS,WRAP,HIGHLIGHT,SYNTAXHILITE,USESINGLE,AUTOSAVE,NODUPLICATE,NOWARN,AUTOSHOW,BEKIND,GLOBALPLUGMENU,SETFONT,MAXPREFSWIDGETS};
enum {MAXTABCHARS=0,MAXHISTORY,MAXFUNCDEPTH,COMPLETIONSIZE,TABWIDTH,MENUWIDTH,MAXBMWIDTH,MAXPREFSINTWIDGETS};
enum {FUNCTIONCOMBO=0,THEMECOMBO,FONTNAMECOMBO,FONTSIZECOMBO,PREFSTERMCOMMAND,PREFSROOTCOMMAND,PREFSQTDOCDIR,PREFSBROWSERCOMMAND,PREFSCURRENTFONT,BMHIGHLIGHTCOLOUR,CURRENTLINECOLOUR,SHORTCUTSCOMBO,MAXPREFSOTHERWIDGETS};

enum {FINDNEXT=1,FINDPREV,FINDREPLACE};

//od enums
enum {SPELLCHECKMENUITEM=0x2000,APPLYWORDBUTTON,IGNOREWORDBUTTON,ADDWORDBUTTON,CANCELSPELLCHECK,CANCELPREFS,DOLINEBOX,DOLIVESEARCH,DOAPISEARCH,DOQT5SEARCH,DOGTKSEARCH,DOCVIEWERGOHOME,TOOLSSAVE,TOOLSCANCEL,TOOLSDELETE,TOOLSSAVEAS};

struct tabMenuStruct
{
	unsigned int	what;
	const char		*label;
	const char		*icon;
};

enum {COPYFOLDERPATH=0xf000,COPYFILEPATH=0xf100,COPYFILENAME=0xf200,SPELLCHECKDOC=0xf300,SRCHILTIE=0xf400,HIDETAB=0xf500,LOCKCONTENTS=0xf600,OPENFROMHERE=0xf700};

enum {TABCONTEXTMENUCNT=(OPENFROMHERE-COPYFOLDERPATH) / 0x100 +1};

enum {NONESRCCODE=0,CPPSRCCODE=0x100,CSRCCODE=0x200,BASHSRCCODE=0x300,PYTHONSRCCODE=0x400,GOSRCCODE=0x500,LUASRCCODE=0x600,YAMLSRCCODE=0x700,PHPSRCCODE=0x800,XMLSRCCODE=0x900,CSSSRCCODE=0xa00,JSSRCCODE=0xb00,MAKESRCCODE=0xc00};

enum {HIDETABSHORTCUT=0,DELETELINESHORTCUT,DELETETOEOLSHORTCUT,DELETETOSOLSHORTCUT,SELECTWORDSHORTCUT,DELETEWORDSHORTCUT,DUPLICATELINESHORTCUT,SELECTLINESHORTCUT,MOVELINEUPSHORTCUT,MOVELINEDOWNSHORTCUT,MOVESELECTIONUPSHORTCUT,MOVESELECTIONDOWNSHORTCUT,SHORTCUTSCOUNT};

enum {FRCASE=0,FRUSEREGEX,FRWRAP,FRALLFILES,FRHIGHLIGHTALL,FRREPLACEALL,FRMAXSWITCHES};

#include "qsourcehighliter.h"
#include "qsourcehighliterthemes.h"
#include "languagedata.h"

#include "QT_menuitem.h"
#include "QT_highlighter.h"
#include "QT_document.h"

class MenuItemClass;

class KKEditClass : public QObject
{
	Q_OBJECT

	public:
		KKEditClass(QApplication *app);
		~KKEditClass();

//app vars
		QMainWindow					*mainWindow;
		QTabWidget					*mainNotebook=NULL;
		QTabBar						*tabBar=NULL;
		QApplication				*application;
		QTimer 						*checkMessages;
		QString						tmpFolderName;
		QString						htmlFile;
		QString						htmlURI;
		QSettings					prefs;
		int							gotManEditor;
		int							gotDoxygen;
		QHash<int,DocumentClass*>	pages;
		int							newPageIndex=1;
		int							sessionID=-1;
		int							queueID=-1;
		int							currentWorkSpace=-1;
		bool						forcedMultInst=false;
		bool						forceDefaultGeom=false;
		bool						sessionBusy=false;
		tabMenuStruct				tabContextMenuItems[TABCONTEXTMENUCNT]={{COPYFOLDERPATH,"Copy Folder Path","edit-copy"},{COPYFILEPATH,"Copy File Path","edit-copy"},{COPYFILENAME,"Copy File Name","edit-copy"},{SPELLCHECKDOC,"Spellcheck Document","tools-check-spelling"},{SRCHILTIE,"Source Hilighting","preferences-system"},{HIDETAB,"Hide Tab","list-remove"},{LOCKCONTENTS,"Toggle Lock Contents","list-remove"},{OPENFROMHERE,"Open From Here","document-open"}};
		const char					*srcMenuNames[20]={"None","C++","C","SH","Python","Go","Lua","YAML","PHP","XML","CSS","Javascript","Make",NULL};
#ifdef _ASPELL_
		AspellConfig				*aspellConfig=NULL;
		AspellSpeller				*spellChecker=0;
		QDialog						*spellCheckGUI=NULL;
		QComboBox					*wordDropBox;
		QLabel						*infoLabel;
		MenuItemClass				*spellCheckMenuItem;
		bool						returnWord=false;
		QString						goodWord;
		QString						badWord;
		bool						cancelCheck=false;
#endif
		QShortcut					*appShortcuts[SHORTCUTSCOUNT];
		QStringList					defaultShortCutsList={"Ctrl+H","Ctrl+Y","Ctrl+?","Ctrl+K","Ctrl+Shift+H","Ctrl+D","Ctrl+Shift+D","Ctrl+L","Ctrl+M","Ctrl+Shift+M","Ctrl+@","Ctrl+'"};

		//sesions
		QHash<int,QString>			sessionNames;
		QString						homeFolder;
		QString						homeDataFolder;
		QString						sessionFolder;
		unsigned int				currentSessionNumber=0xdeadbeef;
		QString						toolsFolder;

		//HistoryClass				*history;

//app functions
		void						initApp(int argc,char** argv);
		void						buildMainGui(void);
		void						readConfigs(void);
		void						findFile(void);
		void						buildTools(void);
		void						showBarberPole(QString windowtitle,QString bodylabel,QString cancellabel,QString controlfile);
		void						buildDocs(void);
		void						showDocs(void);
		void						shutDownApp(void);

		void						buildSpellCheckerGUI(void);
		void						setUpSpellGUI(QString word,DocumentClass *doc);
		bool						checkSelection(QString selection);
		void						checkDoc(DocumentClass *doc);
		void						setAppShortcuts(void);

		void						setToolbarSensitive(void);
		QString						randomName(int len);

		void						writeExitData(void);
		int							yesNoDialog(QString txt,QString info);

//app prefs
//document
		bool						prefsIndent=true;
		bool						prefsShowLineNumbers=true;
		bool						prefsLineWrap=true;
		bool						prefsHighLightline=true;
		bool						prefsSyntaxHilighting=true;
		QFont						prefsDocumentFont;
		unsigned int				prefsTabWidth=4;
		bool						prefsAutoShowCompletions;
//theme
		QColor						prefsHiLiteLineColor;
		QColor						prefsBookmarkHiLiteColor;
//editor
		int							prefsFunctionMenuLayout=0;
		unsigned int				prefsDepth=1;
		QString						prefsToolBarLayout;
		unsigned int				prefsMaxTabChars=20;
		unsigned int				prefsMaxFuncChars=MAXTEXTWIDTH;
		QString						prefsTerminalCommand;
		QString						prefsRootCommand;
		QString						prefsQtDocDir;
//app
		bool						prefsUseGlobalPlugMenu=true;
		int							prefsMsgTimer;
		bool						prefsUseSingle=true;
		bool						prefsNagScreen=false;
		bool						onExitSaveSession=false;
		QString						browserCommand;
//editor vars
		QStatusBar					*statusBar;
		QLabel						*statusText;
		bool						wrapSearch=true;
		bool						insensitiveSearch=true;
		bool						useRegex=false;
		bool						replaceAll=false;
		bool						findInAllFiles=false;
		bool						hightlightAll=true;
		int							currentPage=0;
		bool						closingAllTabs=false;
		bool						noDuplicates=false;
		bool						noWarnings=false;
		unsigned int				autoShowMinChars=4;
		unsigned int				maxBMChars;
		unsigned int 				untitledNumber=1;
//editor functions
		QString						truncateWithElipses(const QString str,unsigned int maxlen);
		void						sortTabs(void);
		void						rebuildTabsMenu(void);
		void						functionSearchDialog(void);
//menubar
		QMenuBar					*menuBar;

//menu vars
//file menu
		QMenu						*fileMenu;
		QMenu						*saveSessionsMenu;
		QMenu						*restoreSessionsMenu;
		MenuItemClass				*saveCurrentSessionMenuItem;
		MenuItemClass				*restoreDefaultSessionMenuItem;


		MenuItemClass				*newMenuItem;
		MenuItemClass				*openMenuItem;
		MenuItemClass				*saveMenuItem;
		MenuItemClass				*saveAsMenuItem;
		MenuItemClass				*saveAllMenuItem;
		MenuItemClass				*printMenuItem;
		MenuItemClass				*closeMenuItem;
		MenuItemClass				*closeAllMenuItem;
		MenuItemClass				*revertMenuItem;

//edit menu
		QMenu						*editMenu;
		MenuItemClass				*undoMenuItem;
		MenuItemClass				*redoMenuItem;
		MenuItemClass				*undoAllMenuItem;
		MenuItemClass				*redoAllMenuItem;
		MenuItemClass				*cutMenuItem;
		MenuItemClass				*copyMenuItem;
		MenuItemClass				*pasteMenuItem;
		MenuItemClass				*findMenuItem;
		MenuItemClass				*findNextMenuItem;
		MenuItemClass				*sortTabsMenuItem;
		MenuItemClass				*showAllTabsMenuItem;
		QMenu						*selectTabMenu;

//view menu
		QMenu						*viewMenu;
		bool						toolbarVisible=true;
		bool						toolWindowVisible=false;
		bool						statusBarVisible=true;
		bool						lineNumbersVisible;
		bool						wrapLine;
		bool						hiliteCurrentLine;
		bool						showWhiteSpace=false;
		bool						showHighLighting;
		bool						showCompletions;
		MenuItemClass				*toggleLineNumbersMenuItem;
		MenuItemClass				*toggleWrapLineMenuItem;
		MenuItemClass				*toggleHiliteCurrentLineMenuItem;
		MenuItemClass				*toggleWhiteSpaceMenuItem;
		MenuItemClass				*toggleSyntaxHilightingMenuItem;
		MenuItemClass				*toggleCompletionsMenuItem;
		MenuItemClass				*toggleToolBarMenuItem;
		MenuItemClass				*toggleToolWindowMenuItem;
		MenuItemClass				*toggleStatusBarMenuItem;

//nav menu
		QMenu						*navMenu;
		MenuItemClass				*goToDefineMenuItem;
		MenuItemClass				*goBackMenu;
		MenuItemClass				*goFowardMenu;
		MenuItemClass				*goToLineDialogMenuItem;

//function menu
		QMenu						*funcMenu;

//bookmarks menu
		QMenu						*bookMarkMenu;


//help menu
		QMenu						*helpMenu;

//menu functions
		MenuItemClass				*makeMenuItemClass(int mainmenu,const QString name,const QKeySequence key,const QString iconname,const QString objectname,int userdata);

//toolbar vars
		QToolBar					*toolBar=NULL;
		QWidget						*lineNumberWidget=NULL;
		QLineEdit					*findGtkApiWidget=NULL;
		QLineEdit					*findQtApiWidget=NULL;
		QLineEdit					*findDefWidget=NULL;
		QWidget						*liveSearchWidget=NULL;
//toolbar functions
		void						setUpToolBar(void);

//file vars
		bool						openFromDialog=false;
//file functions
		void						newFile(const QString data="",const QString filename="");
		bool						openFile(QString filepath,int linenumber=1,bool warn=false);
		bool						openFileDialog(void);
		void						openAsHexDump(void);
		QString						runPipeAndCapture(QString command);
		bool						saveFile(int tabnum,bool ask=true);
		void						newEditor(int what);
		void						runPipe(QString command);
		bool						saveAllFiles(void);
		bool						saveFileAs(int tabnum);
		int							askSaveDialog(const QString filename);
		QStringList					getNewRecursiveTagList(QString filepath);

//document vars
//document functions
		DocumentClass				*getDocumentForTab(int tabnum);
		void						resetAllFilePrefs(void);
		void						goToDefinition(const QString txt);
		void						showLineEntry(void);
		void						gotoLine(int linenumber);
		void						reloadDocument(void);

//bookmark vars
		QHash<int,bookMarkStruct>	bookMarks;
		int							bookMarksIndex=100;
//bookmark functions
		void						rebuildBookMarkMenu(void);
		void						handleBMMenu(QWidget *widget,int what);

//tools menu
		QMenu						*toolsMenu;
//tools vars
		QDialog						*toolsWindow;
		QComboBox					*toolSelect;

//tools functions
		void						rebuildToolsMenu(void);
		QStringList					verifyTool(QString filepath);

//docviewer vars
		QMainWindow					*docView=NULL;
		QWebView					*webView=NULL;
		bool						docviewerVisible=false;
		MenuItemClass				*toggleDocViewMenuItem;
//docviewer functions
		void						buildDocViewer(void);

//prefswindow
//prefswindow vars
		QWidget						*prefsWidgets[MAXPREFSWIDGETS];
		QWidget						*prefsIntWidgets[MAXPREFSINTWIDGETS];
		QWidget						*prefsOtherWidgets[MAXPREFSOTHERWIDGETS];
		QGridLayout					*table;
		QDialog						*prefsWindow;
		QListWidget					*listWidget;
		QToolBar					*fromHBox;
		QAction						*tool[19];
		QDialog						*keyShortcutsWindow;

//prefswindow functions
		void						makePrefsCheck(int widgnum,const QString label,bool onoff,int posx,int posy);
		void						makePrefsDial(int widgnum,const QString label,int value,int minvalue,int maxvalue,int posy);
		void						addIcon(const char* icon,const char* data,int toolnumber,const char* tooltip);
		void						populateDnD(void);
		void						buildPrefsWindow(void);
		void						doPrefs(void);
		void						addIconToList(const char* name,int type);
		void						populateStore(void);
		void						resetKeyCombo(void);

//find replace vars
		QWidget						*findReplaceDialog;
		QComboBox					*findDropBox;
		QComboBox					*replaceDropBox;
		QCheckBox					*frSwitches[FRMAXSWITCHES];
		QPushButton					*frReplace;
		QStringList					findList;
		QStringList					replaceList;
		unsigned int				maxFRHistory=5;
//find replace functions
		void						buildFindReplace(void);
		void						doFindReplace(int response_id);

//search vars
//search functions
		void						searchAPIDocs(const QString txt,int what);

//plugin vars
		QWidget						*globalPlugMenu=NULL;

	public slots:
		void						debugSignalSlot(int);
		void						doTimer(void);
		void						doFileMenuItems();
		void						doEditMenuItems();
		void						doViewMenuItems();
		void						doNavMenuItems();
		void						doBookmarkMenuItems();
		void						doHelpMenuItems();
		void						doToolsMenuItems();
		void						doSelectTab(void);
		void						doTabBarContextMenu(void);
		void						doOddMenuItems(void);
		void						doOddButtons(void);

		void						doDoubleClickPrefs(QListWidgetItem *item);
		void						setPreferences(void);
		void						setBMColour(void);
		void						setLineColour(void);
		void						setFont(void);
		void						addToToolBar(void);
		void						buildGetKeyShortCut(int index);

		void						switchPage(int index);
		bool						closeTab(int index);
		void						closeAllTabs(void);

		void						doFindButton(void);
		void						tabContextMenu(const QPoint &pt);
		void						doTabBarContextMenuSetHilite(void);

		void						doAppShortCuts(void);

		void						doSearchFromBar(const QString txt);
		void						setSearchPrefs(int state);

		void						doSessionsMenuItems(void);

		void						setToolsData(int);

	protected:
	private:
};

#endif
