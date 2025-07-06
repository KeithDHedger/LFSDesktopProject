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

#include <lfstk/LFSTKGlobals.h>

#include "config.h"

#undef GADGETWIDTH
#define GADGETWIDTH	128
#undef DIALOGWIDTH
#define DIALOGWIDTH	(GADGETWIDTH*3)+(BORDER*3)
#define BOXLABEL	"LFS Panel Prefs"

LFSTK_applicationClass		*apc=NULL;
LFSTK_prefsClass				prefs;
LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass				*label=NULL;
LFSTK_labelClass				*personal=NULL;
LFSTK_labelClass				*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_buttonClass			*apply=NULL;

//panel select
LFSTK_buttonClass			*selectPanel=NULL;
LFSTK_lineEditClass			*panelNameEdit=NULL;
infoDataStruct					**panelNames=NULL;
LFSTK_menuClass				*panelMenu=NULL;
int							panelCnt=0;

//panel data
LFSTK_buttonClass			*panelWidth=NULL;
LFSTK_lineEditClass			*panelWidthEdit=NULL;
infoDataStruct					**panelWidthMenu=NULL;
LFSTK_lineEditClass			*panelHeightEdit=NULL;
LFSTK_menuClass				*widthMenu=NULL;

LFSTK_buttonClass			*panelPos=NULL;
LFSTK_lineEditClass			*panelPosEdit=NULL;
infoDataStruct					**panelPosMenu=NULL;
LFSTK_menuClass				*posMenu=NULL;

LFSTK_buttonClass			*panelGrav=NULL;
LFSTK_lineEditClass			*panelGravEdit=NULL;
infoDataStruct					**panelGravMenu=NULL;
LFSTK_menuClass				*gravMenu=NULL;

LFSTK_lineEditClass			*panelOnMonitor=NULL;
LFSTK_lineEditClass			*panelLeftGadgets=NULL;
LFSTK_lineEditClass			*panelrightGadgets=NULL;

LFSTK_toggleButtonClass		*useTheme=NULL;
LFSTK_toggleButtonClass		*noButtons=NULL;
LFSTK_lineEditClass			*panelColourEdit=NULL;
LFSTK_lineEditClass			*panelTextColourEdit=NULL;

std::map<int,const char*>	panelWidthConvertToStr={{-1,"Fill"},{-2,"Shrink"}};
std::map<int,const char*>	panelPosConvertToStr={{-1,"Left"},{-2,"Centre"},{-3,"Right"}};
std::map<int,const char*>	panelGravConvertToStr={{1,"North"},{2,"East"},{3,"South",},{4,"West"}};

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
			{LFSTK_UtilityClass::LFSTK_hashFromKey("panelheight"),{TYPEINT,"panelheight","","",false,atoi(panelHeightEdit->LFSTK_getCStr())}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("panelwidth"),{TYPEINT,"panelwidth","","",false,panelWidthPref}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("onmonitor"),{TYPEINT,"onmonitor","","",false,atoi(panelOnMonitor->LFSTK_getCStr())}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("panelpos"),{TYPEINT,"panelpos","","",false,panelPosPref}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("panelgrav"),{TYPEINT,"panelgrav","","",false,panelGravityPref}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand","",termCommand->LFSTK_getCStr(),false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("logoutcommand"),{TYPESTRING,"logoutcommand","",logout->LFSTK_getCStr(),false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("restartcommand"),{TYPESTRING,"restartcommand","",restart->LFSTK_getCStr(),false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("shutdowncommand"),{TYPESTRING,"shutdowncommand","",shutdown->LFSTK_getCStr(),false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("gadgetsright"),{TYPESTRING,"gadgetsright","",panelrightGadgets->LFSTK_getCStr(),false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("gadgetsleft"),{TYPESTRING,"gadgetsleft","",panelLeftGadgets->LFSTK_getCStr(),false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("usetheme"),{TYPEBOOL,"usetheme","","",useTheme->LFSTK_getValue(),0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("nobuttons"),{TYPEBOOL,"nobuttons","","",noButtons->LFSTK_getValue(),0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("panelcolour"),{TYPESTRING,"panelcolour","",panelColourEdit->LFSTK_getCStr(),false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("textcolour"),{TYPESTRING,"textcolour","",panelTextColourEdit->LFSTK_getCStr(),false,0}},
		};
}

bool applyCB(void *p,void* ud)
{
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
							if((it->second!=NULL) && (strcmp(it->second,panelPosEdit->LFSTK_getCStr())==0))
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
							if((it->second!=NULL) && (strcmp(it->second,panelGravEdit->LFSTK_getCStr())==0))
								{
									panelGravityPref=it->first;
									break;
								}
						}
				}

			getEdits();
			prefs.LFSTK_saveVarsToFile(apc->configDir+"/"+panelNameEdit->LFSTK_getBuffer());
			//prefs.LFSTK_saveVarsToFile("-");
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
	useTheme->LFSTK_setValue(prefs.LFSTK_getBool("usetheme"));
	noButtons->LFSTK_setValue(prefs.LFSTK_getBool("nobuttons"));
	panelColourEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("panelcolour"));
	panelTextColourEdit->LFSTK_setBuffer(prefs.LFSTK_getCString("textcolour"));
}

void getPrefs(void)
{
	char	*env=NULL;

	prefs.prefsMap=
		{
			{LFSTK_UtilityClass::LFSTK_hashFromKey("panelheight"),{TYPEINT,"panelheight","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("panelwidth"),{TYPEINT,"panelwidth","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("onmonitor"),{TYPEINT,"onmonitor","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("panelpos"),{TYPEINT,"panelpos","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("panelgrav"),{TYPEINT,"panelgrav","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("logoutcommand"),{TYPESTRING,"logoutcommand","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("restartcommand"),{TYPESTRING,"restartcommand","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("shutdowncommand"),{TYPESTRING,"shutdowncommand","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("gadgetsright"),{TYPESTRING,"gadgetsright","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("gadgetsleft"),{TYPESTRING,"gadgetsleft","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("usetheme"),{TYPEBOOL,"usetheme","","",true,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("nobuttons"),{TYPEBOOL,"nobuttons","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("panelcolour"),{TYPESTRING,"panelcolour","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("textcolour"),{TYPESTRING,"textcolour","","black",false,0}},
		};

	asprintf(&env,"%s/%s",apc->configDir.c_str(),panelNameEdit->LFSTK_getCStr());
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
	int				pc=0;
	int				sy=0;
	long				menuuserdata[3]={-1,-2,-3};
	int				parentWindow=-1;
	LFSTK_findClass	*find;
	LFSTK_prefsClass	cliprefs("lfspanelprefs",VERSION);
	option			longOptions[]={{"window",1,0,'w'},{0, 0, 0, 0}};
	windowInitStruct	*win;

	apc=new LFSTK_applicationClass();
	win=apc->LFSTK_getDefaultWInit();
	win->windowName=BOXLABEL;
	win->windowType=win->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DIALOG"));
	win->level=ABOVEALL;
	apc->LFSTK_addWindow(win,BOXLABEL,"LFSPanelPrefs");
	wc=apc->mainWindow;
	wc->LFSTK_setDecorations(false,false,false,true);

	cliprefs.prefsMap=
		{
			{LFSTK_UtilityClass::LFSTK_hashFromKey("window"),{TYPEINT,"window","Set transient for window ARG","",false,0}}
		};
	if(cliprefs.LFSTK_argsToPrefs(argc,argv,longOptions,true)==false)
		return(1);
	parentWindow=cliprefs.LFSTK_getInt("window");

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//panel menus
//temp fix
	char	*touchpanel;
	asprintf(&touchpanel,"touch %s/lfspanel.rc",apc->configDir.c_str());
	system(touchpanel);
	free(touchpanel);

	find=new LFSTK_findClass;
	find->LFSTK_setFindType(FILETYPE);
	find->LFSTK_setIgnoreBroken(true);
	find->LFSTK_setIgnoreFolders(true);
	find->LFSTK_setSortDecending(false);
	find->LFSTK_setIgnoreNavLinks(true);
	find->LFSTK_setFileTypes(".rc");
	find->LFSTK_setNameFilter("lfspanel");
	find->LFSTK_findFiles(apc->configDir.c_str(),false);
	find->LFSTK_sortByName();

	panelCnt=find->LFSTK_getDataCount();

	panelNames=new infoDataStruct*[panelCnt];
	pc=0;
	for(int j=0;j<panelCnt;j++)
		{
			if(LFSTK_UtilityClass::LFSTK_strStr(find->data[j].name,"lfspanel").empty()==false)
				{
					panelNames[pc]=new infoDataStruct;
					panelNames[pc]->label=strdup(find->data[j].name.c_str());
					panelNames[pc]->userData=(void*)(long)j;
					pc++;
				}
		}
	panelCnt=pc;	

//panel config
//select
	selectPanel=new LFSTK_buttonClass(wc,"Panel Config",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	selectPanel->LFSTK_setIndicator(DISCLOSURE);

	selectPanel->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWPANELMENU");
	panelMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	panelMenu->LFSTK_setMouseCallBack(NULL,panelSelectCB,NULL);
	panelMenu->LFSTK_addMainMenus(panelNames,panelCnt);

	panelNameEdit=new LFSTK_lineEditClass(wc,panelNames[0]->label.c_str(),BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);	
	sy+=YSPACING;

//do prefs
	getPrefs();

//width
	panelWidth=new LFSTK_buttonClass(wc,"Panel Width",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	panelWidth->LFSTK_setIndicator(DISCLOSURE);
	panelWidth->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWWIDTHMENU");
	panelWidthMenu=new infoDataStruct*[2];
	for(long j=0;j<2;j++)
		{
			panelWidthMenu[j]=new infoDataStruct;
			panelWidthMenu[j]->label=panelWidthConvertToStr[menuuserdata[j]];
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

	panelPosMenu=new infoDataStruct*[4];
	for(int j=0;j<3;j++)
		{
			panelPosMenu[j]=new infoDataStruct;
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
	panelGravMenu=new infoDataStruct*[4];
	for(long j=0;j<4;j++)
		{
			panelGravMenu[j]=new infoDataStruct;
			panelGravMenu[j]->label=strdup(panelGravConvertToStr[j+1]);
			panelGravMenu[j]->userData=(void*)(j+1);
		}
	gravMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	gravMenu->LFSTK_setMouseCallBack(NULL,panelGravCB,NULL);
	gravMenu->LFSTK_addMainMenus(panelGravMenu,4);
	panelGravEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//use theme
	useTheme=new LFSTK_toggleButtonClass(wc,"Use Theme",BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	useTheme->LFSTK_setValue(prefs.LFSTK_getBool("usetheme"));

//no button graphics
	noButtons=new LFSTK_toggleButtonClass(wc,"No Button Graphics",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	noButtons->LFSTK_setValue(prefs.LFSTK_getBool("nobuttons"));
	sy+=YSPACING;

//panel colour
	label=new LFSTK_labelClass(wc,"Panel Colour",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	panelColourEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	
//panel text colour
	label=new LFSTK_labelClass(wc,"Text Colour",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	panelTextColourEdit=new LFSTK_lineEditClass(wc,"black",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
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



