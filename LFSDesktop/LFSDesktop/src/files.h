/*
 *
 * ©K. D. Hedger. Thu 13 Aug 16:54:01 BST 2015 keithdhedger@gmail.com

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

#include "globals.h"

void loadDesktopItems(void);
void addDeskItem(const char *name);
void deleteDeskItem(const char *name);
void launchDesktopFile(const char *name);
bool doDeskItemMenuSelect(void *p,void* ud);

#endif
