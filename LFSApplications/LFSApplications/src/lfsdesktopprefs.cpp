/*
 *
 * ©K. D. Hedger. Wed 16 Aug 13:55:27 BST 2017 keithdhedger@gmail.com

 * This file (lfsdesktopprefs.cpp) is part of LFSApplications.

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
#include "lfstk/LFSTKGlobals.h"

#include <libgen.h>

#define BOXLABEL	"LFS Desktop Prefs"

LFSTK_applicationClass	*apc=NULL;
LFSTK_prefsClass		prefs;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_buttonClass		*apply=NULL;
LFSTK_buttonClass		*button=NULL;

LFSTK_fileDialogClass	*themeDialog=NULL;
LFSTK_lineEditClass		*themeEditBox=NULL;

LFSTK_lineEditClass		*iconSizeEditBox=NULL;
LFSTK_lineEditClass		*gridSizeEditBox=NULL;
LFSTK_lineEditClass		*borderLeftEditBox=NULL;
LFSTK_lineEditClass		*borderRightEditBox=NULL;
LFSTK_lineEditClass		*refreshEditBox=NULL;
LFSTK_lineEditClass		*labelColurFEditBox=NULL;
LFSTK_lineEditClass		*labelColurBEditBox=NULL;
LFSTK_lineEditClass		*labelAlphaColurEditBox=NULL;
LFSTK_toggleButtonClass	*showSuffixCheck=NULL;
LFSTK_toggleButtonClass	*clickExeCheck=NULL;
LFSTK_lineEditClass		*termCommandEditBox=NULL;
LFSTK_lineEditClass		*fontEditBox=NULL;
LFSTK_lineEditClass		*includeEditBox=NULL;
LFSTK_lineEditClass		*excludeEditBox=NULL;

LFSTK_fontDialogClass	*fontButton=NULL;

char					*wd=NULL;
char					*envFile=NULL;
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
	msgBuffer	buffer;

	prefs.prefsMap=
		{
			{prefs.LFSTK_hashFromKey("icontheme"),{TYPESTRING,"icontheme",themeEditBox->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("iconsize"),{TYPEINT,"iconsize","",false,atoi(iconSizeEditBox->LFSTK_getCStr())}},
			{prefs.LFSTK_hashFromKey("gridsize"),{TYPEINT,"gridsize","",false,atoi(gridSizeEditBox->LFSTK_getCStr())}},
			{prefs.LFSTK_hashFromKey("gridborderleft"),{TYPEINT,"gridborderleft","",false,atoi(borderLeftEditBox->LFSTK_getCStr())}},
			{prefs.LFSTK_hashFromKey("gridborderrite"),{TYPEINT,"gridborderrite","",false,atoi(borderRightEditBox->LFSTK_getCStr())}},
			{prefs.LFSTK_hashFromKey("refreshrate"),{TYPEINT,"refreshrate","",false,atoi(refreshEditBox->LFSTK_getCStr())}},
			{prefs.LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand",termCommandEditBox->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("showextension"),{TYPEBOOL,"showextension","",showSuffixCheck->LFSTK_getValue(),0}},
			{prefs.LFSTK_hashFromKey("fontface"),{TYPESTRING,"fontface",fontEditBox->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("labelforeground"),{TYPESTRING,"labelforeground",labelColurFEditBox->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("labelbackground"),{TYPESTRING,"labelbackground",labelColurBEditBox->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("labelalpha"),{TYPESTRING,"labelalpha",labelAlphaColurEditBox->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("includelist"),{TYPESTRING,"includelist",includeEditBox->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("excludelist"),{TYPESTRING,"excludelist",excludeEditBox->LFSTK_getCStr(),false,0}},
			{prefs.LFSTK_hashFromKey("doubleclickexe"),{TYPEBOOL,"doubleclickexe","",clickExeCheck->LFSTK_getValue(),0}}
		};

	prefs.LFSTK_saveVarsToFile(envFile);
	buffer.mType=DESKTOP_MSG;
	sprintf(buffer.mText,"reloadprefs");
	if((msgsnd(queueID,&buffer,strlen(buffer.mText)+1,0))==-1)
		fprintf(stderr,"Can't send message :(\n");

	return(true);
}

bool selectThemeFolder(void *object,void* ud)
{
	char	*buffer;

	LFSTK_lineEditClass	*fd=static_cast<LFSTK_lineEditClass*>(ud);

	themeDialog->LFSTK_showFileDialog(wd,"Select A Folder");
	if(themeDialog->LFSTK_isValid()==true)
		{
			asprintf(&buffer,"%s",themeDialog->LFSTK_getCurrentDir());
			fd->LFSTK_setBuffer(basename(buffer));
			free(buffer);
			free(wd);
			wd=strdup(themeDialog->LFSTK_getCurrentDir().c_str());
		}
	return(true);
}

bool selectFontCB(void *p,void* ud)
{
	const fontDataStruct	*fd;
	LFSTK_lineEditClass		*edbox=static_cast<LFSTK_lineEditClass*>(ud);

	if(ud!=NULL)
		{
			fontButton->LFSTK_showDialog(edbox->LFSTK_getCStr());
			fd=fontButton->LFSTK_getFontData(false);
			if(fd->isValid==true)
				{
					edbox->LFSTK_setBuffer(fd->fontString);
				}
		}
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
#ifdef _ENABLEDEBUG_//TODO//
			col=strdup(apc->globalLib->LFSTK_oneLiner("LD_LIBRARY_PATH=../LFSToolKit/LFSToolKit/app/.libs LFSApplications/app/lfscolourchooser -w %i \"%s\"",pw,ed->LFSTK_getCStr()).c_str());
#else
			col=strdup(apc->globalLib->LFSTK_oneLiner("lfscolourchooser -w %i \"%s\"",pw,ed->LFSTK_getCStr()).c_str());
#endif
			if(strlen(col)>0)
				ed->LFSTK_setBuffer(col);
			free(col);
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent		event;
	int			sy=0;
	char		*buffer=NULL;
	int			c=0;
	int			option_index=0;
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
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKPrefs");
	wc=apc->mainWindow;

	prefs.prefsMap=
		{
			{prefs.LFSTK_hashFromKey("icontheme"),{TYPESTRING,"icontheme","gnome",false,0}},
			{prefs.LFSTK_hashFromKey("iconsize"),{TYPEINT,"iconsize","",false,32}},
			{prefs.LFSTK_hashFromKey("gridsize"),{TYPEINT,"gridsize","",false,64}},
			{prefs.LFSTK_hashFromKey("gridborderleft"),{TYPEINT,"gridborderleft","",false,2}},
			{prefs.LFSTK_hashFromKey("gridborderrite"),{TYPEINT,"gridborderrite","",false,16}},
			{prefs.LFSTK_hashFromKey("refreshrate"),{TYPEINT,"refreshrate","",false,2}},
			{prefs.LFSTK_hashFromKey("termcommand"),{TYPESTRING,"termcommand","xterm -e ",false,0}},
			{prefs.LFSTK_hashFromKey("showextension"),{TYPEBOOL,"showextension","",false,0}},
			{prefs.LFSTK_hashFromKey("fontface"),{TYPESTRING,"fontface","",false,0}},
			{prefs.LFSTK_hashFromKey("labelforeground"),{TYPESTRING,"labelforeground","",false,0}},
			{prefs.LFSTK_hashFromKey("labelbackground"),{TYPESTRING,"labelbackground","",false,0}},
			{prefs.LFSTK_hashFromKey("labelalpha"),{TYPESTRING,"labelalpha","1.0",false,0}},
			{prefs.LFSTK_hashFromKey("includelist"),{TYPESTRING,"includelist","",false,0}},
			{prefs.LFSTK_hashFromKey("excludelist"),{TYPESTRING,"excludelist","",false,0}},
			{prefs.LFSTK_hashFromKey("doubleclickexe"),{TYPEBOOL,"doubleclickexe","",false,0}}
		};

	asprintf(&envFile,"%s/lfsdesktop.rc",apc->configDir.c_str());
	prefs.LFSTK_loadVarsFromFile(envFile);

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;
//theme
	asprintf(&wd,"%s/.icons",apc->userHome.c_str());
	themeDialog=new LFSTK_fileDialogClass(wc,"Select Theme",wd,FOLDERDIALOG);
	button=new LFSTK_buttonClass(wc,"Select Theme",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	themeEditBox=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("icontheme"),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH*4,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setMouseCallBack(NULL,selectThemeFolder,(void*)themeEditBox);
	sy+=YSPACING;

//font
	fontButton=new LFSTK_fontDialogClass(wc,"Select Font",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	fontEditBox=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("fontface"),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH*4,GADGETHITE,BUTTONGRAV);
	fontButton->LFSTK_setMouseCallBack(NULL,selectFontCB,(void*)fontEditBox);
	sy+=YSPACING;

//icon sizes
	label=new LFSTK_labelClass(wc,"Icon Size",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	iconSizeEditBox=new LFSTK_lineEditClass(wc,(char*)std::to_string(prefs.LFSTK_getInt("iconsize")).c_str(),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,"Grid Size",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	gridSizeEditBox=new LFSTK_lineEditClass(wc,(char*)std::to_string(prefs.LFSTK_getInt("gridsize")).c_str(),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,"Borders",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	borderLeftEditBox=new LFSTK_lineEditClass(wc,(char*)std::to_string(prefs.LFSTK_getInt("gridborderleft")).c_str(),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	borderRightEditBox=new LFSTK_lineEditClass(wc,(char*)std::to_string(prefs.LFSTK_getInt("gridborderrite")).c_str(),BORDER*2+GADGETWIDTH+GADGETWIDTH+BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,"Refresh",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	refreshEditBox=new LFSTK_lineEditClass(wc,(char*)std::to_string(prefs.LFSTK_getInt("refreshrate")).c_str(),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,"Fore Colour",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	labelColurFEditBox=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("labelforeground"),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	labelColurFEditBox->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
	labelColurFEditBox->LFSTK_setContextWindow(NULL);
	sy+=YSPACING;
	
	label=new LFSTK_labelClass(wc,"Back Colour",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	labelColurBEditBox=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("labelbackground"),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	labelColurBEditBox->LFSTK_setMouseCallBack(NULL,coleditCB,NULL);
	labelColurBEditBox->LFSTK_setContextWindow(NULL);
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,"Label Alpha",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	labelAlphaColurEditBox=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("labelalpha"),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//show suffix
	label=new LFSTK_labelClass(wc,"Show Suffix",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	showSuffixCheck=new LFSTK_toggleButtonClass(wc,"",BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,CHECKBOXSIZE,BUTTONGRAV);
	showSuffixCheck->LFSTK_setValue(prefs.LFSTK_getBool("showextension"));
	sy+=YSPACING;

//Double click executes
	label=new LFSTK_labelClass(wc,"Dble Click Executes",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	clickExeCheck=new LFSTK_toggleButtonClass(wc,"",BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,CHECKBOXSIZE,BUTTONGRAV);
	clickExeCheck->LFSTK_setValue(prefs.LFSTK_getBool("doubleclickexe"));
	sy+=YSPACING;

//terminal command
	label=new LFSTK_labelClass(wc,"Terminal CMD",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	termCommandEditBox=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("termcommand"),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH*4,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//include list
	label=new LFSTK_labelClass(wc,"Include Disks",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	includeEditBox=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("includelist"),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH*4,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//exclude list
	label=new LFSTK_labelClass(wc,"Exclude Disks",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	excludeEditBox=new LFSTK_lineEditClass(wc,prefs.LFSTK_getCString("excludelist"),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH*4,GADGETHITE,BUTTONGRAV);
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
	apply=new LFSTK_buttonClass(wc,"Apply",DIALOGWIDTH-BORDER-GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	apply->LFSTK_setMouseCallBack(NULL,buttonCB,NULL);
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

	buffer=strdup(wc->globalLib->LFSTK_oneLiner("sed -n '2p' %s/lfsappearance.rc",apc->configDir.c_str()).c_str());//TODO//
	if((queueID=msgget(atoi(buffer),IPC_CREAT|0660))==-1)
		fprintf(stderr,"Can't create message queue :( ...\n");
	free(buffer);

	bool	flag=false;
	int		retcode;
	int		receiveType=IPC_NOWAIT;
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
	free(envFile);

	return(retval);
}