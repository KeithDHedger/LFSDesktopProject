/*
 *
 * ©K. D. Hedger. Sun 19 Nov 19:10:19 GMT 2023 keithdhedger@gmail.com

 * This file (desktopSwitcher.cpp) is part of LFSDock.

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

LFSTK_toggleButtonClass	*switchButton;
bool						switchIsUp=false;

bool deskSwitcherExitCB(LFSTK_gadgetClass*p,void* ud)
{
	geometryStruct	geom2;
	int				adj;

	if(switchButton->LFSTK_getValue()==1)
		return(true);
	adj=extraSpace*posMultiplier;
	p->LFSTK_getGeom(&geom2);	
	p->LFSTK_moveGadget(geom2.x,geom2.y+adj);
	switchIsUp=false;
	return(true);
}

bool deskSwitcherEnterCB(LFSTK_gadgetClass*p,void* ud)
{
	geometryStruct	geom;
	int				adj;

	if(switchIsUp==true)
		return(true);

	adj=extraSpace*posMultiplier;
	p->LFSTK_getGeom(&geom);	
	p->LFSTK_moveGadget(geom.x,geom.y-adj);
	switchIsUp=true;
	return(true);
}

bool deskListCB(void* p,void* ud)
{
	return(true);
}

int addDesktopSwitcer(int x,int y,int grav)
{
	char		*icon=NULL;
	if(switchButton!=NULL)
		{
			printError("Duplicate switcher");
			return(0);
		}
	switchButton=new LFSTK_toggleButtonClass(mainwind,"",x,y,iconSize,iconSize);
	switchButton->LFSTK_setToggleStyle(TOGGLENORMAL);
	switchButton->LFSTK_setMouseCallBack(NULL,deskListCB,NULL);
	switchButton->LFSTK_setMouseMoveCallBack(deskSwitcherEnterCB,deskSwitcherExitCB,NULL);

	setGadgetDetails(switchButton);
	switchButton->LFSTK_setAlpha(1.0);
	switchButton->LFSTK_setStyle(BEVELNONE);

	switchButton->LFSTK_setColourName(NORMALCOLOUR,panelBGColour);
	switchButton->LFSTK_setColourName(PRELIGHTCOLOUR,panelBGColour);
	switchButton->LFSTK_setColourName(ACTIVECOLOUR,panelBGColour);

	switchButton->LFSTK_setFontColourName(NORMALCOLOUR,panelTextColour,true);
	switchButton->LFSTK_setFontColourName(PRELIGHTCOLOUR,panelTextColour,true);
	switchButton->LFSTK_setFontColourName(ACTIVECOLOUR,panelTextColour,true);

	icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"remote-desktop","");
	if(icon!=NULL)
		{
			switchButton->LFSTK_setImageFromPath(icon,LEFT,true);
			free(icon);
		}
	else
		switchButton->LFSTK_setImageFromPath(DATADIR "/pixmaps/windows.png",LEFT,true);

	return(iconSize);
}
