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

LFSTK_buttonClass		*switchButton;
LFSTK_buttonClass		*deskLabel;
std::vector<std::string>	deskNames;

void updateDeskSwitcher(void)
{
	propReturn	pr;

	pr=apc->globalLib->LFSTK_getSingleProp(apc->display,apc->rootWindow,NET_CURRENT_DESKTOP,XA_CARDINAL);
	deskLabel->LFSTK_setLabel(std::to_string(pr.integer+1).c_str());
}

bool deskSwitcherExitCB(LFSTK_gadgetClass*p,void* ud)
{
	if(moveGadget==true)
		setGadgetPosition(p,false);

	exitPopList(p,ud);
	inSomeWindow=false;
	return(true);
}

bool deskSwitcherEnterCB(LFSTK_gadgetClass*p,void* ud)
{
	std::string		label;
	infoDataStruct	ls;

	showhidetActionList(NULL,popActionWindow,popActionList);

	if(moveGadget==true)
		setGadgetPosition(p,true);

	popActionList->LFSTK_freeList();	

	for(int j=0;j<deskNames.size();j++)
		{
			label=deskNames.at(j);
			ls.label=label;
			ls.imageType=NOTHUMB;
			ls.userData=USERDATA(DESKTOPSWITCHER);
			popActionList->LFSTK_appendToList(ls);
		}

	popActionList->LFSTK_updateList();
	popActionList->LFSTK_moveGadget(-2,0);
	popActionWindow->userData=USERDATA(DESKTOPSWITCHER);
	popActionWindow->LFSTK_resizeWindow(popActionList->LFSTK_getListMaxWidth()-2,(GADGETHITE*deskCount)-4);
	showhidetActionList(p,popActionWindow,popActionList);

	XFlush(apc->display);
	inSomeWindow=true;

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

bool desktopSelect(void *object,void* userdata)
{
	unsigned long	d=popActionList->LFSTK_getCurrentListItem();

	XChangeProperty(apc->display,apc->rootWindow,NET_CURRENT_DESKTOP,XA_CARDINAL,32,PropModeReplace,(const unsigned char*)&d,1);
	sendPropNotifyMessage(apc->rootWindow,NET_CURRENT_DESKTOP);
	showhidetActionList(NULL,popActionWindow,popActionList);
	deskLabel->LFSTK_setLabel(std::to_string(d+1).c_str());
	return(true);
}

int addDesktopSwitcer(int x,int y,int grav)
{
	std::string		icon;
	infoDataStruct	ls;
	std::string		label;
	propertyStruct	props;
	propReturn		pr;
	std::string		lc="#a0f0f0f0";
	std::string		pc="#60a0a0a0";
	std::string		ac="#60404040";

	if(switchButton!=NULL)
		{
			printError("Duplicate switcher");
			return(0);
		}
//TODO//
	switchButton=new LFSTK_buttonClass(dockWindow,"",x,normalY,iconWidth,iconHeight);
	setGadgetDetails(switchButton);
	switchButton->LFSTK_setAlpha(1.0);
	switchButton->LFSTK_setStyle(BEVELNONE);
	switchButton->LFSTK_setMouseMoveCallBack(deskSwitcherEnterCB,deskSwitcherExitCB,NULL);

	pr=apc->globalLib->LFSTK_getSingleProp(apc->display,apc->rootWindow,NET_CURRENT_DESKTOP,XA_CARDINAL);
	int ly=0;
	if(dockGravity==PANELSOUTH)
		{
			if(moveGadget==true)
				ly=normalY+iconWidth;
			else
				ly=0+normalY+iconHeight-GADGETHITE/2;
		}

	deskLabel=new LFSTK_buttonClass(dockWindow,std::to_string(pr.integer+1).c_str(),x,ly,iconWidth,GADGETHITE/2);
	setGadgetDetails(deskLabel);
	deskLabel->LFSTK_setFontString(prefs.LFSTK_getCString("font"),true);
	deskLabel->LFSTK_setIgnores(false,false);
	deskLabel->LFSTK_setTile(NULL,0);
	deskLabel->LFSTK_setGadgetColourPair(NORMALCOLOUR,dockBGColour,dockTextColour);
	deskLabel->LFSTK_setStyle(BEVELNONE);

	icon=dockWindow->globalLib->LFSTK_findThemedIcon(desktopTheme,"remote-desktop","");
	if(icon.length()>0)
		switchButton->LFSTK_setImageFromPath(icon,CENTRE,true);
	else
		switchButton->LFSTK_setImageFromPath(DATADIR "/pixmaps/windows.png",CENTRE,true);

//	windowInitStruct	*win;

	pr=apc->globalLib->LFSTK_getSingleProp(apc->display,apc->rootWindow,XInternAtom(apc->display,"_NET_DESKTOP_NAMES",false),XInternAtom(apc->display,"UTF8_STRING",false));
	for(int j=0;j<deskCount;j++)
		{
			if(pr.strlist.size()==0)
				deskNames.push_back("Desktop " + std::to_string(j+1));
			else
				deskNames.push_back(pr.strlist.at(j));
		}
	return(iconWidth);
}
