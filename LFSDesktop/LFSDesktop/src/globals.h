/*
 *
 * ©K. D. Hedger. Thu 13 Aug 16:53:47 BST 2015 kdhedger68713@gmail.com

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

#define DEBUGFUNC(x,...) errLine=__LINE__,errFile=__FILE__,errFunc=__func__;debugFunc((const char*)x,__VA_ARGS__)
#define debugfunc  DEBUGFUNC

#define RESERVED 2
#define MAXBUFFER 512

#include <lfstk/LFSTKGlobals.h>

enum {CACHEFOLDER,DISKFOLDER};
enum {HOMEDATA=0,ROOTDATA};
enum diskButtonID {BUTTONMOUNT=1,BUTTONUNMOUNT,BUTTONEJECT,BUTTONOPEN,BUTTONADDICON,BUTTONREMOVEICON};

struct deskIcons
{
	char			*label;
	char			*uuid;
	char			*dev;
	char			*mountpoint;
	char			*mime;
	char			*icon;
	int				x;
	int				y;
	bool			mounted;
	char			*partname;
	bool			ignore;
	bool			dvd;
	bool			cdrom;
	bool			usb;
	bool			file;
	int				iconhint;
	bool			installed;
	bool			customicon;
};

extern int			deskIconsCnt;
extern int			deskIconsMaxCnt;
extern deskIcons	*deskIconsArray;

extern int			errLine;
extern const char	*errFile;
extern const char	*errFunc;

extern bool shapeset;

//dialogs
extern LFSTK_windowClass	*diskWindow;
extern LFSTK_windowClass	*fileWindow;
extern LFSTK_windowClass	*iconChooser;
extern LFSTK_lineEditClass *iconChooserEdit;

//save/load file
extern args		globalFileData[];
//global file data
extern char		*fileDiskLabel;
extern char		*fileDiskMime;
extern char		*fileDiskPath;
extern char		*fileDiskUUID;
extern char		*fileDiskType;
extern char		*fileCustomIcon;
extern int		fileDiskXPos;
extern int		fileDiskYPos;
extern bool		fileGotCustomIcon;

extern int		foundDiskNumber;
extern bool		isDisk;
extern bool		debugDeskFlag;

extern int		xCnt;
extern int		yCnt;

extern char		*rootDev;
extern const char	*possibleError;

#endif
