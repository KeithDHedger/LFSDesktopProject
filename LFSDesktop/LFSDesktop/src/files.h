/*
 *
 * ©K. D. Hedger. Thu 13 Aug 16:54:01 BST 2015 kdhedger68713@gmail.com

 * This file (files.h) is part of LFSDesktop.

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


#ifndef _FILES_
#define _FILES_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <search.h>

#include "globals.h"
#include "graphics.h"

extern char		*diskInfoPath;
extern char		*cachePath;
extern char		*prefsPath;
extern char		*desktopPath;

extern hsearch_data	hashtab;

int fileExists(char *name);
void makeImage(char *imagepath,diskIconStruct *hashdata);
char* pathToIcon(char* name,const char* catagory);
void makeDiskInfofile(char* diskfilepath,char* label,char* uuid,int x,int y,char* type);
void readDesktopFile(const char* name);
void saveInfofile(int where,char* label,char* mime,char* path,char* uuid,char* type,int x, int y,int iconnum);
void getFreeSlot(int *x,int *y);
char* getMimeType(char *filepath);
char* oneLiner(const char* fmt,...);
void debugFuncxx(const char *fmt, ...);
char* defaultIcon(char *theme,char *name,const char *catagory);

#endif
