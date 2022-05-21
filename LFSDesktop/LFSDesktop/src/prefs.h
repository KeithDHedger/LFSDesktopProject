/*
 *
 * ©K. D. Hedger. Wed Jan 10 20:31:18 GMT 2018 keithdhedger@gmail.com

 * This file (prefs.h) is part of LFSDesktop.

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

#ifndef _PREFS_
#define _PREFS_

#include "globals.h"
struct desktopItemStruct;

extern const char	*iconTheme;
extern int			iconSize;
extern int			gridSize;
extern int			gridBorderLeft;
extern int			gridBorderRight;
extern int			refreshRate;
extern const char	*terminalCommand;
extern bool			showSuffix;
extern const char	*fontFace;
extern const char	*foreCol;
extern const char	*backCol;
extern const char	*backAlpha;
extern const char	*includeList;
extern const char	*excludeList;
extern bool			doubleClickExecute;

void reloadPrefs(void);
void loadPrefs(void);
void saveCacheFile(const char *cachefilepath,desktopItemStruct *cfd);
bool loadCacheFile(const char *cachefilepath,desktopItemStruct *cfd);

#endif
