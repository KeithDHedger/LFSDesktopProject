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
LFSTK_listGadgetClass	*switchList=NULL;
LFSTK_windowClass		*switchWindow=NULL;

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
bool deskListCB(void* p,void* ud);

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
	switchButton->LFSTK_setValue(true);
	deskListCB( switchButton, ud);
	return(true);
}

bool deskListCB(void* p,void* ud)
{
	LFSTK_toggleButtonClass	*bc=static_cast<LFSTK_toggleButtonClass*>(p);
	geometryStruct			geom;
	int						x,y;
	Window					dw;

	if(p!=NULL)
		{	
			if(bc->LFSTK_getValue()==true)
				{
					bc->LFSTK_getGeomWindowRelative(&geom,apc->rootWindow);
					switch(panelGravity)
						{
							case PANELNORTH:
								switchWindow->LFSTK_moveWindow(geom.x+(geom.w/2)-(SLIDERWIDTH/2),geom.y+geom.h,true);
								break;
							case PANELSOUTH:
								switchWindow->LFSTK_moveWindow(geom.x+(geom.w/2)-(SLIDERWIDTH/2),geom.y-(GADGETHITE*6),true);
								break;
						}
					switchWindow->LFSTK_showWindow(true);
					switchWindow->LFSTK_redrawAllGadgets();
					apc->windows->at(apc->LFSTK_findWindow(switchWindow)).showing=true;
				}
			else
				{
					switchWindow->LFSTK_hideWindow();
					apc->windows->at(apc->LFSTK_findWindow(switchWindow)).showing=false;
				}
		}
	return(true);
}

void sendPropNotifyMessage(Window win,Atom msg)
{
	XEvent	event;
	long		mask=SubstructureRedirectMask|SubstructureNotifyMask|PropertyChangeMask;

	event.xproperty.type=PropertyNotify;
	event.xproperty.serial=0;
	event.xproperty.send_event=True;
	event.xproperty.window=win;
	event.xproperty.display=apc->display;
	event.xproperty.atom=msg;
	event.xproperty.state=PropertyNewValue;

	XSendEvent(apc->display,apc->rootWindow,False,mask,&event);
}

bool switchSelect(void *object,void* userdata)
{
	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);
	unsigned long			d=list->LFSTK_getCurrentListItem();

	XChangeProperty(apc->display,apc->rootWindow,NET_CURRENT_DESKTOP,XA_CARDINAL,32,PropModeReplace,(const unsigned char*)&d,1);
	sendPropNotifyMessage(apc->rootWindow,NET_CURRENT_DESKTOP);
	switchButton->LFSTK_setValue(false);
	deskListCB(switchButton,NULL);
	deskSwitcherExitCB(switchButton,NULL);
	return(true);
}

int addDesktopSwitcer(int x,int y,int grav)
{
	char					*icon=NULL;
	windowInitStruct		*win=new windowInitStruct;;
	listLabelStruct		ls;
	std::string			label;
	propertyStruct		props;
	propReturn			pr;

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
	switchButton->LFSTK_setGadgetColours(GADGETBG,panelBGColour,panelBGColour,panelBGColour,panelBGColour);
	switchButton->LFSTK_setGadgetColours(GADGETFG,panelTextColour,panelTextColour,panelTextColour,panelTextColour);

	icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"remote-desktop","");
	if(icon!=NULL)
		{
			switchButton->LFSTK_setImageFromPath(icon,LEFT,true);
			free(icon);
		}
	else
		switchButton->LFSTK_setImageFromPath(DATADIR "/pixmaps/windows.png",LEFT,true);

	win->x=0;
	win->y=0;
	win->w=1;//TODO//
	win->h=1;//GADGETHITE*6;
	apc->LFSTK_addToolWindow(win);
	switchWindow=apc->windows->back().window;
	switchList=new LFSTK_listGadgetClass(switchWindow,"list",0,0,2000,2000);;
	std::string	lc="#60f0f0f0";
	std::string	pc="#60a0a0a0";
	std::string	ac="#60404040";

	switchList->LFSTK_setStyle(BEVELNONE);
	switchList->LFSTK_setLabelAutoColour(true);
	switchList->LFSTK_setListItemsColours(GADGETBG,lc,pc,ac,lc);
	switchList->LFSTK_setListItemsColours(GADGETFG,"red","red","red","red");
	switchList->LFSTK_setGadgetColourPair(NORMALCOLOUR,lc,"red");

	switchList->LFSTK_setMouseCallBack(NULL,switchSelect,NULL);

	pr=apc->globalLib->LFSTK_getSingleProp(apc->display,apc->rootWindow,XInternAtom(apc->display,"_NET_DESKTOP_NAMES",false),XInternAtom(apc->display,"UTF8_STRING",false));
	for(int j=0;j<pr.strlist.size();j++)
		{
			label=pr.strlist.at(j);
			ls.label=strdup((char*)label.c_str());
			ls.imageType=NOTHUMB;
			ls.data.imagePath=NULL;
			ls.userData=USERDATA(j-1);
			switchList->LFSTK_appendToList(ls);
		}
/*
//TODO//
switchList->monoFontString=prefs.LFSTK_getCString(prefs.LFSTK_hashFromKey("font"));
switchList->LFSTK_resetListHeight(2000);
fprintf(stderr,"mfs=>>%s<<\n",switchList->monoFontString);
*/
	switchList->LFSTK_moveGadget(-1,-1);
	switchWindow->LFSTK_resizeWindow(switchList->LFSTK_getListMaxWidth()-2,(GADGETHITE*pr.strlist.size())-4);

	return(iconSize);
}
