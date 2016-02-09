/*
 *
 * ©K. D. Hedger. Thu 13 Aug 16:53:22 BST 2015 kdhedger68713@gmail.com

 * This file (globals.cpp) is part of LFSDesktop.

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

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "globals.h"

int			errLine;
const char	*errFile;
const char	*errFunc;

//save/load file
//global file data
char		*fileDiskLabel=NULL;
char		*fileDiskMime=NULL;
char		*fileDiskPath=NULL;
char		*fileDiskUUID=NULL;
char		*fileDiskType=NULL;
char		*fileCustomIcon=NULL;
int			fileDiskXPos=-1;
int			fileDiskYPos=-1;
bool		fileGotCustomIcon=false;;

int			foundDiskNumber=-1;
bool		isDisk=false;
bool		debugDeskFlag=false;

int			xCnt;
int			yCnt;

//dialogs
LFSTK_windowClass	*diskWindow=NULL;
LFSTK_windowClass	*fileWindow=NULL;
LFSTK_windowClass	*iconChooser=NULL;
LFSTK_lineEditClass *iconChooserEdit=NULL;

args		globalFileData[]=
{
	{"label",TYPESTRING,&fileDiskLabel},
	{"mime",TYPESTRING,&fileDiskMime},
	{"path",TYPESTRING,&fileDiskPath},
	{"uuid",TYPESTRING,&fileDiskUUID},
	{"type",TYPESTRING,&fileDiskType},
	{"icon",TYPESTRING,&fileCustomIcon},
	{"xpos",TYPEINT,&fileDiskXPos},
	{"ypos",TYPEINT,&fileDiskYPos},
	{"custom",TYPEBOOL,&fileGotCustomIcon},

	{NULL,0,NULL}
};


bool shapeset=false;

int			deskIconsCnt=0;
int			deskIconsMaxCnt=30;
deskIcons	*deskIconsArray;

char		*rootDev=NULL;
const char	*possibleError="Unknown";
