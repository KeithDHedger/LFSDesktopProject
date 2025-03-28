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
#include <string.h>
#include <fstream> 
#include <regex>

#include "appmenu.h"

struct desktopFileStruct
{
	std::string	name="";
	std::string	icon="";
	std::string	exec="";
	bool			interm=false;
	bool			display=true;
	std::string	cat="";
};

const char						*myCats[]= {"Settings","Utility","Development","Electronics","Education","Graphics","Network","AudioVideo","Office","Shell","Game","System",NULL};
const char						*catImageNames[]={"preferences-desktop","applications-utilities","applications-development","applications-engineering","applications-science","applications-graphics","applications-internet","applications-multimedia","applications-office","applications-engineering","applications-games","applications-system",NULL};

LFSTK_findClass					*find;
LFSTK_buttonClass				*appButton=NULL;
infoDataStruct						**catagoryMenus;
LFSTK_menuClass					*appMenu=NULL;

int								catPtr[MAXCATS];
//const char						*currentCatName=NULL;
int								catagoryCnt=0;

std::map<int,menuEntryStruct*>	entrydata;
std::map<const char*,bool>		gotCatList;
std::map<const char*,int>		catMenusPtr;
std::map<int,std::string>		desktopFiles;
int								desktopfilescnt=0;

bool buttonCB(void *p,void* ud)
{
	appMenu->LFSTK_showMenu();
	return(true);
}

bool menuCB(void *p,void* ud)
{
	std::string			deskfile;
	std::ifstream		file;
	std::stringstream	ss;
	const std::regex		expexec("Exec=(.*)");
	const std::regex		expterm("Terminal=true");
	std::smatch			m;
	char					*command;
	desktopFileStruct	df;
	std::string			correctedcommand;

	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();

	if((long)ud==-1)
		{
			system("lfsabout &");
			return(true);
		}

	file.open(desktopFiles[(long)ud].c_str(),std::ifstream::in);
	if(file.is_open())
		{
			df.interm=false;
			df.exec=true;
			ss<<file.rdbuf();
			file.close();
			if(std::regex_search(ss.str(),expexec))
				{
					std::string holdsss=ss.str();
					//exec
					std::regex_search(holdsss,m,expexec,std::regex_constants::match_not_eol|std::regex_constants::match_not_bol);
					df.exec=m.str(1);
					//interm
					if(std::regex_search(holdsss,m,expterm,std::regex_constants::match_not_eol|std::regex_constants::match_not_bol))
						df.interm=true;

					correctedcommand=LFSTK_UtilityClass::LFSTK_strReplaceAllStr(df.exec,"%u","",true);
					correctedcommand=LFSTK_UtilityClass::LFSTK_strReplaceAllStr(correctedcommand,"%U","",true);
					correctedcommand=LFSTK_UtilityClass::LFSTK_strReplaceAllStr(correctedcommand,"%f","",true);
					correctedcommand=LFSTK_UtilityClass::LFSTK_strReplaceAllStr(correctedcommand,"%F","",true);

					if(df.interm==false)
						asprintf(&command,"%s &",correctedcommand.c_str());
					else
						asprintf(&command,"%s %s &",prefs.LFSTK_getCString("termcommand"),correctedcommand.c_str());

					system(command);
					free(command);				
				}
		}
	return(true);
}

void addEntries(void)
{
	char					*homeapps;
	int					numfiles;
	desktopFileStruct	df;
	std::ifstream		file;
	std::stringstream	ss;
	const std::regex		expname("Name=(.*)");
	const std::regex		expicon("Icon=([^[:space:]]*).*");
	const std::regex		expnodisplay("NoDisplay=([^[:space:]]*).*");
	const std::regex		expcat("Categories=(.*)");
	std::smatch			m;
	std::string			iconpath;

	asprintf(&homeapps,"%s/.local/share/applications",getenv("HOME"));
	find->LFSTK_setFindType(FILETYPE);
	find->LFSTK_setFullPath(true);
	find->LFSTK_setIgnoreBroken(true);
	find->LFSTK_setSortDecending(false);
	find->LFSTK_setIgnoreNavLinks(true);
	find->LFSTK_setFileTypes(".desktop");	
	find->LFSTK_findFiles("/usr/share/applications",false);
	find->LFSTK_findFiles(homeapps,true);

	numfiles=find->LFSTK_getDataCount();

	catagoryMenus=new infoDataStruct*[MAXCATS];

	for(int j=0;j<numfiles;j++)
		{
			file.open(find->data[j].path.c_str(),std::ifstream::in);
			if(file.is_open())
				{
					ss.str("");
					ss.clear();
					ss<<file.rdbuf();
					file.close();
					if(std::regex_search(ss.str(),expcat))
						{
							std::string holdsss=ss.str();
							//nodisp
							if(std::regex_search(holdsss,m,expnodisplay,std::regex_constants::match_not_eol|std::regex_constants::match_not_bol))
								if(m.str(1).compare("true")==0)
									continue;
							//name
							std::regex_search(holdsss,m,expname,std::regex_constants::match_not_eol|std::regex_constants::match_not_bol);
							df.name=m.str(1);
							//icon
							if(std::regex_search(holdsss,m,expicon,std::regex_constants::match_not_eol|std::regex_constants::match_not_bol))
								df.icon=m.str(1);

							if(std::regex_search(holdsss,m,expcat,std::regex_constants::match_not_eol|std::regex_constants::match_not_bol))
								{
									int	k=0;
									
									while(myCats[k]!=NULL)
										{
											if(m.str(1).find(myCats[k])!=std::string::npos)
												{
													if(gotCatList.find(myCats[k])==gotCatList.end())
														{
															catMenusPtr[myCats[k]]=catagoryCnt;
															catagoryMenus[catagoryCnt]=new infoDataStruct;
															catagoryMenus[catagoryCnt]->subMenus=new infoDataStruct*[100];
															catagoryMenus[catagoryCnt]->hasSubMenu=true;
															catagoryMenus[catagoryCnt]->subMenuCnt=0;
															catagoryMenus[catagoryCnt]->label=strdup(myCats[k]);
															iconpath=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,catImageNames[k],"");
															if(iconpath.length()>0)
																{
																	catagoryMenus[catagoryCnt]->imageType=FILETHUMB;
																	catagoryMenus[catagoryCnt]->imagePath=iconpath;
																}
															catagoryCnt++;
															gotCatList[myCats[k]]=true;
														}
													int submenunum=catagoryMenus[catMenusPtr[myCats[k]]]->subMenuCnt;
													catagoryMenus[catMenusPtr[myCats[k]]]->subMenus[submenunum]=new infoDataStruct;
													catagoryMenus[catMenusPtr[myCats[k]]]->subMenus[submenunum]->label=df.name;
#if 1
//find /usr/share/icons/hicolor/ -iname "*cups*"|tail -n1
													std::string iconpaths=PREFIX "/share/pixmaps/";
													iconpaths=iconpaths+df.icon+".png";
													if(access(iconpaths.c_str(),F_OK)==F_OK)
														{
															catagoryMenus[catMenusPtr[myCats[k]]]->subMenus[submenunum]->imageType=FILETHUMB;
															catagoryMenus[catMenusPtr[myCats[k]]]->subMenus[submenunum]->imagePath=iconpaths;
														}
													else
														{
															iconpaths=PREFIX "/share/icons/hicolor/48x48/apps/";
															iconpaths=iconpaths+df.icon+".png";
															if(access(iconpaths.c_str(),F_OK)==F_OK)
																{
																	catagoryMenus[catMenusPtr[myCats[k]]]->subMenus[submenunum]->imageType=FILETHUMB;
																	catagoryMenus[catMenusPtr[myCats[k]]]->subMenus[submenunum]->imagePath=iconpaths;
																}
															else
																{
																	iconpath=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,df.icon,"");
																	if(iconpath.length()>0)
																		{
																			catagoryMenus[catMenusPtr[myCats[k]]]->subMenus[submenunum]->imageType=FILETHUMB;
																			catagoryMenus[catMenusPtr[myCats[k]]]->subMenus[submenunum]->imagePath=iconpath;
																		}
																}
														}
#endif
													desktopFiles[desktopfilescnt]=find->data[j].path;
													catagoryMenus[catMenusPtr[myCats[k]]]->subMenus[submenunum]->userData=USERDATA(desktopfilescnt);
													desktopfilescnt++;
													catagoryMenus[catMenusPtr[myCats[k]]]->subMenuCnt++;
													break;
												}
											k++;
										}
								}
//	std::cout<<"name="<<df.name<<"\n" <<"exec="<<df.exec<<"\n" <<"interm="<<df.interm<<"\n" <<"icon="<<df.icon<<"\n" <<"cat="<<df.cat <<"\n"<<std::endl;
						}
				}
		}
}

void addExtras(void)
{
	std::string	iconpath;

	catagoryMenus[catagoryCnt]=new infoDataStruct;
	catagoryMenus[catagoryCnt]->label=strdup("--");
	catagoryMenus[catagoryCnt]->userData=NULL;
	catagoryCnt++;

	catagoryMenus[catagoryCnt]=new infoDataStruct;
	catagoryMenus[catagoryCnt]->label=strdup("LFS About");
	catagoryMenus[catagoryCnt]->userData=(void*)-1;
	iconpath=mainwind->globalLib->LFSTK_findThemedIcon(desktopTheme,"help-about","");
	if(iconpath.length()>0)
		{
			catagoryMenus[catagoryCnt]->imagePath=iconpath;
			catagoryMenus[catagoryCnt]->imageType=FILETHUMB;
		}
	catagoryCnt++;
}


int addAppmenu(int x,int y,int grav,bool fromleft)
{
	int			xpos=x;
	int			ypos=y;
	int			width=0;
	int			height=0;
	int			thisgrav=grav;
	int			iconsize=16;
	bool			flag=false;
	int			mencnt=0;
	infoDataStruct	*menuhold;

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
	setGadgetDetails(appButton);

	appMenu=new LFSTK_menuClass(mainwind,xpos,ypos+panelHeight,1,1);

	addEntries();

	do
		{
			flag=false;
			for(int j=0;j<catagoryCnt-1;j++)
				{
					if(catagoryMenus[j]->label.compare(catagoryMenus[j+1]->label)>0)
						{					
							menuhold=catagoryMenus[j+1];
							catagoryMenus[j+1]=catagoryMenus[j];
							catagoryMenus[j]=menuhold;
							flag=true;
						}
				}
		}while(flag==true);

	for(int k=0;k<catagoryCnt;k++)
		{
			do
				{
					flag=false;
					int submenunum=catagoryMenus[k]->subMenuCnt;
					for(int j=0;j<submenunum-1;j++)
						{
							if(catagoryMenus[k]->subMenus[j]->label.compare(catagoryMenus[k]->subMenus[j+1]->label)>0)
								{					
									menuhold=catagoryMenus[k]->subMenus[j+1];
									catagoryMenus[k]->subMenus[j+1]=catagoryMenus[k]->subMenus[j];
									catagoryMenus[k]->subMenus[j]=menuhold;
									flag=true;
								}
						}
				}while(flag==true);
		}

	addExtras();
	appMenu->LFSTK_setMouseCallBack(NULL,menuCB,NULL);
	appMenu->LFSTK_addMainMenus(catagoryMenus,catagoryCnt);
	XMapWindow(mainwind->app->display,appButton->LFSTK_getWindow());

//	if(useTheme==false)
//		{
//			appButton->LFSTK_setTile(NULL,0);
//			appButton->LFSTK_setFontColourName(NORMALCOLOUR,panelTextColour,true);//TODO//
//		}

	appButton->LFSTK_clearWindow();

	return(width);
}
