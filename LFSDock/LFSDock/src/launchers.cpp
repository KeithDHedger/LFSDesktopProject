/*
 *
 * ©K. D. Hedger. Wed 28 Oct 19:47:41 GMT 2015 keithdhedger@gmail.com

 * This file (launchers.cpp) is part of LFSPanel.

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

#include <ftw.h>

#include "launchers.h"

LFSTK_buttonClass				*contextButtons[NOMOREBUTONS];
const char						*contextLabelData[]={"Launch","Remove From Dock","Run Prefs","Quit Dock",NULL};
const char						*contextThemeIconData[]={"media-playback-start","list-remove","LFSTKPrefs","dialog-warning"};
std::vector<launcherDataStruct>	launchersArray;
LFSTK_findClass					*findlaunchers=NULL;
LFSTK_gadgetClass				*currentLauncher=NULL;

LFSTK_windowClass				*launcherContextWC=NULL;
LFSTK_windowClass				*tooltiptWC=NULL;
LFSTK_labelClass					*ttLabel=NULL;

bool launcherContextCB(void *p,void* ud)
{
	int					winnum;
	LFSTK_windowClass	*lwc=static_cast<LFSTK_gadgetClass*>(p)->wc;
	long unsigned int	whatbutton=(long unsigned int)ud;
	launcherDataStruct	lds=launchersArray.at((long unsigned int)lwc->popupFromGadget->userData);

	if(p!=NULL)
		{
			tooltiptWC->LFSTK_moveWindow(-1000,1000,true);
			winnum=lwc->app->LFSTK_findWindow(lwc);
			lwc->app->windows->at(winnum).loopFlag=false;

			switch(whatbutton)
				{
					case BUTTONQUIT:
						realMainLoop=false;
						apc->mainLoop=false;
						break;
					case BUTTONLAUNCH:
						launcherCB(NULL,lwc->popupFromGadget->userData);
						break;
					case BUTTONREMOVE:
						sendNotify("Removing ",lds.name);
						XSync(apc->display,false);
						sleep(1);
						unlink(lds.path.c_str());
						apc->exitValue=0;
						apc->mainLoop=false;
						
						break;
					case BUTTONPREFS:
						{
							std::string	com;
							com="lfsdockprefs -d "+whatDock+" &";
							system(com.c_str());
						}
						break;
				}
		}
	return(true);
}

bool launcherEnterCB(LFSTK_gadgetClass* p,void* ud)
{
	if(p!=NULL)
		{
			launcherDataStruct	lds=launchersArray.at((long unsigned int)ud);
			XEvent			event;
			geometryStruct	geom;
			if(checkInBorder(p)==false)
				return(true);

			currentLauncher=p;
			setGadgetPosition(p,true);

			ttLabel->LFSTK_setLabel(lds.name,true);
			ttLabel->LFSTK_setCairoFontDataParts("s",12);
			tooltiptWC->LFSTK_resizeWindow(ttLabel->LFSTK_getTextRealWidth(lds.name)+4,GADGETHITE-4);
			p->LFSTK_getGeomWindowRelative(&geom,apc->rootWindow);

			switch(dockGravity)
				{
					case PANELNORTH:
						tooltiptWC->LFSTK_moveWindow((geom.x+((iconWidth)/2)-(ICONSPACE*2))-(ttLabel->LFSTK_getTextRealWidth(lds.name)/2),dockWindow->h,true);
						break;
					case PANELSOUTH:
						tooltiptWC->LFSTK_moveWindow((geom.x+((iconWidth)/2)-(ICONSPACE*2))-(ttLabel->LFSTK_getTextRealWidth(lds.name)/2),geom.y-GADGETHITE+extraSpace,true);
						break;
				}
			XRaiseWindow(apc->display,tooltiptWC->window);
		}
	dockWindow->LFSTK_redrawAllGadgets();
	return(true);
}

bool launcherExitCB(LFSTK_gadgetClass* p,void* ud)
{
	if(p!=NULL)
		{
			if(checkInBorder(p)==false)
				return(true);

			currentLauncher=NULL;
			setGadgetPosition(p,FALSE);
			dockWindow->LFSTK_redrawAllGadgets();
			tooltiptWC->LFSTK_moveWindow(-1000,1000,true);
			XRaiseWindow(apc->display,launcherContextWC->window);
		}
	XSync(apc->display,false);
	return(true);
}

bool launcherCB(void *p,void* ud)
{
	launcherDataStruct	lds=launchersArray.at((long unsigned int)ud);
	std::size_t			found;
	std::string			command;
	std::string			args;
	std::string			str;
	std::string			ex=lds.exec;

	found=ex.find(std::string(" "));
	if(found!=std::string::npos)
		{
			command=ex.substr(0,found);
			args=ex.substr(found,-1);
		}
	else
		{
			command=ex;
			args="";
		}

	sendNotify("Launching ",lds.name);
	if(lds.inTerm==false)
		str=apc->globalLib->LFSTK_oneLiner("exec %S %S &\necho $!",command,args);
	else
		str=apc->globalLib->LFSTK_oneLiner("exec %S %S %S &\necho $!",prefs.LFSTK_getString("termcommand"),command,args);
	lds.pid=std::stoul(str);

	if(useTaskBar==true)
		updateTaskBar();

	return(true);
}

void addALAuncher(std::string fpath,launcherDataStruct *entry)
{
	std::map<unsigned long,std::vector<std::string>> lines;
	std::string										entryname="Desktop Entry";
	std::string										tmp;

	entry->inTerm=false;
	lines=LFSTK_UtilityClass::LFSTK_readFullDesktopFile(fpath);
	entry->name=LFSTK_UtilityClass::LFSTK_getFullEntry(entryname,"Name",lines);
	entry->icon=LFSTK_UtilityClass::LFSTK_getFullEntry(entryname,"Icon",lines);
	tmp=LFSTK_UtilityClass::LFSTK_getFullEntry(entryname,"Terminal",lines);
	if(tmp.compare("true")==0)
		entry->inTerm=true;
	tmp=LFSTK_UtilityClass::LFSTK_getFullEntry(entryname,"Exec",lines);
	tmp=LFSTK_UtilityClass::LFSTK_strReplaceAllStr(tmp,"%f","",true);
	tmp=LFSTK_UtilityClass::LFSTK_strReplaceAllStr(tmp,"%F","",true);
	tmp=LFSTK_UtilityClass::LFSTK_strReplaceAllStr(tmp,"%u","",true);
	tmp=LFSTK_UtilityClass::LFSTK_strReplaceAllStr(tmp,"%U","",true);
	entry->exec=tmp;
}

int addLaunchers(int x,int y,int grav)
{
	std::string		icon;
	int				xpos=x;
	int				ypos=y;
	windowInitStruct	*win;
	std::string		lc="#a0f0f0f0";
	std::string		pc="#60a0a0a0";
	std::string		ac="#60404040";
	std::string		iconpath;
	int				ww;
	int				sy=0;

	findlaunchers=new LFSTK_findClass;
	findlaunchers->LFSTK_setFindType(FILETYPE);
	findlaunchers->LFSTK_setDepth(1,1);
	findlaunchers->LFSTK_setFileTypes(".desktop");
	findlaunchers->LFSTK_setFullPath(true);
	findlaunchers->LFSTK_setIgnoreNavLinks(true);
	findlaunchers->LFSTK_findFiles(launchersDir.c_str(),false);
	findlaunchers->LFSTK_sortByName();

	win=apc->LFSTK_getDefaultWInit();
	win->windowType=apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_MENU"));
	win->decorated=false;
	win->level=ABOVEALL;

	apc->LFSTK_addWindow(win,"Context");
	launcherContextWC=apc->windows->back().window;
	launcherContextWC->LFSTK_setWindowColourName(NORMALCOLOUR,lc.c_str());
	for(int j=BUTTONLAUNCH;j<NOMOREBUTONS;j++)
		{
			contextButtons[j]=new LFSTK_buttonClass(launcherContextWC,contextLabelData[j],0,sy,GADGETWIDTH*2,24,NorthWestGravity);
			contextButtons[j]->LFSTK_setMouseCallBack(NULL,launcherContextCB,(void*)(long)(j));
			iconpath=launcherContextWC->globalLib->LFSTK_findThemedIcon("gnome",contextThemeIconData[j],"");
			contextButtons[j]->LFSTK_setImageFromPath(iconpath,LEFT,true);

			setGadgetDetails(contextButtons[j]);
			contextButtons[j]->LFSTK_setTile(NULL,0);
			contextButtons[j]->LFSTK_setFontString(prefs.LFSTK_getCString("font"),true);
			contextButtons[j]->LFSTK_setLabelAutoColour(true);
			contextButtons[j]->LFSTK_setGadgetColours(GADGETBG,lc,pc,ac,lc);
			contextButtons[j]->LFSTK_setGadgetColours(GADGETFG,lc,pc,ac,lc);

			sy+=GADGETHITE;
		}
	ww=contextButtons[0]->LFSTK_getTextRealWidth(contextLabelData[1]);
	launcherContextWC->LFSTK_resizeWindow(ww+contextButtons[0]->imageWidth+8,sy,true);
	launcherContextWC->LFSTK_setKeepAbove(true);
	tooltiptWC=new LFSTK_toolWindowClass(apc->display,dockWindow,apc->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DOCK")),0,0,1,1,"lfstkpopup",apc);
	tooltiptWC->LFSTK_setWindowColourName(NORMALCOLOUR,lc.c_str());

	ttLabel=new LFSTK_labelClass(tooltiptWC,"XXX",-1,-1,GADGETWIDTH*10,GADGETHITE,NorthWestGravity);
	ttLabel->pad=1;
	ttLabel->LFSTK_setGadgetColourPair(NORMALCOLOUR,"#ffffffff","black");
	ttLabel->LFSTK_setTile(NULL,0);

	tooltiptWC->LFSTK_moveWindow(-1000,-1000,true);
	tooltiptWC->LFSTK_showWindow();

	LFSTK_buttonClass	*bc=NULL;
	launcherDataStruct	lds;
	for(int l=0;l<findlaunchers->data.size();l++)
		{
 			addALAuncher(findlaunchers->data.at(l).path,&lds);
			lds.path=findlaunchers->data.at(l).path;
			lds.pid=0;
 
			bc=new LFSTK_buttonClass(dockWindow,"",xpos,normalY,iconWidth,iconHeight);
			bc->LFSTK_setContextWindow(launcherContextWC);

			if(dockGravity==PANELSOUTH)
				bc->contextWindowPos=CONTEXTABOVECENTRE;
			else
				bc->contextWindowPos=CONTEXTCENTRE;
			bc->userData=USERDATA(l);
			bc->LFSTK_setMouseCallBack(NULL,launcherCB,USERDATA(l));
			bc->LFSTK_setGadgetDropCallBack(gadgetDrop,USERDATA(l));
			bc->LFSTK_setMouseMoveCallBack(launcherEnterCB,launcherExitCB,USERDATA(l));
			bc->gadgetAcceptsDnD=true;
			if((lds.icon.empty()==false) && (desktopTheme.empty()==false))
				icon=apc->globalLib->LFSTK_findThemedIcon(desktopTheme,lds.icon,"");
			if(icon.empty()==false)
				bc->LFSTK_setImageFromPath(icon,LEFT,true);
			else
				bc->LFSTK_setImageFromPath(DATADIR "/pixmaps/command.png",LEFT,true);

			setGadgetDetails(bc);
			launchersArray.push_back(lds);
			xpos+=iconWidth+ICONSPACE;
		}
	return(xpos);
}
