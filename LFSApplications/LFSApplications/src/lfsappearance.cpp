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

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_buttonClass		*apply=NULL;
LFSTK_buttonClass		*update=NULL;

LFSTK_buttonClass		*launchButton=NULL;
LFSTK_labelClass		*launchLabel=NULL;

LFSTK_menuClass			*setMenu=NULL;
LFSTK_buttonClass		*loadSet=NULL;
LFSTK_lineEditClass		*currentSet=NULL;

LFSTK_lineEditClass		*key=NULL;

int						queueID=-1;

menuStruct				**groupNameMenuItems=NULL;
LFSTK_findClass			*find;

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
	
	system("killall lfspanel &");	
}

void makeGroup(const char *grpname)
{
	char	*grp;
	char	*command;

	asprintf(&grp,"%s/lfsgroupsprefs/%s",apc->configDir,grpname);
	if(fileExists(grp)!=0)
		mkdir(grp,0755);

	asprintf(&command,"cp \"%s\"/lfs*.rc \"%s\"",apc->configDir,grp);
	system(command);

	free(command);
	free(grp);
	asprintf(&command,"echo -e \"%s\n%s\" > \"%s/lfsappearance.rc\"",currentSet->LFSTK_getCStr(),key->LFSTK_getCStr(),apc->configDir);
	system(command);
	free(command);
}

void setGroup(void)
{
	char	*command;
	char	*grp;

	asprintf(&grp,"%s/lfsgroupsprefs/%s",apc->configDir,currentSet->LFSTK_getCStr());
	if(fileExists(grp)==0)
		{
			asprintf(&command,"cp \"%s/lfsgroupsprefs/%s/\"lfs*.rc \"%s\"",apc->configDir,currentSet->LFSTK_getCStr(),apc->configDir);
			system(command);
			free(command);
		}
	free(grp);

	asprintf(&command,"echo -e \"%s\n%s\" > \"%s/lfsappearance.rc\"",currentSet->LFSTK_getCStr(),key->LFSTK_getCStr(),apc->configDir);
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
					asprintf(&command,"%slfsbackdropprefs &",libpath,wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"TOOLKITPREFS")==0)
				{
					asprintf(&command,"%slfstkprefs &",libpath,wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"DESKTOPPREFS")==0)
				{
					asprintf(&command,"%slfsdesktopprefs &",libpath,wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"WMPREFS")==0)
				{
					asprintf(&command,"%slfswmprefs &",libpath,wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"PANELPREFS")==0)
				{
					asprintf(&command,"%slfspanelprefs &",libpath,wc->window);
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

	asprintf(&command,"%s/lfsgroupsprefs",apc->configDir);

	find->LFSTK_findFiles(command,false);
	find->LFSTK_sortByName();
	menucnt=find->LFSTK_getDataCount()+2;

	groupNameMenuItems=new menuStruct*[menucnt];
	groupNameMenuItems[0]=new menuStruct;
	groupNameMenuItems[0]->label=strdup("Add Group");
	groupNameMenuItems[1]=new menuStruct;
	groupNameMenuItems[1]->label=strdup("--");
	
	for(int j=2;j<menucnt;j++)
		{
			groupNameMenuItems[j]=new menuStruct;
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
	if(strcmp(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel(),"Add Group")==0)
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
	XEvent		event;
	int			sy=0;
	int			sx=BORDER;
	char		*buffer=NULL;

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL);
	wc=apc->mainWindow;

	find=new LFSTK_findClass;
	find->LFSTK_setFindType(FOLDERTYPE);
	find->LFSTK_setFullPath(true);
	find->LFSTK_setIgnoreBroken(false);
	find->LFSTK_setSort(true);
	find->LFSTK_setIgnoreNavLinks(true);

	addGroup();

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//launch sub-prefs
//wallpaper
	launchButton=new LFSTK_buttonClass(wc,"Wallpaper Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"WALLPAPERPREFS");
	launchLabel=new LFSTK_labelClass(wc,"Launch Wallpaper Prefs Dialog",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,LEFT);
	sy+=YSPACING;
//toolkit
	launchButton=new LFSTK_buttonClass(wc,"Toolkit Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"TOOLKITPREFS");
	launchLabel=new LFSTK_labelClass(wc,"Launch Toolkit Prefs Dialog",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,LEFT);
	sy+=YSPACING;
//desktop
	launchButton=new LFSTK_buttonClass(wc,"Desktop Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"DESKTOPPREFS");
	launchLabel=new LFSTK_labelClass(wc,"Launch Desktop Prefs Dialog",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,LEFT);
	sy+=YSPACING;
//wmanager
	launchButton=new LFSTK_buttonClass(wc,"WM Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"WMPREFS");
	launchLabel=new LFSTK_labelClass(wc,"Launch Window Prefs Dialog",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,LEFT);
	launchLabel->LFSTK_setLabelGravity(LEFT);
	sy+=YSPACING;
//panel
	launchButton=new LFSTK_buttonClass(wc,"Panel Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"PANELPREFS");
	launchLabel=new LFSTK_labelClass(wc,"Launch Panel Prefs Dialog",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,LEFT);
	sy+=YSPACING;

//load set
	loadSet=new LFSTK_buttonClass(wc,"Load Set",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	loadSet->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWGROUPS");
	loadSet->LFSTK_setIndicator(DISCLOSURE);

	setMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	setMenu->LFSTK_setMouseCallBack(NULL,menuCB,NULL);
	setMenu->LFSTK_addMainMenus(groupNameMenuItems,find->LFSTK_getDataCount()+2);

	buffer=wc->globalLib->LFSTK_oneLiner("sed -n '1p' %s/lfsappearance.rc",apc->configDir);
	currentSet=new LFSTK_lineEditClass(wc,buffer,BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,BUTTONGRAV);
	free(buffer);
	sy+=YSPACING;

//msg key
	buffer=wc->globalLib->LFSTK_oneLiner("sed -n '2p' %s/lfsappearance.rc",apc->configDir);
	launchLabel=new LFSTK_labelClass(wc,"Msg Key",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	key=new LFSTK_lineEditClass(wc,buffer,BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,BUTTONGRAV);
	free(buffer);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	sx+=GADGETWIDTH+BORDER;
//update
	update=new LFSTK_buttonClass(wc,"Update",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	update->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"UPDATE");
	sx+=GADGETWIDTH+BORDER;
//apply
	apply=new LFSTK_buttonClass(wc,"Apply",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	apply->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"APPLY");

	sy+=YSPACING;

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