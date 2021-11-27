/*
 *
 * ©K. D. Hedger. Sun 31 Oct 12:18:55 GMT 2021 keithdhedger@gmail.com

 * This file (KKEditClass.h) is part of KKEdit.

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

#ifndef _KKEDITCLASS_
#define _KKEDITCLASS_

enum {FILEMENU=0x4000,EDITMENU,VIEWMENU,NAVMENU,FUNCTIONSMENU,BOOKNARKSMENU,TOOLSMENU,PLUGINSMENU,HELPMENU};

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
enum {REMOVEALLBOOKMARKSMENUITEM=0xc000,TOGGLEBOOKMARKMENUITE};

//tools
enum {MANAGETOOLSMENUITEM=0xd000};
//plugins
//help
enum {ABOUTMENUITEM=0xe000,HELPMENUITEM,ONLINEHELPMENUITEM,GETPLUGSMENUITEM};

enum	{AUTOINDENT=0,SHOWNUMS,WRAP,HIGHLIGHT,SYNTAXHILITE,USESINGLE,AUTOSAVE,NODUPLICATE,NOWARN,AUTOSHOW,BEKIND,GLOBALPLUGMENU,SETFONT,MAXPREFSWIDGETS};
enum {MAXTABCHARS=0,MAXHISTORY,MAXFUNCDEPTH,COMPLETIONSIZE,TABWIDTH,MENUWIDTH,MAXBMWIDTH,MAXPREFSINTWIDGETS};
enum {FUNCTIONCOMBO=0,THEMECOMBO,FONTNAMECOMBO,FONTSIZECOMBO,PREFSTERMCOMMAND,PREFSROOTCOMMAND,PREFSBROWSERCOMMAND,PREFSCURRENTFONT,BMHIGHLIGHTCOLOUR,CURRENTLINECOLOUR,MAXPREFSOTHERWIDGETS};


#include "kkedit-includes.h"
class MenuItemClass;

class KKEditClass : public QObject
{
	Q_OBJECT

	public:
		KKEditClass(QApplication *app);
		~KKEditClass();

//app vars
		QMainWindow					*mainWindow;
		QTabWidget						*mainNotebook=NULL;
		QApplication				*application;
		QTimer 						*checkMessages;
		std::string					tmpFolderName;
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

//app functions
		void						initApp(int argc,char** argv);
		void						buildMainGui(void);
		void						readConfigs(void);
		void						findFile(void);
		void						buildTools(void);

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
//app
		bool						prefsUseGlobalPlugMenu=true;
		int							prefsMsgTimer;
		bool						prefsUseSingle=true;

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
//editor functions

//menubar
		QMenuBar					*menuBar;

//menu vars
//file menu
		QMenu						*fileMenu;
		MenuItemClass				*newMenuItem;
		MenuItemClass				*openMenuItem;
		MenuItemClass				*saveMenuItem;
		MenuItemClass				*saveAsMenuItem;
		MenuItemClass				*saveAllMenuItem;
		MenuItemClass				*saveSessionMenuItem;
		MenuItemClass				*restoreSessionMenuItem;
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
		MenuItemClass				*selectTabMenuItem;

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

//function menu
		QMenu						*funcMenu;

//bookmarks menu
		QMenu						*bookMarkMenu;

//tools menu
		QMenu						*toolsMenu;

//help menu
		QMenu						*helpMenu;

//menu functions
		MenuItemClass				*makeMenuItemClass(int mainmenu,const QString name,const QKeySequence key,const QString iconname,const QString objectname,int userdata);

//toolbar vars
		QToolBar					*toolBar=NULL;
		QWidget						*lineNumberWidget;
		QWidget						*findApiWidget;
		QWidget						*findQtApiWidget;
		QWidget						*findDefWidget;
		QWidget						*liveSearchWidget;
//toolbar functions
		void						setUpToolBar(void);

//file vars
//file functions
		void						newFile(const QString data="",const QString filename="");
		bool						openFile(std::string filepath,int linenumber=1,bool warn=false);
		bool						openFileDialog(void);
		void						openAsHexDump(void);
		QString						runPipeAndCapture(QString command);
		bool						saveFile(void);

//document vars
//document functions
		DocumentClass				*getDocumentForTab(int tabnum);
		void						resetAllFilePrefs(void);
		void						goToDefinition(void);
		void						showLineEntry(void);
		void						gotoLine(int linenumber);

//bookmark vars
		QHash<int,bookMarkStruct>	bookMarks;
		int							bookMarksIndex=100;
//bookmark functions
		void						rebuildBookMarkMenu(void);
		void						handleBMMenu(QWidget *widget,int what);

//docviewer vars
		QWidget						*docView=NULL;
		QWidget						*webView=NULL;
		bool						docviewerVisible=false;
		MenuItemClass				*toggleDocViewMenuItem;
		int							docWindowWidth=800;//TODO//
		int							docWindowHeight=600;
		int							docWindowX=100;
		int							docWindowY=100;
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
//prefswindow functions
		void						makePrefsCheck(int widgnum,const QString label,bool onoff,int posx,int posy);
		void						makePrefsDial(int widgnum,const QString label,int value,int minvalue,int maxvalue,int posy);
		void						addIcon(const char* icon,const char* data,int toolnumber,const char* tooltip);
		void						populateDnD(void);
		void						buildPrefsWindow(void);
		void						doPrefs(Widget* widget,uPtr data);
		void						addIconToList(const char* name,int type);
		void						populateStore(void);

//find replace vars
		QWidget						*findReplaceDialog;
		QWidget						*findDropBox;
		QWidget						*replaceDropBox;
		QWidget						*frSwitches[FRMAXSWITCHES];
		QWidget						*frReplace;
//find replace functions
		void						buildFindReplace(void);

//plugin vars
		QWidget						*globalPlugMenu=NULL;

///////////////////////////////////////////TODO/////////////////////////////////////////////////////////////////////////

//		bool					onExitSaveSession=false;
//		bool					noDuplicates=false;
//		bool					noWarnings=false;
//		bool					hightlightAll=true;
//		bool					nagScreen=false;
//		bool					showBMBar=false;

//		unsigned int				autoShowMinChars=4;



//		std::string				styleName;


//theme

//	private slots:
//		void					switchTab(int thispage);




//temp prefs

//app
		void						writeExitData(void);

	public slots:
		void						doTimer(void);
		void						doFileMenuItems();
		void						doEditMenuItems();
		void						doViewMenuItems();
		void						doNavMenuItems();
		void						doBookmarkMenuItems();
		void						doHelpMenuItems();
		void						doToolsMenuItems();

		void						doDoubleClickPrefs(QListWidgetItem *item);
		void						setPreferences(void);
		void						setBMColour(void);
		void						setLineColour(void);
		void						setFont(void);
		void						addToToolBar(void);

	protected:
	private:
//app


	


///////////////////////////////////////////////////////////////////////////////////////////////	




};

//
//#ifdef _ASPELL_
//	AspellCanHaveError*	possible_err;
//#endif
	//globalSlice=new StringSlice;
//
//
//	readConfig();
//	loadKeybindings();
//
//	tmpStyleName=strdup(styleName);
//	tmpHighlightColour=highlightColour;
//
//	asprintf(&htmlFile,"%s/Docview-%s.html",tmpFolderName,globalSlice->randomName(6));
//	asprintf(&htmlURI,"file://%s/Docview-%s.html",tmpFolderName,globalSlice->randomName(6));
//
//#ifdef _ASPELL_
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
//
////do plugins
//	globalPlugins=new PluginClass(loadPluginsFlag);
////set up plugin data
//	globalPlugins->globalPlugData=(plugData*)malloc(sizeof(plugData));
//	globalPlugins->globalPlugData->dataDir=DATADIR;
//	globalPlugins->globalPlugData->gPlugFolder=globalPlugins->plugFolderPaths[GLOBALPLUGS];
//	globalPlugins->globalPlugData->lPlugFolder=globalPlugins->plugFolderPaths[LOCALPLUGS];
//	globalPlugins->globalPlugData->htmlFile=htmlFile;
//	globalPlugins->globalPlugData->thePage=&thePage;
//	globalPlugins->globalPlugData->currentTab=-1;
//	globalPlugins->globalPlugData->tmpFolder=tmpFolderName;
//	globalPlugins->globalPlugData->kkeditVersion=VERSION;
//
//	/*
//		for(int j=0;j<globalPlugins->plugCount;j++)
//			{
//				struct pluginData
//	{
//		char*		name;
//		bool		enabled;
//		GModule*	module;
//		bool		loaded;
//		char*       path;
//	};
//				pluginData* pd=(pluginData*)g_list_nth_data(globalPlugins->plugins,j);
//				printf("num %i name=%s\n",j,pd->name);
//				printf("num %i enabled=%i\n",j,(int)pd->enabled);
//				printf("num %i path=%s\n",j,pd->path);
//			}
//	*/
//
//	localeLang=getenv("LANG");
//
//	history=new HistoryClass;
//	globalSlice->setReturnDupString(true);

#endif
