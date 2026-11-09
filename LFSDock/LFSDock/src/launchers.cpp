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

LFSTK_buttonClass				*contextButtons[NOMORECONTEXTMENUS];
std::vector<launcherDataStruct>	launchersArray;
LFSTK_findClass					*findlaunchers=NULL;
LFSTK_gadgetClass				*currentLauncher=NULL;
LFSTK_windowClass				*launcherContextWC=NULL;

std::string						extraArgsForLaunch;

bool launcherContextCB(void *p,void* ud)
{
	int					winnum;
	LFSTK_windowClass	*lwc=static_cast<LFSTK_gadgetClass*>(p)->wc;
	long unsigned int	whatbutton=(long unsigned int)ud;
	tooltiptWC->LFSTK_moveWindow(-1000,1000,true);
	winnum=lwc->app->LFSTK_findWindow(lwc);
	lwc->app->windows->at(winnum).loopFlag=false;

	dockContextCallback(p,ud);

	return(true);
}

bool gadgetDrop1(void *lwc,propertyStruct *data,void* ud)
{
	std::string			c;
	LFSTK_buttonClass	*bc=static_cast<LFSTK_buttonClass*>(lwc);
	launcherDataStruct	lds=launchersArray.at((long unsigned int)bc->userData);

	c=LFSTK_UtilityClass::LFSTK_strStrip((const char*)data->data);
	extraArgsForLaunch="";

	std::vector<std::string>	ss=LFSTK_UtilityClass::LFSTK_strTok(c,"\n");

	for(std::string str:ss)
		{
			if(str.substr(0,7)=="file://")
				{
					str.replace(0,7,"");
					str=LFSTK_UtilityClass::LFSTK_strStrip(str);
					extraArgsForLaunch+=" \""+str+"\"";
				}
		}

	if(extraArgsForLaunch.empty()==false)
		launcherCB(bc,ud);

	return(true);
}

bool launcherEnterCB(LFSTK_gadgetClass* p,void* ud)
{
	launcherDataStruct	lds=launchersArray.at((long unsigned int)ud);
	showToolTip(p,lds.name);
	noshow=true;
	return(true);
}

bool launcherExitCB(LFSTK_gadgetClass* p,void* ud)
{
	if(moveGadget==true)
		setGadgetPosition(p,false);

	tooltiptWC->LFSTK_hideWindow();
	noshow=false;
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

	args=args+extraArgsForLaunch;
	extraArgsForLaunch="";

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

	for(int j=CONTEXTMENULAUNCH;j<NOMORECONTEXTMENUS;j++)
		{
			contextButtons[j]=new LFSTK_buttonClass(launcherContextWC,labelData[j],0,sy,GADGETWIDTH*2,24,NorthWestGravity);
			contextButtons[j]->LFSTK_setMouseCallBack(NULL,launcherContextCB,(void*)(long)(j));
			iconpath=launcherContextWC->globalLib->LFSTK_findThemedIcon("gnome",themeIconData[j],"");
			contextButtons[j]->LFSTK_setImageFromPath(iconpath,LEFT,true);
			setGadgetDetails(contextButtons[j]);
			contextButtons[j]->LFSTK_setLabelGravity(LEFT);
			contextButtons[j]->LFSTK_setImageGravity(LEFT);
			contextButtons[j]->LFSTK_setTile(NULL,0);
			contextButtons[j]->LFSTK_setFontString(prefs.LFSTK_getCString("font"),true);
			contextButtons[j]->LFSTK_setLabelAutoColour(true);
			contextButtons[j]->LFSTK_setGadgetColours(GADGETBG,lc,pc,ac,lc);
			contextButtons[j]->LFSTK_setGadgetColours(GADGETFG,lc,pc,ac,lc);

			sy+=GADGETHITE;
		}
	ww=contextButtons[0]->LFSTK_getTextRealWidth(labelData[CONTEXTMENUREMOVE]);
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
			bc=new LFSTK_buttonClass(dockWindow,"",xpos,0,iconWidth,dockWindowHeight);
			bc->LFSTK_setContextWindow(launcherContextWC);

			if(dockGravity==PANELSOUTH)
				bc->contextWindowPos=CONTEXTABOVECENTRE;
			else
				bc->contextWindowPos=CONTEXTCENTRE;

			bc->userData=USERDATA(l);
			bc->userStringData="XXX";
			bc->LFSTK_setGadgetDropCallBack(gadgetDrop1,USERDATA(l));
			bc->gadgetAcceptsDnD=true;

//EnterNotify
			bc->LFSTK_connectCB(EnterNotify,[bc](std::string msg) {launcherEnterCB(bc,bc->userData);});
//LeaveNotify
			bc->LFSTK_connectCB(LeaveNotify,[bc](std::string msg) {launcherExitCB(bc,bc->userData);});
//clicks
			bc->LFSTK_connectCB(ButtonPress,[bc](std::string msg)
				{
					if(bc->currentButton==Button3)
						launcherExitCB(bc,NULL);
					if(bc->currentButton==Button1)
						{
							launcherCB(bc,bc->userData);
							launcherExitCB(bc,NULL);
						}		
				});

			if((lds.icon.empty()==false) && (desktopTheme.empty()==false))
				icon=apc->globalLib->LFSTK_findThemedIcon(desktopTheme,lds.icon,"");
			if(icon.empty()==false)
				bc->LFSTK_setImageFromPath(icon,BOTTOM,true);
			else
				bc->LFSTK_setImageFromPath(DATADIR "/pixmaps/command.png",CENTRE,true);

			setGadgetDetails(bc);
			lds.gadget=bc;
			launchersArray.push_back(lds);
			xpos+=iconWidth+ICONSPACE;
			setGadgetPosition(bc,false);
		}
	return(xpos);
}
