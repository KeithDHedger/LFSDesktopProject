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
#include <libgen.h>

#include "lfstk/LFSTKGlobals.h"

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
LFSTK_lineEditClass		*termCommandEditBox=NULL;
LFSTK_lineEditClass		*fontEditBox=NULL;
LFSTK_lineEditClass		*includeEditBox=NULL;
LFSTK_lineEditClass		*excludeEditBox=NULL;

LFSTK_fontDialogClass	*fontButton=NULL;

bool					mainLoop=true;
Display					*display;
char					*wd=NULL;
char					*envFile=NULL;

//prefs
char					*prefsTheme=NULL;
int						prefsIconSize=32;
int						prefsGridSize=64;
int						prefsLeftBorder=2;
int						prefsRightBorder=16;
int						prefsRefresh=2;
char					*prefsTermCommand=NULL;
bool					prefsShowSuffix=false;
char					*prefsFont=NULL;
char					*prefsLabelFColour=NULL;
char					*prefsLabelBColour=NULL;
char					*prefsLabelAlpha=NULL;
char					*prefsIncludeSed=NULL;
char					*prefsExcludeSed=NULL;
//msg
int						queueID=-1;

args					desktopPrefs[]=
{
	{"icontheme",TYPESTRING,&prefsTheme},
	{"iconsize",TYPEINT,&prefsIconSize},
	{"gridsize",TYPEINT,&prefsGridSize},
	{"gridborderleft",TYPEINT,&prefsLeftBorder},
	{"gridborderrite",TYPEINT,&prefsRightBorder},
	{"refreshrate",TYPEINT,&prefsRefresh},
	{"termcommand",TYPESTRING,&prefsTermCommand},
	{"showextension",TYPEBOOL,&prefsShowSuffix},
	{"fontface",TYPESTRING,&prefsFont},
	{"labelforeground",TYPESTRING,&prefsLabelFColour},
	{"labelbackground",TYPESTRING,&prefsLabelBColour},
	{"labelalpha",TYPESTRING,&prefsLabelAlpha},
	{"includelist",TYPESTRING,&prefsIncludeSed},
	{"excludelist",TYPESTRING,&prefsExcludeSed},
	{NULL,0,NULL}
};

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	msgBuffer	buffer;

	free(prefsTheme);
	free(prefsTermCommand);
	free(prefsLabelFColour);
	free(prefsLabelBColour);
	free(prefsLabelAlpha);
	free(prefsIncludeSed);
	free(prefsExcludeSed);
	free(prefsFont);

	prefsTheme=strdup(themeEditBox->LFSTK_getCStr());
	prefsTermCommand=strdup(termCommandEditBox->LFSTK_getCStr());
	prefsLabelFColour=strdup(labelColurFEditBox->LFSTK_getCStr());
	prefsLabelBColour=strdup(labelColurBEditBox->LFSTK_getCStr());
	prefsLabelAlpha=strdup(labelAlphaColurEditBox->LFSTK_getCStr());
	prefsIncludeSed=strdup(includeEditBox->LFSTK_getCStr());
	prefsExcludeSed=strdup(excludeEditBox->LFSTK_getCStr());
	prefsFont=strdup(fontEditBox->LFSTK_getCStr());

	prefsIconSize=atoi(iconSizeEditBox->LFSTK_getCStr());
	prefsGridSize=atoi(gridSizeEditBox->LFSTK_getCStr());
	prefsLeftBorder=atoi(borderLeftEditBox->LFSTK_getCStr());
	prefsRightBorder=atoi(borderRightEditBox->LFSTK_getCStr());
	prefsRefresh=atoi(refreshEditBox->LFSTK_getCStr());

	prefsShowSuffix=showSuffixCheck->LFSTK_getValue();

	wc->globalLib->LFSTK_saveVarsToFile(envFile,desktopPrefs);

	buffer.mType=DESKTOP_MSG;
	sprintf(buffer.mText,"reloaddesk");
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
			wd=strdup(themeDialog->LFSTK_getCurrentDir());			
		}
	return(true);
}

bool selectFontCB(void *p,void* ud)
{
	const fontDataStruct	*fd;
	LFSTK_lineEditClass		*edbox=static_cast<LFSTK_lineEditClass*>(ud);

	if(ud!=NULL)
		{
			fontButton->LFSTK_showDialog("");
			fd=fontButton->LFSTK_getFontData(false);
			if(fd->isValid==true)
				{
					edbox->LFSTK_setBuffer(fd->fontString);
				}
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent		event;
	int			sy=0;
	int			parentWindow=-1;
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
						exit(0);
					case 'w':
						parentWindow=atoi(optarg);
						break;
				}
		}

	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"LFS Desktop Prefs",false);
	display=wc->display;

	asprintf(&envFile,"%s/lfsdesktop.rc",wc->configDir);
	wc->globalLib->LFSTK_loadVarsFromFile(envFile,desktopPrefs);

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;
//theme
	asprintf(&wd,"%s/.icons",wc->userHome);
	themeDialog=new LFSTK_fileDialogClass(wc,"Select Theme",wd,FOLDERDIALOG);
	button=new LFSTK_buttonClass(wc,"Select Theme",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	themeEditBox=new LFSTK_lineEditClass(wc,prefsTheme,BORDER*2+GADGETWIDTH,sy,GADGETWIDTH*4,GADGETHITE,BUTTONGRAV);
	button->LFSTK_setCallBack(NULL,selectThemeFolder,(void*)themeEditBox);
	sy+=YSPACING;

//font
	fontButton=new LFSTK_fontDialogClass(wc,"Select Font",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	fontEditBox=new LFSTK_lineEditClass(wc,prefsFont,BORDER*2+GADGETWIDTH,sy,GADGETWIDTH*4,GADGETHITE,BUTTONGRAV);
	fontButton->LFSTK_setCallBack(NULL,selectFontCB,(void*)fontEditBox);
	sy+=YSPACING;

//icon sizes
	label=new LFSTK_labelClass(wc,"Icon Size",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	iconSizeEditBox=new LFSTK_lineEditClass(wc,(char*)std::to_string(prefsIconSize).c_str(),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,"Grid Size",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	gridSizeEditBox=new LFSTK_lineEditClass(wc,(char*)std::to_string(prefsGridSize).c_str(),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,"Borders",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	borderLeftEditBox=new LFSTK_lineEditClass(wc,(char*)std::to_string(prefsLeftBorder).c_str(),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	borderRightEditBox=new LFSTK_lineEditClass(wc,(char*)std::to_string(prefsRightBorder).c_str(),BORDER*2+GADGETWIDTH+GADGETWIDTH+BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,"Refresh",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	refreshEditBox=new LFSTK_lineEditClass(wc,(char*)std::to_string(prefsRefresh).c_str(),BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,"Fore Colour",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	labelColurFEditBox=new LFSTK_lineEditClass(wc,prefsLabelFColour,BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	
	label=new LFSTK_labelClass(wc,"Back Colour",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	labelColurBEditBox=new LFSTK_lineEditClass(wc,prefsLabelBColour,BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	label=new LFSTK_labelClass(wc,"Label Alpha",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	labelAlphaColurEditBox=new LFSTK_lineEditClass(wc,prefsLabelAlpha,BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//show suffix
	label=new LFSTK_labelClass(wc,"Show Suffix",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	showSuffixCheck=new LFSTK_toggleButtonClass(wc,"",BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,CHECKBOXSIZE,BUTTONGRAV);
	showSuffixCheck->LFSTK_setValue(prefsShowSuffix);
	sy+=YSPACING;

//terminal command
	label=new LFSTK_labelClass(wc,"Terminal CMD",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	termCommandEditBox=new LFSTK_lineEditClass(wc,prefsTermCommand,BORDER*2+GADGETWIDTH,sy,GADGETWIDTH*4,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//include list
	label=new LFSTK_labelClass(wc,"Include Disks",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	includeEditBox=new LFSTK_lineEditClass(wc,prefsIncludeSed,BORDER*2+GADGETWIDTH,sy,GADGETWIDTH*4,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//exclude list
	label=new LFSTK_labelClass(wc,"Exclude Disks",BORDER,sy,GADGETWIDTH,GADGETHITE,LEFT);
	excludeEditBox=new LFSTK_lineEditClass(wc,prefsExcludeSed,BORDER*2+GADGETWIDTH,sy,GADGETWIDTH*4,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);

//apply
	apply=new LFSTK_buttonClass(wc,"Apply",DIALOGWIDTH-BORDER-GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	apply->LFSTK_setCallBack(NULL,buttonCB,NULL);
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());

	buffer=wc->globalLib->LFSTK_oneLiner("sed -n '2p' %s/lfsappearance.rc",wc->configDir);
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

	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);

			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			if(wc->LFSTK_handleWindowEvents(&event)<0)
				mainLoop=false;
		}

	delete wc;
	XCloseDisplay(display);
	free(wd);
	free(envFile);
	free(prefsTheme);
	free(prefsTermCommand);
	free(prefsLabelFColour);
	free(prefsLabelBColour);
	free(prefsLabelAlpha);
	free(prefsIncludeSed);
	free(prefsExcludeSed);
	free(prefsFont);

	return 0;
}