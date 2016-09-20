/*
 *
 * ©K. D. Hedger. Wed 16 Sep 11:51:14 BST 2015 kdhedger68713@gmail.com

 * This file (lfstkprefs.cpp) is part of LFSApplications.

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
#include <libgen.h>

#include <lfstk/LFSTKGlobals.h>

enum {EXIT=0,APPLY,PRINT,NOMOREGBS};
enum {NORMAL=0,PRELIGHT,ACTIVE,INACTIVE,MENUNORMAL,MENUPRELIGHT,MENUACTIVE,MENUINACTIVE,NOMOREBUTTONS};
enum {ENORMAL=0,EPRELIGHT,EACTIVE,EINACTIVE,EFONTNORMAL,EFONTPRELIGHT,EFONTACTIVE,EFONTINACTIVE,EMENUNORMAL,EMENUPRELIGHT,EMENUACTIVE,EMENUINACTIVE,EMENUFONTNORMAL,EMENUFONTPRELIGHT,EMENUFONTACTIVE,EMENUFONTINACTIVE,EWINDOWCOLOUR,EFONTSTRING,EMENUFONTSTRING,EWINDOWTILE,EBUTTONTILE,EMENUITEMTILE,NOMOREBUTTONEDITS};
enum {LNORMBUTTONS=0,LBACKCOLOUR,LFONTCOLOUR,LMENUBUTTONS,LMENUBACKCOLOUR,LMENUFONTCOLOUR,LWINDOWCOLOUR,LSPACER,NOMORELABELS};
//LWINDOWTILE,LBUTTONTILE,LMENUITEMTILE,LSPACER,NOMORELABELS};

enum {FWTILENAME=7,FBTILENAME,FMITEMNAME};

LFSTK_windowClass		*wc;
LFSTK_buttonClass		*guibuttons[NOMOREGBS]={NULL,};
LFSTK_buttonClass		*normalbuttons[NOMOREBUTTONS]={NULL,};
LFSTK_lineEditClass 	*normaledits[NOMOREBUTTONEDITS]={NULL,};
LFSTK_labelClass		*labels[NOMORELABELS]={NULL,};
LFSTK_toggleButtonClass	*autocolour=NULL;
LFSTK_toggleButtonClass	*usetheme=NULL;
LFSTK_buttonClass		*getFileButton=NULL;

bool					mainloop=false;
int						parentWindow=-1;
const char				*buttonnames[]={"Button Normal","Button Prelight","Button Active","Button Inactive","Menu Normal","Menu Prelight","Menu Active","Menu Inactive"};
//const char				*labelnames[]={"Normal Buttons","Back Colour","Font Colour","Menu Items","Back Colour","Font Colour","Window Colour","Font","Menu Item Font","Window Tile","Button Tile","Menu Item Tile","--"};
const char				*labelnames[]={"Normal Buttons","Back Colour","Font Colour","Menu Items","Back Colour","Font Colour","Window Colour","Window Tile","Button Tile","Menu Item Tile","--"};
LFSTK_fileDialogClass	*fc;

void setVars(void)
{
//window clolour
	wc->LFSTK_setWindowColourName(NORMALCOLOUR,normaledits[EWINDOWCOLOUR]->LFSTK_getBuffer()->c_str());
	wc->globalLib->LFSTK_setGlobalString(NORMALCOLOUR,TYPEWINDOW,wc->windowColourNames[NORMALCOLOUR].name);
	wc->LFSTK_clearWindow();

//auto label colour
	wc->globalLib->LFSTK_setAutoLabelColour(autocolour->LFSTK_getValue());
	wc->autoLabelColour=wc->globalLib->LFSTK_getAutoLabelColour();
//fonts
	wc->globalLib->LFSTK_setGlobalString(-1,TYPEFONT,normaledits[EFONTSTRING]->LFSTK_getBuffer()->c_str());
	wc->globalLib->LFSTK_setGlobalString(-1,TYPEMENUITEMFONT,normaledits[EMENUFONTSTRING]->LFSTK_getBuffer()->c_str());

//button colours
	for(int j=NORMALCOLOUR;j<MAXCOLOURS;j++)
		{
			wc->globalLib->LFSTK_setGlobalString(j,TYPEBUTTON,normaledits[ENORMAL+j]->LFSTK_getBuffer()->c_str());
			wc->globalLib->LFSTK_setGlobalString(j,TYPEMENUITEM,normaledits[EMENUNORMAL+j]->LFSTK_getBuffer()->c_str());
			wc->globalLib->LFSTK_setGlobalString(j,TYPEFONTCOLOUR,normaledits[EFONTNORMAL+j]->LFSTK_getBuffer()->c_str());
			wc->globalLib->LFSTK_setGlobalString(j,TYPEMENUITEMFONTCOLOUR,normaledits[EMENUFONTNORMAL+j]->LFSTK_getBuffer()->c_str());
//reset button colours
			normalbuttons[NORMAL+j]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(j,TYPEBUTTON));
			normalbuttons[NORMAL+j]->LFSTK_setFontColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(j,TYPEFONTCOLOUR));
			normalbuttons[NORMAL+j]->LFSTK_setLabelAutoColour(wc->autoLabelColour);
			normalbuttons[NORMAL+j]->LFSTK_setFontString(wc->globalLib->LFSTK_getGlobalString(-1,TYPEFONT));
			normalbuttons[NORMAL+j]->LFSTK_clearWindow();
//reset menu item colours
			normalbuttons[MENUNORMAL+j]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEM));
			normalbuttons[MENUNORMAL+j]->LFSTK_setFontColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEMFONTCOLOUR));
			normalbuttons[MENUNORMAL+j]->LFSTK_setLabelAutoColour(wc->autoLabelColour);
			normalbuttons[MENUNORMAL+j]->LFSTK_setFontString(wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMFONT));
			normalbuttons[MENUNORMAL+j]->LFSTK_clearWindow();
		}

//auto label clour checkbox
	autocolour->LFSTK_setLabelAutoColour(wc->autoLabelColour);
	autocolour->LFSTK_setFontColourName(NORMALCOLOUR,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEFONTCOLOUR));
	autocolour->LFSTK_clearWindow();

//reset label background colours
	for(int j=LNORMBUTTONS;j<NOMORELABELS;j++)
		{
			labels[j]->LFSTK_setLabelAutoColour(wc->autoLabelColour);
			labels[j]->LFSTK_setColourName(NORMALCOLOUR,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEWINDOW));
			labels[j]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(INACTIVECOLOUR,TYPEWINDOW));
			labels[j]->LFSTK_clearWindow();
		}

//themes
	wc->globalLib->LFSTK_setGlobalString(-1,TYPEWINDOWTILE,normaledits[EWINDOWTILE]->LFSTK_getBuffer()->c_str());
	wc->globalLib->LFSTK_setGlobalString(-1,TYPEBUTTONTILE,normaledits[EBUTTONTILE]->LFSTK_getBuffer()->c_str());
	wc->globalLib->LFSTK_setGlobalString(-1,TYPEMENUITEMTILE,normaledits[EMENUITEMTILE]->LFSTK_getBuffer()->c_str());
	wc->globalLib->LFSTK_setUseTheme(usetheme->LFSTK_getValue());

//use theme checkbox
	usetheme->LFSTK_setLabelAutoColour(wc->autoLabelColour);
	usetheme->LFSTK_setFontColourName(NORMALCOLOUR,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEFONTCOLOUR));
	usetheme->LFSTK_clearWindow();
	
}

bool selectfile(void *object,void* ud)
{
	char					*dir;
	const char				*dirpath;
	char					*filepath;

	dir=strdup(normaledits[GETUSERDATA(ud)]->LFSTK_getBuffer()->c_str());
	dirpath=dirname(dir);
	fc->LFSTK_showFileDialog(dirpath,labelnames[GETUSERDATA(ud)-12]);
	if(fc->LFSTK_isValid()==true)
		{
			asprintf(&filepath,"%s/%s",fc->LFSTK_getCurrentDir(),fc->LFSTK_getCurrentFile());
			normaledits[GETUSERDATA(ud)]->LFSTK_setBuffer(filepath);
			free(filepath);
		}
	free(dir);
	return(true);
}

bool callback(void *p,void* ud)
{
	char *env;

	switch((long)ud)
		{
			case EXIT:
				setVars();
				wc->globalLib->LFSTK_saveVarsToFile("-",wc->globalLib->LFSTK_getTKArgs());
				printf("\n");
				mainloop=false;
				return(false);
				break;
				case PRINT:
				setVars();
				wc->globalLib->LFSTK_saveVarsToFile("-",wc->globalLib->LFSTK_getTKArgs());
				printf("\n");
				break;
			case APPLY:
				setVars();
				asprintf(&env,"%s/.config/LFS/lfstoolkit.rc",getenv("HOME"));
				wc->globalLib->LFSTK_saveVarsToFile(env,wc->globalLib->LFSTK_getTKArgs());
				free(env);
				break;
		}
	return(true);
}

bool fontCB(void *object,void* userdata)
{
	LFSTK_fontButtonClass	*fb;

	fb=static_cast<LFSTK_fontButtonClass*>(object);
	if(GETUSERDATA(userdata)==100)
		{
			fb->LFSTK_showDialog(normaledits[EFONTSTRING]->LFSTK_getBuffer()->c_str());
			normaledits[EFONTSTRING]->LFSTK_setBuffer(fb->LFSTK_getFontString());
		}
	else
		{
			fb->LFSTK_showDialog(normaledits[EMENUFONTSTRING]->LFSTK_getBuffer()->c_str());
			normaledits[EMENUFONTSTRING]->LFSTK_setBuffer(fb->LFSTK_getFontString());
		}
}
int main(int argc, char **argv)
{
	XEvent			event;
	int				sx=0;
	int				sy=0;
	geometryStruct	*geom;
	int				bwidth=96;
	int				bhite=24;
	int				spacing=bwidth+10;
	int				vspacing=bhite+10;
	int				xspace=10;
	int				cols[5];
	int				state;
	int				holdsy;
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

	cols[0]=10;
	for(int j=1;j<5;j++)
		cols[j]=cols[j-1]+bwidth+xspace;

	wc=new LFSTK_windowClass(sx,sy,800,600,"LFS Tool Kit Prefs",false);
	wc->LFSTK_setDecorated(true);
	geom=wc->LFSTK_getGeom();

	fc=new LFSTK_fileDialogClass(wc,"Select Window Tile","/");

	guibuttons[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	guibuttons[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	guibuttons[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	guibuttons[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

	guibuttons[PRINT]=new LFSTK_buttonClass(wc,"Test",(geom->w/2)-(64/2),geom->h-32,64,24,SouthGravity);
	guibuttons[PRINT]->LFSTK_setCallBack(NULL,callback,(void*)PRINT);

//buttons label
	sy=10;
	for(int j=LNORMBUTTONS;j<=LFONTCOLOUR;j++)
		{
			labels[j]=new LFSTK_labelClass(wc,labelnames[j],cols[j],sy,bwidth,24,NorthWestGravity);
			labels[j]->LFSTK_setLabelOriention(LEFT);
		}

//buttons
	sy+=vspacing;
	holdsy=sy;
	state=NORMALCOLOUR;
	for(int j=NORMAL;j<=INACTIVE;j++)
		{
			normalbuttons[j]=new LFSTK_buttonClass(wc,buttonnames[j],cols[0],sy,bwidth,24,NorthWestGravity);
			normalbuttons[j]->LFSTK_setActive(false);
			normalbuttons[j]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(state,TYPEBUTTON));
			normalbuttons[j]->LFSTK_setFontColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(state,TYPEFONTCOLOUR));
			state++;
			sy+=vspacing;
		}
//button back col
	state=NORMALCOLOUR;
	sy=holdsy;
	for(int j=ENORMAL;j<=EINACTIVE;j++)
		{
			normaledits[j]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(state,TYPEBUTTON),cols[1],sy,bwidth,24,NorthWestGravity);
			state++;
			sy+=vspacing;
		}
//button font col
	state=NORMALCOLOUR;
	sy=holdsy;
	for(int j=EFONTNORMAL;j<=EFONTINACTIVE;j++)
		{
			normaledits[j]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(state,TYPEFONTCOLOUR),cols[2],sy,bwidth,24,NorthWestGravity);
			state++;
			sy+=vspacing;
		}

	fontbutton=new LFSTK_fontButtonClass(wc,"Select Font",cols[0],sy,bwidth,24,NorthWestGravity);
	fontbutton->LFSTK_setCallBack(NULL,fontCB,USERDATA(100));
	fontbutton->LFSTK_setLabelIsFont(false);
	normaledits[EFONTSTRING]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEFONT),cols[1],sy,cols[3]-cols[1]-10,24,NorthWestGravity);
	sy+=vspacing;

//menu items labels
	state=0;
	for(int j=LMENUBUTTONS;j<=LMENUFONTCOLOUR;j++)
		{
			labels[j]=new LFSTK_labelClass(wc,labelnames[j],cols[state],sy,bwidth,24,NorthWestGravity);
			state++;
			labels[j]->LFSTK_setLabelOriention(LEFT);
		}
//menu items
	sy+=vspacing;
	holdsy=sy;
	state=NORMALCOLOUR;
	for(int j=MENUNORMAL;j<=MENUINACTIVE;j++)
		{
			normalbuttons[j]=new LFSTK_buttonClass(wc,buttonnames[j],cols[0],sy,bwidth,24,NorthWestGravity);
			normalbuttons[j]->LFSTK_setActive(false);
			normalbuttons[j]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(state,TYPEBUTTON));
			normalbuttons[j]->LFSTK_setFontColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(state,TYPEMENUITEMFONTCOLOUR));
			state++;
			sy+=vspacing;
		}
//menu back col
	state=NORMALCOLOUR;
	sy=holdsy;
	for(int j=EMENUNORMAL;j<=EMENUINACTIVE;j++)
		{
			normaledits[j]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(state,TYPEMENUITEM),cols[1],sy,bwidth,24,NorthWestGravity);
			state++;
			sy+=vspacing;
		}
//menu font col
	state=NORMALCOLOUR;
	sy=holdsy;
	for(int j=EMENUFONTNORMAL;j<=EMENUFONTINACTIVE;j++)
		{
			normaledits[j]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(state,TYPEMENUITEMFONTCOLOUR),cols[2],sy,bwidth,24,NorthWestGravity);
			state++;
			sy+=vspacing;
		}

	fontbutton=new LFSTK_fontButtonClass(wc,"Select Font",cols[0],sy,bwidth,24,NorthWestGravity);
	fontbutton->LFSTK_setCallBack(NULL,fontCB,USERDATA(200));
	fontbutton->LFSTK_setLabelIsFont(false);
	normaledits[EMENUFONTSTRING]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEMENUITEMFONT),cols[1],sy,cols[3]-cols[1]-10,24,NorthWestGravity);
	sy+=vspacing;

//window label and colour
	labels[LWINDOWCOLOUR]=new LFSTK_labelClass(wc,labelnames[LWINDOWCOLOUR],cols[0],sy,bwidth,24,NorthWestGravity);
	labels[LWINDOWCOLOUR]->LFSTK_setLabelOriention(LEFT);	
	normaledits[EWINDOWCOLOUR]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEWINDOW),cols[1],sy,bwidth,24,NorthWestGravity);

//auto font colour
	autocolour=new LFSTK_toggleButtonClass(wc,"Auto Colour",cols[2],sy,bwidth,24,NorthWestGravity);
	autocolour->LFSTK_setCallBack(NULL,callback,(void*)PRINT);
	autocolour->LFSTK_setValue(wc->autoLabelColour);
	sy+=vspacing;

//theme parts
//use theme
	usetheme=new LFSTK_toggleButtonClass(wc,"Use Theme",cols[0],sy,bwidth,24,NorthWestGravity);
	usetheme->LFSTK_setCallBack(NULL,callback,(void*)PRINT);
	usetheme->LFSTK_setValue(wc->globalLib->LFSTK_getUseTheme());
	sy+=vspacing;

	getFileButton=new LFSTK_buttonClass(wc,labelnames[FWTILENAME],cols[0],sy,bwidth,24,NorthWestGravity);
	getFileButton->LFSTK_setCallBack(NULL,selectfile,USERDATA(EWINDOWTILE));
	normaledits[EWINDOWTILE]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(-1,TYPEWINDOWTILE),cols[1],sy,cols[3]-cols[1]-10,24,NorthWestGravity);

	sy+=vspacing;
	getFileButton=new LFSTK_buttonClass(wc,labelnames[FBTILENAME],cols[0],sy,bwidth,24,NorthWestGravity);
	getFileButton->LFSTK_setCallBack(NULL,selectfile,USERDATA(EBUTTONTILE));
	normaledits[EBUTTONTILE]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),cols[1],sy,cols[3]-cols[1]-10,24,NorthWestGravity);
	sy+=vspacing;
	getFileButton=new LFSTK_buttonClass(wc,labelnames[FMITEMNAME],cols[0],sy,bwidth,24,NorthWestGravity);
	getFileButton->LFSTK_setCallBack(NULL,selectfile,USERDATA(EMENUITEMTILE));
	normaledits[EMENUITEMTILE]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMTILE),cols[1],sy,cols[3]-cols[1]-10,24,NorthWestGravity);

	sy+=vspacing;
	labels[LSPACER]=new LFSTK_labelClass(wc,labelnames[LSPACER],0,sy,cols[3],8,NorthWestGravity);
	sy+=16;
	
	printf("Current Settings:\n\n");
	callback(NULL,(void*)PRINT);
	printf("\n\n");

	sy+=(vspacing);
	wc->LFSTK_resizeWindow(cols[3],sy);
	wc->LFSTK_showWindow(true);
	wc->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

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

	delete wc;
	return(0);
}
