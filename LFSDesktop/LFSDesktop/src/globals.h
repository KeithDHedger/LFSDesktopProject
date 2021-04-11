/*
 *
 * ©K. D. Hedger. Wed Jan 10 11:33:17 GMT 2018 keithdhedger@gmail.com

 * This file (globals.h) is part of LFSDesktop.

 * LFSDesktop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSDesktop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDesktop.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GLOBALS_
#define _GLOBALS_

#include <getopt.h>
#include <sys/inotify.h>
#include <limits.h>
#include <poll.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <vector>
#include <unistd.h>
#include <glib.h>
#include <libudev.h>

#include "config.h"
#include <lfstk/LFSTKGlobals.h>

#include "callbacks.h"
#include "prefs.h"
#include "disks.h"
#include "files.h"

enum {ISHDDDISK=1000,ISUSBHDD,ISTHUMBDISK,ISCDROM,ISDVDROM,ISIPOD,ISSSD,ISDESKTOPFILE,ISDOCUMENTSFOLDER,ISHOMEFOLDER,ISCOMPUTER,ISGENERIC,ISIMAGEFILE,ISINVALID};
enum {MOUNTDISK=0,UNMOUNTDISK,EJECTDISK,OPENDISK,CUSTOMICONDISK,REMOVECUSTOMDISK,NOMOREBUTONS};
enum {DIALOGRETERROR=0,DIALOGRETAPPLY,DIALOGRETCANCEL};
enum {DESKITEMOPEN,DESKITEMEXECUTE,DESKITEMCUSTOMICON,DESKITEMREMOVECUSTOM,NOMOREDESKITEMBUTONS};

struct	desktopItemStruct
{
	char								*uuid=NULL;
	char								*itemPath=NULL;
	char								*label=NULL;
	char								*iconPath=NULL;
	int									posx=-1;
	int									posy=-1;
	bool								hasCustomIcon=false;
	char								*pathToCustomIcon=NULL;
	bool								isADisk=false;
	bool								isAFolder=false;
	bool								mounted=false;
	int									type=ISINVALID;
	bool								isSymLink=false;
	LFSTK_buttonClass					*item=NULL;
	bool								dirty=false;
};

extern desktopItemStruct				cacheFileData;
extern std::vector<desktopItemStruct>	desktopItems;
extern char								*documentsPath;
extern char								*homePath;
extern char								*diskWatch;
extern char								*desktopWatch;

//main window
extern LFSTK_applicationClass			*apc;
extern LFSTK_windowClass				*wc;
//paths
extern char								*cachePath;
extern char								*prefsPath;
extern char								*desktopPath;

//prefs
extern bool								showSuffix;

//dialogs
extern LFSTK_windowClass				*diskWindow;
extern LFSTK_windowClass				*fileWindow;

//TODO//
//extern LFSTK_windowClass				*iconChooser;
//extern LFSTK_lineEditClass 				*iconChooserEdit;
//extern bool								dialogLoop;
//extern int								dialogRetVal;
//bool dialogCB(void *p,void* ud);
//void dialogRun(LFSTK_windowClass *dialog);

void setIconImage(desktopItemStruct	*cf);
void setItemSize(desktopItemStruct	*cf);
void createDesktopGadget(LFSTK_windowClass *window);

#endif
