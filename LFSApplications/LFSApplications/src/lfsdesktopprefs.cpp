/*
 *
 * ©K. D. Hedger. Sun 13 Sep 14:10:19 BST 2015 kdhedger68713@gmail.com

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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include <lfstk/LFSTKGlobals.h>

//enum {ICONTHEME=0,ICONSIZE,GRIDSIZE,GRIDBORDER,REFRESHRATE,FORECOLOUR,BACKCOLOUR,ALPHA,TERMCOMMAND,FONTFACE,IGNORES,NUMPREFS};
enum {ICONTHEME=0,ICONSIZE,GRIDSIZE,GRIDBORDER,REFRESHRATE,FORECOLOUR,BACKCOLOUR,ALPHA,TERMCOMMAND,FONTFACE,IGNORES,NUMPREFS};

enum {EXIT=0,APPLY,PRINT,NOMOREBUTTONS};
LFSTK_windowClass		*wc;
LFSTK_lineEditClass		*le[NUMPREFS]={NULL,};
LFSTK_labelClass		*lb[NUMPREFS]={NULL,};
LFSTK_labelClass		*spacer=NULL;
LFSTK_buttonClass		*guibc[NOMOREBUTTONS]={NULL,};
LFSTK_toggleButtonClass	*showExt=NULL;
LFSTK_fileDialogClass	*fc;

int						bwidth=96;
int						spacing=bwidth+10;
int						col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;
int						showSuffix;
int						parentWindow=-1;

#define BIG col2-col1

char				*prefs[NUMPREFS]={NULL,};
//const char			*labelNames[]={"Icon Theme","Icon Size","Grid Size","Border","Refresh","Text Colour","Label Colour","Label Alpha","Term Command","","Ignore"};
const char			*labelNames[]={"Icon Theme","Icon Size","Grid Size","Border","Refresh","Text Colour","Label Colour","Label Alpha","Term Command","Font","Ignore"};


int					editSize[]={BIG,bwidth,bwidth,bwidth,bwidth,bwidth,bwidth,bwidth,BIG,BIG,BIG};

args				desktopPrefs[]=
{
	{"icontheme",TYPESTRING,&prefs[ICONTHEME]},
	{"iconsize",TYPESTRING,&prefs[ICONSIZE]},
	{"gridsize",TYPESTRING,&prefs[GRIDSIZE]},
	{"gridborder",TYPESTRING,&prefs[GRIDBORDER]},
	{"refreshrate",TYPESTRING,&prefs[REFRESHRATE]},
	{"termcommand",TYPESTRING,&prefs[TERMCOMMAND]},
	{"showextension",TYPEBOOL,&showSuffix},
	{"fontface",TYPESTRING,&prefs[FONTFACE]},
	{"labelforeground",TYPESTRING,&prefs[FORECOLOUR]},
	{"labelbackground",TYPESTRING,&prefs[BACKCOLOUR]},
	{"labelalpha",TYPESTRING,&prefs[ALPHA]},
	{"noshow",TYPESTRING,&prefs[IGNORES]},
	{NULL,0,NULL}
};

bool				mainloop=false;
char				*env;

void setVars(void)
{
	lb[BACKCOLOUR]->LFSTK_setColourName(INACTIVECOLOUR,le[BACKCOLOUR]->LFSTK_getBuffer()->c_str());
	lb[FORECOLOUR]->LFSTK_setColourName(INACTIVECOLOUR,le[FORECOLOUR]->LFSTK_getBuffer()->c_str());
	for(int j=ICONTHEME; j<NUMPREFS; j++)
		{
			if(lb[j]!=NULL)
				{
					lb[j]->LFSTK_clearWindow();
					if(prefs[j]!=NULL)
						free(prefs[j]);
					prefs[j]=strdup(le[j]->LFSTK_getBuffer()->c_str());
				}
		}
	showSuffix=showExt->LFSTK_getValue();
}

bool callback(void *p,void* ud)
{
	if((long)ud==EXIT)
		{
			wc->LFSTK_clearWindow();
			setVars();
			wc->globalLib->LFSTK_saveVarsToFile("-",desktopPrefs);
			printf("\n");
			mainloop=false;
			return(false);
		}

	switch((long)ud)
		{
			case PRINT:
				wc->LFSTK_clearWindow();
				setVars();
				wc->globalLib->LFSTK_saveVarsToFile("-",desktopPrefs);
				printf("\n");
				break;

			case APPLY:
				wc->LFSTK_clearWindow();
				setVars();
				wc->globalLib->LFSTK_saveVarsToFile(env,desktopPrefs);
				system("lfsdesktop &");
				break;
		}
	return(true);
}

bool setShowExt(void *p,void* ud)
{
	if(showExt->LFSTK_getValue()==true)
		showExt->LFSTK_setLabel("Show Suffix");
	else
		showExt->LFSTK_setLabel("Hide Suffix");

	showExt->LFSTK_clearWindow();
	return(true);
}

bool fontCB(void *object,void* userdata)
{
	LFSTK_fontButtonClass	*fb;

	fb=static_cast<LFSTK_fontButtonClass*>(object);
	fb->LFSTK_showDialog(le[FONTFACE]->LFSTK_getBuffer()->c_str());
	le[FONTFACE]->LFSTK_setBuffer(fb->LFSTK_getFontString());
}

int main(int argc, char **argv)
{
	XEvent			event;
	int				sx=0;
	int				sy=0;
	geometryStruct	*geom;
	int				bhite=24;
	int				vspacing=bhite+10;
	LFSTK_fontButtonClass	*fontbutton;

	int				c=0;
	int				option_index=0;
	const char		*shortOpts="h?w:";
	option 			longOptions[]=
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

	prefs[ICONTHEME]=strdup("gnome");
	prefs[ICONSIZE]=strdup("32");
	prefs[GRIDSIZE]=strdup("64");
	prefs[GRIDBORDER]=strdup("32");
	prefs[REFRESHRATE]=strdup("2");
	prefs[TERMCOMMAND]=strdup("xterm -e ");
	showSuffix=false;
	prefs[FONTFACE]=strdup("Sans;0;0;10");
	prefs[FORECOLOUR]=strdup("#ffffff");
	prefs[BACKCOLOUR]=strdup("#000000");
	prefs[ALPHA]=strdup("#ff");
	prefs[IGNORES]=strdup("");

	wc=new LFSTK_windowClass(sx,sy,800,600,"LFS Desktop Prefs",false);
	wc->LFSTK_setDecorated(true);
	geom=wc->LFSTK_getGeom();

	fc=new LFSTK_fileDialogClass(wc,"","/");

	asprintf(&env,"%s/.config/LFS/lfsdesktop.rc",getenv("HOME"));
	wc->globalLib->LFSTK_loadVarsFromFile(env,desktopPrefs);

	guibc[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	guibc[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	guibc[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	guibc[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

	guibc[PRINT]=new LFSTK_buttonClass(wc,"Print",(geom->w/2)-(64/2),geom->h-32,64,24,SouthGravity);
	guibc[PRINT]->LFSTK_setCallBack(NULL,callback,(void*)PRINT);
	sx=col1;
	sy=10;

	for(int j=ICONTHEME;j<TERMCOMMAND;j++)
		{
			lb[j]=new LFSTK_labelClass(wc,labelNames[j],sx,sy,bwidth,24,NorthWestGravity);
			lb[j]->LFSTK_setLabelAutoColour(true);
			lb[j]->LFSTK_setActive(false);
			lb[j]->LFSTK_setColourName(INACTIVECOLOUR,wc->windowColourNames[NORMALCOLOUR].name);
			sx+=spacing;
			le[j]=new LFSTK_lineEditClass(wc,prefs[j],sx,sy-1,editSize[j],24,NorthWestGravity);
			sy+=vspacing;
			sx=col1;
		}

	showExt=new LFSTK_toggleButtonClass(wc,"Show Suffix",col1+bwidth+10,sy,bwidth,24,NorthWestGravity);
	showExt->LFSTK_setValue(showSuffix);
	showExt->LFSTK_setCallBack(NULL,setShowExt,NULL);
	setShowExt(NULL,NULL);

	sy+=vspacing;
	for(int j=TERMCOMMAND;j<NUMPREFS;j++)
		{
			if(j==FONTFACE)
				{
					lb[FONTFACE]=new LFSTK_labelClass(wc,labelNames[j],sx,sy,1,1,NorthWestGravity);
					fontbutton=new LFSTK_fontButtonClass(wc,"Select Font",sx,sy,bwidth,24,NorthWestGravity);
					fontbutton->LFSTK_setCallBack(NULL,fontCB,NULL);
					fontbutton->LFSTK_setLabelIsFont(false);
					sx+=spacing;
					le[FONTFACE]=new LFSTK_lineEditClass(wc,prefs[FONTFACE],sx,sy-1,editSize[FONTFACE],24,NorthWestGravity);
				}
			else
				{
					lb[j]=new LFSTK_labelClass(wc,labelNames[j],sx,sy,bwidth,24,NorthWestGravity);
					lb[j]->LFSTK_setLabelAutoColour(true);
					lb[j]->LFSTK_setActive(false);
					lb[j]->LFSTK_setColourName(INACTIVECOLOUR,wc->windowColourNames[NORMALCOLOUR].name);
					sx+=spacing;
					le[j]=new LFSTK_lineEditClass(wc,prefs[j],sx,sy-1,editSize[j],24,NorthWestGravity);
				}
			sy+=vspacing;
			sx=col1;
		}

	spacer=new LFSTK_labelClass(wc,"--",0,sy,col3,8,NorthWestGravity);
	sy+=16;
	sy+=vspacing;
	wc->LFSTK_resizeWindow(col2+BIG-bwidth-10,sy);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

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
						break;
					case Expose:
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

	for(int j=ICONTHEME;j<NUMPREFS;j++)
		if(prefs[j]!=NULL)
			free(prefs[j]);

	delete wc;
	free(env);
	return(0);
};