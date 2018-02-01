/*
 *
 * ©K. D. Hedger. Wed Jan 10 11:33:17 GMT 2018 kdhedger68713@gmail.com

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


//#include <Python.h>

#include <getopt.h>
#include <sys/inotify.h>
#include <limits.h>
#include <poll.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>

#include "config.h"

#include <lfstk/LFSTKGlobals.h>

#include "prefs.h"
#include "files.h"
#include "disks.h"

#define EVENT_BUF_LEN	(sizeof(inotify_event)+NAME_MAX)*2

struct	diskDataStruct
{
	LFSTK_buttonClass	*diskImage;
	char				*label;
	char				*devName;
	char				*uuid;
	char				*pathToIcon;
	int					diskType;
	int					posx;
	int					posy;
	bool				hasCustomIcon;
	bool				mounted;
	bool				driveHasMedia;
	int					dataType;
	int					gadgetSize;
	bool				dirty;
};

struct	diskLinkedList
{
	diskLinkedList	*prev;
	diskDataStruct	*data;
	diskLinkedList	*next;
};

struct Hints
{
	unsigned long   flags;
	unsigned long   functions;
	unsigned long   decorations;
	long            inputMode;
	unsigned long   status;
};

enum {TYPENONE=-1,DISKDATATYPE=0,FILEDATATYPE};
enum {HDDDISK,USBHDD,THUMBDISK,CDROM,DVDROM,DESKFOLDER,DESKFILE,DESKTOPFILE};
enum {MOUNTDISK=0,UNMOUNTDISK,EJECTDISK,OPENDISK,CUSTOMICONDISK,REMOVECUSTOMDISK,NOMOREBUTONS};
enum {DIALOGRETERROR=0,DIALOGRETAPPLY,DIALOGRETCANCEL};

//main window
extern LFSTK_windowClass	*wc;
extern bool					mainLoop;
extern Display				*display;

//list
extern diskLinkedList		*diskLL;
extern int					nextXPos;
extern int					nextYPos;

//paths
extern char					*diskInfoPath;
extern char					*cacheDisksPath;
extern char					*cacheDeskPath;
extern char					*prefsPath;
extern char					*desktopPath;

//save data
extern char					*diskUUID;
extern char					*iconPath;
extern int					xPos;
extern int					yPos;
extern bool					customIcon;
extern int					dataType;


extern args					diskData[];

//prefs
extern bool					showSuffix;
extern int					maxXSlots;
extern int					maxYSlots;
extern int					**xySlot;
extern geometryStruct		oldPos;

//dialogs
extern LFSTK_windowClass	*diskWindow;
extern LFSTK_windowClass	*fileWindow;
extern LFSTK_windowClass	*iconChooser;
extern LFSTK_lineEditClass *iconChooserEdit;
extern LFSTK_buttonClass	*diskButtons[];
extern LFSTK_buttonClass	*fileButtons[];
extern bool					dialogLoop;
extern int					dialogRetVal;

void freeAndNull(char **data);
diskLinkedList* isInList(const char *devname);
void newNode(void);
void printDiskData(diskDataStruct *diskstruct);
void setImageSize(diskDataStruct *dnode);
void removeDeleted(void);
void getFreeSlot(int *x,int *y);
void setSlotFromPos(int x,int y,int val);
bool dialogCB(void *p,void* ud);
void dialogRun(LFSTK_windowClass *dialog);
int toNearestInt(int left,int rite);
void setGridXY(diskDataStruct *dnode,int x,int y);
void getRealXY(diskDataStruct *dnode,int *x,int *y);
void getFreeGridXY(int *x,int *y);

#endif
