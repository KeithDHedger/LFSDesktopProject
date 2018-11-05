/*
 *
 * ©K. D. Hedger. Fri 24 Jul 19:33:15 BST 2015 keithdhedger@gmail.com

 * This file (LFSTKGlobals.h) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LFSTKGLOBALS_
#define _LFSTKGLOBALS_

#include <X11/Xft/Xft.h>
#include <iostream>
#include <map>
#include <dirent.h>

#include <cairo.h>
#include <cairo-xlib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define COPYRITE			"Copyright © 2013-2017 K.D.Hedger"
#define PERSONAL			"keithdhedger@gmail.com"
#define	MYWEBSITE			"http://keithhedger.freeddns.org/"

#define DEFAULTFONT			"sans:size=10"
#define DISCLOSURESIZE		6

#define USERDATA(x)			(void*)(long)x
#define GETUSERDATA(x)		(long)x

#define DIALOGWIDTH			450
#define DIALOGHITE			800
#define DIALOGMIDDLE		DIALOGWIDTH/2

#define GADGETWIDTH			80
#define GADGETHITE			24
#define HALFGADGETWIDTH		GADGETWIDTH/2

#define BORDER				16
#define YSPACING			32
#define HALFYSPACING		YSPACING/2
#define BUTTONGRAV			NorthWestGravity

enum {CANCEL=0,APPLY};
enum {NONE=0,LEFT,CENTRE,RIGHT,AUTO,MENU,PRESERVEASPECT,TOOLBAR};
enum gadgetState {NORMALCOLOUR=0,PRELIGHTCOLOUR,ACTIVECOLOUR,INACTIVECOLOUR,MAXCOLOURS};

enum {BUTTONGADGET=1,MENUBUTTONGADGET,LINEEDITGADGET,LABELGADGET,TOGGLEGADGET,IMAGEGADGET,LISTGADGET,DIALOGGADGET,MULTILINEGADGET,SCROLLBARGADGET,MENUGADGET,MENUITEMGADGET};
enum {TYPEINT=1,TYPESTRING,TYPEBOOL};
enum {NORMAL=0,ABOVEALL,BELOWALL};

enum bevelType {BEVELIN=0,BEVELOUT,BEVELNONE};
enum indicatorType {CHECK=0,RADIO,PICTURE,DISCLOSURE,NOINDICATOR};

//dropdata
enum dropDataType {DROPINVALID=0,DROPTEXT,DROPURI};

#ifdef _ENABLEDEBUG_
static bool	showFileData=true;
static void debugFunc(const char *file,const char *func,int line,const char *fmt, ...)
{
	va_list	ap;
	char	*buffer,*subbuffer;

	buffer=(char*)alloca(512);
	subbuffer=(char*)alloca(512);

	buffer[0]=0;
	subbuffer[0]=0;
	va_start(ap, fmt);
	while (*fmt)
		{
			subbuffer[0]=0;
			if(fmt[0]=='%')
				{
					fmt++;
					switch(*fmt)
						{
							case 's':
								sprintf(subbuffer,"%s",va_arg(ap,char*));
								break;
							case 'i':
								sprintf(subbuffer,"%i",va_arg(ap,int));
								break;
							case 'p':
								sprintf(subbuffer,"%p",va_arg(ap,char*));
								break;
							case 'c':
								sprintf(subbuffer,"%c",va_arg(ap,int));
								break;
							case 'f':
								sprintf(subbuffer,"%f",va_arg(ap,double));
								break;
							default:
								sprintf(subbuffer,"%c",fmt[0]);
								break;
						}
				}
			else
				sprintf(subbuffer,"%c",fmt[0]);
			strcat(buffer,subbuffer);
			fmt++;
		}
	va_end(ap);

	if(showFileData==true)
		{
			printf("\nFile: %s\nFunc: %s\nLine: %i\n",basename(file),func,line);
			printf("----USER DATA----\n%s\n----END----\n",buffer);
		}
	else
		{
			printf("%s\n",buffer);
		}
}
#define DEBUGFUNC(x,...) debugFunc(__FILE__,__func__,__LINE__,(const char*)x,__VA_ARGS__)
#else
#define DEBUGFUNC(...) printf("Remove debug code here: %s:%i\n",__FILE__,__LINE__);
#endif

static bool	xLibErrorTK=false;
static bool	xLibWarningsTK=false;
static int	xErrHandler(Display *dpy,XErrorEvent *e)
{
	char buf[256];

	xLibErrorTK=true;
	if(xLibWarningsTK==true)
		{
			buf[0]=0;
			XGetErrorText(dpy,e->error_code,buf,sizeof buf);
			printf("Xlib Error: %s",buf);
		}
	return 0;
}

static void freeAndNull(char **data)
{
	if((data!=NULL) && (*data!=NULL))
		{
			free(*data);
			*data=NULL;
		}
}

struct args
{
	const char*				name;
	int						type;
	void*					data;
};

struct buttonCB
{
	bool					(*pressCallback)(void *,void*);
	bool					(*releaseCallback)(void *,void*);
	void					*userData;
	bool					ignoreCallback;
};

struct geometryStruct
{
	int						x,y;
	unsigned				w,h;
	unsigned				monitor;
};

struct pointStruct
{
	int						x,y;
};

struct rectStruct
{
	int						x,y;
	int						w,h;
};

struct	cairoColor
{
	double					r,g,b,a;
};

struct colourStruct
{
	char					*name;
	long					pixel;
	cairoColor				RGBAColour;
};

class	LFSTK_gadgetClass;
struct	mappedListener
{
	bool				(*function)(void *,XEvent *,int);
	LFSTK_gadgetClass	*gadget=NULL;
	int					type;
};

class	LFSTK_listGadgetClass;
struct listData
{
	LFSTK_listGadgetClass	*mainObject;
	unsigned				userData;
};

struct gadgetStruct
{
	colourStruct	*colour;
	bevelType		bevel;
	indicatorType	indic;
	int				state;
	int				reserveSpace;
	bool			buttonTile;
	geometryStruct	gadgetGeom;
	geometryStruct	indicatorGeom;
	bool			hasIndicator;
	bool			useWindowPixmap;
};

struct windowInitStruct
{
	int			x=0;
	int			y=0;
	int			w=1;
	int			h=1;
	const char	*name="";
	bool		overRide=false;
	bool		loadVars=true;
	bool		shutDisplayOnExit=false;
	const char	*windowType="_NET_WM_WINDOW_TYPE_NORMAL";
	bool		decorated=true;
	int			level=NORMAL;
};

//menus
class	LFSTK_menuClass;
struct menuStruct
{
	char			*label;
	bool			hasSubMenu;
	LFSTK_menuClass	*subMenu;
	void			*userData;
};

//messaging
#define MAX_MSG_SIZE 256
#define WAIT_MSG 0

#define MSGANY 0
struct msgBuffer
{
	long mType;
	char mText[MAX_MSG_SIZE];
};

enum {DESKTOP_MSG=1000,WMANAGER_MSG,PANEL_MSG,WALLPAPER_MSG,TOOLKIT_MSG,APPEARANCE_PREFS_MSG,BACKDROP_PREFS_MSG,DESKTOP_PREFS_MSG,PANEL_PREFS_MSG,TK_PREFS_MSG,WMANAGER_PREFS_MSG};

#include "LFSTKLib.h"
#include "LFSTK_FindClass.h"
#include "LFSTKWindow.h"
#include "LFSTKGadget.h"
#include "LFSTKLabel.h"
#include "LFSTKImage.h"
#include "LFSTKButton.h"
#include "LFSTKMenuButton.h"
#include "LFSTKLineEdit.h"
#include "LFSTKToggleButton.h"
#include "LFSTKScrollBar.h"
#include "LFSTKListGadget.h"
#include "LFSTKFileDialog.h"
#include "LFSTKFontDialog.h"
#include "LFSTKMultiLineEdit.h"
#include "LFSTKMenuItem.h"
#include "LFSTKMenu.h"

#endif