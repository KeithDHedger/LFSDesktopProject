/*
 *
 * ©K. D. Hedger. Thu 23 Dec 13:55:30 GMT 2021 keithdhedger@gmail.com

 * This file (kkedit-includes.h) is part of KKEditQT.

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

#ifndef _KKEDIT_INCLUDES_
#define _KKEDIT_INCLUDES_

//#define _USEMINE_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <libgen.h>
#include <time.h>

#include <string>
#include <vector>
#include <map>

#include <QtWidgets>
#include <QSettings>

#include "config.h"

#ifdef _ASPELL_
#include <aspell.h>
#endif

#ifdef _BUILDDOCVIEWER_
#include <QtWebKitWidgets>
#endif

#define MAXMSGSIZE 1024
#define MSGKEY 0x8000
#define ALLOK 0
#define UNKNOWNARG 1
#define NOMAKEQUEUE 2
#define NOSENDMSG 3
#define WAITMSG 0

#define MSGANY 0
#define MSGSEND 2
#define MSGRECEIVE 1

struct msgStruct
{
	long mType;
	char mText[MAXMSGSIZE];
};

enum msgActions {OPENFILE=100,SAVEFILE,QUITAPP,ACTIVATEAPP};

#define MAXTEXTWIDTH 500

#define KKEDITFOLDER	".KKEditQT"

#define	POLEPATH		PREFIX "/bin/KKEditProgressBarQT"
#define CUSTOMWORDFILE ".KKEditQT/customcompletions"

#define NUMSHORTCUTS	14

enum {PIXBUF_COLUMN,TEXT_COLUMN,BUTTON_NUM};

enum  utilVarType {BOOLVAR=0,INTVAR,CHARVAR,MULTVAR,BADTYPE,DECIMALOUT,HEXOUT,OCTALOUT};

#define GLOBALPLUGS			1
#define LOCALPLUGS			0

#define NEWMENUNAME "newmenu"
#define OPENMENUNAME "openmenu"
#define HEXDUMPMENUNAME "hexdumpmenu"
#define NEWADMINMENUNAME "newadminmenu"

#define NEWEDITORMENUNAME "neweditormenu"
#define MANEDITORMENUNAME "maneditormenu"
#define DOXYBUILDMENUNAME "doxybuildmenu"
#define RECENTMENUNAME "recentmenu"
#define SAVEMENUNAME "savemenu"
#define SAVEASMENUNAME "saveasmenu"
#define SAVEALLMENUNAME "saveallmenu"
#define SAVESESSIONMENUNAME "savesessionmenu"
#define RESTORESESSIONMENUNAME "restoresessionmenu"
#define SAVECURRENTSESSIONMENUNAME "savecurrentsessionmenu"
#define RESTOREDEFAULTSESSIONMENUNAME "restoredefaultsessionmenu"

#define PRINTMENUNAME "printmenu"
#define CLOSEMENUNAME "closemenu"
#define CLOSEALLMENUNAME "closeallmenu"
#define SORTTABSMENUNAME "sorttabsmenu"
#define SHOWALLTABSMENUNAME "showalltabsmenu"
#define SELECTTABMENUNAME "selecttabmenu"
#define REVERTMENUNAME "revertmenu"
#define QUITMENUNAME "quitmenu"
#define UNDOMENUNAME "undomenu"
#define REDOMENUNAME "redomenu"
#define UNDOALLMENUNAME "undoallmenu"
#define REDOALLMENUNAME "redoallmenu"
#define CUTMENUNAME "cutmenu"
#define COPYMENUNAME "copymenu"
#define PASTEMENUNAME "pastemenu"
#define FINDMENUNAME "findmenu"
#define FINDNEXTMENUNAME "findnextmenu"
#define PREFSMENUNAME "prefsmenu"
#define PLUGPREFSMENUNAME "plugprefsmenu"
#define SHOWDOCSMENUNAME "showdocsmenu"
#define SHOWBMBARMENUNAME "showbmbarmenu"
#define SHOWTOOLBARMENUNAME "showtoolbarmenu"
#define SHOWTOOLOUTMENUNAME "showtoolout"
#define SHOWSTATUSMENUNAME "showstatusmenu"
#define SHOWDOCVIEWERMENUNAME "showdocviewer"
#define GOTODEFMENUNAME "gotodefinemenu"
#define OPENINCLUDEMENUNAME "openincludemenu"
#define GOTOLINEMENUNAME "gotolinemenu"
#define SEARCHFORDEFMENUNAME "searchfordefmenu"
#define SEARCHGTKMENUNAME "searchgtkmenu"
#define SEARCHQT5MENUNAME "searchqt5menu"
#define SEARCHDOXYMENUNAME "searchdoxymenu"
#define GOBACKMENUNAME "gobackmenu"
#define GOFORWARDMENUNAME "goforwardmenu"
#define ABOUTMENUNAME "aboutmenu"
#define HELPMENUNAME "helpmenu"
#define ONLINEHELPMENUNAME "onlinehelpmenu"
#define GETPLUGSMENUNAME "getplugsmenu"
#define REMOVEALLBOOKMARKS "removeallbookmarks"
#define TOGGLEBOOKMARK "togglebookmark"

#define FINDNEXTOBJECTNAME "1"
#define FINDPREVOBJECTNAME "2"
#define FINDREPLACEOBJECTNAME "3"

#define TOOLCOMBOBOX "toolcombobox"
#define TOOLNAME "name"
#define TOOLCOMMAND "command"
#define TOOLKEY "shortcutkey"
#define TOOLCOMMENT "comment"
#define TOOLFLAGS "flags"
#define TOOLRUNINTERM "interm"
#define TOOLSHOWINPOPUP "inpopup"
#define TOOLALWAYSINPOPUP "alwayspopup"
#define TOOLRUNSYNC "toolrunsync"
#define TOOLSHOWDOC "toolshowdoc"
#define TOOLCLEAROP "clearview"
#define TOOLRUNASROOT "runasroot"
#define TOOLUSEPOLE "usebar"
#define TOOLIGNOREOUT "toolignoreout"
#define TOOLPASTEOUT "toolpasteout"
#define TOOLREPLACEALL "toolreplaceall"
#define TOOLVIEWOUT "toolviewout"

#define TOOL_ASYNC 8
#define TOOL_IGNORE_OP 0
#define TOOL_PASTE_OP 1
#define TOOL_REPLACE_OP 2
#define TOOL_SHOW_DOC 4
#define	TOOL_INSERT_MASK 0b10111
#define	TOOL_VIEW_OP 16

#define DEBUGSTR(x) QTextStream(stderr) << x << Qt::endl;
//#define DEBUGSTR(x)

//enum toolEnums {TOOL_NAME=0,TOOL_COMMAND,TOOL_COMMENT,TOOL_IN_TERM,TOOL_FLAGS,TOOL_INPOPUP,TOOL_ALWAYS_IN_POPUP,TOOL_CLEAR_VIEW,TOOL_RUN_AS_ROOT,TOOL_USE_BAR,TSHORTCUTKEY};
enum toolEnums {TOOL_ALWAYS_IN_POPUP=0,TOOL_CLEAR_VIEW,TOOL_COMMAND,TOOL_COMMENT,TOOL_FLAGS,TOOL_INPOPUP,TOOL_IN_TERM,TOOL_NAME,TOOL_RUN_AS_ROOT,TOOL_SHORTCUT_KEY,TOOL_USE_BAR,TOOL_END};
//alwayspopup	1
//clearview	0
//command	echo '%d--%f--%t--%h--%i--%m' > /dev/pts/2
//comment	test tool
//flags	0
//inpopup	0
//interm	1
//name	AAA Test
//runasroot	0
//shortcutkey	0
//usebar	0

#define MAXSESSIONS 24

struct args
{
	const char*	name;
	int			type;
	void*		data;
};

enum {TYPEINT=1,TYPESTRING,TYPEBOOL,TYPELIST};

class KKEditClass;
class DocumentClass;

struct bookMarkStruct
{
	DocumentClass*		doc;
	int					line;
	int					docIndex;
	QString				bmLabel;
	QAction				*menu;
	int					bmKey;
};

#include "KKEditClass.h"

#endif
