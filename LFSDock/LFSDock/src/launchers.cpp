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

//LFSTK_windowClass	*contextWindow=NULL;
LFSTK_buttonClass	*contextButtons[NOMOREBUTONS];

const char			*contextLabelData[]={"Launch","Remove From Dock","TBD","TBD",NULL};
const char			*contextThemeIconData[]={"media-playback-start","list-remove","dialog-warning","dialog-warning"};

void addALAuncher(const char *fpath,menuEntryStruct	*entry)
{
	size_t		start_pos=0;
	std::string	from;
	std::string	str;
	bool		goodkey;
	GKeyFile	*kf=g_key_file_new();
	char		*execstring;

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
			free(execstring);
		}
	g_key_file_free(kf);
}

int launcherBuildCB(const char *fpath,const struct stat *sb,int typeflag)
{
	menuEntryStruct	entry;
	char			*icon=NULL;
	launcherList	*newlist=NULL;
	launcherList	*looplist=NULL;

	if(typeflag!=FTW_F)
		return(0);

	entry.icon=NULL;
	entry.name=NULL;
	entry.exec=NULL;
	entry.inTerm=false;

	addALAuncher(fpath,&entry);

	if((entry.name!=NULL) && (entry.exec!=NULL))
		{
			newlist=new launcherList;
			newlist->entry=entry;
			newlist->next=NULL;
			newlist->bc=NULL;
			newlist->desktopFilePath=fpath;
			newlist->icon=entry.icon;
			if(ll!=NULL)
				{
					looplist=ll;
					while(looplist->next!=NULL)
						looplist=looplist->next;
					looplist->next=newlist;
				}
			else
				ll=newlist;
		}
	else
		{
			if(entry.name!=NULL)
				free(entry.name);
			if(entry.exec!=NULL)
				free(entry.exec);
			if(entry.icon!=NULL)
				free(entry.icon);
			if(icon!=NULL)
				free(icon);
		}
	return(0);
}

int addLaunchers(int x,int y,int grav)
{
	char				*launchers;
	launcherList		*loopll=NULL;
	char				*icon=NULL;
	int				xpos=x;
	int				ypos=y;
	windowInitStruct	*win;
	std::string		lc="#a0f0f0f0";
	std::string		pc="#60a0a0a0";
	std::string		ac="#60404040";
	char				*iconpath=NULL;
	int				ww;
	int				sy=0;

	asprintf(&launchers,"%s/launchers-DOCK",apc->configDir.c_str());
	ftw(launchers,launcherBuildCB,16);

	popWindow=new LFSTK_toolWindowClass(apc->display,mainwind,"_NET_WM_WINDOW_TYPE_MENU",0,0,100,100,"lfstkpopup",apc);
	popLabel=new LFSTK_labelClass(popWindow,"ANAME",0,0,GADGETWIDTH*8,GADGETHITE,WestGravity);
	popLabel->LFSTK_setCairoFontDataParts("sB",20);
	popLabel->LFSTK_setTile(NULL,0);
	popWindow->LFSTK_setWindowColourName(NORMALCOLOUR,"#c0808080");

	win=new windowInitStruct;
	win->app=apc;
	win->name="";
	win->loadVars=true;
	win->x=100;
	win->y=100;
	win->w=200;
	win->h=200;
	win->wc=mainwind;
	win->windowType="_NET_WM_WINDOW_TYPE_DOCK";
	win->decorated=false;
	win->overRide=true;
	win->level=ABOVEALL;

	apc->LFSTK_addWindow(win,"hello");
	contextWindow=apc->windows->back().window;
	contextWindow->LFSTK_setWindowColourName(NORMALCOLOUR,lc.c_str());

	for(int j=BUTTONLAUNCH;j<NOMOREBUTONS;j++)
		{
			contextButtons[j]=new LFSTK_buttonClass(contextWindow,contextLabelData[j],0,sy,GADGETWIDTH*2,24,NorthWestGravity);
			contextButtons[j]->LFSTK_setMouseCallBack(NULL,contextCB,(void*)(long)(j+1));
			iconpath=contextWindow->globalLib->LFSTK_findThemedIcon("gnome",contextThemeIconData[j],"");
			contextButtons[j]->LFSTK_setImageFromPath(iconpath,LEFT,true);

			setGadgetDetails(contextButtons[j]);
			contextButtons[j]->LFSTK_setCairoFontDataParts("sB",20);
			contextButtons[j]->LFSTK_setTile(NULL,0);
			contextButtons[j]->LFSTK_setFontString(prefs.LFSTK_getCString(prefs.LFSTK_hashFromKey("font")),true);
			contextButtons[j]->LFSTK_setLabelAutoColour(true);
			contextButtons[j]->LFSTK_setGadgetColours(GADGETBG,lc,pc,ac,lc);
			contextButtons[j]->LFSTK_setGadgetColours(GADGETFG,lc,pc,ac,lc);

			free(iconpath);
			sy+=GADGETHITE;
		}
	ww=contextButtons[0]->LFSTK_getTextRealWidth(contextLabelData[1]);
	contextWindow->LFSTK_resizeWindow(ww+contextButtons[0]->imageWidth+8,sy,true);

	loopll=ll;
	while(loopll!=NULL)
		{
			icon=NULL;
			loopll->bc=new LFSTK_buttonClass(mainwind,"",xpos,ypos,iconSize,iconSize);
			loopll->bc->LFSTK_setContextWindow(contextWindow);
			loopll->bc->contextWindowPos=CONTEXTABOVECENTRE;
			loopll->bc->userData=loopll;
			loopll->bc->LFSTK_setMouseCallBack(NULL,launcherCB,(void*)loopll);
			loopll->bc->LFSTK_setGadgetDropCallBack(gadgetDrop,(void*)loopll);
			loopll->bc->LFSTK_setMouseMoveCallBack(enterCB,exitCB,USERDATA(loopll));
			loopll->bc->gadgetAcceptsDnD=true;

			if((loopll->icon!=NULL) && (desktopTheme!=NULL))
				icon=apc->globalLib->LFSTK_findThemedIcon(desktopTheme,loopll->icon,"");
			if(icon!=NULL)
				loopll->bc->LFSTK_setImageFromPath(icon,LEFT,true);
			else
				loopll->bc->LFSTK_setImageFromPath(DATADIR "/pixmaps/command.png",LEFT,true);

			setGadgetDetails(loopll->bc);

			if(icon!=NULL)
				free(icon);
			loopll=loopll->next;
			xpos+=iconSize;
		}
	free(launchers);

	return(xpos);
}
