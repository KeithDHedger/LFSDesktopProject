/*
 *
 * ©K. D. Hedger. Mon  7 Sep 13:20:24 BST 2015 kdhedger68713@gmail.com

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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <lfstk/LFSTKGlobals.h>

#define BWIDTH 104
#define subscnt 26

enum {BACTIVEFRAME=0,BACTIVEFILL,BINACTIVEFRAME,BINACTIVEFILL,BWIDGETCOLOUR,NOMOREBUTTONS};
enum {EACTIVEFRAME=0,EACTIVEFILL,EINACTIVEFRAME,EINACTIVEFILL,EWIDGETCOLOUR,ETHEMEPATH,ETERMCOMMAND,EPLACEMENT,ENUMDESKS,ELIVEUPDATE,NOMOREEDITS};
enum {ACTIVEFRAME=0,ACTIVEFRAMEFILL,INACTIVEFRAME,INACTIVEFRAMEFILL,TEXTCOLOUR};
enum {EXIT=0,APPLY,PRINT,NOMORE};
enum {THEMELABEL=0,TERMLABEL,PLACELABEL,DESKLABEL,UPDATELABEL,LSPACER,NOMORELABELS};
enum places {NOPLACE=0,UNDERMOUSE,CENTREMMONITOR,CENTRESCREEN,MOUSEMONITOR,NOMOREPLACES};

const char				*buttonnames[]= {"Active Frame","Active Fill","Inactive Frame","Inactive Fill","Text Colour"};
const char				*labelnames[]= {"Theme Path","Term Command"," Place Windows","Desktops", "Update","--"};
const char				*placeNames[]={"Smart Place On Screen","Under Mouse","Centre On Monitor With Mouse","Centre On Screen","Smart Place On Monitor With Mouse"};

//prefs
int						placement=0;
char					*titleFont=NULL;
char					*numberOfDesktops=NULL;
char					*liveUpdate=NULL;
char					*pathToTheme=NULL;
char					*terminalCommand=NULL;

char					*fontColours[5];
int						doswapdesk=-1;


menuItemStruct			*placeMenu;
menuItemStruct			*fontMenu;
menuItemStruct			*fontSubMenus[subscnt];

LFSTK_lineEditClass		*fontEdit;
LFSTK_lineEditClass		*fontSizeEdit;
LFSTK_labelClass		*label;
LFSTK_labelClass		*labelFontSize;
LFSTK_toggleButtonClass	*bold;
LFSTK_toggleButtonClass	*italic;
LFSTK_labelClass		*text;

char					*holdFont;
unsigned				fSize=10;

char					*fontName=NULL;
char					*fontSize=NULL;
bool					isBold=false;
bool					isItalic=false;

args					wmPrefs[]=
{
	{"wmactive_frame",TYPESTRING,&fontColours[ACTIVEFRAME]},
	{"wmactive_fill",TYPESTRING,&fontColours[ACTIVEFRAMEFILL]},
	{"wminactive_frame",TYPESTRING,&fontColours[INACTIVEFRAME]},
	{"wminactive_fill",TYPESTRING,&fontColours[INACTIVEFRAMEFILL]},
	{"widgetcolour",TYPESTRING,&fontColours[TEXTCOLOUR]},
	{"placement",TYPEINT,&placement},
	{"titlefont",TYPESTRING,&titleFont},
	{"desktops",TYPESTRING,&numberOfDesktops},
	{"liveupdate",TYPESTRING,&liveUpdate},
	{"theme",TYPESTRING,&pathToTheme},
	{"termcommand",TYPESTRING,&terminalCommand},
	{"fontname",TYPESTRING,&fontName},
	{"fontsize",TYPESTRING,&fontSize},
	{"bold",TYPEBOOL,&isBold},
	{"italic",TYPEBOOL,&isItalic},
	{NULL,0,NULL}
};

bool					mainloop=false;
LFSTK_windowClass		*wc;
LFSTK_buttonClass		*bc[NOMOREBUTTONS]= {NULL,};
LFSTK_buttonClass		*guibc[NOMORE]={NULL,};
LFSTK_lineEditClass		*le[NOMOREEDITS]= {NULL,};
LFSTK_labelClass		*lb[NOMORELABELS]= {NULL,};
LFSTK_menuButtonClass	*mb=NULL;
LFSTK_menuButtonClass	*fontButton=NULL;

char					*env;

void placeToString(long place)
{
	le[EPLACEMENT]->LFSTK_setBuffer(placeNames[place]);
	le[EPLACEMENT]->LFSTK_clearWindow();
}

void buildFontString(void)
{
	const char	*boldstr="";
	const char	*italicstr="";

	if(isBold==true)
		boldstr=":bold";
	if(isItalic==true)
		italicstr=":italic";

	fontSize=strdup(static_cast<const char*>(fontSizeEdit->LFSTK_getBuffer()->c_str()));

	if(titleFont!=NULL)
		free(titleFont);
	asprintf(&titleFont,"%s:size=%s%s%s",fontName,fontSize,boldstr,italicstr);

	text->LFSTK_setFontString(titleFont);
	text->LFSTK_clearWindow();
}

void setVars(void)
{
	for(int j=BACTIVEFRAME; j<NOMOREBUTTONS; j++)
		if(bc[j]!=NULL)
			{
				bc[j]->LFSTK_setColourName(INACTIVECOLOUR,le[j]->LFSTK_getBuffer()->c_str());
				bc[j]->LFSTK_clearWindow();
			}
	for(int j=EACTIVEFRAME; j<ETHEMEPATH; j++)
		if(le[j]!=NULL)
			{
				if(fontColours[j]!=NULL)
					free(fontColours[j]);
				fontColours[j]=strdup((char*)le[j]->LFSTK_getBuffer()->c_str());
				le[j]->LFSTK_clearWindow();
			}
	for(int j=THEMELABEL; j<NOMORELABELS; j++)
		if(lb[j]!=NULL)
			lb[j]->LFSTK_clearWindow();

	buildFontString();

	if(numberOfDesktops!=NULL)
		free(numberOfDesktops);
	numberOfDesktops=strdup(static_cast<const char*>(le[ENUMDESKS]->LFSTK_getBuffer()->c_str()));
	if(liveUpdate!=NULL)
		free(liveUpdate);
	liveUpdate=strdup(static_cast<const char*>(le[ELIVEUPDATE]->LFSTK_getBuffer()->c_str()));
	if(pathToTheme!=NULL)
		free(pathToTheme);
	pathToTheme=strdup(static_cast<const char*>(le[ETHEMEPATH]->LFSTK_getBuffer()->c_str()));
	if(terminalCommand!=NULL)
		free(terminalCommand);
	terminalCommand=strdup(static_cast<const char*>(le[ETERMCOMMAND]->LFSTK_getBuffer()->c_str()));
}

bool placeCB(void *p,void* ud)
{
	menuItemStruct	*menuitem=(menuItemStruct*)ud;

	if(menuitem==NULL)
		return(true);
	placement=(int)(long)menuitem->userData;
	placeToString((long)menuitem->userData);

	return(true);
}

bool styleCB(void *object,void* userdata)
{
	if((long)userdata==0)
		isBold=!isBold;
	else
		isItalic=!isItalic;

	buildFontString();
	return(true);
}

bool fontsCB(void *object,void* userdata)
{
	free(fontName);
	fontName=strdup(static_cast<LFSTK_gadgetClass*>(object)->LFSTK_getLabel());
	fontEdit->LFSTK_setBuffer(fontName);
	buildFontString();
	return(true);
}

bool callback(void *p,void* ud)
{
	if((long)ud==EXIT)
		{
			wc->LFSTK_clearWindow();
			setVars();
			wc->globalLib->LFSTK_saveVarsToFile("-",wmPrefs);
			printf("\n");
			mainloop=false;
			return(false);
		}

	switch((long)ud)
		{
		case PRINT:
			wc->LFSTK_clearWindow();
			setVars();
			wc->globalLib->LFSTK_saveVarsToFile("-",wmPrefs);
			printf("\n");
			break;

		case APPLY:
			wc->LFSTK_clearWindow();
			setVars();
			wc->globalLib->LFSTK_saveVarsToFile(env,wmPrefs);
			system("killall lfswmanager");
			sleep(1);
			system("lfswmanager &");
			break;
			break;
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent			event;
	int				sx=0;
	int				sy=0;
	geometryStruct	*geom;
	int				bwidth=BWIDTH;
	int				bhite=24;
	int				spacing=bwidth+10;
	int				vspacing=bhite+10;
	int				col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;
	char			*hfont;

	fontColours[ACTIVEFRAME]=strdup("#000000");
	fontColours[ACTIVEFRAMEFILL]=strdup("#00ffff");
	fontColours[INACTIVEFRAME]=strdup("#000000");
	fontColours[INACTIVEFRAMEFILL]=strdup("#ffffff");
	fontColours[TEXTCOLOUR]=strdup("#ffffff");

	titleFont=strdup("sans:size=10");
	numberOfDesktops=strdup("12");
	liveUpdate=strdup("5");
	pathToTheme=strdup("");
	terminalCommand=strdup("xterm -e ");

	wc=new LFSTK_windowClass(sx,sy,800,600,"LFS WM Prefs",false);
	wc->LFSTK_setDecorated(true);
	geom=wc->LFSTK_getGeom();

	fontName=strdup("sans");
	fontSize=strdup("12");
	isBold=false;
	isItalic=false;

	asprintf(&env,"%s/.config/LFS/lfswmanager.rc",getenv("HOME"));
	wc->globalLib->LFSTK_loadVarsFromFile(env,wmPrefs);

	guibc[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	guibc[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	guibc[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	guibc[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

	guibc[PRINT]=new LFSTK_buttonClass(wc,"Print",(geom->w/2)-(64/2),geom->h-32,64,24,SouthGravity);
	guibc[PRINT]->LFSTK_setCallBack(NULL,callback,(void*)PRINT);
	sx=col1;
	sy=10;
	int state=0;
	for (int j=BACTIVEFRAME; j<NOMOREBUTTONS; j++)
		{
			bc[j]=new LFSTK_buttonClass(wc,buttonnames[state],sx,sy,bwidth,24,NorthWestGravity);
			bc[j]->LFSTK_setActive(false);
			bc[j]->LFSTK_setLabelAutoColour(true);
			bc[j]->LFSTK_setColourName(INACTIVECOLOUR,fontColours[state]);
			bc[j]->LFSTK_setTile("",-1);
			sx+=spacing;
			le[j]=new LFSTK_lineEditClass(wc,fontColours[state],sx,sy-1,bwidth,24,NorthWestGravity);
			sy+=vspacing;
			sx=col1;
			state++;
		}

	sx=col1;
	for(int j=THEMELABEL; j<PLACELABEL; j++)
		{
			lb[j]=new LFSTK_labelClass(wc,labelnames[j],sx,sy,bwidth,24,NorthWestGravity);
			lb[j]->LFSTK_setLabelAutoColour(true);
			lb[j]->LFSTK_setLabelOriention(0);
			sx+=spacing;
			le[ETHEMEPATH+j]=new LFSTK_lineEditClass(wc,labelnames[j],sx,sy-1,col2-col1,24,NorthWestGravity);
			sy+=vspacing;
			sx=col1;
		}

	unsigned	catcnt[subscnt]={0,};
	FILE		*fp;
	char		*command;
	char		lower='a';
	char		upper='A';
	char		*out=NULL;
	char		**fontsAZ[subscnt];
	char		line[1024];

	line[0]=0;

	for(int j=0;j<subscnt;j++)
		{
			asprintf(&command,"%s%c%c%s","fc-list : family|awk -F, '{print $1}'|sed -n /^[",lower,upper,"]/p|sort -u|wc -l");
			command=wc->globalLib->LFSTK_oneLiner("%s",command);
			if(command==NULL)
				catcnt[j]=0;
			else
				catcnt[j]=atoi(command);
			fontsAZ[j]=(char**)calloc(catcnt[j],sizeof(char*));
			fontSubMenus[j]=new menuItemStruct[catcnt[j]];
			lower++;
			upper++;
			free(command);
		}

	lower='a';
	upper='A';
	unsigned	cnt=0;
	for(int j=0;j<subscnt;j++)
		{
			cnt=0;
			asprintf(&command,"%s%c%c%s","fc-list : family|awk -F, '{print $1}'|sed -n /^[",lower,upper,"]/p|sort -u");
			fp=popen(command, "r");
			while(fgets(line,1024,fp))
				{
					line[strlen(line)-1]=0;
					fontsAZ[j][cnt]=strdup(line);
					cnt++;
				}
			lower++;
			upper++;
			free(command);
		}

	for(int j=0;j<subscnt;j++)
		{
			fontSubMenus[j]=new menuItemStruct[catcnt[j]];
			for(int k=0;k<catcnt[j];k++)
				fontSubMenus[j][k].label=fontsAZ[j][k];
		}

	upper='A';
	fontMenu=new menuItemStruct[subscnt];
	for(int j=0;j<subscnt;j++)
		{
			asprintf(&command,"Fonts %c   ",upper);
			fontMenu[j].label=command;
			if(catcnt[j]>0)
				{
					fontMenu[j].subMenus=fontSubMenus[j];
					fontMenu[j].subMenuCnt=catcnt[j];
				}
			upper++;
		}

	fontButton=new LFSTK_menuButtonClass(wc,"Fonts",sx,sy,bwidth,24,NorthWestGravity);
	fontButton->LFSTK_addMenus(fontMenu,subscnt);
	fontButton->LFSTK_setCallBack(NULL,fontsCB,NULL);

	sx+=spacing;
	fontEdit=new LFSTK_lineEditClass(wc,fontName,sx,sy-1,col2-col1,24,NorthWestGravity);
	sy+=vspacing;
	sx=col1;

	bold=new LFSTK_toggleButtonClass(wc,"Bold",sx,sy,bwidth,24,NorthWestGravity);
	bold->LFSTK_setValue(isBold);
	sx+=spacing;
	italic=new LFSTK_toggleButtonClass(wc,"Italic",sx,sy,bwidth,24,NorthWestGravity);
	italic->LFSTK_setValue(isItalic);
	bold->LFSTK_setCallBack(NULL,styleCB,(void*)0);
	italic->LFSTK_setCallBack(NULL,styleCB,(void*)1);
	sy+=vspacing;
	sx=col1;

	labelFontSize=new LFSTK_labelClass(wc,"Font Size",sx,sy,bwidth,24,NorthWestGravity);
	labelFontSize->LFSTK_setLabelAutoColour(true);
	labelFontSize->LFSTK_setLabelOriention(0);

	sx+=spacing;
	fontSizeEdit=new LFSTK_lineEditClass(wc,fontSize,sx,sy-1,bwidth,24,NorthWestGravity);
	//fontSizeEdit->LFSTK_setCallBack(xxx,xxx,(void*)123);
	sy+=vspacing;
	sx=col1;

	text=new LFSTK_labelClass(wc,"ABCDEFGHI abcdefghi 1234567890",10,sy,col3-bwidth-30,24,NorthWestGravity);
	text->LFSTK_setFontString(titleFont);
	text->LFSTK_setLabelOriention(1);
	text->LFSTK_setLabelAutoColour(false);

/////	text->LFSTK_setFontColourName(NORMALCOLOUR,"black");
	sy+=vspacing;
	sx=col1;

	placeMenu=new menuItemStruct[NOMOREPLACES];
	for(int j=0;j<NOMOREPLACES;j++)
		{
			placeMenu[j].label=placeNames[j];
			placeMenu[j].userData=(void*)(long)j;
			placeMenu[j].bc=NULL;
			placeMenu[j].subMenus=NULL;
			placeMenu[j].subMenuCnt=0;
			placeMenu[j].useIcon=false;
			placeMenu[j].useImage=false;
		}

	mb=new LFSTK_menuButtonClass(wc,labelnames[PLACELABEL],sx,sy,bwidth,24,NorthWestGravity);
	mb->LFSTK_addMenus(placeMenu,NOMOREPLACES);
	le[EPLACEMENT]=new LFSTK_lineEditClass(wc,"",sx+spacing,sy-1,col2-col1,24,NorthWestGravity);
	placeToString(placement);
	mb->LFSTK_setCallBack(NULL,placeCB,NULL);

	sy+=vspacing;

	for(int j=DESKLABEL; j<LSPACER; j++)
		{
			lb[j]=new LFSTK_labelClass(wc,labelnames[j],sx,sy,bwidth,24,NorthWestGravity);
			lb[j]->LFSTK_setLabelAutoColour(true);
			lb[j]->LFSTK_setLabelOriention(0);

			sx+=spacing;
			le[ETHEMEPATH+j]=new LFSTK_lineEditClass(wc,labelnames[j],sx,sy-1,bwidth,24,NorthWestGravity);
			sy+=vspacing;
			sx=col1;
		}

	le[ENUMDESKS]->LFSTK_setBuffer(numberOfDesktops);
	le[ELIVEUPDATE]->LFSTK_setBuffer(liveUpdate);
	le[ETHEMEPATH]->LFSTK_setBuffer(pathToTheme);
	le[ETERMCOMMAND]->LFSTK_setBuffer(terminalCommand);
	sy-=vspacing;

	sy+=vspacing;
	lb[LSPACER]=new LFSTK_labelClass(wc,labelnames[LSPACER],0,sy,col3,8,NorthWestGravity);
	sy+=16;
	sy+=vspacing;

	wc->LFSTK_resizeWindow(col3-10-bwidth,sy);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

	printf("Current Settings:\n\n");
	callback(NULL,(void*)PRINT);
	printf("\n\n");

	mainloop=true;
	while(mainloop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);
			switch(event.type)
				{
				case KeyRelease:
					if(ml->gadget==fontSizeEdit)
						buildFontString();
					break;
				case Expose:
					wc->LFSTK_setActive(true);
					wc->LFSTK_clearWindow();
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

	for(int j=0;j<subscnt;j++)
		{
			for(int k=0;k<catcnt[j];k++)
				{
					if(fontsAZ[j][k]!=NULL)
						free(fontsAZ[j][k]);
				}
			if(&fontSubMenus[j]!=NULL)
				delete fontSubMenus[j];
		}

	for(int j=0;j<NOMORE;j++)
		delete guibc[j];
	for(int j=0;j<NOMOREBUTTONS;j++)
		delete bc[j];
	for(int j=0;j<NOMOREEDITS;j++)
		delete le[j];
	for(int j=0;j<NOMORELABELS;j++)
		delete lb[j];
	delete mb;
	delete fontButton;

	delete fontEdit;
	delete fontSizeEdit;
	delete label;
	delete labelFontSize;
	delete bold;
	delete italic;

//TODO//
//delete wc;

	return 0;
}