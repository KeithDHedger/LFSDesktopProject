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

#include <getopt.h>
#include <libgen.h>

#include "lfstk/LFSTKGlobals.h"

struct					monitorInfo
{
	char	*path;
	int		mode;
};

LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_buttonClass		*apply=NULL;

//file select
LFSTK_fileDialogClass	*fileDialog;
LFSTK_buttonClass		*dialogButton=NULL;

//root window
LFSTK_lineEditClass		*mainBackdropEdit=NULL;
LFSTK_lineEditClass		*rootColourEdit=NULL;
LFSTK_menuButtonClass	*mainMode=NULL;
LFSTK_lineEditClass		*mainModeEdit=NULL;
LFSTK_toggleButtonClass	*multipleMonitors=NULL;

//monitors
LFSTK_menuButtonClass	*monitorsMenuButton=NULL;
menuItemStruct			*monitorsMenu=NULL;
LFSTK_lineEditClass		*monitorModeEdit=NULL;
LFSTK_lineEditClass		*monitorBackdropEdit=NULL;
LFSTK_menuButtonClass	*monitorMode=NULL;
char					*monitorBackdrops[20]={NULL,};
monitorInfo				monitors[20]={{NULL,0},};

int						selectedMonitor=0;

bool					mainLoop=true;
Display					*display;
char					*wd=NULL;
Window					parentWindow=-1;

//prefs
char					*mainPrefs;
char					*monitorPrefs;
char					*wallpaperPath=NULL;
char					*mainColour=NULL;
int						backdropMode=0;
bool					multiMode=false;

//modes
const char				*modeLabel[5]={"Stretch","Tile","Centre","Scale","Zoom"};
menuItemStruct			*modeMenu=NULL;

args					prefs[]=
{
	{"backdrop",TYPESTRING,&wallpaperPath},
	{"colour",TYPESTRING,&mainColour},
	{"mainmode",TYPEINT,&backdropMode},
	{"multimode",TYPEBOOL,&multiMode},
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
	FILE	*fd=NULL;

	if(ud!=NULL)
		{
			if(strcmp((char*)ud,"APPLY")==0)
				{
					free(wallpaperPath);
					wallpaperPath=strdup(mainBackdropEdit->LFSTK_getBuffer()->c_str());
					free(mainColour);
					mainColour=strdup(rootColourEdit->LFSTK_getBuffer()->c_str());
					wc->globalLib->LFSTK_saveVarsToFile(mainPrefs,prefs);
					fd=fopen(monitorPrefs,"w");
					if(fd!=NULL)
						{
							for(long j=0;j<wc->LFSTK_getMonitorCount();j++)
								fprintf(fd,"%i\n%s\n",monitors[j].mode,monitors[j].path);
							fclose(fd);
						}
					system("lfssetwallpaper");
					system("climsg -s 'reloadbg' -k 666");
				}

			if(strcmp((char*)ud,"MULTIMODE")==0)
				multiMode=multipleMonitors->LFSTK_getValue();
		}
	return(true);
}

bool selectFile(void *object,void* ud)
{
	char				*filepath;
	LFSTK_lineEditClass	*edbox=static_cast<LFSTK_lineEditClass*>(ud);

	fileDialog->LFSTK_showFileDialog(NULL,"Select A File");
	if(fileDialog->LFSTK_isValid()==true)
		{
			asprintf(&filepath,"%s/%s",fileDialog->LFSTK_getCurrentDir(),fileDialog->LFSTK_getCurrentFile());
			free(wd);
			wd=strdup(fileDialog->LFSTK_getCurrentDir());
			edbox->LFSTK_setBuffer(filepath);
			free(monitors[selectedMonitor].path);
			monitors[selectedMonitor].path=strdup(filepath);
			free(filepath);
		}
	return(true);
}

bool mainModeCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(ud==NULL)
		return(true);
	mainModeEdit->LFSTK_setBuffer(menuitem->label);
	backdropMode=(int)(long)menuitem->userData;
	return(true);
}

bool monitorModeCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(ud==NULL)
		return(true);

	monitorModeEdit->LFSTK_setBuffer(menuitem->label);
	monitors[selectedMonitor].mode=(int)(long)menuitem->userData;
	return(true);
}

bool monitorMenuCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(ud==NULL)
		return(true);
	monitorsMenuButton->LFSTK_setLabel(menuitem->label);
	selectedMonitor=(int)(long)menuitem->userData;
	monitorBackdropEdit->LFSTK_setBuffer(monitors[selectedMonitor].path);
	monitorModeEdit->LFSTK_setBuffer(modeLabel[monitors[selectedMonitor].mode]);
	
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

	asprintf(&monitorrc,"%s/lfsmonitors.rc",wc->configDir);
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
				}
			fclose(fd);
		}
	else
		{
			for(int j=0;j<wc->LFSTK_getMonitorCount();j++)
				{
					monitors[j].mode=0;
					monitors[j].path=strdup("");
				}
		}
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=0;
	int						c=0;
	int						option_index=0;
	const char				*shortOpts="h?w:";
	option 					longOptions[]=
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

	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"Backdrop Prefs",false);
	display=wc->display;

	asprintf(&wd,"%s",wc->userHome);
	asprintf(&mainPrefs,"%s/lfssetwallpaper.rc",wc->configDir);
	asprintf(&monitorPrefs,"%s/lfsmonitors.rc",wc->configDir);

	fileDialog=new LFSTK_fileDialogClass(wc,"Select File",NULL,false,"lfsbackdropprefs");

	wc->globalLib->LFSTK_loadVarsFromFile(mainPrefs,prefs);

	copyrite=new LFSTK_labelClass(wc,COPYRITE,0,sy,DIALOGWIDTH,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,0,sy,DIALOGWIDTH,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//root window
//backdrop
	dialogButton=new LFSTK_buttonClass(wc,"Select File",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	mainBackdropEdit=new LFSTK_lineEditClass(wc,wallpaperPath,(BORDER*2)+GADGETWIDTH,sy,DIALOGWIDTH-(BORDER*3)-GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	dialogButton->LFSTK_setCallBack(NULL,selectFile,(void*)mainBackdropEdit);
	sy+=YSPACING;
//colour
	label=new LFSTK_labelClass(wc,"Root Colour",BORDER,sy,GADGETWIDTH,GADGETHITE);
	rootColourEdit=new LFSTK_lineEditClass(wc,mainColour,(BORDER*2)+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//mode
	modeMenu=new menuItemStruct[5];
	for(long j=0;j<5;j++)
		{
			modeMenu[j].label=(char*)modeLabel[j];
			modeMenu[j].userData=(void*)j;
		}
	mainMode=new LFSTK_menuButtonClass(wc,"Main Mode",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	mainMode->LFSTK_addMenus(modeMenu,5);
	mainModeEdit=new LFSTK_lineEditClass(wc,modeLabel[backdropMode],(BORDER*2)+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	mainModeEdit->gadgetAcceptsDnD=false;
	mainMode->LFSTK_setCallBack(NULL,mainModeCB,NULL);
	sy+=YSPACING;
//multi mode
	multipleMonitors=new LFSTK_toggleButtonClass(wc,"Multiple Monitors",BORDER,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	multipleMonitors->LFSTK_setValue(multiMode);
	multipleMonitors->LFSTK_setCallBack(NULL,buttonCB,(void*)"MULTIMODE");
	sy+=YSPACING;

//monitors
	monitorsMenu=new menuItemStruct[wc->LFSTK_getMonitorCount()];
	for(long j=0;j<wc->LFSTK_getMonitorCount();j++)
		{
			char	*lab;
			asprintf(&lab,"Monitor %i",j);
			monitorsMenu[j].label=lab;
			monitorsMenu[j].userData=(void*)j;
		}
	monitorsMenuButton=new LFSTK_menuButtonClass(wc,"Monitor 0",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	monitorsMenuButton->LFSTK_addMenus(monitorsMenu,wc->LFSTK_getMonitorCount());
	monitorsMenuButton->LFSTK_setCallBack(NULL,monitorMenuCB,NULL);
	loadMonitorInfo();
	sy+=YSPACING;

//monitor info
//backdrop
	dialogButton=new LFSTK_buttonClass(wc,"Select File",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	monitorBackdropEdit=new LFSTK_lineEditClass(wc,monitors[0].path,(BORDER*2)+GADGETWIDTH,sy,DIALOGWIDTH-(BORDER*3)-GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	dialogButton->LFSTK_setCallBack(NULL,selectFile,(void*)monitorBackdropEdit);
	sy+=YSPACING;
//mode
	monitorMode=new LFSTK_menuButtonClass(wc,"Monitor Mode",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	monitorMode->LFSTK_addMenus(modeMenu,5);
	monitorModeEdit=new LFSTK_lineEditClass(wc,modeLabel[monitors[selectedMonitor].mode],(BORDER*2)+GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	monitorModeEdit->gadgetAcceptsDnD=false;
	monitorMode->LFSTK_setCallBack(NULL,monitorModeCB,NULL);
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
	apply=new LFSTK_buttonClass(wc,"Apply",DIALOGWIDTH-GADGETWIDTH-BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
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
					//printf("expose\n");
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
	free(wd);
	free(mainPrefs);
	free(monitorPrefs);
	return 0;
}

