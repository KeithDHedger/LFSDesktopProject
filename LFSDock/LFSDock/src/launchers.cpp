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

	asprintf(&launchers,"%s/launchers-DOCK",apc->configDir.c_str());
	ftw(launchers,launcherBuildCB,16);

	popWindow=new LFSTK_toolWindowClass(apc->display,mainwind,"_NET_WM_WINDOW_TYPE_MENU",0,0,100,100,"lfstkpopup",apc);
	popLabel=new LFSTK_labelClass(popWindow,"ANAME",0,0,GADGETWIDTH*8,GADGETHITE,WestGravity);
	popLabel->LFSTK_setCairoFontDataParts("sB",20);
	popLabel->LFSTK_setTile(NULL,0);
	popWindow->LFSTK_setWindowColourName(NORMALCOLOUR,"#c0808080");

	loopll=ll;
	while(loopll!=NULL)
		{
			icon=NULL;
			loopll->bc=new LFSTK_buttonClass(mainwind,"",xpos,ypos,iconSize,iconSize);
			loopll->bc->LFSTK_setMouseCallBack(NULL,launcherCB,(void*)loopll);
			loopll->bc->LFSTK_setGadgetDropCallBack(gadgetDrop,(void*)loopll);
	
			loopll->bc->LFSTK_setMouseMoveCallBack(enterCB,exitCB,USERDATA(loopll));
			loopll->bc->gadgetAcceptsDnD=true;
//fprintf(stderr,"name=>>%s x=%i<<\n",loopll->entry.name,xpos);

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
