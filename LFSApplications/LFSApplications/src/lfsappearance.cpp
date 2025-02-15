/*
 *
 * ©K. D. Hedger. Tue 15 Aug 17:26:09 BST 2017 keithdhedger@gmail.com

 * This file (lfsappearance.cpp) is part of LFSApplications.

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

#include "config.h"
#include <lfstk/LFSTKGlobals.h>

#undef GADGETWIDTH
#undef GADGETHITE
#undef HALFGADGETWIDTH
#undef DIALOGWIDTH
#undef DIALOGMIDDLE

#define GADGETWIDTH		96
#define GADGETHITE		24
#define HALFGADGETWIDTH	GADGETWIDTH/2
#define LABELWIDTH		(GADGETWIDTH*2)+BORDER

#define DIALOGWIDTH		BORDER*2+GADGETWIDTH+LABELWIDTH
#define DIALOGMIDDLE	DIALOGWIDTH/2

#define BOXLABEL		"LFS Appearance"

LFSTK_applicationClass		*apc=NULL;
LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass				*personal=NULL;
LFSTK_labelClass				*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_buttonClass			*apply=NULL;
LFSTK_buttonClass			*update=NULL;

LFSTK_buttonClass			*launchButton=NULL;
LFSTK_labelClass				*launchLabel=NULL;

LFSTK_menuClass				*setMenu=NULL;
LFSTK_buttonClass			*loadSet=NULL;
LFSTK_lineEditClass			*currentSet=NULL;

LFSTK_lineEditClass			*key=NULL;
LFSTK_ExpanderGadgetClass	*multi=NULL;

int							queueID=-1;

infoDataStruct				**groupNameMenuItems=NULL;
LFSTK_findClass				*find;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

int fileExists(const char *name)
{
	struct stat buffer;
	return (stat(name,&buffer));
}

void updateDesktop(void)
{
	bool	flag=false;
	int		retcode;
	int		receiveType=IPC_NOWAIT;
	msgBuffer	buffer;

//flush message queue
	while(flag==false)
		{
			retcode=msgrcv(queueID,&buffer,MAX_MSG_SIZE,MSGANY,receiveType);
			if(retcode<=1)
				flag=true;
		}

	system("lfssetwallpaper &>/dev/null");

	buffer.mType=DESKTOP_MSG;
	sprintf(buffer.mText,"reloadbg");
	if((msgsnd(queueID,&buffer,strlen(buffer.mText)+1,0))==-1)
		fprintf(stderr,"Can't send message :(\n");

	sprintf(buffer.mText,"reloadprefs");
	if((msgsnd(queueID,&buffer,strlen(buffer.mText)+1,0))==-1)
		fprintf(stderr,"Can't send message :(\n");

	buffer.mType=WMANAGER_MSG;
	sprintf(buffer.mText,"reloadtheme");
	if((msgsnd(queueID,&buffer,strlen(buffer.mText)+1,0))==-1)
		fprintf(stderr,"Can't send message :(\n");
	
	system("killall lfspanel lfsdock &");
}

void makeGroup(const char *grpname)
{
	char	*grp;
	char	*command;

	asprintf(&grp,"%s/lfsgroupsprefs/%s",apc->configDir.c_str(),grpname);
	if(fileExists(grp)!=0)
		mkdir(grp,0755);

	asprintf(&command,"cp \"%s\"/lfs*.rc \"%s\"",apc->configDir.c_str(),grp);
	system(command);

	free(command);
	free(grp);
	asprintf(&command,"echo -e \"%s\n%s\" > \"%s/lfsappearance.rc\"",currentSet->LFSTK_getCStr(),key->LFSTK_getCStr(),apc->configDir.c_str());
	system(command);
	free(command);
}

void setGroup(void)
{
	char	*command;
	char	*grp;

	asprintf(&grp,"%s/lfsgroupsprefs/%s",apc->configDir.c_str(),currentSet->LFSTK_getCStr());
	if(fileExists(grp)==0)
		{
			asprintf(&command,"cp \"%s/lfsgroupsprefs/%s/\"lfs*.rc \"%s\"",apc->configDir.c_str(),currentSet->LFSTK_getCStr(),apc->configDir.c_str());
			system(command);
			free(command);
		}
	free(grp);

	asprintf(&command,"echo -e \"%s\n%s\" > \"%s/lfsappearance.rc\"",currentSet->LFSTK_getCStr(),key->LFSTK_getCStr(),apc->configDir.c_str());
	system(command);
	free(command);
}

bool buttonCB(void *p,void* ud)
{
	char	*command;
#ifdef _ENABLEDEBUG_
	const char	*libpath="LD_LIBRARY_PATH=../LFSToolKit/LFSToolKit/app/.libs LFSApplications/app/";
#else
	const char	*libpath="";
#endif

	if(ud!=NULL)
		{
			if(strcmp((char*)ud,"WALLPAPERPREFS")==0)
				{
					asprintf(&command,"%slfsbackdropprefs -w %i  &",libpath,wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"TOOLKITPREFS")==0)
				{
					asprintf(&command,"%slfstkprefs -w %i &",libpath,wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"DESKTOPPREFS")==0)
				{
					asprintf(&command,"%slfsdesktopprefs -w %i  &",libpath,wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"WMPREFS")==0)
				{
					asprintf(&command,"%slfswmpref -w %i s &",libpath,wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"LFSWM2PREFS")==0)
				{
					asprintf(&command,"%slfswm2prefs -w %i &",libpath,wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"PANELPREFS")==0)
				{
					asprintf(&command,"%slfspanelprefs -w %i  &",libpath,wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"DOCKPREFS")==0)
				{
					asprintf(&command,"%slfsdockprefs -w %i  &",libpath,wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"TRAYPREFS")==0)
				{
					asprintf(&command,"%slfstrayprefs -w %i &",libpath,wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"UPDATE")==0)
				{
					makeGroup(currentSet->LFSTK_getCStr());
					updateDesktop();
				}
			if(strcmp((char*)ud,"APPLY")==0)
				{
					setGroup();
					updateDesktop();
				}
			if(strcmp((char*)ud,"SHOWGROUPS")==0)
				{
					setMenu->LFSTK_showMenu();
				}

		}
	return(true);
}

void addGroup(void)
{
	char	*command;
	int		menucnt;

	asprintf(&command,"%s/lfsgroupsprefs",apc->configDir.c_str());

	find->LFSTK_findFiles(command,false);
	find->LFSTK_sortByName();
	menucnt=find->LFSTK_getDataCount()+2;

	groupNameMenuItems=new infoDataStruct*[menucnt];
	groupNameMenuItems[0]=new infoDataStruct;
	groupNameMenuItems[0]->label=strdup("Add Group");
	groupNameMenuItems[1]=new infoDataStruct;
	groupNameMenuItems[1]->label=strdup("--");
	
	for(int j=2;j<menucnt;j++)
		{
			groupNameMenuItems[j]=new infoDataStruct;
			groupNameMenuItems[j]->label=strdup(find->data[j-2].name.c_str());
		}
	free(command);
}

void doNewGroup(void)
{
	makeGroup(currentSet->LFSTK_getCStr());
	addGroup();
	setMenu->LFSTK_addMainMenus(groupNameMenuItems,find->LFSTK_getDataCount()+2);
}

bool menuCB(void *p,void* ud)
{
	if(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel().compare("Add Group")==0)
		{
			doNewGroup();
			return(true);
		}
	currentSet->LFSTK_setBuffer(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel());
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();

	return(true);
}

int main(int argc, char **argv)
{
	XEvent				event;
	int					sy=0;
	int					sx=BORDER;
	char					*buffer=NULL;
	std::vector<hitRect>	hrs;

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKPrefs");
	wc=apc->mainWindow;

	multi=new LFSTK_ExpanderGadgetClass(wc,"",0,0,1,1);
	multi->stretchX=false;
	multi->stretchY=false;
	multi->lockY=LOCKTOTOP;
	multi->lockX=LOCKTOCENTRE;
	multi->gadgetStretch=STRETCH;

	find=new LFSTK_findClass;
	find->LFSTK_setFindType(FOLDERTYPE);
	find->LFSTK_setFullPath(true);
	find->LFSTK_setIgnoreBroken(false);
	find->LFSTK_setSortDecending(false);
	find->LFSTK_setIgnoreNavLinks(true);

	addGroup();
	
	multi->LFSTK_setHitRects(hrs);
	hrs.clear();

	hrs.push_back({0,sy,DIALOGWIDTH,GADGETHITE,NULL});

	copyrite=new LFSTK_labelClass(wc,COPYRITE,0,0,1,1);
	copyrite->LFSTK_setLabelGravity(CENTRE);
	hrs.back().gadget=copyrite;
	sy+=HALFYSPACING;

	hrs.push_back({0,sy,DIALOGWIDTH,GADGETHITE,NULL});
	personal=new LFSTK_labelClass(wc,PERSONAL,0,0,1,1);
	personal->LFSTK_setLabelGravity(CENTRE);
	hrs.back().gadget=personal;
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//launch sub-prefs
//wallpaper
	launchButton=new LFSTK_buttonClass(wc,"Wallpaper Prefs",0,0,1,1,BUTTONGRAV);
	launchButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"WALLPAPERPREFS");
	hrs.push_back({BORDER,sy,GADGETWIDTH,GADGETHITE,launchButton});
	launchLabel=new LFSTK_labelClass(wc,"Launch Wallpaper Prefs Dialog",0,0,1,1,LEFT);
	hrs.push_back({BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,launchLabel});
	sy+=YSPACING;

//toolkit
	launchButton=new LFSTK_buttonClass(wc,"Toolkit Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"TOOLKITPREFS");
	hrs.push_back({BORDER,sy,GADGETWIDTH,GADGETHITE,launchButton});
	launchLabel=new LFSTK_labelClass(wc,"Launch Toolkit Prefs Dialog",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,LEFT);
	hrs.push_back({BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,launchLabel});
	sy+=YSPACING;

//desktop
	launchButton=new LFSTK_buttonClass(wc,"Desktop Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"DESKTOPPREFS");
	hrs.push_back({BORDER,sy,GADGETWIDTH,GADGETHITE,launchButton});
	launchLabel=new LFSTK_labelClass(wc,"Launch Desktop Prefs Dialog",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,LEFT);
	hrs.push_back({BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,launchLabel});
	sy+=YSPACING;

//wmanager
//defunct
//	launchButton=new LFSTK_buttonClass(wc,"WM Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
//	launchButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"WMPREFS");
//	hrs.push_back({BORDER,sy,GADGETWIDTH,GADGETHITE,launchButton});
//	launchLabel=new LFSTK_labelClass(wc,"Launch Window Prefs Dialog",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,LEFT);
//	launchLabel->LFSTK_setLabelGravity(LEFT);
//	hrs.push_back({BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,launchLabel});
//	sy+=YSPACING;

//lfswm2
	launchButton=new LFSTK_buttonClass(wc,"LFSWM2 Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"LFSWM2PREFS");
	hrs.push_back({BORDER,sy,GADGETWIDTH,GADGETHITE,launchButton});
	launchLabel=new LFSTK_labelClass(wc,"Launch LFSWM2 Prefs Dialog",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,LEFT);
	launchLabel->LFSTK_setLabelGravity(LEFT);
	hrs.push_back({BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,launchLabel});
	sy+=YSPACING;

//panel
	launchButton=new LFSTK_buttonClass(wc,"Panel Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"PANELPREFS");
	hrs.push_back({BORDER,sy,GADGETWIDTH,GADGETHITE,launchButton});
	launchLabel=new LFSTK_labelClass(wc,"Launch Panel Prefs Dialog",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,LEFT);
	hrs.push_back({BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,launchLabel});
	sy+=YSPACING;

//dock
	launchButton=new LFSTK_buttonClass(wc,"Dock Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"DOCKPREFS");
	hrs.push_back({BORDER,sy,GADGETWIDTH,GADGETHITE,launchButton});
	launchLabel=new LFSTK_labelClass(wc,"Launch Dock Prefs Dialog",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,LEFT);
	hrs.push_back({BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,launchLabel});
	sy+=YSPACING;

//tray
	launchButton=new LFSTK_buttonClass(wc,"Tray Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"TRAYPREFS");
	hrs.push_back({BORDER,sy,GADGETWIDTH,GADGETHITE,launchButton});
	launchLabel=new LFSTK_labelClass(wc,"Launch Tray Prefs Dialog",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,LEFT);
	hrs.push_back({BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,launchLabel});
	sy+=YSPACING;

//load set
	loadSet=new LFSTK_buttonClass(wc,"Load Set",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	loadSet->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWGROUPS");
	loadSet->LFSTK_setIndicator(DISCLOSURE);
	hrs.push_back({BORDER,sy,GADGETWIDTH,GADGETHITE,loadSet});

	setMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	setMenu->LFSTK_setMouseCallBack(NULL,menuCB,NULL);
	setMenu->LFSTK_addMainMenus(groupNameMenuItems,find->LFSTK_getDataCount()+2);

	buffer=strdup(wc->globalLib->LFSTK_oneLiner("sed -n '1p' %S/lfsappearance.rc",apc->configDir).c_str());
	currentSet=new LFSTK_lineEditClass(wc,buffer,BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,BUTTONGRAV);
	hrs.push_back({BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,currentSet});
	free(buffer);
	sy+=YSPACING;

//msg key//TODO//
	buffer=strdup(wc->globalLib->LFSTK_oneLiner("sed -n '2p' %S/lfsappearance.rc",apc->configDir).c_str());
	launchLabel=new LFSTK_labelClass(wc,"Msg Key",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	hrs.push_back({BORDER,sy,GADGETWIDTH,GADGETHITE,launchLabel});
	key=new LFSTK_lineEditClass(wc,buffer,BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,BUTTONGRAV);
	hrs.push_back({BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,key});
	free(buffer);
	sy+=YSPACING;

//line
//	seperator=new LFSTK_buttonClass(wc,"--",0,sy,10000,GADGETHITE,BUTTONGRAV);
//	seperator->LFSTK_setStyle(BEVELNONE);
//	seperator->gadgetDetails.buttonTile=false;
//	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
//	hrs.push_back({0,sy,10000,GADGETHITE,seperator});//HHMMMMmmmmm
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	hrs.push_back({sx,sy,GADGETWIDTH,GADGETHITE,quit});
	sx+=GADGETWIDTH+BORDER;
//update
	update=new LFSTK_buttonClass(wc,"Update",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	update->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"UPDATE");
	hrs.push_back({sx,sy,GADGETWIDTH,GADGETHITE,update});
	sx+=GADGETWIDTH+BORDER;
//apply
	apply=new LFSTK_buttonClass(wc,"Apply",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	apply->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"APPLY");
	hrs.push_back({sx,sy,GADGETWIDTH,GADGETHITE,apply});

	sy+=YSPACING;
	multi->LFSTK_setGadgetSize(DIALOGWIDTH,sy);
	multi->LFSTK_setHitRects(hrs);

	wc->LFSTK_resizeWindow(BORDER*3+GADGETWIDTH+LABELWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

	if((queueID=msgget(atoi(key->LFSTK_getCStr()),IPC_CREAT|0660))==-1)
		fprintf(stderr,"Can't create message queue :( ...\n");

	int retval=apc->LFSTK_runApp();

	delete setMenu;
	delete apc;
	return(retval);
}