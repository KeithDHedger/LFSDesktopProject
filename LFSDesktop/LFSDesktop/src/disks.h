/*
 *
 * ©K. D. Hedger. Thu Jan 11 13:01:38 GMT 2018 kdhedger68713@gmail.com

 * This file (disks.h) is part of LFSDesktop.

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

#ifndef _DISKS_
#define _DISKS_

#include "globals.h"

enum {HOMEPB=0,DESKTOPPB,COMPUTERPB,BOOKMARKPB,USBDISKPB,HDDRIVEPB,ROMPB,DVDROMPB,CDROMPB,NETWORKDIVE,NUMPBS};

void loadDisks(void);
void updateDisks(void);
bool doDiskMenuSelect(void *p,void* ud);

#endif
