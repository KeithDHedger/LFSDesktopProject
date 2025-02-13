/*
 *
 * ©K. D. Hedger. Mon 21 Sep 13:41:36 BST 2015 keithdhedger@gmail.com

 * This file (clock.cpp) is part of LFSPanel.

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

#include <unistd.h>
#include <signal.h>
#include <ctime>

#include "globals.h"

LFSTK_labelClass	*clockButton=NULL;

void updateClock(void)
{
	char		clockbuffer[256];
	time_t		rawtime;
	struct tm	*timeinfo;

	time(&rawtime);
	timeinfo=localtime(&rawtime);

	strftime(clockbuffer,255,"%I:%M:%S",timeinfo);
	clockButton->LFSTK_setLabel(clockbuffer);
}

int addClock(int x,int y,int grav)
{
	int	xpos=0;
	int width=BWIDTH;
	int	retval=width;

	if((panelGravity==PANELEAST) || (panelGravity==PANELWEST))
		{
			printError("Clock not allowed with this panel's orientation.");
			return(0);
		}

	if(clockButton!=NULL)
		{
			printError("Duplicate clock");
			return(0);
		}
	if(grav==NorthWestGravity)
		xpos=x;
	else
		xpos=x-width+1;

	clockButton=new LFSTK_labelClass(mainwind,"--:--:--",xpos,0,width,panelHeight,CENTRE,grav);

	setGadgetDetails(clockButton);

	return(retval);
}
