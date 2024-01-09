/*
 *
 * ©K. D. Hedger. Sun 19 Nov 19:09:27 GMT 2023 keithdhedger@gmail.com

 * This file (clock.cpp) is part of LFSDock.

 * LFSDock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSDock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDock.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "globals.h"

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
	clockButton->LFSTK_clearWindow();
	XFlush(apc->display);
}

int addClock(int x,int y,int grav)
{
	if(clockButton!=NULL)
		{
			printError("Duplicate clock");
			return(0);
		}

	clockButton=new LFSTK_buttonClass(dockWindow,"--:--:--",x,y+(iconWidth/2)-(GADGETHITE/2),iconWidth,GADGETHITE);
	setGadgetDetails(clockButton);
	clockButton->LFSTK_setFontString(prefs.LFSTK_getCString(prefs.LFSTK_hashFromKey("font")),true);
	clockButton->LFSTK_setIgnores(false,false);
	clockButton->LFSTK_setTile(NULL,0);
	//clockButton->LFSTK_setLabelAutoColour(true);
	clockButton->LFSTK_setGadgetColourPair(NORMALCOLOUR,dockBGColour,dockTextColour);
	clockButton->LFSTK_setStyle(BEVELNONE);	

	//clockButton->drawLabelBG=true;
	//clockButton->autoLabelBGColour=true;

	return(iconWidth);
}
