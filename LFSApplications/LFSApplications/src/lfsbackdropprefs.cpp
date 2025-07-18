/*
 *
 * ©K. D. Hedger. Fri 18 Aug 16:52:24 BST 2017 keithdhedger@gmail.com

 * This file (lfsbackdropprefs.cpp) is part of LFSApplications.

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

#include <libgen.h>

#define BOXLABEL		"LFS Backdrop Prefs"

struct					monitorInfo
{
	char		*path;
	int		mode;
};

LFSTK_applicationClass	*apc=NULL;
LFSTK_prefsClass			prefs;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_buttonClass		*apply=NULL;

//file select
LFSTK_fileDialogClass	*fileDialog;
LFSTK_buttonClass		*dialogButton=NULL;

//root window
LFSTK_lineEditClass		*mainBackdropEdit=NULL;
LFSTK_lineEditClass		*rootColourEdit=NULL;
LFSTK_buttonClass		*mainMode=NULL;
LFSTK_lineEditClass		*mainModeEdit=NULL;
LFSTK_toggleButtonClass	*multipleMonitors=NULL;
LFSTK_menuClass			*rootMenu=NULL;
infoDataStruct				**rootModeMenu=NULL;

//monitors
LFSTK_buttonClass		*monitorsMenuButton=NULL;
LFSTK_menuClass			*monitorSelectMenu=NULL;
infoDataStruct				**monitorsMenu=NULL;

LFSTK_lineEditClass		*monitorModeEdit=NULL;
LFSTK_lineEditClass		*monitorBackdropEdit=NULL;
LFSTK_buttonClass		*monitorMode=NULL;
infoDataStruct				**monitorModes=NULL;
LFSTK_menuClass			*monitorModeMenu=NULL;

monitorInfo				monitors[20]={{NULL,0},};
int						selectedMonitor=0;

char						*wd=NULL;
Window					parentWindow=-1;

//prefs
char						*mainPrefs;
char						*monitorPrefs;
int						backdropMode=0;
bool						multiMode=false;

//modes
const char				*modeLabel[5]={"Stretch","Tile","Centre","Scale","Zoom"};

//msg
int						queueID=-1;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
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
			std::string col;
			col=apc->globalLib->LFSTK_oneLiner("lfscolourchooser -w %i \"%S\"",pw,ed->LFSTK_getBuffer());
			if(col.empty()==false)
				ed->LFSTK_setBuffer(col);
		}
	return(true);
}

bool buttonCB(void *p,void* ud)
{
	FILE	*fd=NULL;
	msgBuffer	buffer;

	if(ud!=NULL)
		{
			if(strcmp((char*)ud,"SHOWMONITORMODESMENU")==0)
				{
					monitorModeMenu->LFSTK_showMenu();
				}
			if(strcmp((char*)ud,"SHOWROOTMENU")==0)
				{
					rootMenu->LFSTK_showMenu();
				}
			if(strcmp((char*)ud,"SHOWMONITORMENU")==0)
				{
					monitorSelectMenu->LFSTK_showMenu();
				}

			if(strcmp((char*)ud,"APPLY")==0)
				{
					prefs.prefsMap=
						{
							{LFSTK_UtilityClass::LFSTK_hashFromKey("backdrop"),{TYPESTRING,"backdrop","",mainBackdropEdit->LFSTK_getCStr(),false,0}},
							{LFSTK_UtilityClass::LFSTK_hashFromKey("colour"),{TYPESTRING,"colour","",rootColourEdit->LFSTK_getCStr(),false,0}},
							{LFSTK_UtilityClass::LFSTK_hashFromKey("mainmode"),{TYPEINT,"mainmode","","",false,backdropMode}},
							{LFSTK_UtilityClass::LFSTK_hashFromKey("multimode"),{TYPEBOOL,"multimode","","",multipleMonitors->LFSTK_getValue(),0}},
						};
					prefs.LFSTK_saveVarsToFile(mainPrefs);
					fd=fopen(monitorPrefs,"w");
					if(fd!=NULL)
						{
							for(long j=0;j<apc->LFSTK_getMonitorCount();j++)
								fprintf(fd,"%i\n%s\n",monitors[j].mode,monitors[j].path);
							fclose(fd);
						}
					system("lfssetwallpaper");
					buffer.mType=DESKTOP_MSG;
					sprintf(buffer.mText,"reloaddesk");
					if((msgsnd(queueID,&buffer,strlen(buffer.mText)+1,0))==-1)
						fprintf(stderr,"Can't send message :(\n");
				}
		}
	return(true);
}

bool selectFile(void *object,void* ud)
{
	char				*filepath;
	LFSTK_lineEditClass	*edbox=static_cast<LFSTK_lineEditClass*>(ud);

	fileDialog->LFSTK_showFileDialog(NULL,"Select A File");//TODO//
	if(fileDialog->LFSTK_isValid()==true)
		{
			asprintf(&filepath,"%s/%s",fileDialog->LFSTK_getCurrentDir().c_str(),fileDialog->LFSTK_getCurrentFile().c_str());
			free(wd);
			wd=strdup(fileDialog->LFSTK_getCurrentDir().c_str());//TODO//
			edbox->LFSTK_setBuffer(filepath);
			free(monitors[selectedMonitor].path);
			monitors[selectedMonitor].path=strdup(filepath);
			free(filepath);
		}
	return(true);
}

bool mainModeCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	mainModeEdit->LFSTK_setBuffer(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel());
	backdropMode=(int)(long)ud;
	return(true);
}

bool monitorModeCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	monitorModeEdit->LFSTK_setBuffer(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel());
	monitors[selectedMonitor].mode=(int)(long)ud;
	return(true);
}

bool monitorMenuCB(void *p,void* ud)
{

	monitorsMenuButton->LFSTK_setLabel(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel());
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	selectedMonitor=(int)(long)ud;
	monitorModeEdit->LFSTK_setBuffer(modeLabel[monitors[selectedMonitor].mode]);
	monitorBackdropEdit->LFSTK_setBuffer(monitors[selectedMonitor].path);
	return(true);
}

void loadMonitorInfo(void)
{
	FILE*	fd=NULL;
	char	*buffer=NULL;
	int		monnum=0;
	char	*monitorrc;
	int		numchars;
	size_t	n;

	asprintf(&monitorrc,"%s/lfsmonitors.rc",apc->configDir.c_str());
	fd=fopen(monitorrc,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer=NULL;
					numchars=getline(&buffer,&n,fd);
					if(numchars!=-1)
						{
							buffer[numchars-1]=0;
							monitors[monnum].mode=atoi(buffer);
							free(buffer);
							buffer=NULL;
							numchars=getline(&buffer,&n,fd);
							buffer[numchars-1]=0;
							monitors[monnum].path=buffer;
							monnum++;
						}
					else
						free(buffer);
				}
			fclose(fd);
		}
	else
		{
			for(int j=0;j<apc->LFSTK_getMonitorCount();j++)
				{
					monitors[j].mode=0;
					monitors[j].path=strdup("");
				}
		}
	free(monitorrc);
}

int main(int argc, char **argv)
{
	XEvent			event;
	int				sy=0;
	std::string		bffr;
	LFSTK_prefsClass	cliprefs("lfsbackdropprefs",VERSION);
	option			longOptions[]={{"window",1,0,'w'},{0, 0, 0, 0}};
	windowInitStruct	*win;

	apc=new LFSTK_applicationClass();
	win=apc->LFSTK_getDefaultWInit();
	win->windowName=BOXLABEL;
	win->windowType=win->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DIALOG"));
	win->level=ABOVEALL;
	apc->LFSTK_addWindow(win,BOXLABEL,"LFSBackdropPrefs");
	wc=apc->mainWindow;
	wc->LFSTK_setDecorations(false,false,false,false);

	cliprefs.prefsMap=
		{
			{LFSTK_UtilityClass::LFSTK_hashFromKey("window"),{TYPEINT,"window","Set transient for window ARG","",false,0}}
		};
	if(cliprefs.LFSTK_argsToPrefs(argc,argv,longOptions,true)==false)
		return(1);
	parentWindow=cliprefs.LFSTK_getInt("window");

	asprintf(&wd,"%s",apc->userHome);
	asprintf(&mainPrefs,"%s/lfssetwallpaper.rc",apc->configDir.c_str());
	asprintf(&monitorPrefs,"%s/lfsmonitors.rc",apc->configDir.c_str());

	fileDialog=new LFSTK_fileDialogClass(wc,"Select File",NULL,FILEDIALOG,"lfsbackdropprefs");

	prefs.prefsMap=
		{
			{LFSTK_UtilityClass::LFSTK_hashFromKey("backdrop"),{TYPESTRING,"backdrop","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("colour"),{TYPESTRING,"colour","","#404040",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("mainmode"),{TYPEINT,"mainmode","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("multimode"),{TYPEBOOL,"multimode","","",false,0}},
		};

	prefs.LFSTK_loadVarsFromFile(mainPrefs);
	//prefs.LFSTK_saveVarsToFile("-");

	copyrite=new LFSTK_labelClass(wc,COPYRITE,0,sy,DIALOGWIDTH,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,0,sy,DIALOGWIDTH,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//root window
//backdrop
	dialogButton=new LFSTK_buttonClass(wc,"Select File",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	mainBackdropEdit=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("backdrop"),(BORDER*2)+GADGETWIDTH,sy,DIALOGWIDTH-(BORDER*3)-GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	dialogButton->LFSTK_setMouseCallBack(NULL,selectFile,(void*)mainBackdropEdit);

	sy+=YSPACING;
//colour
	label=new LFSTK_labelClass(wc,"Root Colour",BORDER,sy,GADGETWIDTH,GADGETHITE);
	rootColourEdit=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("colour"),(BORDER*2)+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	rootColourEdit->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
	rootColourEdit->LFSTK_setContextWindow(NULL);
	
	sy+=YSPACING;
//mode
	rootModeMenu=new infoDataStruct*[5];
	for(long j=0;j<5;j++)
		{
			rootModeMenu[j]=new infoDataStruct;
			rootModeMenu[j]->label=strdup(modeLabel[j]);
			rootModeMenu[j]->userData=(void*)j;
		}

	mainMode=new LFSTK_buttonClass(wc,"Main Mode",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	mainMode->LFSTK_setIndicator(DISCLOSURE);
	mainMode->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWROOTMENU");
	rootMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	rootMenu->LFSTK_setMouseCallBack(NULL,mainModeCB,NULL);
	rootMenu->LFSTK_addMainMenus(rootModeMenu,5);

	mainModeEdit=new LFSTK_lineEditClass(wc,modeLabel[prefs.LFSTK_getInt("mainmode")],(BORDER*2)+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	mainModeEdit->gadgetAcceptsDnD=false;
	sy+=YSPACING;
//multi mode
	multipleMonitors=new LFSTK_toggleButtonClass(wc,"Multiple Monitors",BORDER,sy,GADGETWIDTH*2,CHECKBOXSIZE,BUTTONGRAV);
	multipleMonitors->LFSTK_setValue(prefs.LFSTK_getBool("multimode"));
	sy+=YSPACING;

//monitors
	monitorsMenu=new infoDataStruct*[apc->LFSTK_getMonitorCount()];
	for(long j=0;j<apc->LFSTK_getMonitorCount();j++)
		{
			monitorsMenu[j]=new infoDataStruct;
			//asprintf(&monitorsMenu[j]->label,"Monitor %i",j);
			monitorsMenu[j]->label="Monitor "+std::to_string(j);
			monitorsMenu[j]->userData=(void*)j;
		}
	monitorsMenuButton=new LFSTK_buttonClass(wc,"Monitor 0",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);

	monitorsMenuButton->LFSTK_setIndicator(DISCLOSURE);

	monitorsMenuButton->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWMONITORMENU");
	monitorSelectMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	monitorSelectMenu->LFSTK_setMouseCallBack(NULL,monitorMenuCB,NULL);
	monitorSelectMenu->LFSTK_addMainMenus(monitorsMenu,apc->LFSTK_getMonitorCount());

	loadMonitorInfo();
	sy+=YSPACING;

//monitor info
//backdrop
	dialogButton=new LFSTK_buttonClass(wc,"Select File",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	monitorBackdropEdit=new LFSTK_lineEditClass(wc,monitors[0].path,(BORDER*2)+GADGETWIDTH,sy,DIALOGWIDTH-(BORDER*3)-GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	dialogButton->LFSTK_setMouseCallBack(NULL,selectFile,(void*)monitorBackdropEdit);
	sy+=YSPACING;
//mode
	monitorMode=new LFSTK_buttonClass(wc,"Monitor Mode",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	monitorMode->LFSTK_setIndicator(DISCLOSURE);
	monitorMode->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SHOWMONITORMODESMENU");
	monitorModes=new infoDataStruct*[5];
	for(long j=0;j<5;j++)
		{
			monitorModes[j]=new infoDataStruct;
			monitorModes[j]->label=strdup(modeLabel[j]);
			monitorModes[j]->userData=(void*)j;
		}
	monitorModeMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	monitorModeMenu->LFSTK_setMouseCallBack(NULL,monitorModeCB,NULL);
	monitorModeMenu->LFSTK_addMainMenus(rootModeMenu,5);

	monitorModeEdit=new LFSTK_lineEditClass(wc,modeLabel[monitors[selectedMonitor].mode],(BORDER*2)+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	monitorModeEdit->gadgetAcceptsDnD=false;
	sy+=YSPACING;

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
	apply=new LFSTK_buttonClass(wc,"Apply",DIALOGWIDTH-GADGETWIDTH-BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	apply->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"APPLY");

	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

	bffr=wc->globalLib->LFSTK_oneLiner("sed -n '2p' %S/lfsappearance.rc",apc->configDir);
	if((queueID=msgget(std::stoi(bffr,nullptr,10),IPC_CREAT|0660))==-1)
		fprintf(stderr,"Can't create message queue :( ...\n");

	bool			flag=false;
	int			retcode;
	int			receiveType=IPC_NOWAIT;
	msgBuffer	mbuffer;

	while(flag==false)
		{
			retcode=msgrcv(queueID,&mbuffer,MAX_MSG_SIZE,MSGANY,receiveType);
			if(retcode<=1)
				flag=true;
		}

	int retval=apc->LFSTK_runApp();
	delete apc;
	free(wd);
	free(mainPrefs);
	free(monitorPrefs);
	for(int j=0;j<20;j++)
		{
			if(monitors[j].path!=NULL)
				free(monitors[j].path);
		}

	return(retval);
}

