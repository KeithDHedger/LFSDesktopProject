/*
 *
 * ©K. D. Hedger. Thu 17 Aug 15:46:21 BST 2017 keithdhedger@gmail.com

 * This file (lfswm2prefs.cpp) is part of LFSApplications.

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

#include <getopt.h>

#include "config.h"
#include <lfstk/LFSTKGlobals.h>
#include <libgen.h>

#define MAXMENUS			5
#define EDITBOXWIDTH		GADGETWIDTH*4
#define BOXLABEL			"LFS WM Prefs"
#define COLOURBUTTONS	3
#define RESIZEMENUSIZE	4
#define TITLEPOSMENUSIZE	3
#define FORCEMENUSIZE	3
#define MODS1MENUSIZE	8
#define MODS2MENUSIZE	5

LFSTK_applicationClass	*apc=NULL;
LFSTK_prefsClass			prefs;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_buttonClass		*test=NULL;
LFSTK_buttonClass		*apply=NULL;

//preview colours
enum {FRAMEBG=0,FRAMEFG,TEXTCOL};
LFSTK_buttonClass		*previewButtons[COLOURBUTTONS]={NULL,};
LFSTK_lineEditClass		*previeColourEdit[COLOURBUTTONS]={NULL,};
const char				*previewButtonLabels[COLOURBUTTONS]={"Frame BG","Frame FG","Text Colour"};
const char				*prefsnames[COLOURBUTTONS]={"framebg","framefg","textcolour"};

//theme
//font
LFSTK_fontDialogClass	*fontSelector=NULL;
LFSTK_lineEditClass		*fontEdit=NULL;

LFSTK_fileDialogClass	*themeFolder=NULL;
LFSTK_buttonClass		*themeButton=NULL;
LFSTK_lineEditClass		*themeEdit=NULL;

//placement
LFSTK_buttonClass		*placeWindowMenu=NULL;
LFSTK_lineEditClass		*placeWindowEdit=NULL;
menuStruct				**placementMenus;
const char				*placementMenuNames[]={"No placement","Under Mouse","Centre On Monitor With Mouse","Centre On Screen","TODO"};
LFSTK_menuClass			*placeMenu=NULL;

//resize
LFSTK_buttonClass		*resizeWindowMenu=NULL;
LFSTK_lineEditClass		*resizeWindowEdit=NULL;
menuStruct				**resizeMenus;
const char				*resizeMenuNames[]={"Fast Resize","Live Resize","TBD","Scale Resize"};
LFSTK_menuClass			*resizeMenu=NULL;

//title postion
LFSTK_buttonClass		*titlePosWindowMenu=NULL;
LFSTK_lineEditClass		*titlePosWindowEdit=NULL;
menuStruct				**titlePosMenus;
const char				*titlePosMenuNames[]={"Left","Centre","Right"};
LFSTK_menuClass			*titlePosMenu=NULL;

//force dock stack order
LFSTK_buttonClass		*forceDockStackWindowMenu=NULL;
LFSTK_lineEditClass		*forceDockStackWindowEdit=NULL;
menuStruct				**forceDockStackMenus;
const char				*forceDockStackMenuNames[]={"Set By App","Force Above","Force Below"};
LFSTK_menuClass			*forceDockStackMenu=NULL;

//modkeys 1
LFSTK_buttonClass		*modkeys1WindowMenu=NULL;
LFSTK_lineEditClass		*modkeys1WindowEdit=NULL;
menuStruct				**modkeys1Menus;
const char				*modkeys1MenuNames[]={"None","Shift","Caps Lock","Shift+Caps Lock","Control","Shift+Control","Caps Lock+Control","Shift+Caps Lock+Control"};
//const char				*modkeys1MenuNames[]={"None","Shift","Caps Lock","Control"};
LFSTK_menuClass			*modkeys1Menu=NULL;

//modkeys 2
LFSTK_buttonClass		*modkeys2WindowMenu=NULL;
LFSTK_lineEditClass		*modkeys2WindowEdit=NULL;
menuStruct				**modkeys2Menus;
const char				*modkeys2MenuNames[]={"Mod1","Mod2","Mod3","Mod4","Mod5"};
LFSTK_menuClass			*modkeys2Menu=NULL;

std::string				m1="None";
std::string				m2="Mod1";
long unsigned			mk1=0;
long unsigned			mk2=8;

LFSTK_lineEditClass		*titleBarSizeEdit=NULL;
LFSTK_lineEditClass		*bottomBarSizeEdit=NULL;
LFSTK_lineEditClass		*leftBarSizeEdit=NULL;
LFSTK_lineEditClass		*rightBarSizeEdit=NULL;

LFSTK_lineEditClass		*deskCountEdit=NULL;
LFSTK_lineEditClass		*rescanEdit=NULL;

LFSTK_toggleButtonClass	*useTheme=NULL;

char						*envFile=NULL;
int						parentWindow=-1;


//prefs
int						prefsPlacementTemp=2;
int						prefsResizeTemp=2;
int						titlePosTemp=1;
int						forceDockStackTemp=1;
int						prefsModkeys1Temp=0;

int						modKey1=0;
int						modKey2=0;
//msg
int						queueID=-1;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	const fontDataStruct	*fd;
	msgBuffer			mbuffer;

	if(ud!=NULL)
		{
			if(strcmp((char*)ud,"MODKEYS1")==0)
				{
					modkeys1Menu->LFSTK_showMenu();
				}
			if(strcmp((char*)ud,"MODKEYS2")==0)
				{
					modkeys2Menu->LFSTK_showMenu();
				}

			if(strcmp((char*)ud,"FORCEDOCKSTACK")==0)
				{
					forceDockStackMenu->LFSTK_showMenu();
				}

			if(strcmp((char*)ud,"TITLEPOS")==0)
				{
					titlePosMenu->LFSTK_showMenu();
				}

			if(strcmp((char*)ud,"RESIZEMODE")==0)
				{
					resizeMenu->LFSTK_showMenu();
				}

			if(strcmp((char*)ud,"SHOWPLACEMENU")==0)
				{
					placeMenu->LFSTK_showMenu();
				}

			if(strcmp((char*)ud,"SELECTFONT")==0)
				{
					fontSelector->LFSTK_showDialog(fontEdit->LFSTK_getCStr());
					fd=fontSelector->LFSTK_getFontData(false);
					if(fd->isValid==true)
						fontEdit->LFSTK_setBuffer(fd->fontString);
					return(true);
				}

			if(strcmp((char*)ud,"SELECTTHEME")==0)
				{
					themeFolder->LFSTK_showFileDialog(NULL,"Select Theme Folder");
					if(themeFolder->LFSTK_isValid()==true)
						{
							themeEdit->LFSTK_setBuffer(themeFolder->LFSTK_getCurrentDir());
						}
					return(true);
				}

			if(strcmp((char*)ud,"APPLY")==0)
				{
					prefs.prefsMap=
						{
							{prefs.LFSTK_hashFromKey("framebg"),{TYPESTRING,"framebg",previeColourEdit[FRAMEBG]->LFSTK_getCStr(),false,0}},
							{prefs.LFSTK_hashFromKey("framefg"),{TYPESTRING,"framefg",previeColourEdit[FRAMEFG]->LFSTK_getCStr(),false,0}},
							{prefs.LFSTK_hashFromKey("textcolour"),{TYPESTRING,"textcolour",previeColourEdit[TEXTCOL]->LFSTK_getCStr(),false,0}},
							{prefs.LFSTK_hashFromKey("titlebarsize"),{TYPEINT,"titlebarsize","",false,atoi(titleBarSizeEdit->LFSTK_getCStr())}},
							{prefs.LFSTK_hashFromKey("bottombarsize"),{TYPEINT,"bottombarsize","",false,atoi(bottomBarSizeEdit->LFSTK_getCStr())}},
							{prefs.LFSTK_hashFromKey("leftsidebarsize"),{TYPEINT,"leftsidebarsize","",false,atoi(leftBarSizeEdit->LFSTK_getCStr())}},
							{prefs.LFSTK_hashFromKey("ritesidebarsize"),{TYPEINT,"ritesidebarsize","",false,atoi(rightBarSizeEdit->LFSTK_getCStr())}},

							{prefs.LFSTK_hashFromKey("titlefont"),{TYPESTRING,"titlefont",fontEdit->LFSTK_getCStr(),false,0}},
							{prefs.LFSTK_hashFromKey("titleposition"),{TYPEINT,"titleposition","",false,titlePosTemp}},
							{prefs.LFSTK_hashFromKey("usetheme"),{TYPEBOOL,"usetheme","",useTheme->LFSTK_getValue(),0}},
							{prefs.LFSTK_hashFromKey("theme"),{TYPESTRING,"theme",themeEdit->LFSTK_getCStr(),false,0}},

							{prefs.LFSTK_hashFromKey("placement"),{TYPEINT,"placement","",false,prefsPlacementTemp}},
							{prefs.LFSTK_hashFromKey("desktops"),{TYPEINT,"desktops","",false,atoi(deskCountEdit->LFSTK_getCStr())}},
							{prefs.LFSTK_hashFromKey("resizemode"),{TYPEINT,"resizemode","",false,prefsResizeTemp}},
							{prefs.LFSTK_hashFromKey("rescanprefs"),{TYPEINT,"rescanprefs","",false,atoi(rescanEdit->LFSTK_getCStr())}},
							{prefs.LFSTK_hashFromKey("forcedocksstack"),{TYPEINT,"forcedocksstack","",false,forceDockStackTemp}},

							{prefs.LFSTK_hashFromKey("modkeys"),{TYPEINT,"modkeys","",false,prefsModkeys1Temp}}
						};

					prefs.LFSTK_saveVarsToFile(envFile);
					//prefs.LFSTK_saveVarsToFile("-");
					mbuffer.mType=LFSWM2_MSG;
					sprintf(mbuffer.mText,"reloadtheme");
					if((msgsnd(queueID,&mbuffer,strlen(mbuffer.mText)+1,0))==-1)
						fprintf(stderr,"Can't send message :(\n");
					return(true);
				}
		}
	return(true);
}

bool menuCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	placeWindowEdit->LFSTK_setBuffer(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel());
	prefsPlacementTemp=(int)(long)ud;
	return(true);
}

bool resizeMenuCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	resizeWindowEdit->LFSTK_setBuffer(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel());
	prefsResizeTemp=(int)(long)ud;
	return(true);
}

bool modskeyMenuCB(void *p,void* ud)
{
	std::string str=modkeys1WindowMenu->LFSTK_getLabel();

	if((unsigned long)ud<8)
		{
			m1=static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel();
			mk1=(long unsigned)ud;
		}
	else
		{
			m2=static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel();
			mk2=(long unsigned)ud;
		}
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();

	str=m1+"+"+m2;
	modkeys1WindowEdit->LFSTK_setBuffer(str.c_str());
	prefsModkeys1Temp=mk1+mk2;
	return(true);
}

bool titlePosMenuCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	titlePosWindowEdit->LFSTK_setBuffer(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel());
	titlePosTemp=(int)(long)ud+1;
	return(true);
}

bool forceDockStackMenuCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	forceDockStackWindowEdit->LFSTK_setBuffer(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel());
	forceDockStackTemp=(int)(long)ud;
	return(true);
}

bool coleditCB(void *p,void* ud)
{
	int					pw=parentWindow;
	LFSTK_lineEditClass	*ed=static_cast<LFSTK_lineEditClass*>(p);

	if(ed==NULL)
		return(true);

	if(pw==-1)
		pw=wc->window;

	if((ed->mouseEvent->state & Button3Mask)!=0)
		{
			char *col=NULL;
#ifdef _ENABLEDEBUG_
			col=apc->globalLib->LFSTK_oneLiner("LD_LIBRARY_PATH=../LFSToolKit/LFSToolKit/app/.libs LFSApplications/app/lfscolourchooser -w %i \"%s\"",pw,ed->LFSTK_getCStr());
#else
			col=apc->globalLib->LFSTK_oneLiner("lfscolourchooser -w %i \"%s\"",pw,ed->LFSTK_getCStr());
#endif
			if(strlen(col)>0)
				ed->LFSTK_setBuffer(col);
			free(col);
			for(int j=0;j<COLOURBUTTONS;j++)
				{
					previewButtons[j]->LFSTK_setColourName(NORMALCOLOUR,previeColourEdit[j]->LFSTK_getCStr());
					previewButtons[j]->LFSTK_setFontColourName(NORMALCOLOUR,"black",false);
					previewButtons[j]->LFSTK_clearWindow();
				}
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent		event;
	int			sy=0;
	int			sx=BORDER;
	int			c=0;
	int			option_index=0;
	bool			flag=false;
	int			retcode;
	int			receiveType=IPC_NOWAIT;
	msgBuffer	mbuffer;
	char			*buffer=NULL;

	const char	*shortOpts="h?w:";		
	option		longOptions[]=
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
						printf("Right click in a colour edit box for a colour chooser.\n");
						exit(0);
					case 'w':
						parentWindow=atoi(optarg);
						break;
				}
		}

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL);
	wc=apc->mainWindow;

	buffer=wc->globalLib->LFSTK_oneLiner("sed -n '2p' %s/lfsappearance.rc",apc->configDir);
	if((queueID=msgget(atoi(buffer),IPC_CREAT|0660))==-1)
		fprintf(stderr,"Can't create message queue :( ...\n");
	free(buffer);
	while(flag==false)
		{
			retcode=msgrcv(queueID,&mbuffer,MAX_MSG_SIZE,MSGANY,receiveType);
			if(retcode<=1)
				flag=true;
		}

	prefs.prefsMap=
		{
			{prefs.LFSTK_hashFromKey("theme"),{TYPESTRING,"theme","",false,0}},
			{prefs.LFSTK_hashFromKey("desktops"),{TYPEINT,"desktops","",false,6}},
			{prefs.LFSTK_hashFromKey("placement"),{TYPEINT,"placement","",false,2}},
			{prefs.LFSTK_hashFromKey("titlefont"),{TYPESTRING,"titlefont","sans:size=14:bold",false,0}},
			{prefs.LFSTK_hashFromKey("titleposition"),{TYPEINT,"titleposition","",false,2}},
			{prefs.LFSTK_hashFromKey("titlebarsize"),{TYPEINT,"titlebarsize","",false,20}},
			{prefs.LFSTK_hashFromKey("leftsidebarsize"),{TYPEINT,"leftsidebarsize","",false,2}},
			{prefs.LFSTK_hashFromKey("ritesidebarsize"),{TYPEINT,"ritesidebarsize","",false,2}},
			{prefs.LFSTK_hashFromKey("bottombarsize"),{TYPEINT,"bottombarsize","",false,8}},
			{prefs.LFSTK_hashFromKey("framebg"),{TYPESTRING,"framebg","grey",false,15}},
			{prefs.LFSTK_hashFromKey("framefg"),{TYPESTRING,"framefg","white",false,0}},
			{prefs.LFSTK_hashFromKey("textcolour"),{TYPESTRING,"textcolour","black",false,0}},
			{prefs.LFSTK_hashFromKey("forcedocksstack"),{TYPEINT,"forcedocksstack","",false,1}},
			{prefs.LFSTK_hashFromKey("rescanprefs"),{TYPEINT,"rescanprefs","",false,10}},
			{prefs.LFSTK_hashFromKey("usetheme"),{TYPEBOOL,"usetheme","",false,0}},
			{prefs.LFSTK_hashFromKey("resizemode"),{TYPEINT,"resizemode","",false,2}},
			{prefs.LFSTK_hashFromKey("modkeys"),{TYPEINT,"modkeys","",false,64}}
		};

	asprintf(&envFile,"%s/lfswm2.rc",apc->configDir);
	prefs.LFSTK_loadVarsFromFile(envFile);

	prefsPlacementTemp=prefs.LFSTK_getInt("placement");
	prefsResizeTemp=prefs.LFSTK_getInt("resizemode");
	titlePosTemp=prefs.LFSTK_getInt("titleposition");
	forceDockStackTemp=prefs.LFSTK_getInt("forcedocksstack");

	themeFolder=new LFSTK_fileDialogClass(wc,"Select File",NULL,FOLDERDIALOG,"lfswmprefstheme");

	placementMenus=new menuStruct*[MAXMENUS];
	for(long j=0;j<MAXMENUS;j++)
		{
			placementMenus[j]=new menuStruct;
			placementMenus[j]->label=strdup(placementMenuNames[j]);
			placementMenus[j]->userData=(void*)j;
		}

	resizeMenus=new menuStruct*[RESIZEMENUSIZE];
	for(long j=0;j<RESIZEMENUSIZE;j++)
		{
			resizeMenus[j]=new menuStruct;
			resizeMenus[j]->label=strdup(resizeMenuNames[j]);
			resizeMenus[j]->userData=(void*)j;
		}

	titlePosMenus=new menuStruct*[TITLEPOSMENUSIZE];
	for(long j=0;j<TITLEPOSMENUSIZE;j++)
		{
			titlePosMenus[j]=new menuStruct;
			titlePosMenus[j]->label=strdup(titlePosMenuNames[j]);
			titlePosMenus[j]->userData=(void*)j;
		}

	forceDockStackMenus=new menuStruct*[FORCEMENUSIZE];
	for(long j=0;j<FORCEMENUSIZE;j++)
		{
			forceDockStackMenus[j]=new menuStruct;
			forceDockStackMenus[j]->label=strdup(forceDockStackMenuNames[j]);
			forceDockStackMenus[j]->userData=(void*)j;
		}

	modkeys1Menus=new menuStruct*[MODS1MENUSIZE];
	for(long j=0;j<MODS1MENUSIZE;j++)
		{
			modkeys1Menus[j]=new menuStruct;
			modkeys1Menus[j]->label=strdup(modkeys1MenuNames[j]);
			//if(j==0)
				modkeys1Menus[j]->userData=(void*)j;
			//	modkeys1Menus[j]->userData=0;
			//else
				//modkeys1Menus[j]->userData=(void*)(unsigned long)(1<<(j-1));
			
			//fprintf(stderr,"menu ud=%x\n",modkeys1Menus[j]->userData);
		}
	modkeys2Menus=new menuStruct*[MODS2MENUSIZE];
	for(long j=0;j<MODS2MENUSIZE;j++)
		{
			modkeys2Menus[j]=new menuStruct;
			modkeys2Menus[j]->label=strdup(modkeys2MenuNames[j]);
			modkeys2Menus[j]->userData=(void*)(unsigned long)(1<<(j+3));
			//modkeys2Menus[j]->userData=(void*)(unsigned long)(1<<(j));
		}

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//frame colours
	for(int j=0;j<COLOURBUTTONS;j++)
		{
			previewButtons[j]=new LFSTK_buttonClass(wc,previewButtonLabels[j],sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
			previewButtons[j]->LFSTK_setLabelAutoColour(true);
			previewButtons[j]->LFSTK_setTile(NULL,0);
			previewButtons[j]->LFSTK_setColourName(NORMALCOLOUR,prefs.LFSTK_getCString(prefsnames[j]));
			previewButtons[j]->LFSTK_setFontColourName(NORMALCOLOUR,"black",false);
			previewButtons[j]->LFSTK_setIgnores(false,true);
			sx+=GADGETWIDTH+BORDER;
			previeColourEdit[j]=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString(prefsnames[j]),sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
			previeColourEdit[j]->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
			previeColourEdit[j]->LFSTK_setContextWindow(NULL);

			sy+=YSPACING;
			sx=BORDER;
		}

//title bar size
	label=new LFSTK_labelClass(wc,"Title Bar Size",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	sx+=GADGETWIDTH+BORDER;
	titleBarSizeEdit=new LFSTK_lineEditClass(wc,std::to_string(prefs.LFSTK_getInt("titlebarsize")).c_str(),sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//bottom bar size
	label=new LFSTK_labelClass(wc,"Bottom Bar Size",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	sx+=GADGETWIDTH+BORDER;
	bottomBarSizeEdit=new LFSTK_lineEditClass(wc,std::to_string(prefs.LFSTK_getInt("bottombarsize")).c_str(),sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//left bar size
	label=new LFSTK_labelClass(wc,"Left Bar Size",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	sx+=GADGETWIDTH+BORDER;
	leftBarSizeEdit=new LFSTK_lineEditClass(wc,std::to_string(prefs.LFSTK_getInt("leftsidebarsize")).c_str(),sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//right bar size
	label=new LFSTK_labelClass(wc,"Right Bar Size",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	sx+=GADGETWIDTH+BORDER;
	rightBarSizeEdit=new LFSTK_lineEditClass(wc,std::to_string(prefs.LFSTK_getInt("ritesidebarsize")).c_str(),sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//font
	fontSelector=new LFSTK_fontDialogClass(wc,"Select Font",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	fontSelector->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SELECTFONT");
	sx+=GADGETWIDTH+BORDER;
	fontEdit=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("titlefont"),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//title position
	titlePosWindowMenu=new LFSTK_buttonClass(wc,"Title Position",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	titlePosWindowMenu->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"TITLEPOS");
	titlePosMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	titlePosMenu->LFSTK_setMouseCallBack(NULL,titlePosMenuCB,NULL);
	titlePosMenu->LFSTK_addMainMenus(titlePosMenus,TITLEPOSMENUSIZE);

	sx+=GADGETWIDTH+BORDER;
	titlePosWindowEdit=new LFSTK_lineEditClass(wc,titlePosMenuNames[prefs.LFSTK_getInt("titleposition")-1],sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//use theme
	useTheme=new LFSTK_toggleButtonClass(wc,"Use Theme",BORDER,sy,GADGETWIDTH,CHECKBOXSIZE,NorthWestGravity);
	useTheme->LFSTK_setValue(prefs.LFSTK_getBool("usetheme"));
	sy+=YSPACING-8;
	sx=BORDER;

//theme
	themeButton=new LFSTK_buttonClass(wc,"Select Theme",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	themeButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SELECTTHEME");
	sx+=GADGETWIDTH+BORDER;
	themeEdit=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("theme"),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//placement
	placeWindowMenu=new LFSTK_buttonClass(wc,"Placement",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	placeWindowMenu->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWPLACEMENU");
	placeMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	placeMenu->LFSTK_setMouseCallBack(NULL,menuCB,NULL);
	placeMenu->LFSTK_addMainMenus(placementMenus,MAXMENUS);

	sx+=GADGETWIDTH+BORDER;
	placeWindowEdit=new LFSTK_lineEditClass(wc,placementMenuNames[prefs.LFSTK_getInt("placement")],sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//desks
	label=new LFSTK_labelClass(wc,"Desktops",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	sx+=GADGETWIDTH+BORDER;
	deskCountEdit=new LFSTK_lineEditClass(wc,std::to_string(prefs.LFSTK_getInt("desktops")).c_str(),sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//resize mode
	resizeWindowMenu=new LFSTK_buttonClass(wc,"Resisize Mode",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	resizeWindowMenu->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"RESIZEMODE");
	resizeMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	resizeMenu->LFSTK_setMouseCallBack(NULL,resizeMenuCB,NULL);
	resizeMenu->LFSTK_addMainMenus(resizeMenus,RESIZEMENUSIZE);

	sx+=GADGETWIDTH+BORDER;
	resizeWindowEdit=new LFSTK_lineEditClass(wc,resizeMenuNames[prefs.LFSTK_getInt("resizemode")],sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//rescan prefs
	label=new LFSTK_labelClass(wc,"Rescan Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	sx+=GADGETWIDTH+BORDER;
	rescanEdit=new LFSTK_lineEditClass(wc,std::to_string(prefs.LFSTK_getInt("rescanprefs")).c_str(),sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//force dock stack order
	forceDockStackWindowMenu=new LFSTK_buttonClass(wc,"Dock Stacking",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	forceDockStackWindowMenu->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"FORCEDOCKSTACK");
	forceDockStackMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	forceDockStackMenu->LFSTK_setMouseCallBack(NULL,forceDockStackMenuCB,NULL);
	forceDockStackMenu->LFSTK_addMainMenus(forceDockStackMenus,FORCEMENUSIZE);

	sx+=GADGETWIDTH+BORDER;
	forceDockStackWindowEdit=new LFSTK_lineEditClass(wc,forceDockStackMenuNames[prefs.LFSTK_getInt("forcedocksstack")],sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//modkeys1
	modkeys1WindowMenu=new LFSTK_buttonClass(wc,"Mod Keys 1",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	modkeys1WindowMenu->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"MODKEYS1");
	modkeys1Menu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	modkeys1Menu->LFSTK_setMouseCallBack(NULL,modskeyMenuCB,NULL);
	modkeys1Menu->LFSTK_addMainMenus(modkeys1Menus,MODS1MENUSIZE);
	sx+=GADGETWIDTH+BORDER;

//modkeys2
	modkeys2WindowMenu=new LFSTK_buttonClass(wc,"Mod Keys 2",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	modkeys2WindowMenu->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"MODKEYS2");
	modkeys2Menu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	modkeys2Menu->LFSTK_setMouseCallBack(NULL,modskeyMenuCB,NULL);
	modkeys2Menu->LFSTK_addMainMenus(modkeys2Menus,MODS2MENUSIZE);

	sx+=GADGETWIDTH+BORDER;
	std::string name;

	prefsModkeys1Temp=prefs.LFSTK_getInt("modkeys");

	mk1=prefsModkeys1Temp&7;
	name=modkeys1MenuNames[prefsModkeys1Temp&7];
	m1=name;
	name+="+";
	
	int cnt=0;
	int mult=3;
	for(int j=0;j<6;j++)
		{
			if(((((prefs.LFSTK_getInt("modkeys")&0xf8)>>mult)) & 1)==1)
				{
					cnt=j;
					break;
				}
			mult++;
		}
	name+=modkeys2MenuNames[cnt];
	m2=modkeys2MenuNames[cnt];
	mk2=prefs.LFSTK_getInt("modkeys")&0xf8;

	prefsModkeys1Temp=prefs.LFSTK_getInt("modkeys");
	modkeys1WindowEdit=new LFSTK_lineEditClass(wc,name.c_str(),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;


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
	apply->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"APPLY");
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();

	delete themeFolder;
	delete placeMenu;
	free(envFile);
	delete apc;
	return(retval);
}