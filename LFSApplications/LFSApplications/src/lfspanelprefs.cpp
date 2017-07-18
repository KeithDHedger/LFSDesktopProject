/*
 *
 * ©K. D. Hedger. Thu 29 Oct 15:23:23 GMT 2015 kdhedger68713@gmail.com

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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include <lfstk/LFSTKGlobals.h>

#define BIG col2-col1+bwidth
#define MAXPANELS 20

enum {EXIT=0,DEFAULT,APPLY,NOMOREBUTTONS};
enum {PANELWIDTH=0,PANELPOS,PANELGRAV,PANELHITE,PANELMONITOR,TERMCOMMAND,LOGOUTCOMMAND,RESTARTCOMMAND,SHUTDOWNCOMMAND,LEFTGADGETS,RITEGADGETS,OPTIONSCNT};
enum {WIDTHFILL=-1,WIDTHSHRINK=-2,WIDTHCNT=3};
enum {LEFTPOS=-1,CENTREPOS=-2,RIGHTPOS=-3,POSCNT=4};
enum {NORTHGRAV=1,EASTGRAV=2,SOUTHGRAV=3,WESTGRAV=4,GRAVCNT=4};
enum {LPANELHITE=0,LONMONITOR,LTERMCOMMAND,LLOGOUT,LRESTART,LSHUTDOWN,LLEFTGADGETS,LRITEGADGETS,LBAR,MAXLABELS};

LFSTK_buttonClass		*guibc[NOMOREBUTTONS]={NULL,};
LFSTK_windowClass		*wc;
LFSTK_lineEditClass		*currentPanel=NULL;
LFSTK_menuButtonClass	*panelSelect=NULL;
LFSTK_menuButtonClass	*panelOptionsMenus[OPTIONSCNT]={NULL,};
LFSTK_lineEditClass		*panelOptionsEdit[OPTIONSCNT]={NULL,};
LFSTK_labelClass		*labels[MAXLABELS];

const char				*panelOptionString[OPTIONSCNT][4]={{"Fill","Shrink","Custom",""},{"Left","Centre","Right","Custom"},{"North","East","South","West"}};
const char				*labelNames[MAXLABELS]={"Panel Height","On Monitor","Terminal","Logout","Restart","Shutdown","Left Gadgets","Right Gadgets","--"};

menuItemStruct			*panels;
menuItemStruct			panelWidthMenu[WIDTHCNT];
menuItemStruct			panelPosMenu[POSCNT];
menuItemStruct			panelGravMenu[GRAVCNT];

int						bwidth=96;
int						bigbwidth=128;
int						spacing=bwidth+10;
int						col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;
bool					mainloop=false;
int						numGroups=0;
char					currentBuffer[256];
int						parentWindow=-1;

const monitorStruct		*mons=NULL;
//prefs
char					*terminalCommand=NULL;
int						panelHeight;
int						panelWidth;
int						onMonitor;
char					*logoutCommand=NULL;
char					*restartCommand=NULL;
char					*shutdownCommand=NULL;
char					*rightGadgets=NULL;
char					*leftGadgets=NULL;
int						panelPos;
int						panelGravity;

args	panelPrefs[]=
{
	{"panelheight",TYPEINT,&panelHeight},
	{"panelwidth",TYPEINT,&panelWidth},
	{"onmonitor",TYPEINT,&onMonitor},
	{"termcommand",TYPESTRING,&terminalCommand},
	{"logoutcommand",TYPESTRING,&logoutCommand},
	{"restartcommand",TYPESTRING,&restartCommand},
	{"shutdowncommand",TYPESTRING,&shutdownCommand},
	{"gadgetsright",TYPESTRING,&rightGadgets},
	{"gadgetsleft",TYPESTRING,&leftGadgets},
	{"panelpos",TYPEINT,&panelPos},
	{"panelgrav",TYPEINT,&panelGravity},
	{NULL,0,NULL}
};

void clearPrefs(void)
{
	if(terminalCommand!=NULL)
		free(terminalCommand);
	terminalCommand=NULL;
	if(logoutCommand!=NULL)
		free(logoutCommand);
	logoutCommand=NULL;
	if(restartCommand!=NULL)
		free(restartCommand);
	restartCommand=NULL;
	if(shutdownCommand!=NULL)
		free(shutdownCommand);
	shutdownCommand=NULL;
	if(rightGadgets!=NULL)
		free(rightGadgets);
	rightGadgets=NULL;
	if(leftGadgets!=NULL)
		free(leftGadgets);
	leftGadgets=NULL;

	panelOptionsEdit[PANELWIDTH]->LFSTK_setBuffer("");
	panelOptionsEdit[PANELHITE]->LFSTK_setBuffer("");
	panelOptionsEdit[PANELPOS]->LFSTK_setBuffer("");
	panelOptionsEdit[PANELGRAV]->LFSTK_setBuffer("");
	panelOptionsEdit[PANELMONITOR]->LFSTK_setBuffer("");
	panelOptionsEdit[TERMCOMMAND]->LFSTK_setBuffer("");
	panelOptionsEdit[LOGOUTCOMMAND]->LFSTK_setBuffer("");
	panelOptionsEdit[RESTARTCOMMAND]->LFSTK_setBuffer("");
	panelOptionsEdit[SHUTDOWNCOMMAND]->LFSTK_setBuffer("");
	panelOptionsEdit[LEFTGADGETS]->LFSTK_setBuffer("");
	panelOptionsEdit[RITEGADGETS]->LFSTK_setBuffer("");
}

void setEdits(void)
{
	char	buffer[512];

	sprintf(buffer,"%i",panelWidth);
	panelOptionsEdit[PANELWIDTH]->LFSTK_setBuffer(buffer);
	sprintf(buffer,"%i",panelHeight);
	panelOptionsEdit[PANELHITE]->LFSTK_setBuffer(buffer);
	sprintf(buffer,"%i",panelPos);
	panelOptionsEdit[PANELPOS]->LFSTK_setBuffer(buffer);
	sprintf(buffer,"%i",panelGravity);
	panelOptionsEdit[PANELGRAV]->LFSTK_setBuffer(buffer);
	sprintf(buffer,"%i",onMonitor);
	panelOptionsEdit[PANELMONITOR]->LFSTK_setBuffer(buffer);
	if(terminalCommand!=NULL)
		panelOptionsEdit[TERMCOMMAND]->LFSTK_setBuffer(terminalCommand);
	if(logoutCommand!=NULL)
		panelOptionsEdit[LOGOUTCOMMAND]->LFSTK_setBuffer(logoutCommand);
	if(restartCommand!=NULL)
		panelOptionsEdit[RESTARTCOMMAND]->LFSTK_setBuffer(restartCommand);
	if(shutdownCommand!=NULL)
		panelOptionsEdit[SHUTDOWNCOMMAND]->LFSTK_setBuffer(shutdownCommand);
	if(leftGadgets!=NULL)
		panelOptionsEdit[LEFTGADGETS]->LFSTK_setBuffer(leftGadgets);
	if(rightGadgets!=NULL)
		panelOptionsEdit[RITEGADGETS]->LFSTK_setBuffer(rightGadgets);
}

void getPrefs(void)
{
	char	*env=NULL;

	asprintf(&env,"%s/.config/LFS/%s",getenv("HOME"),currentPanel->LFSTK_getBuffer()->c_str());
	wc->globalLib->LFSTK_loadVarsFromFile(env,panelPrefs);
	free(env);
	setEdits();
}

void setDefaults(void)
{
	clearPrefs();

	terminalCommand=strdup("xterm -e ");
	logoutCommand=strdup("xterm");
	restartCommand=strdup("xterm");
	shutdownCommand=strdup("xterm");
	leftGadgets=strdup("A");
	rightGadgets=strdup("L");

	panelHeight=16;
	panelWidth=-1;
	onMonitor=0;
	panelPos=CENTREPOS;
	panelGravity=NORTHGRAV;
}

void setPrefs(void)
{
	if(panelOptionsEdit[PANELWIDTH]->LFSTK_getBuffer()->length()>0)
		panelWidth=atoi(panelOptionsEdit[PANELWIDTH]->LFSTK_getBuffer()->c_str());

	if(panelOptionsEdit[PANELHITE]->LFSTK_getBuffer()->length()>0)
		panelHeight=atoi(panelOptionsEdit[PANELHITE]->LFSTK_getBuffer()->c_str());

	if(panelOptionsEdit[PANELPOS]->LFSTK_getBuffer()->length()>0)
		panelPos=atoi(panelOptionsEdit[PANELPOS]->LFSTK_getBuffer()->c_str());

	if(panelOptionsEdit[PANELGRAV]->LFSTK_getBuffer()->length()>0)
		panelGravity=atoi(panelOptionsEdit[PANELGRAV]->LFSTK_getBuffer()->c_str());

	if(panelOptionsEdit[PANELMONITOR]->LFSTK_getBuffer()->length()>0)
		onMonitor=atoi(panelOptionsEdit[PANELMONITOR]->LFSTK_getBuffer()->c_str());

	if(terminalCommand!=NULL)
		free(terminalCommand);
	if(panelOptionsEdit[TERMCOMMAND]->LFSTK_getBuffer()->length()>0)
		terminalCommand=strdup(panelOptionsEdit[TERMCOMMAND]->LFSTK_getBuffer()->c_str());
	else
		terminalCommand=NULL;

	if(logoutCommand!=NULL)
		free(logoutCommand);
	if(panelOptionsEdit[LOGOUTCOMMAND]->LFSTK_getBuffer()->length()>0)
		logoutCommand=strdup(panelOptionsEdit[LOGOUTCOMMAND]->LFSTK_getBuffer()->c_str());
	else
		logoutCommand=NULL;

	if(restartCommand!=NULL)
		free(restartCommand);
	if(panelOptionsEdit[RESTARTCOMMAND]->LFSTK_getBuffer()->length()>0)
		restartCommand=strdup(panelOptionsEdit[RESTARTCOMMAND]->LFSTK_getBuffer()->c_str());
	else
		restartCommand=NULL;

	if(shutdownCommand!=NULL)
		free(shutdownCommand);
	if(panelOptionsEdit[SHUTDOWNCOMMAND]->LFSTK_getBuffer()->length()>0)
		shutdownCommand=strdup(panelOptionsEdit[SHUTDOWNCOMMAND]->LFSTK_getBuffer()->c_str());
	else
		shutdownCommand=NULL;

	if(rightGadgets!=NULL)
		free(rightGadgets);
	if(panelOptionsEdit[RITEGADGETS]->LFSTK_getBuffer()->length()>0)
		rightGadgets=strdup(panelOptionsEdit[RITEGADGETS]->LFSTK_getBuffer()->c_str());
	else
		rightGadgets=NULL;

	if(leftGadgets!=NULL)
		free(leftGadgets);
	if(panelOptionsEdit[LEFTGADGETS]->LFSTK_getBuffer()->length()>0)
		leftGadgets=strdup(panelOptionsEdit[LEFTGADGETS]->LFSTK_getBuffer()->c_str());
	else
		leftGadgets=NULL;
}

bool callback(void *p,void* ud)
{
	char	*env=NULL;

	switch((long)ud)
		{
			case EXIT:
				mainloop=false;
				return(false);
				break;
			case DEFAULT:
				setDefaults();
				setEdits();
				break;
			case APPLY:
				setPrefs();
				asprintf(&env,"%s/.config/LFS/%s",getenv("HOME"),currentPanel->LFSTK_getBuffer()->c_str());
				wc->globalLib->LFSTK_saveVarsToFile("-",panelPrefs);
				wc->globalLib->LFSTK_saveVarsToFile(env,panelPrefs);
				free(env);
				system("killall lfspanel");
				system("lfslaunchpanels");
				break;
		}
	return(true);
}

bool panelSelectCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(menuitem==NULL)
		return(true);

	currentPanel->LFSTK_setBuffer(menuitem->label);
	clearPrefs();
	getPrefs();
	currentPanel->LFSTK_clearWindow();
	return(true);
}

bool setWidthCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;
	char			buffer[16];
	if(menuitem==NULL)
		return(true);

	snprintf(buffer,16,"%i",(long)menuitem->userData);
	panelOptionsEdit[PANELWIDTH]->LFSTK_setBuffer(buffer);
	panelOptionsEdit[PANELWIDTH]->LFSTK_clearWindow();
	return(true);
}

bool setPosCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;
	char			buffer[16];
	if(menuitem==NULL)
		return(true);

	snprintf(buffer,16,"%i",(long)menuitem->userData);
	panelOptionsEdit[PANELPOS]->LFSTK_setBuffer(buffer);
	panelOptionsEdit[PANELPOS]->LFSTK_clearWindow();
	return(true);
}

bool setGravCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;
	char			buffer[16];
	if(menuitem==NULL)
		return(true);

	snprintf(buffer,16,"%i",(long)menuitem->userData);
	panelOptionsEdit[PANELGRAV]->LFSTK_setBuffer(buffer);
	panelOptionsEdit[PANELGRAV]->LFSTK_clearWindow();
	return(true);
}

int main(int argc, char **argv)
{
	XEvent					event;
	int						sx=0;
	int						sy=0;
	const geometryStruct	*geom;
	int						bhite=24;
	int						vspacing=bhite+10;
	FILE*					fp=NULL;
	char					*command;
	char					*lfspanels;
	char					buffer[512];
	int						c=0;
	int						option_index=0;
	const char				*shortOpts="h?w:";
	Display					*display;
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

	wc=new LFSTK_windowClass(sx,sy,1,1,"LFSPanel Prefs",false);
	display=wc->display;
	wc->LFSTK_setDecorated(true);
	wc->autoLabelColour=true;
	geom=wc->LFSTK_getWindowGeom();
	guibc[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	guibc[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	guibc[DEFAULT]=new LFSTK_buttonClass(wc,"Defaults",(geom->w/2)-32,geom->h-32,64,24,SouthGravity);
	guibc[DEFAULT]->LFSTK_setCallBack(NULL,callback,(void*)DEFAULT);

	guibc[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	guibc[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

//select panel
	sx=col1;
	sy=16;
	panelSelect=new LFSTK_menuButtonClass(wc,"Panel Config",sx,sy,bwidth,24,NorthWestGravity);
	panelSelect->LFSTK_setStyle(BEVELOUT);
	panelSelect->LFSTK_setLabelGravity(CENTRE);
	panels=new menuItemStruct[MAXPANELS];
	panelSelect->LFSTK_setCallBack(NULL,panelSelectCB,NULL);
	
	sx+=spacing;;
	currentPanel=new LFSTK_lineEditClass(wc,"lfspanel.rc",sx,sy-1,BIG,24,NorthWestGravity);

	int		cnt=0;
	snprintf(buffer,512,"find %s/.config/LFS -maxdepth 1 -mindepth 1 -type f -name \"lfspanel*.rc\"",getenv("HOME"));
	fp=popen(buffer,"r");
	if(fp!=NULL)
		{
			while(fgets(buffer,512,fp))
				{
					buffer[strlen(buffer)-1]=0;
					panels[cnt].label=strdup(basename(buffer));
					panels[cnt].userData=(void*)(long)cnt;
					panels[cnt].subMenus=NULL;
					panels[cnt].imagePath=NULL;
					panels[cnt].bc=NULL;
					cnt++;
				}
			pclose(fp);
		}

	panelSelect->LFSTK_addMenus(panels,cnt);
	sy+=vspacing;

//panel width
	for(int j=0;j<WIDTHCNT;j++)
		{
			panelWidthMenu[j].label=panelOptionString[PANELWIDTH][j];
			panelWidthMenu[j].subMenus=NULL;
			panelWidthMenu[j].imagePath=NULL;
			panelWidthMenu[j].bc=NULL;
		}
	panelWidthMenu[0].userData=(void*)-1;
	panelWidthMenu[1].userData=(void*)-2;
	panelWidthMenu[2].userData=(void*)0;

	sx=col1;
	panelOptionsMenus[PANELWIDTH]=new LFSTK_menuButtonClass(wc,"Panel Width",sx,sy,bwidth,24,NorthWestGravity);
	panelOptionsMenus[PANELWIDTH]->LFSTK_addMenus(panelWidthMenu,WIDTHCNT);
	panelOptionsMenus[PANELWIDTH]->LFSTK_setCallBack(NULL,setWidthCB,NULL);
	sx+=spacing;
	panelOptionsEdit[PANELWIDTH]=new LFSTK_lineEditClass(wc,"",sx,sy,BIG,24,NorthWestGravity);
	sy+=vspacing;

//panel height
	sx=col1;
	labels[LPANELHITE]=new LFSTK_labelClass(wc,labelNames[LPANELHITE],sx,sy,bwidth,24,NorthWestGravity);
	sx+=spacing;
	panelOptionsEdit[PANELHITE]=new LFSTK_lineEditClass(wc,"",sx,sy,BIG,24,NorthWestGravity);
	sy+=vspacing;
	
//panel position
	for(int j=0;j<POSCNT;j++)
		{
			panelPosMenu[j].label=panelOptionString[PANELPOS][j];
			panelPosMenu[j].subMenus=NULL;
			panelPosMenu[j].imagePath=NULL;
			panelPosMenu[j].bc=NULL;
		}
	panelPosMenu[0].userData=(void*)LEFTPOS;
	panelPosMenu[1].userData=(void*)CENTREPOS;
	panelPosMenu[2].userData=(void*)RIGHTPOS;
	panelPosMenu[3].userData=(void*)0;

	sx=col1;
	panelOptionsMenus[PANELPOS]=new LFSTK_menuButtonClass(wc,"Panel Position",sx,sy,bwidth,24,NorthWestGravity);
	panelOptionsMenus[PANELPOS]->LFSTK_addMenus(panelPosMenu,POSCNT);
	panelOptionsMenus[PANELPOS]->LFSTK_setCallBack(NULL,setPosCB,NULL);
	sx+=spacing;
	panelOptionsEdit[PANELPOS]=new LFSTK_lineEditClass(wc,"",sx,sy,BIG,24,NorthWestGravity);
	sy+=vspacing;

//panel grav
	for(int j=0;j<GRAVCNT;j++)
		{
			panelGravMenu[j].label=panelOptionString[PANELGRAV][j];
			panelGravMenu[j].subMenus=NULL;
			panelGravMenu[j].imagePath=NULL;
			panelGravMenu[j].bc=NULL;
		}
	panelGravMenu[0].userData=(void*)NORTHGRAV;
	panelGravMenu[1].userData=(void*)EASTGRAV;
	panelGravMenu[2].userData=(void*)SOUTHGRAV;
	panelGravMenu[3].userData=(void*)WESTGRAV;

	sx=col1;
	panelOptionsMenus[PANELGRAV]=new LFSTK_menuButtonClass(wc,"Panel Gravity",sx,sy,bwidth,24,NorthWestGravity);
	panelOptionsMenus[PANELGRAV]->LFSTK_addMenus(panelGravMenu,GRAVCNT);
	panelOptionsMenus[PANELGRAV]->LFSTK_setCallBack(NULL,setGravCB,NULL);
	sx+=spacing;
	panelOptionsEdit[PANELGRAV]=new LFSTK_lineEditClass(wc,"",sx,sy,BIG,24,NorthWestGravity);
	sy+=vspacing;

//on monitor
	sx=col1;
	labels[LONMONITOR]=new LFSTK_labelClass(wc,labelNames[LONMONITOR],sx,sy,bwidth,24,NorthWestGravity);
	sx+=spacing;
	panelOptionsEdit[PANELMONITOR]=new LFSTK_lineEditClass(wc,"",sx,sy,BIG,24,NorthWestGravity);
	sy+=vspacing;

//term command
	sx=col1;
	labels[LTERMCOMMAND]=new LFSTK_labelClass(wc,labelNames[LTERMCOMMAND],sx,sy,bwidth,24,NorthWestGravity);
	sx+=spacing;
	panelOptionsEdit[TERMCOMMAND]=new LFSTK_lineEditClass(wc,"",sx,sy,BIG,24,NorthWestGravity);
	sy+=vspacing;

//logout command
	sx=col1;
	labels[LLOGOUT]=new LFSTK_labelClass(wc,labelNames[LLOGOUT],sx,sy,bwidth,24,NorthWestGravity);
	sx+=spacing;
	panelOptionsEdit[LOGOUTCOMMAND]=new LFSTK_lineEditClass(wc,"",sx,sy,BIG,24,NorthWestGravity);
	sy+=vspacing;
//restart command
	sx=col1;
	labels[LRESTART]=new LFSTK_labelClass(wc,labelNames[LRESTART],sx,sy,bwidth,24,NorthWestGravity);
	sx+=spacing;
	panelOptionsEdit[RESTARTCOMMAND]=new LFSTK_lineEditClass(wc,"",sx,sy,BIG,24,NorthWestGravity);
	sy+=vspacing;
//shutdown command
	sx=col1;
	labels[LSHUTDOWN]=new LFSTK_labelClass(wc,labelNames[LSHUTDOWN],sx,sy,bwidth,24,NorthWestGravity);
	sx+=spacing;
	panelOptionsEdit[SHUTDOWNCOMMAND]=new LFSTK_lineEditClass(wc,"",sx,sy,BIG,24,NorthWestGravity);
	sy+=vspacing;
//gadgets left
	sx=col1;
	labels[LLEFTGADGETS]=new LFSTK_labelClass(wc,labelNames[LLEFTGADGETS],sx,sy,bwidth,24,NorthWestGravity);
	sx+=spacing;
	panelOptionsEdit[LEFTGADGETS]=new LFSTK_lineEditClass(wc,"",sx,sy,BIG,24,NorthWestGravity);
	sy+=vspacing;
//gadgets rite
	sx=col1;
	labels[LRITEGADGETS]=new LFSTK_labelClass(wc,labelNames[LRITEGADGETS],sx,sy,bwidth,24,NorthWestGravity);
	sx+=spacing;
	panelOptionsEdit[RITEGADGETS]=new LFSTK_lineEditClass(wc,"",sx,sy,BIG,24,NorthWestGravity);
	sy+=vspacing;

	setDefaults();
	getPrefs();

	sx=col1;
	labels[LBAR]=new LFSTK_labelClass(wc,labelNames[LBAR],0,sy,col3,8,NorthWestGravity);
	sy+=vspacing;
	sy+=16;
	
	wc->LFSTK_resizeWindow(col3-10,sy);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

	XFlush(wc->display);
	XSync(wc->display,false);

	mainloop=true;
	while(mainloop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);
			switch(event.type)
				{
						break;
					case Expose:
						wc->LFSTK_clearWindow();
						wc->LFSTK_sendMessage("_NET_ACTIVE_WINDOW",0,0,0,0,0);
						break;
					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
						break;
					case ClientMessage:
					case SelectionNotify:
						if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
							mainloop=false;
						if(wc->acceptDnd==true)
							{
								wc->LFSTK_handleDnD(&event);
							}
					default:
						break;
				}
		}

	wc->LFSTK_hideWindow();

	delete wc;
	XCloseDisplay(display);
	return(0);
}
