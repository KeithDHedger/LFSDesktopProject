/*
 *
 * ©K. D. Hedger. Wed Jan 10 20:31:18 GMT 2018 kdhedger68713@gmail.com

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

#define MAXGRIDY 16
#define MAXGRIDX 16

extern args	desktopPrefs[];

extern char	*iconTheme;
extern int	iconSize;
extern int	gridSize;
extern int	gridBorderLeft;
extern int	gridBorderRight;
extern int	refreshRate;
extern char	*terminalCommand;
extern bool	showSuffix;
extern char	*fontFace;
extern char	*foreCol;
extern char	*backCol;
extern char	*backAlpha;
extern char	*includeList;
extern char	*excludeList;

void saveVarsToFile(const char* filepath,args* dataptr);
bool loadVarsFromFile(char* filepath,args* dataptr);
void freePrefs(void);
void reloadPrefs(void);

#endif
