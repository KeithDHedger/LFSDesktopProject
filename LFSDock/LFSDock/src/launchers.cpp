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
#include <glib.h>

#include "launchers.h"

LFSTK_buttonClass				*contextButtons[NOMOREBUTONS];
const char						*contextLabelData[]={"Launch","Remove From Dock","Run Prefs","TBD",NULL};
const char						*contextThemeIconData[]={"media-playback-start","list-remove","LFSTKPrefs","dialog-warning"};
std::vector<launcherDataStruct>	launchersArray;
LFSTK_findClass					*findlaunchers=NULL;

bool launcherEnterCB(LFSTK_gadgetClass*p,void* ud)
{
	if(p!=NULL)
		{
			std::string			label;
			infoDataStruct		ls;
			launcherDataStruct	lds=launchersArray.at((long unsigned int)ud);

			setGadgetPosition(p,true);
			popActionList->LFSTK_freeList();	
			ls.label=lds.name;
			ls.imageType=NOTHUMB;
			ls.imagePath;
			ls.userData=USERDATA(p);

			popActionList->LFSTK_appendToList(ls);
			popActionList->LFSTK_updateList();
			popActionList->LFSTK_moveGadget(-1,-1);
			popActionWindow->userData=USERDATA(LAUNCHER);
			popActionWindow->LFSTK_resizeWindow(popActionList->LFSTK_getListMaxWidth()-2,GADGETHITE-2);
			showhidetActionList(p,popActionWindow,popActionList);
			inSomeWindow=true;
		}
	return(true);
}

bool launcherExitCB(LFSTK_gadgetClass *p,void* ud)
{
	if(p!=NULL)
		{
			setGadgetPosition(p,false);
			inSomeWindow=false;
		}
	return(true);
}

bool launcherCB(void *p,void* ud)
{
	launcherDataStruct	lds=launchersArray.at((long unsigned int)ud);
	Window				win=None;
	std::string			ex=lds.exec;
	std::size_t			found;
	std::string			command;
	std::string			args;
	std::string			str;
	std::string			whch;
	propReturn			pr;

	showhidetActionList(NULL,popActionWindow,popActionList);

	if(p!=NULL)
		{
			win=getWindowByClass(lds.name);
			if(win!=None)
				{
					pr=apc->globalLib->LFSTK_getSingleProp(apc->display,apc->rootWindow,NET_ACTIVE_WINDOW,XA_WINDOW);
					if(pr.window==win)
						{
							XUnmapWindow(apc->display,win);
							return(true);
						}
					sendClientMessage(win,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
					return(true);
				}
			win=getWindowByPID(lds.pid);
			if(win!=None)
				{
					if(pr.window==win)
						{
							XUnmapWindow(apc->display,win);
							return(true);
						}
					sendClientMessage(win,"_NET_ACTIVE_WINDOW",0,0,0,0,0);
					return(true);
				}
		}

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

	whch=apc->globalLib->LFSTK_oneLiner(std::string("which '%s'"),command.c_str());

	sendNotify("Launching ",lds.name.c_str());

	if(lds.inTerm==false)
		str=apc->globalLib->LFSTK_oneLiner(std::string("exec %s %s &\necho $!"),whch.c_str(),args.c_str());
	else
		str=apc->globalLib->LFSTK_oneLiner(std::string("exec %s %s %s &\necho $!"),prefs.LFSTK_getCString("termcommand"),whch.c_str(),args.c_str());
	lds.pid=std::stoul(str,nullptr,0);
	if(useTaskBar==true)
		updateTaskBar();

	return(true);
}

void addALAuncher(const char *fpath,desktopFileStruct *entry)
{
	size_t		start_pos=0;
	std::string	from;
	std::string	str;
	bool			goodkey;
	GKeyFile		*kf=g_key_file_new();
	char			*execstring;

	entry->icon=NULL;
	entry->name=NULL;
	entry->exec=NULL;
	entry->inTerm=false;

	goodkey=g_key_file_load_from_file(kf,fpath,G_KEY_FILE_NONE,NULL);
	if(goodkey==true)
		{
			entry->name=g_key_file_get_string(kf,"Desktop Entry",G_KEY_FILE_DESKTOP_KEY_NAME,NULL);
			entry->icon=g_key_file_get_string(kf,"Desktop Entry",G_KEY_FILE_DESKTOP_KEY_ICON,NULL);
			entry->inTerm=g_key_file_get_boolean(kf,"Desktop Entry",G_KEY_FILE_DESKTOP_KEY_TERMINAL,NULL);
			execstring=g_key_file_get_string(kf,"Desktop Entry",G_KEY_FILE_DESKTOP_KEY_EXEC,NULL);
			str=execstring;
			from="%f";
			while((start_pos=str.find(from,start_pos))!=std::string::npos)
				str.replace(start_pos, from.length(),"");
			from="%U";
			start_pos=0;
			while((start_pos=str.find(from,start_pos))!=std::string::npos)
				str.replace(start_pos, from.length(),"");
			entry->exec=strdup(str.c_str());
			freeAndNull(&execstring);
		}
	g_key_file_free(kf);
}

int addLaunchers(int x,int y,int grav)
{
	char				*launchers;
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
	findlaunchers->LFSTK_setDepth(1,1);
	findlaunchers->LFSTK_setFileTypes(".desktop");
	findlaunchers->LFSTK_setFullPath(true);
	findlaunchers->LFSTK_findFiles(launchersDir.c_str(),false);
	findlaunchers->LFSTK_sortByName();

	win=new windowInitStruct;//TODO//
	win->app=apc;
	win->windowName="";
	win->loadVars=true;
	win->x=100;
	win->y=100;
	win->w=200;
	win->h=200;
	win->wc=dockWindow;
	win->windowType=apc->appAtomsHashed.at(apc->globalLib->prefs.LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_MENU"));
	win->app=apc;
	win->decorated=false;
	win->overRide=true;
	win->level=ABOVEALL;

	apc->LFSTK_addWindow(win,"");
	launcherContextWindow=apc->windows->back().window;
	launcherContextWindow->LFSTK_setWindowColourName(NORMALCOLOUR,lc.c_str());

	for(int j=BUTTONLAUNCH;j<NOMOREBUTONS;j++)
		{
			contextButtons[j]=new LFSTK_buttonClass(launcherContextWindow,contextLabelData[j],0,sy,GADGETWIDTH*2,24,NorthWestGravity);
			contextButtons[j]->LFSTK_setMouseCallBack(NULL,contextCB,(void*)(long)(j));
			iconpath=launcherContextWindow->globalLib->LFSTK_findThemedIcon("gnome",contextThemeIconData[j],"");
			contextButtons[j]->LFSTK_setImageFromPath(iconpath,LEFT,true);

			setGadgetDetails(contextButtons[j]);
			contextButtons[j]->LFSTK_setCairoFontDataParts("sB",20);
			contextButtons[j]->LFSTK_setTile(NULL,0);
			contextButtons[j]->LFSTK_setFontString(prefs.LFSTK_getCString(prefs.LFSTK_hashFromKey("font")),true);
			contextButtons[j]->LFSTK_setLabelAutoColour(true);
			contextButtons[j]->LFSTK_setGadgetColours(GADGETBG,lc,pc,ac,lc);
			contextButtons[j]->LFSTK_setGadgetColours(GADGETFG,lc,pc,ac,lc);

			sy+=GADGETHITE;
		}
	ww=contextButtons[0]->LFSTK_getTextRealWidth(contextLabelData[1]);
	launcherContextWindow->LFSTK_resizeWindow(ww+contextButtons[0]->imageWidth+8,sy,true);

	desktopFileStruct	entry;
	LFSTK_buttonClass	*bc=NULL;
	launcherDataStruct	lds;
	for(int l=0;l<findlaunchers->data.size();l++)
		{
			entry.icon=NULL;
			entry.name=NULL;
			entry.exec=NULL;
			entry.inTerm=false;

 			addALAuncher(findlaunchers->data.at(l).path.c_str(),&entry);

			lds.name=entry.name;
			lds.exec=entry.exec;
			lds.inTerm=entry.inTerm;
			lds.path=findlaunchers->data.at(l).path;
			lds.pid=0;
 
			bc=new LFSTK_buttonClass(dockWindow,"",xpos,normalY,iconWidth,iconHeight);
			bc->LFSTK_setContextWindow(launcherContextWindow);
			if(dockGravity==PANELSOUTH)
				bc->contextWindowPos=CONTEXTABOVECENTRE;
			else
				bc->contextWindowPos=CONTEXTCENTRE;
			bc->userData=USERDATA(l);
			bc->LFSTK_setMouseCallBack(NULL,launcherCB,USERDATA(l));
			bc->LFSTK_setGadgetDropCallBack(gadgetDrop,USERDATA(l));
			bc->LFSTK_setMouseMoveCallBack(launcherEnterCB,launcherExitCB,USERDATA(l));
			bc->gadgetAcceptsDnD=true;
			if((entry.icon!=NULL) && (desktopTheme.length()>0))
				icon=apc->globalLib->LFSTK_findThemedIcon(desktopTheme,entry.icon,"");
			if(icon.length()>0)
				bc->LFSTK_setImageFromPath(icon,LEFT,true);
			else
				bc->LFSTK_setImageFromPath(DATADIR "/pixmaps/command.png",LEFT,true);

			setGadgetDetails(bc);
			launchersArray.push_back(lds);
			g_free(entry.name);
			g_free(entry.exec);
			g_free(entry.icon);

			xpos+=iconWidth;
		}

	return(xpos);
}
