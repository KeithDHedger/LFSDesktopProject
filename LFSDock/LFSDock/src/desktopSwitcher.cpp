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
	LFSTK_toggleButtonClass	*bc=static_cast<LFSTK_toggleButtonClass*>(p);
	geometryStruct			geom;
	int						x,y;
	Window					dw;

	if(p!=NULL)
		{	
			if(bc->LFSTK_getValue()==1)
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
	return(true);
}

int addDesktopSwitcer(int x,int y,int grav)
{
	char					*icon=NULL;
	windowInitStruct		*win=new windowInitStruct;;
	listLabelStruct		ls;
	std::string			label;
	propertyStruct		props;

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

	win->x=0;
	win->y=0;
	win->w=100;//TODO//
	win->h=GADGETHITE*6;
	apc->LFSTK_addToolWindow(win);
	switchWindow=apc->windows->back().window;
	switchList=new LFSTK_listGadgetClass(switchWindow,"list",0,0,200,GADGETHITE*6);;
	switchList->LFSTK_setMouseCallBack(NULL,switchSelect,NULL);

//	//switchWindow->LFSTK_setTile(NULL,0);
////	switchWindow->LFSTK_setWindowColourName(NORMALCOLOUR,"#00000000");
////	switchList->LFSTK_setTile(NULL,0);
////	//switchList->LFSTK_setAlpha(128);
////	switchList->LFSTK_setColourName(NORMALCOLOUR,"#00000000");
switchList->LFSTK_setStyle(BEVELNONE);
	propReturn pr;

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
	return(iconSize);
}
