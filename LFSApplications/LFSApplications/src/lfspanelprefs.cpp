/*
 *
 * ©K. D. Hedger. Wed 23 Aug 12:54:30 BST 2017 kdhedger68713@gmail.com

 * This file (lfspanelprefs.cpp) is part of LFSApplications.

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

#include <string>
#include <getopt.h>

#include "lfstk/LFSTKGlobals.h"
#undef DIALOGWIDTH
#define DIALOGWIDTH (GADGETWIDTH*4)+(BORDER*3)

LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_buttonClass			*apply=NULL;

//panel select
LFSTK_menuButtonClass		*selectPanel=NULL;
LFSTK_lineEditClass			*panelNameEdit=NULL;
menuItemStruct				*panelNames=NULL;
int							panelCnt=0;

//panel data
LFSTK_menuButtonClass		*panelWidth=NULL;
LFSTK_lineEditClass			*panelWidthEdit=NULL;
menuItemStruct				*panelWidthMenu=NULL;
LFSTK_lineEditClass			*panelHeightEdit=NULL;

LFSTK_menuButtonClass		*panelPos=NULL;
LFSTK_lineEditClass			*panelPosEdit=NULL;
menuItemStruct				*panelPosMenu=NULL;

LFSTK_menuButtonClass		*panelGrav=NULL;
LFSTK_lineEditClass			*panelGravEdit=NULL;
menuItemStruct				*panelGravMenu=NULL;

LFSTK_lineEditClass			*panelOnMonitor=NULL;
LFSTK_lineEditClass			*panelLeftGadgets=NULL;
LFSTK_lineEditClass			*panelrightGadgets=NULL;

std::map<int,const char*>	panelWidthConvertToStr={{-1,"Fill"},{-2,"Shrink"}};
std::map<int,const char*>	panelPosConvertToStr={{-1,"Left"},{-2,"Centre"},{-3,"Right"}};
std::map<int,const char*>	panelGravConvertToStr{{1,"North"},{2,"East"},{3,"South",},{4,"West"}};

//action data
LFSTK_lineEditClass			*termCommand=NULL;
LFSTK_lineEditClass			*logout=NULL;
LFSTK_lineEditClass			*restart=NULL;
LFSTK_lineEditClass			*shutdown=NULL;

bool						mainLoop=true;
Display						*display;

int							panelHeightPref=32;
int							panelWidthPref=-2;
int							onMonitorPref=1;
int							panelPosPref=-2;
int							panelGravityPref=1;
char						*leftGadgetsPref=strdup("AWw");
char						*rightGadgetsPref=strdup("MDCL");
char						*terminalCommandPref=strdup("xterm -e ");
char						*logoutCommandPref=strdup("");
char						*restartCommandPref=strdup("sudo reboot");
char						*shutdownCommandPref=strdup("sudo shutdown -h now");

args	panelPrefs[]=
{
	{"panelheight",TYPEINT,&panelHeightPref},
	{"panelwidth",TYPEINT,&panelWidthPref},
	{"onmonitor",TYPEINT,&onMonitorPref},
	{"panelpos",TYPEINT,&panelPosPref},
	{"panelgrav",TYPEINT,&panelGravityPref},
	{"termcommand",TYPESTRING,&terminalCommandPref},
	{"logoutcommand",TYPESTRING,&logoutCommandPref},
	{"restartcommand",TYPESTRING,&restartCommandPref},
	{"shutdowncommand",TYPESTRING,&shutdownCommandPref},
	{"gadgetsright",TYPESTRING,&rightGadgetsPref},
	{"gadgetsleft",TYPESTRING,&leftGadgetsPref},
	{NULL,0,NULL}
};

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

void getEdits(void)
{
//printf("h=%i\n",atoi(panelHeightEdit->LFSTK_getBuffer()->c_str()));
	panelHeightPref=atoi(panelHeightEdit->LFSTK_getBuffer()->c_str());
	onMonitorPref=atoi(panelOnMonitor->LFSTK_getBuffer()->c_str());
	terminalCommandPref=strdup(termCommand->LFSTK_getBuffer()->c_str());
	logoutCommandPref=strdup(logout->LFSTK_getBuffer()->c_str());
	restartCommandPref=strdup(restart->LFSTK_getBuffer()->c_str());
	shutdownCommandPref=strdup(shutdown->LFSTK_getBuffer()->c_str());
	leftGadgetsPref=strdup(panelLeftGadgets->LFSTK_getBuffer()->c_str());
	rightGadgetsPref=strdup(panelrightGadgets->LFSTK_getBuffer()->c_str());

//	panelHeightPref=atoi(panelHeightEdit->LFSTK_getBuffer()->c_str());
//	panelHeightPref=atoi(panelHeightEdit->LFSTK_getBuffer()->c_str());
//	panelHeightPref=atoi(panelHeightEdit->LFSTK_getBuffer()->c_str());
//	panelHeightPref=atoi(panelHeightEdit->LFSTK_getBuffer()->c_str());
//	panelHeightPref=atoi(panelHeightEdit->LFSTK_getBuffer()->c_str());
return;

//	panelWidthEdit->LFSTK_setBuffer(panelWidthConvertToStr[panelWidthPref]);

	panelHeightEdit->LFSTK_setBuffer(std::to_string(panelHeightPref).c_str());
//	panelPosEdit->LFSTK_setBuffer(panelPosConvertToStr[panelPosPref]);
//	panelGravEdit->LFSTK_setBuffer(panelGravConvertToStr[panelGravityPref]);
	panelOnMonitor->LFSTK_setBuffer(std::to_string(onMonitorPref).c_str());
	termCommand->LFSTK_setBuffer(terminalCommandPref);
	logout->LFSTK_setBuffer(logoutCommandPref);
	restart->LFSTK_setBuffer(restartCommandPref);
	shutdown->LFSTK_setBuffer(shutdownCommandPref);
	panelLeftGadgets->LFSTK_setBuffer(leftGadgetsPref);
	panelrightGadgets->LFSTK_setBuffer(rightGadgetsPref);
}

bool applyCB(void *p,void* ud)
{
	char	*env=NULL;
	int		key;
	std::map<int,const char*>::const_iterator it;

	if(ud!=NULL)
		{
//panel width
			if(isdigit(panelWidthEdit->LFSTK_getBuffer()->c_str()[0])==true)
				panelWidthPref=atoi(panelWidthEdit->LFSTK_getBuffer()->c_str());
			else
				{
					if(strcmp(panelWidthEdit->LFSTK_getBuffer()->c_str(),"Fill")==0)
						panelWidthPref=-1;
					else
						panelWidthPref=-2;
				}
//panel pos
			if(isdigit(panelPosEdit->LFSTK_getBuffer()->c_str()[0])==true)
				panelPosPref=atoi(panelPosEdit->LFSTK_getBuffer()->c_str());
			else
				{
					key=-1;
					for(it=panelPosConvertToStr.begin();it != panelPosConvertToStr.end();++it)
						{
							if(strcmp(it->second,panelPosEdit->LFSTK_getBuffer()->c_str())==0)
								{
									panelPosPref=it->first;
									break;
								}
						}
				}
//panel grav
			if(isdigit(panelGravEdit->LFSTK_getBuffer()->c_str()[0])==true)
				panelGravityPref=atoi(panelGravEdit->LFSTK_getBuffer()->c_str());
			else
				{
					key=-1;
					for(it=panelGravConvertToStr.begin();it != panelGravConvertToStr.end();++it)
						{
							if(strcmp(it->second,panelGravEdit->LFSTK_getBuffer()->c_str())==0)
								{
									panelGravityPref=it->first;
									break;
								}
						}
				}

			getEdits();
			asprintf(&env,"%s/%s",wc->configDir,panelNameEdit->LFSTK_getBuffer()->c_str());
			wc->globalLib->LFSTK_saveVarsToFile(env,panelPrefs);
			free(env);
		}
	return(true);
}

void setEdits(void)
{
	panelWidthEdit->LFSTK_setBuffer(panelWidthConvertToStr[panelWidthPref]);
	panelHeightEdit->LFSTK_setBuffer(std::to_string(panelHeightPref).c_str());
	panelPosEdit->LFSTK_setBuffer(panelPosConvertToStr[panelPosPref]);
	panelGravEdit->LFSTK_setBuffer(panelGravConvertToStr[panelGravityPref]);
	panelOnMonitor->LFSTK_setBuffer(std::to_string(onMonitorPref).c_str());
	termCommand->LFSTK_setBuffer(terminalCommandPref);
	logout->LFSTK_setBuffer(logoutCommandPref);
	restart->LFSTK_setBuffer(restartCommandPref);
	shutdown->LFSTK_setBuffer(shutdownCommandPref);
	panelLeftGadgets->LFSTK_setBuffer(leftGadgetsPref);
	panelrightGadgets->LFSTK_setBuffer(rightGadgetsPref);
}

void getPrefs(void)
{
	char	*env=NULL;

	asprintf(&env,"%s/%s",wc->configDir,panelNameEdit->LFSTK_getBuffer()->c_str());
	wc->globalLib->LFSTK_loadVarsFromFile(env,panelPrefs);
	free(env);
}

bool panelSelectCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(ud==NULL)
		return(true);

	panelNameEdit->LFSTK_setBuffer(menuitem->label);
	getPrefs();
	setEdits();
	return(true);
}

bool panelWidthCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(ud==NULL)
		return(true);

	panelWidthEdit->LFSTK_setBuffer(panelWidthConvertToStr[(int)(long)menuitem->userData]);
	return(true);
}

bool panelPositionCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(ud==NULL)
		return(true);

	panelPosEdit->LFSTK_setBuffer(panelPosConvertToStr[(int)(long)menuitem->userData]);
	return(true);
}

bool panelGravCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(ud==NULL)
		return(true);

	panelGravEdit->LFSTK_setBuffer(panelGravConvertToStr[(int)(long)menuitem->userData]);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=0;
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

	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"LFSPanel Prefs",false);
	display=wc->display;
	//wc->LFSTK_initDnD();

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//menus
	FILE	*fp=NULL;
	char	*env=NULL;
	char	*buffer=NULL;
	size_t	n;
	int		retchars=0;
	int		numpanels=0;
	long	menuuserdata[3]={-1,-2,-3};

	buffer=wc->globalLib->LFSTK_oneLiner("find %s -maxdepth 1 -mindepth 1 -type f -name \"lfspanel*.rc\"|wc -l",wc->configDir);
	numpanels=atoi(buffer);
	free(buffer);
	buffer=NULL;
	panelNames=new menuItemStruct[numpanels];
	panelCnt=0;
	asprintf(&env,"find %s -maxdepth 1 -mindepth 1 -type f -name \"lfspanel*.rc\"",wc->configDir);
	fp=popen(env,"r");
	if(fp!=NULL)
		{
			while((retchars=getline(&buffer,&n,fp))!=-1)
				{
					if(retchars!=-1)
						{
							buffer[retchars-1]=0;
							panelNames[panelCnt].label=strdup(basename(buffer));
							panelNames[panelCnt].imagePath=NULL;
							panelNames[panelCnt].userData=(void*)(long)panelCnt;
							panelCnt++;
							free(buffer);
						}
					buffer=NULL;
				}
			pclose(fp);
		}

//panel config
//select
	selectPanel=new LFSTK_menuButtonClass(wc,"Panel Config",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	selectPanel->LFSTK_setCallBack(NULL,panelSelectCB,NULL);
	selectPanel->LFSTK_addMenus(panelNames,numpanels);

	panelNameEdit=new LFSTK_lineEditClass(wc,panelNames[0].label,BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*3,GADGETHITE,BUTTONGRAV);	
	sy+=YSPACING;

//do prefs
	getPrefs();

//width
	panelWidth=new LFSTK_menuButtonClass(wc,"Panel Width",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	panelWidthMenu=new menuItemStruct[2];
	for(int j=0;j<2;j++)
		{
			panelWidthMenu[j].label=(char*)panelWidthConvertToStr[menuuserdata[j]];
			panelWidthMenu[j].imagePath=NULL;
			panelWidthMenu[j].userData=(void*)menuuserdata[j];
		}
	panelWidth->LFSTK_addMenus(panelWidthMenu,2);
	panelWidthEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*3,GADGETHITE,BUTTONGRAV);
	panelWidth->LFSTK_setCallBack(NULL,panelWidthCB,NULL);
	sy+=YSPACING;

//hite
	label=new LFSTK_labelClass(wc,"Panel Height",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	panelHeightEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*3,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//panel position
	panelPos=new LFSTK_menuButtonClass(wc,"Panel Position",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	panelPosMenu=new menuItemStruct[4];
	for(int j=0;j<3;j++)
		{
			panelPosMenu[j].label=(char*)panelPosConvertToStr[menuuserdata[j]];
			panelPosMenu[j].imagePath=NULL;
			panelPosMenu[j].userData=(void*)menuuserdata[j];
		}
	panelPos->LFSTK_addMenus(panelPosMenu,3);

	panelPosEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*3,GADGETHITE,BUTTONGRAV);
	panelPos->LFSTK_setCallBack(NULL,panelPositionCB,NULL);
	sy+=YSPACING;

//panel grav
	panelGrav=new LFSTK_menuButtonClass(wc,"Panel Gravity",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	panelGravMenu=new menuItemStruct[4];
	for(long j=0;j<4;j++)
		{
			panelGravMenu[j].label=(char*)panelGravConvertToStr[j+1];
			panelGravMenu[j].imagePath=NULL;
			panelGravMenu[j].userData=(void*)(j+1);
		}
	panelGrav->LFSTK_addMenus(panelGravMenu,4);

	panelGravEdit=new LFSTK_lineEditClass(wc,"",BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*3,GADGETHITE,BUTTONGRAV);
	panelGrav->LFSTK_setCallBack(NULL,panelGravCB,NULL);
	sy+=YSPACING;

//on monitor
	label=new LFSTK_labelClass(wc,"On Monitor",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	panelOnMonitor=new LFSTK_lineEditClass(wc,std::to_string(onMonitorPref).c_str(),BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*3,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

//term comm
	label=new LFSTK_labelClass(wc,"Term command",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	termCommand=new LFSTK_lineEditClass(wc,terminalCommandPref,BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*3,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//logout
	label=new LFSTK_labelClass(wc,"Logout",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	logout=new LFSTK_lineEditClass(wc,logoutCommandPref,BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*3,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//restart
	label=new LFSTK_labelClass(wc,"Restart",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	restart=new LFSTK_lineEditClass(wc,restartCommandPref,BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*3,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//shutdown
	label=new LFSTK_labelClass(wc,"Shutdown",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	shutdown=new LFSTK_lineEditClass(wc,shutdownCommandPref,BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*3,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//left gadgets
	label=new LFSTK_labelClass(wc,"Left Gadgets",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	panelLeftGadgets=new LFSTK_lineEditClass(wc,leftGadgetsPref,BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*3,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
//rite gadgets
	label=new LFSTK_labelClass(wc,"Right Gadgets",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	panelrightGadgets=new LFSTK_lineEditClass(wc,rightGadgetsPref,BORDER+GADGETWIDTH+BORDER,sy,GADGETWIDTH*3,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	setEdits();

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
	apply->LFSTK_setCallBack(NULL,applyCB,(void*)"APPLY");
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
						}
						break;
				}
		}

	delete wc;
	XCloseDisplay(display);
	panelWidthConvertToStr.clear();
	panelPosConvertToStr.clear();
	panelGravConvertToStr.clear();
	delete panelGravMenu;
	delete panelPosMenu;
	delete panelWidthMenu;
	free(leftGadgetsPref);
	free(rightGadgetsPref);
	free(terminalCommandPref);
	free(logoutCommandPref);
	free(restartCommandPref);
	free(shutdownCommandPref);

	for(int j=0;j<panelCnt;j++)
		free(panelNames[j].label);
	delete panelNames;
	return 0;
}


