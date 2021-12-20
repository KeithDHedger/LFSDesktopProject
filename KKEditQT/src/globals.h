/*
 *
 * ©K. D. Hedger. Tue  2 Nov 16:02:39 GMT 2021 keithdhedger@gmail.com

 * This file (globals.h) is part of KKEdit.

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

#ifndef _GLOBALS_
#define _GLOBALS_

#include "kkedit-includes.h"
#include "config.h"
#include "internet.h"

struct toolStruct
{
	char*				menuName;
	char*				filePath;
	char*				command;
	int					flags;
	bool				inTerminal;					
	bool				inPopUp;
	bool				alwaysPopup;
	bool				clearView;
	char*				comment;
	bool				global;
	bool				runAsRoot;
	int					keyCode;
	bool				useBar;
};

extern HistoryClass*	history;

extern GList*			newBookMarksList;

extern bool				showBMBar;
#ifndef _USEQT5_
extern GtkWidget*		bmHighlightBox;
#else
//TODO//
#endif
extern int				bmMarkNumber;

extern char*			prefsToolBarLayout;


//find replaceAll
extern GSList*			findList;
extern GSList*			replaceList;
extern unsigned int		maxFRHistory;

//app stuff
extern bool				busyFlag;

extern bool				autoSelected;



extern bool				noSyntax;
//extern bool				singleUse;





extern bool				onExitSaveSession;
extern bool				restoreBookmarks;
extern char*			styleName;
extern bool				noDuplicates;
extern bool				noWarnings;


extern bool				autoCheck;
extern unsigned int		autoShowMinChars;


extern unsigned int		maxBMChars;


extern char*			browserCommand;

//key short cuts
//TODO//


extern toolStruct*		selectedToolFromDrop;


extern GList*			toolsList;

extern int				currentTabNumber;
extern int 				untitledNumber;
extern QAction			*gotoLineButton;
extern QAction			*findApiButton;
extern QAction			*findQtApiButton;
extern QAction			*findFuncDefButton;
extern QAction			*liveSearchButton;

extern char*			functionSearchText;
extern char*			thePage;

#ifdef _BUILDDOCVIEWER_
#endif

#include "pluginclass.h"

extern PluginClass*		globalPlugins;

//save and load var lists
extern int				intermarg;
extern int				flagsarg;
extern int				inpopup;
extern int				alwayspopup;
extern int				clearview;
extern char*			commandarg;
extern char*			commentarg;
extern char*			menuname;
extern int				rootarg;
extern int				keycode;
extern int				usebar;



//extern args				keybindings_rc[];


void plugRunFunction(gpointer data,gpointer funcname);


void getMimeType(char* filepath,void* ptr);
void setLanguage(pageStruct* page);




void buildToolsList(void);






void debugFree(char** ptr,const char* message);






extern KKEditClass *kkedit;
#endif




