/*
 *
 * ©K. D. Hedger. Fri 31 Jul 17:35:44 BST 2015 keithdhedger@gmail.com

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
#include <string.h>

#include <fstream> 
#include <regex>


#include "appmenu.h"

const char			*myCats[]= {"Settings","Utility","Development","Electronics","Education","Graphics","Network","AudioVideo","Office","Shell","Game","System",NULL};
const char			*catImageNames[]={"preferences-desktop","applications-utilities","applications-development","applications-engineering","applications-science","applications-graphics","applications-internet","applications-multimedia","applications-office","applications-engineering","applications-games","applications-system",NULL};

LFSTK_findClass		*find;
LFSTK_buttonClass	*appButton=NULL;
menuStruct			**catagoryMenus;
LFSTK_menuClass		*appMenu=NULL;

int					catPtr[MAXCATS];
const char			*currentCatName=NULL;
int					catagoryCnt=0;

bool buttonCB(void *p,void* ud)
{
	appMenu->LFSTK_showMenu();
	return(true);
}

bool menuCB(void *p,void* ud)
{
	char	*command;
	int		entry=(int)(long)ud;

	if(entry==-1)
		{
			system("lfsabout &");
			return(true);
		}

	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();

	if(entrydata.at(entry)->inTerm==false)
		asprintf(&command,"%s &",entrydata.at(entry)->exec);
	else
		asprintf(&command,"%s %s &",prefs.LFSTK_getCString("termcommand"),entrydata.at(entry)->exec);

	system(command);
	free(command);

	return(true);
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

std::map<int,menuEntryStruct*>	entrydata;

void addDesktopFiles(int catnum,const char *catname)//TODO//
{
	std::ifstream		file;
	std::stringstream	ss;
	std::string			cat=".*Categorie.*";
	std::regex			exp;
	const std::regex	expname("Name=(.*)");
	const std::regex	expicon("Icon=([^[:space:]]*).*");
	const std::regex	expexec("Exec=(.*)");
	const std::regex	expnodisplay("NoDisplay=([^[:space:]]*).*");
	const std::regex	expterm("Terminal=true");
	std::vector<int>	fname;
	std::smatch			m;
	char				*iconpath;
	char				*homeapps;

	asprintf(&homeapps,"%s/.local/share/applications",getenv("HOME"));
	find->LFSTK_setFindType(FILETYPE);
	find->LFSTK_setFullPath(true);
	find->LFSTK_setIgnoreBroken(false);
	find->LFSTK_setSort(true);
	find->LFSTK_setIgnoreNavLinks(true);
	find->LFSTK_setFileTypes(".desktop");	
	find->LFSTK_findFiles("/usr/share/applications",false);
	find->LFSTK_findFiles(homeapps,true);
	find->LFSTK_sortByPath();

	int	numfiles=find->LFSTK_getDataCount();

	for(int j=0;j<catagoryCnt;j++)
		{
			cat=".*Categorie.*";
			cat+=catagoryMenus[j]->label;
			exp=cat;
			fname.clear();
			for(int entries=0;entries<numfiles;entries++)
				{
					ss.str("");
					file.open(find->data[entries].path.c_str(),std::ifstream::in);
					ss<<file.rdbuf();
					file.close();
					if(std::regex_search(ss.str(),exp))
						{
							menuEntryStruct *me=new menuEntryStruct;
							std::string holsss=ss.str();
							//name
							std::regex_search(holsss,m,expname,std::regex_constants::match_not_eol|std::regex_constants::match_not_bol );
							me->name=strdup(m.str(1).c_str());
							//exec
							std::regex_search(holsss,m,expexec,std::regex_constants::match_not_eol|std::regex_constants::match_not_bol);
							me->exec=strdup(m.str(1).c_str());
							//interm
							if(std::regex_search(holsss,m,expterm,std::regex_constants::match_not_eol|std::regex_constants::match_not_bol))
								me->inTerm=true;
							else
								me->inTerm=false;
							//icon
							if(std::regex_search(holsss,m,expicon,std::regex_constants::match_not_eol|std::regex_constants::match_not_bol))
								me->icon=strdup(m.str(1).c_str());
							else
								me->icon=strdup("empty");
							entrydata[entries]=me;
							fname.push_back(entries);
						}
				}

			if(fname.size()>0)
				{
					catagoryMenus[j]->subMenus=new menuStruct*[fname.size()];
					catagoryMenus[j]->hasSubMenu=true;
					catagoryMenus[j]->subMenuCnt=fname.size();
					for(int k=0;k<fname.size();k++)
						{
							catagoryMenus[j]->subMenus[k]=new menuStruct;
							catagoryMenus[j]->subMenus[k]->label=strdup(entrydata.at(fname[k])->name);
							catagoryMenus[j]->subMenus[k]->userData=USERDATA(fname[k]);
							iconpath=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,entrydata.at(fname[k])->icon,"");
							if(iconpath!=NULL)
								{
									catagoryMenus[j]->subMenus[k]->imageType=FILETHUMB;
									catagoryMenus[j]->subMenus[k]->data.imagePath=iconpath;
								}
						}
					}
		}
	free(homeapps);
}

void addCatagories(void)
{
	const char	*iconpath=NULL;
	int			catcnt=0;

	catagoryCnt=0;
	catcnt=0;
	catagoryMenus=new menuStruct*[MAXCATS];
	while(myCats[catcnt]!=NULL)
		{
			currentCatName=myCats[catcnt];
			if(ftw("/usr/share/applications",ftwCatCallback,16)==1)
				{
					catagoryMenus[catagoryCnt]=new menuStruct;
					catagoryMenus[catagoryCnt]->label=strdup(myCats[catcnt]);
					catagoryMenus[catagoryCnt]->userData=USERDATA(catcnt);
					iconpath=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,catImageNames[catcnt],"");
					if(iconpath!=NULL)
						{
							catagoryMenus[catagoryCnt]->imageType=FILETHUMB;
							catagoryMenus[catagoryCnt]->data.imagePath=strdup(iconpath);
						}
					catPtr[catcnt]=catagoryCnt;
					catagoryCnt++;
				}
			catcnt++;
		}
	addDesktopFiles(catagoryCnt,myCats[catagoryCnt]);
}

void addExtras(void)
{
	char	*iconpath=NULL;

	catagoryMenus[catagoryCnt]=new menuStruct;
	catagoryMenus[catagoryCnt]->label=strdup("--");
	catagoryMenus[catagoryCnt]->userData=NULL;
	catagoryCnt++;

	catagoryMenus[catagoryCnt]=new menuStruct;
	catagoryMenus[catagoryCnt]->label=strdup("LFS About");
	catagoryMenus[catagoryCnt]->userData=(void*)-1;
	iconpath=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"help-about","");
	if(iconpath!=NULL)
		{
			catagoryMenus[catagoryCnt]->data.imagePath=iconpath;
			catagoryMenus[catagoryCnt]->imageType=FILETHUMB;
		}
	catagoryCnt++;

}

int addAppmenu(int x,int y,int grav,bool fromleft)
{
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

	find=new LFSTK_findClass;
	setSizes(&xpos,&ypos,&width,&height,&iconsize,&thisgrav,fromleft);

	appButton=new LFSTK_buttonClass(mainwind,"",xpos,ypos,width,height,thisgrav);
	appButton->LFSTK_setImageFromPath("/usr/share/pixmaps/LFSTux.png",LEFT,true);
	appButton->LFSTK_setMouseCallBack(NULL,buttonCB,NULL);
	appMenu=new LFSTK_menuClass(mainwind,xpos,ypos+panelHeight,1,1);
	
	addCatagories();
	addExtras();
	appMenu->LFSTK_setMouseCallBack(NULL,menuCB,NULL);
	appMenu->LFSTK_addMainMenus(catagoryMenus,catagoryCnt);
	XMapWindow(mainwind->app->display,appButton->LFSTK_getWindow());
	appButton->LFSTK_clearWindow();
	return(width);
}
