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
//extern GSList*			findList;
//extern GSList*			replaceList;
extern unsigned int		maxFRHistory;


extern char*			browserCommand;


extern toolStruct*		selectedToolFromDrop;


extern GList*			toolsList;

#include "pluginclass.h"

extern PluginClass*		globalPlugins;

void plugRunFunction(gpointer data,gpointer funcname);


void getMimeType(char* filepath,void* ptr);
void setLanguage(pageStruct* page);




void buildToolsList(void);



void debugFree(char** ptr,const char* message);






extern KKEditClass *kkedit;
#endif




