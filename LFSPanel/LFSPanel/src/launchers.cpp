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

#include "launchers.h"

launcherList	*ll=NULL;

int launcherCB(const char *fpath,const struct stat *sb,int typeflag)
{
	FILE			*fp;
	char			buffer[512];
	char			*splitstr=NULL;
	menuEntryStruct	entry;
	char			*icon=NULL;
	launcherList	*newlist=NULL;
	launcherList	*looplist=NULL;
	char			*testpercent;

	if(typeflag!=FTW_F)
		return(0);

	entry.name=NULL;
	entry.exec=NULL;
	entry.inTerm=false;

	fp=fopen(fpath,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,512,fp))
				{
					if(buffer[strlen(buffer)-1]=='\n')
						buffer[strlen(buffer)-1]=0;
					splitstr=NULL;
					splitstr=strtok(buffer,"=");
					if(splitstr!=NULL)
						{
							if(strcmp(splitstr,"Name")==0)
								{
									splitstr=strtok(NULL,"=");
									entry.name=strdup(splitstr);
								}

							if(strcmp(splitstr,"Icon")==0)
								{
									splitstr=strtok(NULL,"=");
									icon=strdup(splitstr);
								}

							if(strcmp(splitstr,"Exec")==0)
								{
									splitstr=strtok(NULL,"=");
									testpercent=strchr(splitstr,'%');
									if(testpercent!=NULL)
										*testpercent=0;
									entry.exec=strdup(splitstr);
								}

							if(strcmp(splitstr,"Terminal")==0)
								{
									splitstr=strtok(NULL,"=");
									if(strcasecmp(splitstr,"true")==0)
										entry.inTerm=true;
									else
										entry.inTerm=false;
								}
						}
				}
			fclose(fp);
		}

	if((entry.name!=NULL) && (entry.exec!=NULL))
		{
			newlist=new launcherList;
			newlist->entry=entry;
			newlist->next=NULL;
			newlist->bc=NULL;
			newlist->icon=icon;
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
			if(icon!=NULL)
				free(icon);
		}
	return(0);
}

bool launcherCB(void *p,void* ud)
{
	launcherList	*launcher=(launcherList*)ud;
	char			*command;

	if(launcher==NULL)
		return(true);

	if(launcher->entry.inTerm==false)
		asprintf(&command,"%s &",launcher->entry.exec);
	else
		asprintf(&command,"%s %s &",terminalCommand,launcher->entry.exec);

	system(command);
	free(command);
	return(true);
}

int addLaunchers(int x,int y,int grav,bool fromleft)
{
	char			*launchers;
	launcherList	*loopll;
	ll=NULL;
	//const char		*themedicon=NULL;
	const char		*icon=NULL;
	int				xpos=x;
	int				ypos=y;
	int				width=0;
	int				height=0;
	int				thisgrav=grav;
	int				iconsize=16;
	int				maxwidth=0;
	int				sx,sy;

	asprintf(&launchers,"%s/.config/LFS/launchers-%s",getenv("HOME"),panelID);
	ftw(launchers,launcherCB,16);

	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);
	maxwidth=width;
	sx=xpos;
	sy=ypos;

	loopll=ll;
	while(loopll!=NULL)
		{
			loopll->bc=new LFSTK_buttonClass(mainwind,"",sx,sy,width,height,thisgrav);
			loopll->bc->LFSTK_setCallBack(NULL,launcherCB,(void*)loopll);
			if(loopll->icon!=NULL)
				icon=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,loopll->icon,"");
			if(icon!=NULL)
				loopll->bc->LFSTK_setImageFromPath(icon,LEFT,true);
			else
			//	loopll->bc->LFSTK_setIconFromPath(DATADIR "/pixmaps/command.png",iconsize);
				loopll->bc->LFSTK_setImageFromPath(DATADIR "/pixmaps/command.png",LEFT,true);
			loopll=loopll->next;
			if((grav==PANELNORTH) || (grav==PANELSOUTH))
				sx+=width;
			else
				sy+=height;
			maxwidth+=width;
		}
	return(maxwidth-width);
}
