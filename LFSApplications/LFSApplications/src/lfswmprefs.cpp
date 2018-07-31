/*
 *
 * ©K. D. Hedger. Thu 17 Aug 15:46:21 BST 2017 keithdhedger@gmail.com

 * This file (lfswmprefs.cpp) is part of LFSApplications.

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

#define MAXMENUS			5
#define EDITBOXWIDTH		GADGETWIDTH*4

LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_buttonClass		*test=NULL;
LFSTK_buttonClass		*apply=NULL;

//preview colours
LFSTK_buttonClass		*previewButtons[5]={NULL,};
LFSTK_lineEditClass		*previeColourEdit[5]={NULL,};
const char				*previewButtonLabels[5]={"Active Frame","Active Fill","Inactive Frame","Inactive Fill","Widget Colour"};

//theme
LFSTK_fileDialogClass	*themeFolder=NULL;
LFSTK_buttonClass		*themeButton=NULL;
LFSTK_lineEditClass		*themeEdit=NULL;

LFSTK_lineEditClass		*terminalEdit=NULL;

LFSTK_lineEditClass		*deskCountEdit=NULL;
LFSTK_lineEditClass		*refreshEdit=NULL;
LFSTK_lineEditClass		*rescanEdit=NULL;

LFSTK_fontDialogClass	*fontSelector=NULL;
LFSTK_lineEditClass		*fontEdit=NULL;

bool					mainLoop=true;
Display					*display;
char					*envFile=NULL;

//placement
LFSTK_menuButtonClass	*placeWindowMenu=NULL;
LFSTK_lineEditClass		*placeWindowEdit=NULL;
menuItemStruct			*placementMenus;
const char				*placementMenuNames[]={"Smart Place On Screen","Under Mouse","Centre On Monitor With Mouse","Centre On Screen","Smart Place On Monitor With Mouse"};

//prefs
char					*prefsColours[5]={NULL,};
char					*prefsFont=NULL;
char					*prefsTheme=NULL;
char					*prefsTermCommand=NULL;
int						prefsPlacement=2;
int						prefsPlacementTemp=2;
int						prefsDeskCnt=6;
int						prefsRefresh=15;
int						prefsRescan=4;

args					lfsWMPrefs[]=
{
	{"wmactive_frame",TYPESTRING,&prefsColours[0]},
	{"wmactive_fill",TYPESTRING,&prefsColours[1]},
	{"wminactive_frame",TYPESTRING,&prefsColours[2]},
	{"wminactive_fill",TYPESTRING,&prefsColours[3]},
	{"widgetcolour",TYPESTRING,&prefsColours[4]},
	{"titlefont",TYPESTRING,&prefsFont},
	{"theme",TYPESTRING,&prefsTheme},
	{"termcommand",TYPESTRING,&prefsTermCommand},
	{"placement",TYPEINT,&prefsPlacement},
	{"desktops",TYPEINT,&prefsDeskCnt},
	{"liveupdate",TYPEINT,&prefsRefresh},
	{"rescanprefs",TYPEINT,&prefsRescan},
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
	const fontDataStruct	*fd;
	char					*buffer;

	if(ud!=NULL)
		{
			if(strcmp((char*)ud,"SELECTFONT")==0)
				{
					fontSelector->LFSTK_showDialog("");
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
							asprintf(&buffer,"%s",themeFolder->LFSTK_getCurrentDir());
							themeEdit->LFSTK_setBuffer(basename(buffer));
							free(buffer);
						}
					return(true);
				}

			if(strcmp((char*)ud,"TEST")==0)
				{
					for(int j=0;j<5;j++)
						{
							previewButtons[j]->LFSTK_setColourName(NORMALCOLOUR,previeColourEdit[j]->LFSTK_getCStr());
							previewButtons[j]->LFSTK_setFontColourName(NORMALCOLOUR,"black",false);
							previewButtons[j]->LFSTK_clearWindow();
						}
					return(true);
				}
			if(strcmp((char*)ud,"APPLY")==0)
				{
					for(int j=0;j<5;j++)
						{
							free(prefsColours[j]);
							prefsColours[j]=strdup(previeColourEdit[j]->LFSTK_getCStr());
							previewButtons[j]->LFSTK_setColourName(NORMALCOLOUR,previeColourEdit[j]->LFSTK_getBuffer()->c_str());
							previewButtons[j]->LFSTK_setFontColourName(NORMALCOLOUR,"black",false);
							previewButtons[j]->LFSTK_clearWindow();
						}
					free(prefsFont);
					prefsFont=strdup(fontEdit->LFSTK_getCStr());
					free(prefsTheme);
					prefsTheme=strdup(themeEdit->LFSTK_getCStr());
					free(prefsTermCommand);
					prefsTermCommand=strdup(terminalEdit->LFSTK_getCStr());
					prefsPlacement=prefsPlacementTemp;
					prefsDeskCnt=atoi(deskCountEdit->LFSTK_getCStr());
					prefsRefresh=atoi(refreshEdit->LFSTK_getCStr());
					prefsRescan=atoi(rescanEdit->LFSTK_getCStr());
					wc->globalLib->LFSTK_saveVarsToFile(envFile,lfsWMPrefs);
					system("climsg -s 'reloadtheme' -k 667");
					return(true);
				}
		}
	return(true);
}

bool menuCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(ud==NULL)
		return(true);

	placeWindowEdit->LFSTK_setBuffer(menuitem->label);
	prefsPlacementTemp=(int)(long)menuitem->userData;
	printf("Selected Menu Label:%s num=%i\n",menuitem->label,prefsPlacement);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent		event;
	int			sy=0;
	int			sx=BORDER;
	int			parentWindow=-1;
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

	prefsColours[0]=strdup("black");
	prefsColours[1]=strdup("#00ffff");
	prefsColours[2]=strdup("black");
	prefsColours[3]=strdup("white");
	prefsColours[4]=strdup("white");

	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"LFS WM Prefs",false);
	display=wc->display;

	asprintf(&envFile,"%s/lfswmanager.rc",wc->configDir);
	wc->globalLib->LFSTK_loadVarsFromFile(envFile,lfsWMPrefs);
	prefsPlacementTemp=prefsPlacement;

	themeFolder=new LFSTK_fileDialogClass(wc,"Select File",NULL,true,"lfswmprefstheme");

	placementMenus=new menuItemStruct[MAXMENUS];

	for(long j=0;j<MAXMENUS;j++)
		{
			placementMenus[j].label=(char*)placementMenuNames[j];
			placementMenus[j].imagePath=NULL;
			placementMenus[j].userData=(void*)j;
		}

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//frame colours
	for(int j=0;j<5;j++)
		{
			previewButtons[j]=new LFSTK_buttonClass(wc,previewButtonLabels[j],sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
			previewButtons[j]->LFSTK_setLabelAutoColour(true);
			previewButtons[j]->LFSTK_setTile(NULL,0);
			previewButtons[j]->LFSTK_setColourName(NORMALCOLOUR,prefsColours[j]);
			previewButtons[j]->LFSTK_setFontColourName(NORMALCOLOUR,"black",false);
			previewButtons[j]->LFSTK_setIgnoreCB(true);
			sx+=GADGETWIDTH+BORDER;
			previeColourEdit[j]=new LFSTK_lineEditClass(wc,prefsColours[j],sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
			sy+=YSPACING;
			sx=BORDER;
		}

//font
	fontSelector=new LFSTK_fontDialogClass(wc,"Select Font",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	fontSelector->LFSTK_setCallBack(NULL,buttonCB,(void*)"SELECTFONT");
	sx+=GADGETWIDTH+BORDER;
	fontEdit=new LFSTK_lineEditClass(wc,prefsFont,sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//theme
	themeButton=new LFSTK_buttonClass(wc,"Select Theme",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	themeButton->LFSTK_setCallBack(NULL,buttonCB,(void*)"SELECTTHEME");
	sx+=GADGETWIDTH+BORDER;
	themeEdit=new LFSTK_lineEditClass(wc,prefsTheme,sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//placement
	placeWindowMenu=new LFSTK_menuButtonClass(wc,"Placement",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	placeWindowMenu->LFSTK_setCallBack(NULL,menuCB,NULL);
	placeWindowMenu->LFSTK_addMenus(placementMenus,MAXMENUS);
	placeWindowMenu->LFSTK_setLabelGravity(CENTRE);
	sx+=GADGETWIDTH+BORDER;
	placeWindowEdit=new LFSTK_lineEditClass(wc,placementMenuNames[prefsPlacement],sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//desks
	label=new LFSTK_labelClass(wc,"Desktops",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=GADGETWIDTH+BORDER;
	deskCountEdit=new LFSTK_lineEditClass(wc,std::to_string(prefsDeskCnt).c_str(),sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;
//update
	label=new LFSTK_labelClass(wc,"Update",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=GADGETWIDTH+BORDER;
	refreshEdit=new LFSTK_lineEditClass(wc,std::to_string(prefsRefresh).c_str(),sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//rescan prefs
	label=new LFSTK_labelClass(wc,"Rescan Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=GADGETWIDTH+BORDER;
	rescanEdit=new LFSTK_lineEditClass(wc,std::to_string(prefsRescan).c_str(),sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;


//term command
	label=new LFSTK_labelClass(wc,"Term Command",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=GADGETWIDTH+BORDER;
	terminalEdit=new LFSTK_lineEditClass(wc,prefsTermCommand,sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
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
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);

//test
	test=new LFSTK_buttonClass(wc,"Test Colours",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	test->LFSTK_setCallBack(NULL,buttonCB,(void*)"TEST");

//apply
	apply=new LFSTK_buttonClass(wc,"Apply",DIALOGWIDTH-BORDER-GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	apply->LFSTK_setCallBack(NULL,buttonCB,(void*)"APPLY");
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				{
					ml->function(ml->gadget,&event,ml->type);
				}

			switch(event.type)
				{
					case ButtonRelease:
						break;
					case LeaveNotify:
						break;
					case Expose:
						wc->LFSTK_clearWindow();
						break;

					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
						wc->globalLib->LFSTK_setCairoSurface(wc->display,wc->window,wc->visual,&wc->sfc,&wc->cr,event.xconfigurerequest.width,event.xconfigurerequest.height);
						wc->LFSTK_clearWindow();
						break;

					case ClientMessage:
					case SelectionNotify:
						{
							if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
								{
									wc->LFSTK_hideWindow();
									mainLoop=false;
								}
							if(wc->acceptDnd==true)
								wc->LFSTK_handleDnD(&event);
						}
						break;
				}
		}

	delete wc;
	XCloseDisplay(display);
	free(envFile);
	for(int j=0;j<5;j++)
		free(prefsColours[j]);
	free(prefsFont);
	free(prefsTheme);
	free(prefsTermCommand);
	delete placementMenus;

	return 0;
}