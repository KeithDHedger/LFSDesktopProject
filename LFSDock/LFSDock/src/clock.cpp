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

//LFSTK_labelClass	*clockButton=NULL;
LFSTK_buttonClass	*clockButton=NULL;

void updateClock(void)
{
	char		clockbuffer[256];
	time_t		rawtime;
	struct tm	*timeinfo;

	time(&rawtime);
	timeinfo=localtime(&rawtime);

	strftime(clockbuffer,255,"%I:%M:%S",timeinfo);
	clockButton->LFSTK_setLabel(clockbuffer);
	//clockButton->drawLabelBG=true;
	//clockButton->autoLabelBGColour=true;
//			clockButton->LFSTK_clearWindow();
//clockButton->LFSTK_setLabelBGColour(1.0,1,1.0,1.0);
//	clockButton->drawLabelBG=true;
//	//clockButton->autoLabelBGColour=true;
clockButton->LFSTK_clearWindow();
			XFlush(apc->display);
	
}

int addClock(int x,int y,int grav)
{
	int	xpos=0;
	int width=iconSize;
	int	retval=width;

	if(clockButton!=NULL)
		{
			printError("Duplicate clock");
			return(0);
		}
	if(grav==NorthWestGravity)
		xpos=x;
	else
		xpos=x-width;

	clockButton=new LFSTK_buttonClass(mainwind,"--:--:--",xpos,((iconSize+extraSpace)/2)-(GADGETHITE/2),iconSize,GADGETHITE,NorthWestGravity);

	clockButton->LFSTK_setFontString(prefs.LFSTK_getCString(prefs.LFSTK_hashFromKey("font")),true);

	setGadgetDetails(clockButton);
	clockButton->LFSTK_setAlpha(1.0);

	clockButton->LFSTK_setIgnores(false,false);
	clockButton->LFSTK_setTile(NULL,0);

	clockButton->LFSTK_setColourName(NORMALCOLOUR,panelBGColour);
	clockButton->LFSTK_setFontColourName(NORMALCOLOUR,panelTextColour,true);
	clockButton->LFSTK_setStyle(BEVELNONE);

	clockButton->drawLabelBG=true;
	clockButton->autoLabelBGColour=true;

	return(retval);
}
