/*
 *
 * ©K. D. Hedger. Fri 31 Jul 17:35:44 BST 2015 kdhedger68713@gmail.com

 * This file (appMenu.cpp) is part of LFSWManager.

 * LFSWManager is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSWManager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWManager.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ftw.h>
#include <fnmatch.h>

#include "appmenu.h"

const char				*myCats[]= {"Settings","Utility","Development","Education","Graphics","Network","AudioVideo","Office","Shell","Game","System",NULL};
const char				*catImageNames[]={"preferences-desktop","applications-utilities","applications-development","applications-science","applications-graphics","applications-internet","applications-multimedia","applications-office","applications-engineering","applications-games","applications-system",NULL};

LFSTK_menuButtonClass	*appButton=NULL;
menuItemStruct			*catagoryMenus;
menuItemStruct			*catagorySubMenus[MAXCATS];
int						subMenusCnt[MAXCATS];
int						catPtr[MAXCATS];
const char				*currentCatName=NULL;
int						catagoryCnt=0;
int						catcnt=0;

bool callback(void *p,void* ud)
{
	menuEntryStruct	*menu;
	char			*command;

	if(ud==NULL)
		return(true);;

	if((long)((menuItemStruct*)ud)->userData==-1)
		{
			system("lfsabout &");
			return(true);
		}
	
	menu=(menuEntryStruct*)((menuItemStruct*)ud)->userData;
	if(menu==NULL)
		return(true);;

	if(menu->inTerm==false)
		asprintf(&command,"%s &",menu->exec);
	else
		asprintf(&command,"%s %s &",terminalCommand,menu->exec);

	system(command);
	free(command);
	return(true);
}

int ftwCallback(const char *fpath,const struct stat *sb,int typeflag)
{
	char	command[512];
	char	*splitstr=NULL;
	FILE	*fp=NULL;
	bool	interm=false;
	int		mycatcnt=0;
	bool	gotcat;
	bool	gotname;
	bool	goticon;
	bool	gotexec;
	char	*holdname=NULL;
	char	*holdicon=NULL;
	char	*holdexec=NULL;
	bool	nodisplay=false;
	char	*testpercent=NULL;

	if(typeflag!=FTW_F)
		return(0);

	sprintf(command,"cat %s",fpath);
	gotcat=false;
	gotname=false;
	goticon=false;
	gotexec=false;
	interm=false;
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			while(fgets(command,512,fp))
				{
					if(command[strlen(command)-1]=='\n')
						command[strlen(command)-1]=0;
					splitstr=NULL;
					splitstr=strtok(command,"=");
					if(splitstr!=NULL)
						{
							if(strncmp(splitstr,"Categories",10)==0)
								{
									mycatcnt=0;
									splitstr=strtok(NULL,"=");
									while(myCats[mycatcnt]!=NULL)
										{
											if(strcasestr(splitstr,myCats[mycatcnt])!=NULL)
												{
													gotcat=true;
													break;
												}
											mycatcnt++;
										}
									}

							if(strcmp(splitstr,"Name")==0)
								{
									splitstr=strtok(NULL,"=");
									gotname=true;
									holdname=strdup(splitstr);
								}

							if(strcmp(splitstr,"Icon")==0)
								{
									splitstr=strtok(NULL,"=");
									holdicon=strdup(splitstr);
								}
							if(strcmp(splitstr,"Exec")==0)
								{
									splitstr=strtok(NULL,"=");
									testpercent=strchr(splitstr,'%');
									if(testpercent!=NULL)
										*testpercent=0;
									holdexec=strdup(splitstr);
								}
							if(strcmp(splitstr,"NoDisplay")==0)
								{
									splitstr=strtok(NULL,"=");
									if(strcasecmp(splitstr,"true")==0)
										nodisplay=true;
								}
							if(strcmp(splitstr,"Terminal")==0)
								{
									splitstr=strtok(NULL,"=");
									if(strcasecmp(splitstr,"true")==0)
										interm=true;
									else
										interm=false;
								}
						}
				}

			if((gotcat==true) && (nodisplay==false))
				{
					menuEntryStruct	*ms=new menuEntryStruct;

					catagorySubMenus[catPtr[mycatcnt]][subMenusCnt[catPtr[mycatcnt]]].userData=(void*)ms;
					if(gotname==true)
						{
							catagorySubMenus[catPtr[mycatcnt]][subMenusCnt[catPtr[mycatcnt]]].label=holdname;
							ms->name=holdname;
						}

					if(holdicon!=NULL)
						{
							const char *iconpath=NULL;
							iconpath=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,holdicon,"");
							free(holdicon);
							holdicon=NULL;
							if(iconpath!=NULL)
								{
									catagorySubMenus[catPtr[mycatcnt]][subMenusCnt[catPtr[mycatcnt]]].imagePath=(char*)strdup(iconpath);
								}
							else
								{
									catagorySubMenus[catPtr[mycatcnt]][subMenusCnt[catPtr[mycatcnt]]].imagePath=NULL;
								}
						}

					if(holdexec!=NULL)
						ms->exec=holdexec;
					if(interm==true)
						ms->inTerm=true;
					else
						ms->inTerm=false;
					subMenusCnt[catPtr[mycatcnt]]++;
				}

			pclose(fp);
		}
	return(0);
}

void sortEntries(void)
{
	bool			flag=true;
	menuItemStruct	thold;

	for(int catagory=0;catagory<catagoryCnt;catagory++)
		{
			flag=true;
			while(flag==true)
				{
					flag=false;
					for(int cnt=0;cnt<subMenusCnt[catagory]-1;cnt++)
						{
							if(strcmp(catagorySubMenus[catagory][cnt].label,catagorySubMenus[catagory][cnt+1].label)>0)
								{
									flag=true;
									thold=catagorySubMenus[catagory][cnt+1];
									catagorySubMenus[catagory][cnt+1]=catagorySubMenus[catagory][cnt];
									catagorySubMenus[catagory][cnt]=thold;
									break;
								}
						}
				}
		}
}

void addEntries(void)
{
	for(int j=0;j<catagoryCnt;j++)
		{
			subMenusCnt[j]=0;
			for(int k=0;k<MAXENTRYS;k++)
				{
					catagorySubMenus[j][k].label=NULL;
					catagorySubMenus[j][k].userData=NULL;
					catagorySubMenus[j][k].bc=NULL;
					catagorySubMenus[j][k].subMenus=NULL;
					catagorySubMenus[j][k].imagePath=NULL;
				}
		}

	ftw("/usr/share/applications",ftwCallback,16);
	for(int j=0;j<MAXCATS;j++)
		{
			catagoryMenus[j].subMenus=catagorySubMenus[j];
			catagoryMenus[j].subMenuCnt=subMenusCnt[j];
		}
	sortEntries();
}

int ftwCatCallback(const char *fpath,const struct stat *sb,int typeflag)
{
	FILE	*fp=NULL;
	char	command[512];
	bool	gotcat;
	bool	gotnoshow;
	char	*splitstr=NULL;
	bool	foundshow;

	if(typeflag!=FTW_F)
		return(0);

	fp=fopen(fpath,"r");
	if(fp!=NULL)
		{
			gotnoshow=false;
			gotcat=false;
			foundshow=false;
			while(fgets(command,512,fp))
				{
					if(command[strlen(command)-1]=='\n')
						command[strlen(command)-1]=0;

					splitstr=NULL;
					splitstr=strtok(command,"=");
					if(splitstr!=NULL)
						{
							if(strcmp(splitstr,"Categories")==0)
								{
									splitstr=strtok(NULL,"=");
									if(strcasestr(splitstr,currentCatName)!=NULL)
										gotcat=true;
								}

							if(strcmp(splitstr,"NoDisplay")==0)
								{
									foundshow=true;
									splitstr=strtok(NULL,"=");
									if(strcasecmp(splitstr,"true")==0)
										gotnoshow=true;
								}
						}
					if((foundshow==true) && (gotcat==true))
						break;
				}
			fclose(fp);
		}
	else
		return(0);
	if((gotcat==true) && (gotnoshow==false))
		return(1);
	return(0);
}

void addCatagories(void)
{
	const char	*iconpath=NULL;

	catagoryCnt=0;
	catcnt=0;
	catagoryMenus=new menuItemStruct[MAXCATS];
	while(myCats[catcnt]!=NULL)
		{
			currentCatName=myCats[catcnt];
			if(ftw("/usr/share/applications",ftwCatCallback,16)==1)
				{
					catagorySubMenus[catagoryCnt]=new menuItemStruct[MAXENTRYS];
					catagoryMenus[catagoryCnt].label=myCats[catcnt];
					catagoryMenus[catagoryCnt].userData=NULL;
					catagoryMenus[catagoryCnt].bc=NULL;
					catagoryMenus[catagoryCnt].subMenus=NULL;
					catagoryMenus[catagoryCnt].subMenuCnt=0;
					catPtr[catcnt]=catagoryCnt;
					iconpath=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,catImageNames[catcnt],"");
					if(iconpath!=NULL)
						{
							catagoryMenus[catagoryCnt].imagePath=(char*)strdup(iconpath);
						}

					catagoryCnt++;
				}
			catcnt++;
		}
}

void addExtras(void)
{
	const char	*iconpath=NULL;

	catagoryMenus[catagoryCnt].label="--";
	catagoryMenus[catagoryCnt].userData=NULL;
	catagoryMenus[catagoryCnt].bc=NULL;
	catagoryMenus[catagoryCnt].subMenus=NULL;
	catagoryMenus[catagoryCnt].subMenuCnt=0;
	catagoryMenus[catagoryCnt].imagePath=NULL;
	catagoryCnt++;

	catagoryMenus[catagoryCnt].label="About LFS Desktop";
	catagoryMenus[catagoryCnt].userData=(void*)-1;
	catagoryMenus[catagoryCnt].bc=NULL;
	catagoryMenus[catagoryCnt].subMenus=NULL;
	catagoryMenus[catagoryCnt].subMenuCnt=0;
	catagoryMenus[catagoryCnt].imagePath=NULL;

	iconpath=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"help-about","");

	if(iconpath!=NULL)
		{
			catagoryMenus[catagoryCnt].imagePath=(char*)strdup(iconpath);
		}
	catagoryCnt++;
}

int addAppmenu(int x,int y,int grav,bool fromleft)
{
	int maxwid=0;
	int	catnum=0;
	int	retval;
	int	xpos=x;
	int	ypos=y;
	int	width=0;
	int	height=0;
	int	thisgrav=grav;
	int	iconsize=16;

	if(appButton!=NULL)
		{
			printError("Duplicate app menu");
			return(0);
		}

	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);

	appButton=new LFSTK_menuButtonClass(mainwind,"",xpos,ypos,width,height,thisgrav);
	appButton->LFSTK_setImageFromPath("/usr/share/pixmaps/LFSTux.png",LEFT,true);
	addCatagories();
	addEntries();
	addExtras();
	appButton->LFSTK_addMenus(catagoryMenus,catagoryCnt);
	XMapWindow(mainwind->display,appButton->LFSTK_getWindow());
	appButton->LFSTK_setCallBack(NULL,callback,NULL);
	appButton->LFSTK_clearWindow();
	return(width);
}
