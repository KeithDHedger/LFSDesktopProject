/*
 *
 * ©K. D. Hedger. Sun 20 Sep 18:36:44 BST 2015 keithdhedger@gmail.com

 * This file (logout.cpp) is part of LFSPanel.

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

#include "logout.h"

LFSTK_buttonClass	*logoutButton=NULL;
menuStruct			**logoutMenus;
LFSTK_menuClass		*logoutMenu=NULL;

const char			*logoutLabels[]={"Logout","Restart","Shutdown"};
const char			*logoutImages[]={DATADIR "/pixmaps/logout.png",DATADIR "/pixmaps/reboot.png",DATADIR "/pixmaps/shutdown.png"};
const char			*logoutIconNames[]={"system-log-out","system-restart","system-shutdown","application-exit"};

bool logoutMenuCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();

	switch((long)ud)
		{
			case LOGOUT:
#ifdef _ENABLEDEBUG_
				DEBUGFUNC("logoutCommand=>>%s<<\n",prefs.LFSTK_getCString("logoutcommand"));
				realMainLoop=false;
				apc->mainLoop=false;
#else
				system(prefs.LFSTK_getCString("logoutcommand"));
#endif
				break;
			case RESTART:
#ifdef _ENABLEDEBUG_
				DEBUGFUNC("restartCommand=>>%s<<\n",prefs.LFSTK_getCString("restartcommand"));
#else
				system(prefs.LFSTK_getCString("restartcommand"));
#endif
				break;
			case SHUTDOWN:
#ifdef _ENABLEDEBUG_
				DEBUGFUNC("shutdownCommand=>>%s<<\n",prefs.LFSTK_getCString("shutdowncommand"));
#else
				system(prefs.LFSTK_getCString("shutdowncommand"));
#endif
				break;
		}

	return(true);
}

bool logoutCB(void *p,void* ud)
{
	geometryStruct geom;

	static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getGeom(&geom);
	logoutMenu->x=geom.x;
	logoutMenu->LFSTK_showMenu();
	return(true);
}

int  addLogout(int x,int y,int grav,bool fromleft)
{
	char	*themedicon=NULL;
	char	*icon=NULL;
	int		xpos=x;
	int		ypos=y;
	int		width=0;
	int		height=0;
	int		thisgrav=grav;
	int		iconsize=16;

	if(logoutButton!=NULL)
		{
			printError("Duplicate logout");
			return(0);
		}

	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);

	logoutButton=new LFSTK_buttonClass(mainwind,"",xpos,ypos,width,height,thisgrav);
	icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,logoutIconNames[NUMLOGOUTENTRYS],"");

	if(icon!=NULL)
		{
			logoutButton->LFSTK_setImageFromPath(icon,LEFT,true);
			free(icon);
		}
	else
		logoutButton->LFSTK_setImageFromPath(DATADIR "/pixmaps/exit.png",LEFT,true);

	setGadgetDetails(logoutButton);

	logoutButton->LFSTK_setMouseCallBack(NULL,logoutCB,NULL);
	logoutMenu=new LFSTK_menuClass(mainwind,xpos,ypos+panelHeight,1,1);

	logoutMenus=new menuStruct*[NUMLOGOUTENTRYS];

	for(int j=LOGOUT;j<NUMLOGOUTENTRYS;j++)
		{
			logoutMenus[j]=new menuStruct;
			logoutMenus[j]->label=strdup(logoutLabels[j]);
			logoutMenus[j]->userData=(void*)(long)j;

			themedicon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,logoutIconNames[j],"");
			if(themedicon!=NULL)
				{
					logoutMenus[j]->imagePath=themedicon;
					free(themedicon);
				}
			else
				{
					logoutMenus[j]->imagePath=logoutImages[j];
				}
			logoutMenus[j]->imageType=FILETHUMB;
		}

	logoutMenu->LFSTK_setMouseCallBack(NULL,logoutMenuCB,NULL);
	logoutMenu->LFSTK_addMainMenus(logoutMenus,NUMLOGOUTENTRYS);
	return(width);
}
