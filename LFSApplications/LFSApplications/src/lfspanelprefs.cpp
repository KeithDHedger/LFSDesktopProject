/*
 *
 * ©K. D. Hedger. Wed 23 Aug 12:54:30 BST 2017 keithdhedger@gmail.com

 * This file (lfspanelprefs.cpp) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <getopt.h>

#include "lfstk/LFSTKGlobals.h"

#undef GADGETWIDTH
#define GADGETWIDTH	128
#undef DIALOGWIDTH
#define DIALOGWIDTH	(GADGETWIDTH*3)+(BORDER*3)
#define BOXLABEL	"LFS Panel Prefs"

LFSTK_applicationClass		*apc=NULL;
LFSTK_prefsClass			prefs;
LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_buttonClass			*apply=NULL;

//panel select
LFSTK_buttonClass			*selectPanel=NULL;
LFSTK_lineEditClass			*panelNameEdit=NULL;
menuStruct					**panelNames=NULL;
LFSTK_menuClass				*panelMenu=NULL;
int							panelCnt=0;

//panel data
LFSTK_buttonClass			*panelWidth=NULL;
LFSTK_lineEditClass			*panelWidthEdit=NULL;
menuStruct					**panelWidthMenu=NULL;
LFSTK_lineEditClass			*panelHeightEdit=NULL;
LFSTK_menuClass				*widthMenu=NULL;

LFSTK_buttonClass			*panelPos=NULL;
LFSTK_lineEditClass			*panelPosEdit=NULL;
menuStruct					**panelPosMenu=NULL;
LFSTK_menuClass				*posMenu=NULL;

LFSTK_buttonClass			*panelGrav=NULL;
LFSTK_lineEditClass			*panelGravEdit=NULL;
menuStruct					**panelGravMenu=NULL;
LFSTK_menuClass				*gravMenu=NULL;

LFSTK_lineEditClass			*panelOnMonitor=NULL;
LFSTK_lineEditClass			*panelLeftGadgets=NULL;
LFSTK_lineEditClass			*panelrightGadgets=NULL;

std::map<int,const char*>	panelWidthConvertToStr={{-1,"Fill"},{-2,"Shrink"}};
std::map<int,const char*>	panelPosConvertToStr={{-1,"Left"},{-2,"Centre"},{-3,"Right"}};
std::map<int,const char*>	panelGravConvertToStr{{1,"North"},{2,"East"},{3,"South",},{4,"West"}};

//action data
LFSTK_lineEditClass			*termCommand=NULL;
LFSTK_lineEditClass			*logout=NULL;
LFSTK_lineEditClass			*restart=NULL;
LFSTK_lineEditClass			*shutdown=NULL;

int							panelWidthPref=-2;
int							panelPosPref=-2;
int							panelGravityPref=1;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	if(ud!=NULL)
		{
			if(strcmp((char*)ud,"SHOWPANELMENU")==0)
				panelMenu->LFSTK_showMenu();
			if(strcmp((char*)ud,"SHOWWIDTHMENU")==0)
				widthMenu->LFSTK_showMenu();
			if(strcmp((char*)ud,"SHOWPOSMENU")==0)
				posMenu->LFSTK_showMenu();
			if(strcmp((char*)ud,"SHOWGRAVMENU")==0)
				gravMenu->LFSTK_showMenu();
		}
	return(true);
}

void getEdits(void)
{
	prefs.prefsMap=
		{
			{prefs.LFSTK_hashFromKey("panelheight"),{TYPEINT,"panelheight","",false,atoi(panelHeightEdit->LFSTK_getCStr())}},
			{prefs.LFSTK_hashFromKey("panelwidth"),{TYPEINT,"panelwidth","",false,panelWidthPref}},
			{prefs.LFSTK_hashFromKey("onmonitor"),{TYPEINT,"onmonitor","",false,atoi(panelOnMonitor->LFSTK_getCStr())}},
			{prefs.LFSTK_hashFromKey("panelpos"),{TYPEINT,"panelpos","",false,panelPosPref}},
			{prefs.LFSTK_hashFromKey("panelgrav"),{TYPEINT,"panelgrav","",false,panelGravityPref}},
			{prefs.LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand",termCommand->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("logoutcommand"),{TYPESTRING,"logoutcommand",logout->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("restartcommand"),{TYPESTRING,"restartcommand",restart->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("shutdowncommand"),{TYPESTRING,"shutdowncommand",shutdown->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("gadgetsright"),{TYPESTRING,"gadgetsright",panelrightGadgets->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("gadgetsleft"),{TYPESTRING,"gadgetsleft",panelLeftGadgets->LFSTK_getCStr(),false,0}},
		};
}

bool applyCB(void *p,void* ud)
{
	char	*env=NULL;
	std::map<int,const char*>::const_iterator it;

	if(ud!=NULL)
		{
//panel width
			if(isdigit(panelWidthEdit->LFSTK_getCStr()[0])==true)
				panelWidthPref=atoi(panelWidthEdit->LFSTK_getCStr());
			else
				{
					if(strcmp(panelWidthEdit->LFSTK_getCStr(),"Fill")==0)
						panelWidthPref=-1;
					else
						panelWidthPref=-2;
				}
//panel pos
			if(isdigit(panelPosEdit->LFSTK_getCStr()[0])==true)
				panelPosPref=atoi(panelPosEdit->LFSTK_getCStr());
			else
				{
					for(it=panelPosConvertToStr.begin();it != panelPosConvertToStr.end();++it)
						{
							if(strcmp(it->second,panelPosEdit->LFSTK_getCStr())==0)
								{
									panelPosPref=it->first;
									break;
								}
						}
				}
//panel grav
			if(isdigit(panelGravEdit->LFSTK_getCStr()[0])==true)
				panelGravityPref=atoi(panelGravEdit->LFSTK_getCStr());
			else
				{
					for(it=panelGravConvertToStr.begin();it != panelGravConvertToStr.end();++it)
						{
							if(strcmp(it->second,panelGravEdit->LFSTK_getCStr())==0)
								{
									panelGravityPref=it->first;
									break;
								}
						}
				}

			getEdits();
			asprintf(&env,"%s/%s",apc->configDir,panelNameEdit->LFSTK_getCStr());
			//wc->globalLib->LFSTK_saveVarsToFile(env,panelPrefs);
			prefs.LFSTK_saveVarsToFile(env);
			//prefs.LFSTK_saveVarsToFile("-");
			free(env);
		}
	return(true);
}

void setEdits(void)
{
	panelWidthEdit->LFSTK_setBuffer(panelWidthConvertToStr[prefs.LFSTK_getInt("panelwidth")]);
	panelHeightEdit->LFSTK_setBuffer(std::to_string(prefs.LFSTK_getInt("panelheight")).c_str());
	panelPosEdit->LFSTK_setBuffer(panelPosConvertToStr[prefs.LFSTK_getInt("panelpos")]);
	panelGravEdit->LFSTK_setBuffer(panelGravConvertToStr[prefs.LFSTK_getInt("panelgrav")]);
	panelOnMonitor->LFSTK_setBuffer(std::to_string(prefs.LFSTK_getInt("onmonitor")).c_str());
	termCommand->LFSTK_setBuffer(prefs.LFSTK_getCString("termcommand"));
	logout->LFSTK_setBuffer(prefs.LFSTK_getCString("logoutcommand"));
	restart->LFSTK_setBuffer(prefs.LFSTK_getCString("restartcommand"));
	shutdown->LFSTK_setBuffer(prefs.LFSTK_getCString("shutdowncommand"));
	panelLeftGadgets->LFSTK_setBuffer(prefs.LFSTK_getCString("gadgetsleft"));
	panelrightGadgets->LFSTK_setBuffer(prefs.LFSTK_getCString("gadgetsright"));
}

void getPrefs(void)
{
	char	*env=NULL;

	prefs.prefsMap=
		{
			{prefs.LFSTK_hashFromKey("panelheight"),{TYPEINT,"panelheight","",false,0}},
			{prefs.LFSTK_hashFromKey("panelwidth"),{TYPEINT,"panelwidth","",false,0}},
			{prefs.LFSTK_hashFromKey("onmonitor"),{TYPEINT,"onmonitor","",false,0}},
			{prefs.LFSTK_hashFromKey("panelpos"),{TYPEINT,"panelpos","",false,0}},
			{prefs.LFSTK_hashFromKey("panelgrav"),{TYPEINT,"panelgrav","",false,0}},
			{prefs.LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand","",false,0}},
			{prefs.LFSTK_hashFromKey("logoutcommand"),{TYPESTRING,"logoutcommand","",false,0}},
			{prefs.LFSTK_hashFromKey("restartcommand"),{TYPESTRING,"restartcommand","",false,0}},
			{prefs.LFSTK_hashFromKey("shutdowncommand"),{TYPESTRING,"shutdowncommand","",false,0}},
			{prefs.LFSTK_hashFromKey("gadgetsright"),{TYPESTRING,"gadgetsright","",false,0}},
			{prefs.LFSTK_hashFromKey("gadgetsleft"),{TYPESTRING,"gadgetsleft","",false,0}},
		};

	asprintf(&env,"%s/%s",apc->configDir,panelNameEdit->LFSTK_getCStr());
	prefs.LFSTK_loadVarsFromFile(env);
	free(env);
}

bool panelSelectCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	panelNameEdit->LFSTK_setBuffer(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel());
	getPrefs();
	setEdits();
	return(true);
}

bool panelWidthCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	panelWidthEdit->LFSTK_setBuffer(panelWidthConvertToStr[(int)(long)ud]);
	return(true);
}

bool panelPositionCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	panelPosEdit->LFSTK_setBuffer(panelPosConvertToStr[(int)(long)ud]);
	return(true);
}

bool panelGravCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	panelGravEdit->LFSTK_setBuffer(panelGravConvertToStr[(int)(long)ud]);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent			event;
	int				sy=0;
	long			menuuserdata[3]={-1,-2,-3};
	int				parentWindow=-1;
	LFSTK_findClass	*find;
	int				c=0;
	int				option_index=0;
	const char		*shortOpts="h?w:";		
	option			longOptions[]=
		{
			{"window",1,0,'w'},
			{"help",0,0,'h'},
			{0, 0, 0, 0}
		};

	while(1)
		{
			option_index=0;
			c=getopt_long_only(argc,argv,shortOpts,longOptions,&option_index);
			if (c==-1)
				break;
			switch (c)
				{
					case 'h':
					case '?':
						printf("-?,-h,--help\t\tPrint this help\n");
						printf("-w,--window\t\tSet transient for window\n");
						exit(0);
					case 'w':
						parentWindow=atoi(optarg);
						break;
				}
		}

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL);
	wc=apc->mainWindow;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//panel menus
//temp fix
	char	*touchpanel;
	asprintf(&touchpanel,"touch %s/lfspanel.rc",apc->configDir);
	system(touchpanel);
	free(touchpanel);

	find=new LFSTK_findClass;
	find->LFSTK_setFindType(FILETYPE);
	find->LFSTK_setIgnoreBroken(true);
	find->LFSTK_setSort(false);
	find->LFSTK_setFileTypes("lfspanel");
	find->LFSTK_findFiles(apc->configDir,false);
	find->LFSTK_sortByName();

	panelCnt=find->LFSTK_getDataCount();

	panelNames=new menuStruct*[panelCnt];
	for(int j=0;j<panelCnt;j++)
		{
			panelNames[j]=new menuStruct;
			panelNames[j]->label=strdup(find->data[j].name.c_str());
			panelNames[j]->userData=(void*)(long)j;
		}

//panel config
//select
	selectPanel=new LFSTK_buttonClass(wc,"Panel Config",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	selectPanel->LFSTK_setIndicator(DISCLOSURE);

	selectPanel->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWPANELMENU");
	panelMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	panelMenu->LFSTK_setMouseCallBack(NULL,panelSelectCB,NULL);
	panelMenu->LFSTK_addMainMenus(panelNames,panelCnt);

	panelNameEdit=new LFSTK_lineEditClass(wc,panelNames[0]->label,BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);	
	sy+=YSPACING;

//do prefs
	getPrefs();

//width
	panelWidth=new LFSTK_buttonClass(wc,"Panel Width",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	panelWidth->LFSTK_setIndicator(DISCLOSURE);
	panelWidth->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWWIDTHMENU");
	panelWidthMenu=new menuStruct*[2];
	for(long j=0;j<2;j++)
		{
			panelWidthMenu[j]=new menuStruct;
			panelWidthMenu[j]->label=strdup(panelWidthConvertToStr[menuuserdata[j]]);
			panelWidthMenu[j]->userData=(void*)menuuserdata[j];
		}
	widthMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	widthMenu->LFSTK_setMouseCallBack(NULL,panelWidthCB,NULL);
	widthMenu->LFSTK_addMainMenus(panelWidthMenu,2);

	panelWidthEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//hite
	label=new LFSTK_labelClass(wc,"Panel Height",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	panelHeightEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//panel position
	panelPos=new LFSTK_buttonClass(wc,"Panel Position",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	panelPos->LFSTK_setIndicator(DISCLOSURE);
	panelPos->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWPOSMENU");

	panelPosMenu=new menuStruct*[4];
	for(int j=0;j<3;j++)
		{
			panelPosMenu[j]=new menuStruct;
			panelPosMenu[j]->label=strdup(panelPosConvertToStr[menuuserdata[j]]);
			panelPosMenu[j]->userData=(void*)menuuserdata[j];
		}
	posMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	posMenu->LFSTK_setMouseCallBack(NULL,panelPositionCB,NULL);
	posMenu->LFSTK_addMainMenus(panelPosMenu,3);
	panelPosEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//panel grav
	panelGrav=new LFSTK_buttonClass(wc,"Panel Gravity",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	panelGrav->LFSTK_setIndicator(DISCLOSURE);
	panelGrav->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWGRAVMENU");
	panelGravMenu=new menuStruct*[4];
	for(long j=0;j<4;j++)
		{
			panelGravMenu[j]=new menuStruct;
			panelGravMenu[j]->label=strdup(panelGravConvertToStr[j+1]);
			panelGravMenu[j]->userData=(void*)(j+1);
		}
	gravMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	gravMenu->LFSTK_setMouseCallBack(NULL,panelGravCB,NULL);
	gravMenu->LFSTK_addMainMenus(panelGravMenu,4);
	panelGravEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//on monitor
	label=new LFSTK_labelClass(wc,"On Monitor",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	panelOnMonitor=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//term comm
	label=new LFSTK_labelClass(wc,"Terminal CMD",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	termCommand=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//logout
	label=new LFSTK_labelClass(wc,"Logout",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	logout=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//restart
	label=new LFSTK_labelClass(wc,"Restart",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	restart=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//shutdown
	label=new LFSTK_labelClass(wc,"Shutdown",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	shutdown=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//left gadgets
	label=new LFSTK_labelClass(wc,"Left Gadgets",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	panelLeftGadgets=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//rite gadgets
	label=new LFSTK_labelClass(wc,"Right Gadgets",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	panelrightGadgets=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	setEdits();

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);

//apply
	apply=new LFSTK_buttonClass(wc,"Apply",DIALOGWIDTH-BORDER-GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	apply->LFSTK_setMouseCallBack(NULL,applyCB,(void*)"APPLY");
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

	int retval=apc->LFSTK_runApp();

	delete panelMenu;
	delete widthMenu;
	delete posMenu;
	delete gravMenu;
	delete apc;

	return(retval);
}


