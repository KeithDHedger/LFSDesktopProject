/*
 *
 * ©K. D. Hedger. Sun 20 Sep 14:41:10 BST 2015 keithdhedger@gmail.com

 * This file (globals.cpp) is part of LFSPanel.

 * LFSPanel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSPanel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSPanel.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "globals.h"

//prefs
char				*terminalCommand=NULL;
int					panelHeight=16;
int					panelWidth=-1;
const monitorStruct	*mons=NULL;
int					onMonitor=0;
char				*logoutCommand=NULL;
char				*restartCommand=NULL;
char				*shutdownCommand=NULL;
char				*rightGadgets=NULL;
char				*leftGadgets=NULL;
int					panelPos=PANELCENTRE;
int					panelGravity=PANELNORTH;

char				*desktopTheme=NULL;
const char			*panelID="";

int					refreshRate=1;

int					iconSize=16;
bool				useAlarm=false;

//panel window
LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*mainwind=NULL;
int						rightOffset=0;
int						leftOffset=0;

//atoms
Atom				WM_STATE=None;
Atom				NET_WM_WINDOW_TYPE_NORMAL=None;
Atom				NET_WM_STATE_HIDDEN=None;
Atom				NET_WM_WINDOW_TYPE_DIALOG=None;
Atom				NET_WM_DESKTOP=None;
Atom				NET_WM_WINDOW_TYPE=None;
Atom				NET_WM_STATE=None;

void setSizes(int *x,int *y,int *w,int *h,int *size,int *grav,bool fromleft)
{
	*w=panelHeight;
	*h=panelHeight;
	*size=(*w)-12;

	switch(*grav)
		{
			case PANELNORTH:
			case PANELSOUTH:
				if(fromleft==true)
					{
						*grav=NorthWestGravity;
						*x=*x;
					}
				else
					{
						*grav=NorthEastGravity;
						*x=*x-*w+1;
					}
				*y=0;
				break;

			case PANELEAST:
			case PANELWEST:
				if(fromleft==true)
					{
						*grav=NorthWestGravity;
						*y=*x;
					}
				else
					{
						*grav=SouthWestGravity;
						*y=*x-*h+1;
					}
				*x=0;
				break;
		}
}

