/*
 *
 * ©K. D. Hedger. Sat 15 Feb 12:57:49 GMT 2025 keithdhedger@gmail.com

 * This file (lfstrayprefs.cpp) is part of LFSApplications.

 * LFSApplications is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSApplications is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSApplications.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "config.h"
#include <lfstk/LFSTKGlobals.h>

#define BOXLABEL			"LFSTray Prefs"
#define EDITBOXWIDTH		GADGETWIDTH*4
#define TRAYPOSMENUSIZE	8

enum TrayPos{NW=1,NE,SE,SW,N,E,S,W};

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_buttonClass		*apply=NULL;

//prefs
LFSTK_lineEditClass		*onMonitor=NULL;
LFSTK_lineEditClass		*iconSize=NULL;
LFSTK_lineEditClass		*gravity=NULL;
LFSTK_toggleButtonClass	*vertical=NULL;
LFSTK_toggleButtonClass	*below=NULL;
LFSTK_toggleButtonClass	*noDups=NULL;
LFSTK_toggleButtonClass	*reverse=NULL;
LFSTK_buttonClass		*tileFilepath=NULL;
LFSTK_lineEditClass		*tileFilepathEdit=NULL;

//tray postion
LFSTK_buttonClass		*trayPosWindowMenu=NULL;
LFSTK_lineEditClass		*trayPosWindowEdit=NULL;
infoDataStruct			**trayPosMenus;
const char				*trayPosMenuNames[]={"NW","NE","SE","SW","N","E","S","W"};
LFSTK_menuClass			*trayPosMenu=NULL;
int						grav=0;

LFSTK_prefsClass			prefs("lfstrayprefs",VERSION);
char						*envFile=NULL;
int						parentWindow=-1;
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
	LFSTK_buttonClass	*button=NULL;

	if(strcmp((char*)ud,"TRAYPOS")==0)
		trayPosMenu->LFSTK_showMenu();

	if(strcmp((char*)ud,"SELECTIMAGE")==0)
		{
			LFSTK_fileDialogClass	*image=NULL;
			image=new LFSTK_fileDialogClass(wc,"Select File",NULL,FILEDIALOG,"lfstrayfilepath");

			image->LFSTK_showFileDialog(NULL,"Select Image Path");
			if(image->LFSTK_isValid()==true)
				tileFilepathEdit->LFSTK_setBuffer(image->LFSTK_getCurrentPath());
			delete image;
			return(true);
		}

	if(strcmp((char*)ud,"APPLY")==0)
		{
			prefs.LFSTK_deleteFromPrefs("window");

			prefs.LFSTK_setInt("monitor",atoi(onMonitor->LFSTK_getCStr()));
			prefs.LFSTK_setInt("iconsize",atoi(iconSize->LFSTK_getCStr()));
			prefs.LFSTK_setInt("gravity",grav);
			prefs.LFSTK_setBool("vertical",vertical->LFSTK_getValue());
			prefs.LFSTK_setBool("below",below->LFSTK_getValue());
			prefs.LFSTK_setBool("no-duplicates",noDups->LFSTK_getValue());
			prefs.LFSTK_setString("filepath",tileFilepathEdit->LFSTK_getCStr());
			prefs.LFSTK_setBool("reverseorder",reverse->LFSTK_getValue());
			prefs.LFSTK_saveVarsToFile(envFile);
			//prefs.LFSTK_saveVarsToFile("-");
			apc->globalLib->LFSTK_oneLiner("%s","pgrep \"^lfstray$\" |xargs kill -SIGUSR1");

		}
	return(true);
}

bool trayPosMenuCB(void *p,void* ud)
{
	static_cast<LFSTK_gadgetClass*>(p)->wc->LFSTK_hideWindow();
	trayPosWindowEdit->LFSTK_setBuffer(static_cast<LFSTK_gadgetClass*>(p)->LFSTK_getLabel());
	grav=(int)(long)ud+1;
	return(true);
}

int main(int argc, char **argv)
{
	int			sy=0;
	int			sx=BORDER;
	bool			flag=false;
	int			retcode;
	int			receiveType=IPC_NOWAIT;
	msgBuffer	mbuffer;
	std::string	bffr;
	LFSTK_buttonClass	*seperator=NULL;

	option longOptions[]=
		{
			{"window",required_argument,0,'w'},
			{"monitor",required_argument,0,'m'},
			{"iconsize",required_argument,0,'i'},
			{"gravity",required_argument,0,'g'},
			{"vertical",no_argument,0,'V'},
			{"below",no_argument,0,'b'},
			{"reverseorder",no_argument,0,'r'},
			{"no-duplicates",no_argument,0,'d'},
			{"filepath",required_argument,0,'f'},
			{0, 0, 0, 0}
		};

	apc=new LFSTK_applicationClass();
	apc->LFSTK_getDefaultWInit();
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTrayPrefs");
	wc=apc->mainWindow;
	wc->LFSTK_setKeepAbove(true);
	wc->LFSTK_setDecorations(false,false,false,true);

	bffr=wc->globalLib->LFSTK_oneLiner("sed -n '2p' %S/lfsappearance.rc",apc->configDir);
	if((queueID=msgget(std::stoi(bffr,nullptr,10),IPC_CREAT|0660))==-1)
		fprintf(stderr,"Can't create message queue :( ...\n");

	while(flag==false)
		{
			retcode=msgrcv(queueID,&mbuffer,MAX_MSG_SIZE,MSGANY,receiveType);
			if(retcode<=1)
				flag=true;
		}

	prefs.prefsMap=
		{
			{LFSTK_UtilityClass::LFSTK_hashFromKey("monitor"),{TYPEINT,"monitor","Place on monitor ARG","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("iconsize"),{TYPEINT,"iconsize","Iconsize ARG","",false,32}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("gravity"),{TYPEINT,"gravity","Gravity NW ARG=1,NE ARG=2,SE ARG=3,SW ARG=4,N ARG=5,E ARG=6,S ARG=7,W ARG=8","",false,NW}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("vertical"),{TYPEBOOL,"vertical","Vertical systray ( default horizontal )","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("below"),{TYPEBOOL,"below","Below all windows ( default above )","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("reverseorder"),{TYPEBOOL,"reverseorder","Reverse sort order of trays (by _NET_WM_NAME property )","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("filepath"),{TYPESTRING,"filepath","Use external file, if ARG begins with '#' use solid colour, eg '#ff0000'","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("no-duplicates"),{TYPEBOOL,"no-duplicates","Don't allow duplicate items ( by _NET_WM_NAME property )","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("window"),{TYPEINT,"window","Set transient for window ARG","",false,-1}},
		};

	parentWindow=prefs.LFSTK_getInt("window");
	asprintf(&envFile,"%s/lfstray.rc",apc->configDir.c_str());
	prefs.LFSTK_loadVarsFromFile(envFile);

	if(prefs.LFSTK_argsToPrefs(argc,argv,longOptions,true)==false)
		return(1);

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//monitor
	label=new LFSTK_labelClass(wc,"On monitor",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	sx+=GADGETWIDTH+BORDER;
	onMonitor=new LFSTK_lineEditClass(wc,std::to_string(prefs.LFSTK_getInt("monitor")).c_str(),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;
//iconsize
	label=new LFSTK_labelClass(wc,"Icon Size",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	sx+=GADGETWIDTH+BORDER;
	iconSize=new LFSTK_lineEditClass(wc,std::to_string(prefs.LFSTK_getInt("iconsize")).c_str(),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//tray position
	trayPosWindowMenu=new LFSTK_buttonClass(wc,"Tray Position",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	trayPosWindowMenu->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"TRAYPOS");
	trayPosMenu=new LFSTK_menuClass(wc,BORDER+GADGETWIDTH,sy,1,1);
	trayPosMenu->LFSTK_setMouseCallBack(NULL,trayPosMenuCB,NULL);

	trayPosMenus=new infoDataStruct*[TRAYPOSMENUSIZE];
	for(long j=0;j<TRAYPOSMENUSIZE;j++)
		{
			trayPosMenus[j]=new infoDataStruct;
			trayPosMenus[j]->label=strdup(trayPosMenuNames[j]);
			trayPosMenus[j]->userData=(void*)j;
		}
	trayPosMenu->LFSTK_addMainMenus(trayPosMenus,TRAYPOSMENUSIZE);
	grav=prefs.LFSTK_getInt("gravity");

	sx+=GADGETWIDTH+BORDER;
	trayPosWindowEdit=new LFSTK_lineEditClass(wc,trayPosMenuNames[prefs.LFSTK_getInt("gravity")-1],sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//vertical
	vertical=new LFSTK_toggleButtonClass(wc,"Vertical Tray",BORDER,sy,GADGETWIDTH*2,CHECKBOXSIZE,NorthWestGravity);
	vertical->LFSTK_setValue(prefs.LFSTK_getBool("vertical"));
	sy+=YSPACING-8;
	sx=BORDER;

//below
	below=new LFSTK_toggleButtonClass(wc,"Tray Below Windows",BORDER,sy,GADGETWIDTH*2,CHECKBOXSIZE,NorthWestGravity);
	below->LFSTK_setValue(prefs.LFSTK_getBool("below"));
	sy+=YSPACING-8;
	sx=BORDER;

//reverse
	reverse=new LFSTK_toggleButtonClass(wc,"Reverse Order",BORDER,sy,GADGETWIDTH*2,CHECKBOXSIZE,NorthWestGravity);
	reverse->LFSTK_setValue(prefs.LFSTK_getBool("reverseorder"));
	sy+=YSPACING-8;
	sx=BORDER;

//no-duplicates
	noDups=new LFSTK_toggleButtonClass(wc,"No Duplicate Icons",BORDER,sy,GADGETWIDTH*2,CHECKBOXSIZE,NorthWestGravity);
	noDups->LFSTK_setValue(prefs.LFSTK_getBool("no-duplicates"));
	sy+=YSPACING-8;
	sx=BORDER;

//filepath
	tileFilepath=new LFSTK_buttonClass(wc,"Background",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	tileFilepath->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)"SELECTIMAGE");
	sx+=GADGETWIDTH+BORDER;
	//tileFilepathEdit=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("filepath"),sx,sy,EDITBOXWIDTH*2-BORDER-100,GADGETHITE,BUTTONGRAV);
	tileFilepathEdit=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("filepath"),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
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
	//wc->LFSTK_showWindow();
	//wc->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());

	int retval=apc->LFSTK_runApp();

	delete apc;
	cairo_debug_reset_static_data();
	return retval;
}