/*
 *
 * ©K. D. Hedger. Tue  2 Nov 14:04:26 GMT 2021 keithdhedger@gmail.com

 * This file (kkedit-includes.h) is part of KKEdit.

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

#ifndef _KKEDIT_INCLUDES_
#define _KKEDIT_INCLUDES_

//#define _USEMINE_

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <glib.h>
#include <unistd.h>
#include <sys/types.h>

#include <linux/limits.h>

#include <string>
#include <vector>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/ipc.h>
#include <sys/msg.h>

#include "config.h"
#include <QtWidgets>
#include <QSettings>

#include <libgen.h>
#include <time.h>

#undef _DEBUG_FREE_
//#define _DEBUG_FREE_ 1

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

#define TOOL_ASYNC 8
#define TOOL_IGNORE_OP 0
#define TOOL_PASTE_OP 1
#define TOOL_REPLACE_OP 2
#define TOOL_SHOW_DOC 4
#define	TOOL_INSERT_MASK 0b10111
#define	TOOL_VIEW_OP 16

#define VALIDFILENAMECHARS "[A-Za-z0-9_-./ <>]"
#define VALIDFUNCTIONCHARS "[A-Za-z0-9_*:/@()-,.#;][\"]"

#define BOOKMAXMARKMENULEN 40
#define MAXRECENT 10
#define MAXTEXTWIDTH 500

#define MARK_TYPE_1		"one"
#define MARK_TYPE_2		"two"

#define USEURI			-1
#define USEFILE			-2

#define KKEDITFOLDER	".KKEditQT"

#define	POLEPATH		PREFIX "/bin/KKEditProgressBarQT"
#define CUSTOMWORDFILE ".KKEditQT/customcompletions"

#define NUMSHORTCUTS	14

enum {PIXBUF_COLUMN,TEXT_COLUMN,BUTTON_NUM};

enum  utilVarType {BOOLVAR=0,INTVAR,CHARVAR,MULTVAR,BADTYPE,DECIMALOUT,HEXOUT,OCTALOUT};

#define PLATFORM	"qt"
typedef long		uPtr;
typedef QWidget 	Widget;


class KKEditClass;
class DocumentClass;
class HistoryClass;

#ifdef _ASPELL_
#include <aspell.h>
#endif


#include "kkedit-plugins.h"
#include "globals.h"

#include "textbuffer.h"
#include "backclass.h"

#ifdef _BUILDDOCVIEWER_
#include <QtWebKitWidgets>
#endif

#include "qsourcehighliter.h"
#include "qsourcehighliterthemes.h"
#include "languagedata.h"


#include "QT_menuitem.h"
#include "QT_highlighter.h"
#include "QT_document.h"

#include "KKEditClass.h"
#include "navcallbacks.h"

#include "plugins.h"
#include "callbacks.h"

#include "searchcallbacks.h"

#include "files.h"
#include "completion.h"

#define VISIBLE __attribute__((visibility("default")))
#define PROTECTED __attribute__((visibility("protected")))

#endif
