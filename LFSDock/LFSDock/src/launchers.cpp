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
#include <stdio.h>
#include <string>
#include <glib.h>

#include "launchers.h"

LFSTK_windowClass		*popWindow=NULL;

bool moveCB(LFSTK_gadgetClass*p,void* ud)
{
	if(ud!=NULL)
		{
			XEvent			event;
			geometryStruct	geom;
			printf(">>>Mouse In %p<<<\n",(launcherList*)ud);
			LFSTK_labelClass	*label=NULL;
			label=((launcherList*)ud)->label;
popWindow->LFSTK_resizeWindow(((launcherList*)ud)->label->LFSTK_getTextRealWidth("This is a mouse enter callback"),GADGETHITE);
//popWindow->LFSTK_resizeWindow(100,GADGETHITE);
fprintf(stderr,"label=%s\n",((launcherList*)ud)->entry.name);
label->LFSTK_setLabel(((launcherList*)ud)->entry.name,true);
label->LFSTK_clearWindow();
			p->LFSTK_getGeomWindowRelative(&geom,apc->rootWindow);	
			popWindow->LFSTK_moveWindow(geom.x,geom.y-GADGETHITE,true);
			popWindow->LFSTK_showWindow();
			popWindow->LFSTK_clearWindow(true);
		}
	return(true);
}

bool exitCB(LFSTK_gadgetClass*p,void* ud)
{
	if(ud!=NULL)
		{
			printf(">>>Mouse Out %s<<<\n",(const char*)ud);
			popWindow->LFSTK_hideWindow();
		}
	return(true);
}

launcherList	*ll=NULL;
std::string	popUpString;

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

int addLaunchers(int x,int y,int grav,bool fromleft)
{
	char			*launchers;
	launcherList	*loopll;
	ll=NULL;
	char			*icon=NULL;
	int				xpos=x;
	int				ypos=y;
	int				width=0;
	int				height=0;
	int				thisgrav=grav;
	int				iconsize=16;
	int				maxwidth=0;
	int				sx,sy;

	asprintf(&launchers,"%s/launchers-DOCK",apc->configDir);
	ftw(launchers,launcherBuildCB,16);

	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);
	maxwidth=width;
	sx=xpos;
	sy=ypos;

	popWindow=new LFSTK_toolWindowClass(apc->display,mainwind,"_NET_WM_WINDOW_TYPE_MENU",0,0,200,100,"lfstkpopup",apc);
	//poplabel->LFSTK_setCairoFontDataParts("sB",20);
	//poplabel->LFSTK_setTile(NULL,0);

	//popWindow->LFSTK_resizeWindow(poplabel->LFSTK_getTextRealWidth("This is a mouse enter callback"),GADGETHITE);
	popWindow->LFSTK_setWindowColourName(NORMALCOLOUR,"#c0808080");



	loopll=ll;
	while(loopll!=NULL)
		{
			icon=NULL;
			loopll->bc=new LFSTK_buttonClass(mainwind,"",sx,sy,width,height,thisgrav);
			loopll->bc->LFSTK_setMouseCallBack(NULL,launcherCB,(void*)loopll);
			loopll->bc->LFSTK_setGadgetDropCallBack(gadgetDrop,(void*)loopll);
	
	loopll->label=new LFSTK_labelClass(popWindow,loopll->entry.name,0,0,GADGETWIDTH*8,GADGETHITE,WestGravity);
			loopll->bc->LFSTK_setMouseMoveCallBack(moveCB,exitCB,USERDATA(loopll));
fprintf(stderr,"name=>>%s<<\n",loopll->entry.name);
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
			if((grav==PANELNORTH) || (grav==PANELSOUTH))
				{
					if(fromleft==false)
						sx-=width;
					else
						sx+=width;
				}
			else
				sy+=height;
			maxwidth+=width;
		}
	free(launchers);

	return(maxwidth-width);
}
