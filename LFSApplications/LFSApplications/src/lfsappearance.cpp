/*
 *
 * ©K. D. Hedger. Tue 15 Aug 17:26:09 BST 2017 kdhedger68713@gmail.com

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

#include "lfstk/LFSTKGlobals.h"

#undef GADGETWIDTH
#undef GADGETHITE
#undef HALFGADGETWIDTH
#undef DIALOGWIDTH
#undef DIALOGMIDDLE

#define GADGETWIDTH				96
#define GADGETHITE				24
#define HALFGADGETWIDTH			GADGETWIDTH/2
#define LABELWIDTH				(GADGETWIDTH*2)+BORDER

#define DIALOGWIDTH				BORDER*2+GADGETWIDTH+LABELWIDTH
#define DIALOGMIDDLE			DIALOGWIDTH/2

LFSTK_windowClass				*wc=NULL;
LFSTK_labelClass				*label=NULL;
LFSTK_labelClass				*personal=NULL;
LFSTK_labelClass				*copyrite=NULL;
LFSTK_buttonClass				*seperator=NULL;
LFSTK_buttonClass				*quit=NULL;
LFSTK_buttonClass				*apply=NULL;
LFSTK_buttonClass				*update=NULL;

LFSTK_buttonClass				*launchButton=NULL;
LFSTK_labelClass				*launchLabel=NULL;

LFSTK_menuButtonClass			*loadSet=NULL;
LFSTK_lineEditClass				*currentSet=NULL;

bool							mainLoop=true;
Display							*display;
int								parentWindow=-1;

std::map<int,char*>				groupNames;
menuItemStruct					*groupNameMenuItems=NULL;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

int fileExists(const char *name)
{
	struct stat buffer;
	return (stat(name,&buffer));
}

void updateDesktop(void)
{
	system("killall lfsdesktop &>/dev/null &");
	system("lfssetwallpaper &>/dev/null");
	system("killall lfswmanager &");
	system("killall lfspanel &");
}

void makeGroup(const char *grpname)
{
	char	*grp;
	char	*command;

	asprintf(&grp,"%s/lfsgroupsprefs/%s",wc->configDir,grpname);
	if(fileExists(grp)!=0)
		mkdir(grp,0755);

	asprintf(&command,"cp \"%s\"/lfs*.rc \"%s\"",wc->configDir,grp);
	system(command);

	free(command);
	free(grp);
}

void setGroup(void)
{
	char	*command;
	char	*grp;

	asprintf(&grp,"%s/lfsgroupsprefs/%s",wc->configDir,currentSet->LFSTK_getBuffer()->c_str());
	if(fileExists(grp)==0)
		{
			asprintf(&command,"cp \"%s/lfsgroupsprefs/%s/\"lfs*.rc \"%s\"",wc->configDir,currentSet->LFSTK_getBuffer()->c_str(),wc->configDir);
			system(command);
			free(command);
		}
	free(grp);

	asprintf(&command,"echo \"%s\" > \"%s/lfsappearance.rc\"",currentSet->LFSTK_getBuffer()->c_str(),wc->configDir);
	system(command);
	free(command);
}

bool buttonCB(void *p,void* ud)
{
	char	*command;
	
	if(ud!=NULL)
		{
			if(strcmp((char*)ud,"WALLPAPERPREFS")==0)
				{
					asprintf(&command,"lfsbackdropprefs --window=%i",wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"TOOLKITPREFS")==0)
				{
					asprintf(&command,"lfstkprefs --window=%i",wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"DESKTOPPREFS")==0)
				{
					asprintf(&command,"lfsdesktopprefs --window=%i",wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"WMPREFS")==0)
				{
					asprintf(&command,"lfswmprefs --window=%i",wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"PANELPREFS")==0)
				{
					asprintf(&command,"lfspanelprefs --window=%i",wc->window);
					system(command);
					free(command);
				}
			if(strcmp((char*)ud,"UPDATE")==0)
				{
					makeGroup(currentSet->LFSTK_getBuffer()->c_str());
					updateDesktop();
				}
			if(strcmp((char*)ud,"APPLY")==0)
				{
					setGroup();
					updateDesktop();
				}
		}
	return(true);
}

void addGroup(void)
{
	FILE*			fp=NULL;
	char			*buffer=NULL;
	char			*command;
	size_t			numchars;
	int				mapcnt=0;

	groupNames[mapcnt++]=strdup("Add Group");
	groupNames[mapcnt++]=strdup("--");

	asprintf(&command,"find %s/lfsgroupsprefs -mindepth 1 -maxdepth 1 -type d -printf \"%%f\\n\"|sort",wc->configDir);
	fp=popen(command,"r");
	if(fp!=NULL)
		{
			buffer=NULL;
			numchars=0;
			while(getline(&buffer,&numchars,fp)!=-1)
				{
					buffer[strlen(buffer)-1]=0;
					groupNames[mapcnt++]=buffer;
					buffer=NULL;
					numchars=0;
				}
			free(buffer);
			pclose(fp);
			groupNameMenuItems=new menuItemStruct[mapcnt];
			for(int j=0;j<mapcnt;j++)
				groupNameMenuItems[j].label=groupNames[j];
		}
	free(command);
}

void doNewGroup(void)
{
	makeGroup(currentSet->LFSTK_getBuffer()->c_str());
	for(int j=0;j<groupNames.size();j++)
		free(groupNames[j]);
	delete[] groupNameMenuItems;
	groupNames.clear();
	addGroup();
	loadSet->LFSTK_updateMenus(groupNameMenuItems,groupNames.size());
}

bool menuCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(ud==NULL)
		return(true);

	if(strcmp(menuitem->label,"Add Group")==0)
		{
			doNewGroup();
			return(true);
		}

	printf("Selected Menu Label:%s\n",menuitem->label);
	currentSet->LFSTK_setBuffer(menuitem->label);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent		event;
	int			sy=0;
	int			sx=BORDER;
	char		*buffer=NULL;

	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"LFS Appearance",false);
	display=wc->display;
//	wc->LFSTK_initDnD();

	groupNames.clear();
	addGroup();

	buffer=wc->globalLib->LFSTK_oneLiner("cat %s/lfsappearance.rc",wc->configDir);
	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//launch sub-prefs
//wallpaper
	launchButton=new LFSTK_buttonClass(wc,"Wallpaper Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setCallBack(NULL,buttonCB,(void*)"WALLPAPERPREFS");
	launchLabel=new LFSTK_labelClass(wc,"Launch Wallpaper Prefs Dialog",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//toolkit
	launchButton=new LFSTK_buttonClass(wc,"Toolkit Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setCallBack(NULL,buttonCB,(void*)"TOOLKITPREFS");
	launchLabel=new LFSTK_labelClass(wc,"Launch Toolkit Prefs Dialogg",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//desktop
	launchButton=new LFSTK_buttonClass(wc,"Desktop Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setCallBack(NULL,buttonCB,(void*)"DESKTOPPREFS");
	launchLabel=new LFSTK_labelClass(wc,"Launch Desktop Prefs Dialogg",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//wmanager
	launchButton=new LFSTK_buttonClass(wc,"WM Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setCallBack(NULL,buttonCB,(void*)"WMPREFS");
	launchLabel=new LFSTK_labelClass(wc,"Launch Window Prefs Dialogg",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//panel
	launchButton=new LFSTK_buttonClass(wc,"Panel Prefs",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	launchButton->LFSTK_setCallBack(NULL,buttonCB,(void*)"PANELPREFS");
	launchLabel=new LFSTK_labelClass(wc,"Launch Panel Prefs Dialogg",BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//load set
	loadSet=new LFSTK_menuButtonClass(wc,"Load Set",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	loadSet->LFSTK_setCallBack(NULL,menuCB,NULL);
	loadSet->LFSTK_addMenus(groupNameMenuItems,groupNames.size());
	loadSet->LFSTK_setLabelGravity(CENTRE);

	currentSet=new LFSTK_lineEditClass(wc,buffer,BORDER*2+GADGETWIDTH,sy,LABELWIDTH,GADGETHITE,BUTTONGRAV);
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
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);
	sx+=GADGETWIDTH+BORDER;
//update
	update=new LFSTK_buttonClass(wc,"Update",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	update->LFSTK_setCallBack(NULL,buttonCB,(void*)"UPDATE");
	sx+=GADGETWIDTH+BORDER;
//apply
	apply=new LFSTK_buttonClass(wc,"Apply",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	apply->LFSTK_setCallBack(NULL,buttonCB,(void*)"APPLY");

	sy+=YSPACING;

	wc->LFSTK_resizeWindow(BORDER*3+GADGETWIDTH+LABELWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

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
						}
						break;
				}
		}

	delete wc;
	XCloseDisplay(display);

	for(int j=0;j<groupNames.size();j++)
		free(groupNames[j]);
	delete[] groupNameMenuItems;
	groupNames.clear();

	cairo_debug_reset_static_data();
	return 0;
}