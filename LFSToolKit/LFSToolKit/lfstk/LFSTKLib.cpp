/*
 *
 * ©K. D. Hedger. Fri 21 Aug 12:28:57 BST 2015 keithdhedger@gmail.com

 * This file (LFSTK_lib.cpp) is part of LFSToolKit.

 * LFSToolKit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSToolKit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSToolKit.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Portions of this file is from here:
 * https://github.com/rahra/cairo_jpg
 * Many thanks!
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ftw.h>
#include <fnmatch.h>
#include <fcntl.h>
#include <jpeglib.h>

#include "lfstk/LFSTKGlobals.h"

enum {DEFNORMAL,DEFPRELIE,DEFACTIVE,DEFINACTIVE,DEFFONTNORMAL,DEFFONTPRELIE,DEFFONTACTIVE,DEFFONTINACTIVE};

const char	*defaultColourStrings[]={"grey50","grey80","grey40","grey90"};
const char	*defaultFontColourStrings[]={"white","black","white","grey80"};
const char	*defaultFontString="sans-serif:size=10";
const char	*defaultMonoFontString="mono:size=12";
const char	*defaultCursorColour="black";
const char	*defaultThemePath="/usr/share/themes/Steel.lfstk";//TODO//
const char	*defaultFrameStrings[]={"black","#00ffff","black","white","white"};
const char	*defaultWindowTile="";
const char	*defaultButtonTile="";
const char	*defaultMenuItemTile="";

char		filterBuffer[256];
char		retBuffer[512];

LFSTK_lib::~LFSTK_lib()
{
}

/**
* Sets a global string or the default.
*
* \param state Gadget state.
* \param type Type of string to set.
* \param str New string.
*/
void LFSTK_lib::LFSTK_setGlobalString(int state,int type,const char *str)//TODO//set prrefs?
{
	char		*ptr=NULL;
	const char	*strstate[]={"normal","prelight","active","inactive"};
	char		prefsname[256];

	switch(type)
		{
			case TYPEWINDOW:
				snprintf(prefsname,255,"window_%s",strstate[state]);
				this->prefs.LFSTK_setString(prefsname,str);
				break;
			case TYPEWINDOWTILE:
				this->prefs.LFSTK_setString("windowtile",str);
				break;
			case TYPEBUTTON:
				snprintf(prefsname,255,"button_%s",strstate[state]);
				this->prefs.LFSTK_setString(prefsname,str);
				break;
			case TYPEBUTTONTILE:
				this->prefs.LFSTK_setString("buttontile",str);
				break;
			case TYPEMENUITEM:
				snprintf(prefsname,255,"menuitem_%s",strstate[state]);
				this->prefs.LFSTK_setString(prefsname,str);
				break;
			case TYPEMENUITEMTILE:
				this->prefs.LFSTK_setString("menuitemtile",str);
				break;
			case TYPEFONTCOLOUR:
				snprintf(prefsname,255,"font_%s",strstate[state]);
				this->prefs.LFSTK_setString(prefsname,str);
				break;
			case TYPEMENUITEMFONTCOLOUR:
				snprintf(prefsname,255,"menuitem_font_%s",strstate[state]);
				this->prefs.LFSTK_setString(prefsname,str);
				break;
			case TYPEMENUITEMFONT:
				this->prefs.LFSTK_setString("menuitem_font",str);
				break;
			case TYPEFONT:
				this->prefs.LFSTK_setString("font",str);
				break;
			case TYPEMONOFONT:
				this->prefs.LFSTK_setString("monofont",str);
				break;
			case TYPECURSORCOLOUR:
				this->prefs.LFSTK_setString("cursorcolour",str);
				break;
			case TYPESBTROUGHCOLOUR:
				this->prefs.LFSTK_setString("sbtroughcolour",str);
				break;
			case TYPELISTTROUGHCOLOUR:
				this->prefs.LFSTK_setString("listtroughcolour",str);
				break;
		}
}

/**
* Gets a global string or the default.
*
* \param state Gadget state.
* \param type Type of string to get.
* \note type=TYPEWINDOW=0,TYPEBUTTON,TYPEMENUITEM,TYPEFONTCOLOUR,TYPEMENUITEMFONTCOLOUR,TYPEMENUITEMFONT,TYPEFONT
* \return const char* String.
* \note Don't free return value.
* \note If global string not loaded return default.
*/
const char *LFSTK_lib::LFSTK_getGlobalString(int state,int type)
{
	const char	*ptr=NULL;
	bool		isfontcolour=false;
	const char	*strstate[]={"normal","prelight","active","inactive"};
	char		prefsname[256];

	switch(type)
		{
			case TYPEWINDOW:
				snprintf(prefsname,255,"window_%s",strstate[state]);
				ptr=this->prefs.LFSTK_getCString(prefsname);
				if(ptr==NULL)
					ptr=defaultColourStrings[state];
				break;
			case TYPEWINDOWTILE:
				ptr=this->prefs.LFSTK_getCString("windowtile");
				break;
			case TYPEBUTTON:
				snprintf(prefsname,255,"button_%s",strstate[state]);
				ptr=this->prefs.LFSTK_getCString(prefsname);
				if(ptr==NULL)
					ptr=defaultColourStrings[state];
				break;
			case TYPEBUTTONTILE:
				ptr=this->prefs.LFSTK_getCString("buttontile");
				if(ptr==NULL)
					ptr=defaultButtonTile;
				break;
			case TYPEMENUITEM:
				snprintf(prefsname,255,"menuitem_%s",strstate[state]);
				ptr=this->prefs.LFSTK_getCString(prefsname);
				if(ptr==NULL)
					ptr=defaultColourStrings[state];
				break;
			case TYPEMENUITEMTILE:
				ptr=this->prefs.LFSTK_getCString("menuitemtile");
				if(ptr==NULL)
					ptr=defaultMenuItemTile;
				break;
			case TYPEFONTCOLOUR:
				isfontcolour=true;
				snprintf(prefsname,255,"font_%s",strstate[state]);
				if(ptr==NULL)
					ptr=defaultFontColourStrings[state];
				break;
			case TYPEMENUITEMFONTCOLOUR:
				isfontcolour=true;
				snprintf(prefsname,255,"menuitem_font_%s",strstate[state]);
				ptr=this->prefs.LFSTK_getCString(prefsname);
				if(ptr==NULL)
					ptr=defaultFontColourStrings[state];
				break;
			case TYPEMENUITEMFONT:
				ptr=this->prefs.LFSTK_getCString("menuitem_font");
				if(ptr==NULL)
					ptr=defaultFontString;			
				break;
			case TYPEFONT:
				ptr=this->prefs.LFSTK_getCString("font");
				if(ptr==NULL)
					ptr=defaultFontString;
				break;
			case TYPEMONOFONT:
				ptr=this->prefs.LFSTK_getCString("monofont");
				if(ptr==NULL)
					ptr=defaultMonoFontString;
				break;
			case TYPECURSORCOLOUR:
				ptr=this->prefs.LFSTK_getCString("cursorcolour");
				if(ptr==NULL)
					ptr=defaultCursorColour;
				break;
			case TYPESBTROUGHCOLOUR:
				ptr=this->prefs.LFSTK_getCString("sbtroughcolour");
				if(ptr==NULL)
					ptr=defaultColourStrings[state];
				break;
			case TYPELISTTROUGHCOLOUR:
				ptr=this->prefs.LFSTK_getCString("listtroughcolour");
				if(ptr==NULL)
					ptr=defaultColourStrings[state];
				break;
		}

	if(ptr==NULL)
		{
			if(isfontcolour==false)
				ptr=defaultColourStrings[state];
			else
				ptr=defaultFontColourStrings[state];
		}

	return(ptr);
}

LFSTK_lib::LFSTK_lib(bool loadvars)
{
	this->LFSTK_reloadPrefs();
}

void LFSTK_lib::LFSTK_reloadPrefs(void)
{
	char *env=NULL;

	this->prefs.prefsMap=
		{
			{this->prefs.LFSTK_hashFromKey("window_normal"),{TYPESTRING,"window_prelight","",false,0}},
			{this->prefs.LFSTK_hashFromKey("window_prelight"),{TYPESTRING,"window_prelight","",false,0}},
			{this->prefs.LFSTK_hashFromKey("window_active"),{TYPESTRING,"window_active","",false,0}},
			{this->prefs.LFSTK_hashFromKey("window_inactive"),{TYPESTRING,"window_inactive","",false,0}},
			{this->prefs.LFSTK_hashFromKey("windowtile"),{TYPESTRING,"windowtile","",false,0}},

			{this->prefs.LFSTK_hashFromKey("button_normal"),{TYPESTRING,"button_normal","",false,0}},
			{this->prefs.LFSTK_hashFromKey("button_prelight"),{TYPESTRING,"button_prelight","",false,0}},
			{this->prefs.LFSTK_hashFromKey("button_active"),{TYPESTRING,"button_active","",false,0}},
			{this->prefs.LFSTK_hashFromKey("button_inactive"),{TYPESTRING,"button_inactive","",false,0}},
			{this->prefs.LFSTK_hashFromKey("buttontile"),{TYPESTRING,"buttontile","",false,0}},

			{this->prefs.LFSTK_hashFromKey("menuitem_normal"),{TYPESTRING,"menuitem_normal","",false,0}},
			{this->prefs.LFSTK_hashFromKey("menuitem_prelight"),{TYPESTRING,"menuitem_prelight","",false,0}},
			{this->prefs.LFSTK_hashFromKey("menuitem_active"),{TYPESTRING,"menuitem_active","",false,0}},
			{this->prefs.LFSTK_hashFromKey("menuitem_inactive"),{TYPESTRING,"menuitem_inactive","",false,0}},

			{this->prefs.LFSTK_hashFromKey("menuitem_font"),{TYPESTRING,"menuitem_font","",false,0}},
			{this->prefs.LFSTK_hashFromKey("menuitem_font_normal"),{TYPESTRING,"menuitem_font_normal","",false,0}},
			{this->prefs.LFSTK_hashFromKey("menuitem_font_prelight"),{TYPESTRING,"menuitem_font_prelight","",false,0}},
			{this->prefs.LFSTK_hashFromKey("menuitem_font_active"),{TYPESTRING,"menuitem_font_active","",false,0}},
			{this->prefs.LFSTK_hashFromKey("menuitem_font_inactive"),{TYPESTRING,"menuitem_font_inactive","",false,0}},
			{this->prefs.LFSTK_hashFromKey("menuitemtile"),{TYPESTRING,"menuitemtile","",false,0}},

			{this->prefs.LFSTK_hashFromKey("font"),{TYPESTRING,"font","",false,0}},
			{this->prefs.LFSTK_hashFromKey("font_normal"),{TYPESTRING,"font_normal","",false,0}},
			{this->prefs.LFSTK_hashFromKey("font_prelight"),{TYPESTRING,"font_prelight","",false,0}},
			{this->prefs.LFSTK_hashFromKey("font_active"),{TYPESTRING,"font_active","",false,0}},
			{this->prefs.LFSTK_hashFromKey("font_inactive"),{TYPESTRING,"font_inactive","",false,0}},

			{this->prefs.LFSTK_hashFromKey("autotextcolour"),{TYPEBOOL,"autotextcolour","",false,0}},
			{this->prefs.LFSTK_hashFromKey("usetheme"),{TYPEBOOL,"usetheme","",false,0}},
			{this->prefs.LFSTK_hashFromKey("themepath"),{TYPESTRING,"themepath","",false,0}},
			{this->prefs.LFSTK_hashFromKey("monofont"),{TYPESTRING,"monofont","",false,0}},
			{this->prefs.LFSTK_hashFromKey("cursorcolour"),{TYPESTRING,"cursorcolour","",false,0}},
			{this->prefs.LFSTK_hashFromKey("sbtroughcolour"),{TYPESTRING,"sbtroughcolour","",false,0}},
			{this->prefs.LFSTK_hashFromKey("listtroughcolour"),{TYPESTRING,"listtroughcolour","",false,0}}
		};

	asprintf(&env,"%s/.config/LFS/lfstoolkit.rc",getenv("HOME"));
	this->prefs.LFSTK_loadVarsFromFile(env);
	free(env);
}

/**
* Get best font coloue choice.
* \param pixel Background pixel colour.
* \return const char* Colour name.
* \note Returns "black" or "white" depending on back colour.
* \note Don't free returned array.
*/
const char* LFSTK_lib::bestFontColour(long pixel)
{
	int		r=0,g=0,b=0;
	long	pix=pixel;

	r=(pix & 0xff0000)>>16;
	g=(pix & 0x00ff00)>>8;
	b=(pix & 0x0000ff);

	if((r+r+r+b+g+g+g+g)>>3>128)
		return("black");
	else
		return("white");
}

/**
* Handle gadget events.
* \param self* Gadget.
* \param XEvent* Pointer to Xevent from main loop.
* \param type Gadget type.
* \return bool Event handled.
*/
bool LFSTK_lib::LFSTK_gadgetEvent(void *self,XEvent *e,int type)
{
	bool				retval=true;
	LFSTK_gadgetClass	*gadget=NULL;
	geometryStruct		geom;

	gadget=static_cast<LFSTK_gadgetClass*>(self);
	gadget->xEvent=e;

	switch (e->type)
		{
			case EnterNotify:
				retval=gadget->mouseEnter(&e->xbutton);
				break;
			case LeaveNotify:
				retval=gadget->mouseExit(&e->xbutton);
				break;
			case ButtonRelease:
				gadget->wc->app->isDragging=false;
				if(gadget->firstClick==false)
					{
						gadget->isDoubleClick=false;
						gadget->firstClick=true;
						gadget->lastTime=e->xbutton.time;
					}
				else
					{
						gadget->firstClick=false;
						if(e->xbutton.time-gadget->lastTime<gadget->wc->dbClick)
							{
								gadget->isDoubleClick=true;
							}
						else
							{
								gadget->lastTime=e->xbutton.time;
								gadget->firstClick=true;
								gadget->isDoubleClick=false;
							}
					}

				retval=gadget->mouseUp(&e->xbutton);
				break;

			case ButtonPress:
				gadget->currentButton=e->xbutton.button;
				if((gadget->LFSTK_getContextWindow()!=NULL) && (gadget->currentButton==Button3))
					{
						LFSTK_windowClass	*lwc=gadget->LFSTK_getContextWindow();
						const geometryStruct *wingeom=lwc->LFSTK_getWindowGeom();

						lwc->popupFromGadget=gadget;
						gadget->LFSTK_getGeomWindowRelative(&geom,gadget->wc->app->rootWindow);
						switch(gadget->contextWindowPos)
							{
								case CONTEXTLEFT:
									lwc->LFSTK_moveWindow(geom.x,geom.y+(geom.h/4),true);
									break;
								case CONTEXTRIGHT:
									lwc->LFSTK_moveWindow(geom.x+geom.w-wingeom->w,geom.y+(geom.h),true);
									break;
								case CONTEXTATMOUSE:
									break;
							}
						
						gadget->wc->app->LFSTK_runWindowLoop(lwc);
						lwc->popupFromGadget=NULL;
						retval=true;
						break;
					}

				if((type==LINEEDITGADGET) || (type==MULTILINEGADGET))
					XSetInputFocus(gadget->wc->app->display,e->xbutton.window,RevertToNone,CurrentTime);
				retval=gadget->mouseDown(&e->xbutton);
				break;

			case MotionNotify:
				if(gadget->currentButton==Button1)
					retval=gadget->mouseDrag(&e->xmotion);
				//printf("MotionNotify\n");
				break;
			case Expose:
				if (e->xexpose.count==0)
					gadget->LFSTK_clearWindow();
				break;

			case FocusIn:
				//printf("focus in libev\n");
				retval=gadget->gotFocus(e);
				break;
			case FocusOut:
				//printf("focus out libev\n");
				retval=gadget->lostFocus(e);
				break;

			case KeyRelease:
				retval=gadget->keyRelease(&e->xkey);
				//printf("KeyRelease\n");
				break;
			case KeyPress:
				retval=gadget->keyPress(&e->xkey);
				//printf("KeyPress\n");
				break;
	
			case ConfigureNotify:
				//printf("conf>>>>>>>>>\n");
				break;
			case GravityNotify:
				//printf("grav>>>>>>>>>>>\n");
				break;
			case ResizeRequest:
				//printf("resize\n");
				break;
			case ClientMessage:
				//printf("ClientMessage from lib\n");
				break;
			case SelectionRequest:
				//fprintf(stderr,"from SelectionRequest lib\n");
				break;
		}

	gadget->xEvent=NULL;
	if(retval==false)
		{
			switch (e->type)
				{
					case EnterNotify:
					case LeaveNotify:
					case ButtonPress:
					case ButtonRelease:
					case KeyPress:
					case KeyRelease:
						if(gadget->toParent==true)
							{
								e->xbutton.window=gadget->parent;
								e->xbutton.subwindow=gadget->window;
								e->xbutton.send_event=true;
								XSendEvent(gadget->wc->app->display,gadget->parent,true,0xffff,(XEvent*)e);
								retval=false;
							}
						else
							retval=true;
						break;
				}
		}
	return(retval);
}

/**
* Get global auto text colour.
* \return bool Use auto text colour in gadgets.
*/
bool LFSTK_lib::LFSTK_getAutoLabelColour(void)
{
	return(this->prefs.LFSTK_getBool("autotextcolour"));
}

/**
* Set global auto text colour.
* \param bool Use auto text colour in gadgets.
*/
void LFSTK_lib::LFSTK_setAutoLabelColour(bool toset)
{
	this->prefs.LFSTK_setBool("autotextcolour",toset);
}

/**
* Get whether to use theme.
* \return bool Use theme pixmaps in gadgets.
*/
bool LFSTK_lib::LFSTK_getUseTheme(void)
{
	return(this->prefs.LFSTK_getBool("usetheme"));
}

/**
* Set whether to use theme.
*/
void LFSTK_lib::LFSTK_setUseTheme(bool use)
{
	this->prefs.LFSTK_setBool("usetheme",use);
}

/**
* Get path to themed icon.
* \param theme Theme name ( case sensitive ).
* \param icon Icon name ( case insensitive ).
* \param catagory Catagory or "" NOT NULL ( case insensitive ).
* \return char* string caller should free.
* \note returned string is set to NULL on error.
*/
#define GLOBALICONS 0
#define GLOBALPIXMAPS 2
#define GLOBALPIXMAPSEND 5

char* LFSTK_lib::LFSTK_findThemedIcon(const char *theme,const char *icon,const char *catagory)
{
	char        *iconpath=NULL;
	const char  *iconthemes[3];
	const char  *iconfolders[GLOBALPIXMAPSEND];
	bool		maskdot=false;
	char		*holdicon=NULL;
	
	if(icon[0]=='/')
		return(strdup(icon));

	holdicon=strdup(icon);
	if(strcasecmp(&holdicon[strlen(holdicon)-4],".png")==0)
		{
			maskdot=true;
			holdicon[strlen(holdicon)-4]=0;
		}
	if(theme==NULL)
		theme="gnome";
	
	iconthemes[0]=theme;
	iconthemes[1]="hicolor";
	iconthemes[2]="gnome";

	iconfolders[0]="~/.icons";
	iconfolders[1]="/usr/share/icons";

	iconfolders[2]="/usr/share/pixmaps";
	iconfolders[3]="/usr/share/icons/hicolor";
	iconfolders[4]="~/.local/share/icons";

	iconpath=NULL;
	for(int j=GLOBALICONS;j<GLOBALPIXMAPS;j++)
		{
			for(int k=0;k<3;k++)
				{
					iconpath=this->LFSTK_oneLiner("find %s/\"%s\"/*/%s -iname '*%s.png' 2>/dev/null|sort --version-sort|tail -n1 2>/dev/null",iconfolders[j],iconthemes[k],catagory,holdicon);

					if((iconpath!=NULL) && (strlen(iconpath)>1))
						goto breakReturn;
					if(iconpath!=NULL)
						free(iconpath);
					iconpath=NULL;
				}
		}

	if(iconpath==NULL)
		{
			for(int j=GLOBALPIXMAPS;j<GLOBALPIXMAPSEND;j++)
				{
					iconpath=this->LFSTK_oneLiner("find %s -iname '*%s.*'",iconfolders[j],holdicon);
					if((iconpath!=NULL) && (strlen(iconpath)>1))
						goto breakReturn;
					if(iconpath!=NULL)
						free(iconpath);
					iconpath=NULL;
				}
		}

breakReturn:
	free(holdicon);
	return(iconpath);
}

/**
* Execute and return stdout.
* \param const std::string fmt,... standard printf fmt but only recognises "%s","%%" and "%i".
* \return std::string.
* \note Synchronous ONLY.
*/
std::string LFSTK_lib::LFSTK_oneLiner(const std::string fmt,...)
{
	FILE		*fp;
	va_list		ap;
	char		*buffer=(char*)alloca(MAXBUFFER);
	std::string	str="";

	va_start(ap, fmt);
	int cnt=0;
	while(cnt<fmt.length())
		{
			if(fmt.at(cnt)=='%')
				{
					cnt++;
					switch(fmt.at(cnt))
						{
							case 's':
								str+=va_arg(ap,char*);
								break;
							case 'i':
								str+=std::to_string(va_arg(ap,int));
								break;
							case '%':
								str+="%";
								break;
							default:
								str+=fmt.at(cnt);
								break;
						}
				}
			else
				{
					str+=fmt.at(cnt);
				}
			cnt++;
		}
	va_end(ap);

	fp=popen(str.c_str(),"r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			fgets(buffer,MAXBUFFER-1,fp);
			if(strlen(buffer)>0)
				{
					if(buffer[strlen(buffer)-1] =='\n')
						buffer[strlen(buffer)-1]=0;
				}
			pclose(fp);
			str=buffer;
		}
	return(str);
}

/**
* Execute and return stout from string.
* \return char* Allocated string caller should free.
* \note Synchronous ONLY.
* \note To be removed.
*/
char* LFSTK_lib::LFSTK_oneLiner(const char* fmt,...)
{
	FILE	*fp;
	va_list	ap;
	char	*buffer,*subbuffer;

	buffer=(char*)alloca(MAXBUFFER);
	subbuffer=(char*)alloca(MAXBUFFER);

	buffer[0]=0;
	subbuffer[0]=0;
	va_start(ap, fmt);
	while (*fmt)
		{
			subbuffer[0]=0;
			if(fmt[0]=='%')
				{
					fmt++;
					switch(*fmt)
						{
							case 's':
								sprintf(subbuffer,"%s",va_arg(ap,char*));
								break;
							case 'i':
								sprintf(subbuffer,"%i",va_arg(ap,int));
								break;
							case '%':
								sprintf(subbuffer,"%%");
								break;
							default:
								sprintf(subbuffer,"%c",fmt[0]);
								break;
						}
				}
			else
				sprintf(subbuffer,"%c",fmt[0]);
			strcat(buffer,subbuffer);
			fmt++;
		}
	va_end(ap);

	fp=popen(buffer,"r");
	if(fp!=NULL)
		{
			buffer[0]=0;
			fgets(buffer,MAXBUFFER,fp);
			if(strlen(buffer)>0)
				{
					if(buffer[strlen(buffer)-1] =='\n')
						buffer[strlen(buffer)-1]=0;
				}
			pclose(fp);
			return(strdup(buffer));
		}
	return(NULL);
}

/**
* Get if point (x,y) is in rect (geom).
* \param pointStruct *point.
* \param geometryStruct *geom.
* \return true=inrect.
*/
bool LFSTK_lib::LFSTK_pointInRect(pointStruct *point,geometryStruct *geom)
{
	if((point->x>geom->x) && (point->x<(geom->x+geom->w)) && (point->y>geom->y) && (point->y<(geom->y+geom->h)))
		return(true);

	return(false);
}

/**
* Set colour struct from colour name.
* \param Display *display.
* \param Colormap cm.
* \param colourStruct *colptr Struct to init.
* \param const char *name Colour name 
*/
void LFSTK_lib::LFSTK_setColourFromName(Display *display,Colormap cm,colourStruct *colptr,const char *name)
{
	XColor tc,sc;
	if(colptr->name!=NULL)
		{
			free(colptr->name);
			colptr->name=strdup(name);

			XFreeColors(display,cm,(long unsigned int*)&colptr->pixel,1,0);
		}
	XAllocNamedColor(display,cm,name,&sc,&tc);
	colptr->pixel=sc.pixel;

	colptr->RGBAColour.r=((colptr->pixel>>16) & 0xff)/256.0;
	colptr->RGBAColour.g=((colptr->pixel>>8) & 0xff)/256.0;
	colptr->RGBAColour.b=((colptr->pixel>>0) & 0xff)/256.0;
}

/**
* Get pixel from colour name.
* \param Display *display.
* \param Colormap cm.
* \param name Colour name eg "#ff00ff".
* \return unsigned long Pixel colour.
*/
unsigned long LFSTK_lib::LFSTK_getColourFromName(Display *display,Colormap cm,const char *name)
{
	unsigned long	retval=0x000000;
	XColor			tc,sc;

	if(XAllocNamedColor(display,cm,name,&sc,&tc)!=0)
		{
			retval=sc.pixel;
			XFreeColors(display,cm,&sc.pixel,1,0);
		}

	return(retval);
}

/**
* Set cairo surface and context for window.
* \param Display* display.
* \param Window window.
* \param Visual visual.
* \param cairo_surface_t **sfc.
* \param cairo_t **cr.
* \param int width.
* \param int height.
*/
void LFSTK_lib::LFSTK_setCairoSurface(Display *display,Window window,Visual *visual,cairo_surface_t **sfc,cairo_t **cr,int width,int height)
{

	if(*sfc==NULL)
		*sfc=cairo_xlib_surface_create(display,window,visual,width,height);
	else
		cairo_xlib_surface_set_size(*sfc,width,height);

	if(*cr==NULL)
		*cr=cairo_create(*sfc);
}

/**
* Get mime type of file.
* \param char *path.
* \return char*.
* \note Caller owns returned allocated string.
* \note "application/octet-stream" returned if path doesn't exist.
*/
char* LFSTK_lib::LFSTK_getMimeType(const char* path)
{
	if(access(path,F_OK)==0)
		return(this->LFSTK_oneLiner("file -L -b --mime-type \"%s\"",path));
	else
		return(strdup("application/octet-stream"));
}

/*! This function decompresses a JPEG image from a memory buffer and creates a
 * Cairo image surface.
 * @param data Pointer to JPEG data (i.e. the full contents of a JPEG file read
 * into this buffer).
 * @param len Length of buffer in bytes.
 * @return Returns a pointer to a cairo_surface_t structure. It should be
 * checked with cairo_surface_status() for errors.
 */
cairo_surface_t* LFSTK_lib::cairo_image_surface_create_from_jpeg_mem(const unsigned char* data, size_t len)
{
	struct jpeg_decompress_struct	cinfo;
	struct jpeg_error_mgr			jerr;
	JSAMPROW						row_pointer[1];
	cairo_surface_t					*sfc;
 
   // initialize jpeg decompression structures
	cinfo.err=jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo,(const unsigned char*)data, len);
	jpeg_read_header(&cinfo,true);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	cinfo.out_color_space=JCS_EXT_BGRA;
#else
	cinfo.out_color_space=JCS_EXT_ARGB;
#endif

   // start decompressor
	jpeg_start_decompress(&cinfo);

   // create Cairo image surface
	sfc=cairo_image_surface_create(CAIRO_FORMAT_RGB24,cinfo.output_width,cinfo.output_height);
	if(cairo_surface_status(sfc)!=CAIRO_STATUS_SUCCESS)
		{
			jpeg_destroy_decompress(&cinfo);
			return(sfc);
		}

   // loop over all scanlines and fill Cairo image surface
	while(cinfo.output_scanline<cinfo.output_height)
		{
			row_pointer[0]=cairo_image_surface_get_data(sfc)+(cinfo.output_scanline * cairo_image_surface_get_stride(sfc));
			jpeg_read_scanlines(&cinfo,row_pointer,1);
		}

   // finish and close everything
	cairo_surface_mark_dirty(sfc);
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

   // set jpeg mime data
	cairo_surface_set_mime_data(sfc,CAIRO_MIME_TYPE_JPEG,(const unsigned char*)data,len,free,(void*)data);

	return sfc;
}

/*! This function reads an JPEG image from a file an creates a Cairo image
 * surface. Internally the filesize is determined with fstat(2) and then the
 * whole data is read at once.
 * @param filename Pointer to filename of JPEG file.
 * @return Returns a pointer to a cairo_surface_t structure. It should be
 * checked with cairo_surface_status() for errors.
 * @note If the returned surface is invalid you can use errno to determine
 * further reasons. Errno is set according to fopen(3) and malloc(3). If you
 * intend to check errno you shall set it to 0 before calling this function
 * because it does not modify errno itself.
 */
cairo_surface_t* LFSTK_lib::LFSTK_cairo_image_surface_create_from_jpeg(const char *filename)
{
	const unsigned char		*data;
	int						infile;
	struct stat				stat;
	char					magic[]="\xff\xd8\xff";

   // open input file
	if((infile=open(filename,O_RDONLY))==-1)
		return(NULL);

   // get stat structure for file size
	if (fstat(infile,&stat)==-1)
		return(NULL);

   // allocate memory
	if((data=(const unsigned char*)malloc(stat.st_size))==NULL)
		return(NULL);

   // read data
	if(read(infile,(void*)data,stat.st_size)<stat.st_size)
		return(NULL);

	char *ptr=(char*)data;
	bool flag=true;
	for(int j=0;j<3;j++)
		if(ptr[j]!=magic[j])
			flag=false;

	close(infile);
	if(flag==false)
		{
			fprintf(stderr,"not a jpeg\n");
			return(NULL);
		}
	return cairo_image_surface_create_from_jpeg_mem(data, stat.st_size);
}

/**
* Returns the root window pixmap
* \return Pixmap
* \note Returns pixmap or 0.
*/
Pixmap LFSTK_lib::LFSTK_getWindowPixmap(Display *display,Window win)
{
	Pixmap			currentRootPixmap=None;
    Atom			act_type;
    int				act_format;
    unsigned long	nitems;
    unsigned long	bytes_after;
    unsigned char	*data=NULL;
    Atom			_XROOTPMAP_ID;

    _XROOTPMAP_ID=XInternAtom(display,"_XROOTPMAP_ID",False);
    if (XGetWindowProperty(display,win,_XROOTPMAP_ID,0,1,False,XA_PIXMAP,&act_type,&act_format,&nitems,&bytes_after,&data)==Success)
    	{
	        if (data)
	        	{
					currentRootPixmap=*((Pixmap *)data);
					XFree(data);
        		}
    	}
	return currentRootPixmap;
}

/**
* Create a cairo sruface from file path.
* \return cairo_surface_t*
*/
cairo_surface_t	*LFSTK_lib::LFSTK_createSurfaceFromPath(const char *path)
{
	cairo_status_t	cs=CAIRO_STATUS_SUCCESS;
	cairo_surface_t	*tempimage=NULL;
	char			*suffix=NULL;

	if(path==NULL)
		tempimage=NULL;
	else
		{
			suffix=strrchr((char*)path,'.');
			if((suffix!=NULL) && (strcasecmp(suffix,".png")==0))
				{
					tempimage=cairo_image_surface_create_from_png(path);
					cs=cairo_surface_status(tempimage);
				}
			else
				cs=CAIRO_STATUS_INVALID_FORMAT;

			if(cs!=CAIRO_STATUS_SUCCESS)
				{
					if((suffix!=NULL) && (strcasecmp(suffix,".jpg")==0))
						{
							tempimage=this->LFSTK_cairo_image_surface_create_from_jpeg(path);
							cs=cairo_surface_status(tempimage);
						}
					else
						cs=CAIRO_STATUS_INVALID_FORMAT;
				}
		}

	if(cs!=CAIRO_STATUS_SUCCESS)
		tempimage=NULL;
	return(tempimage);
}

/**
* Get path to theme.
* \return const char* path. 
*/
const char* LFSTK_lib::LFSTK_getThemePath(void)
{
	return(this->prefs.LFSTK_getCString("themepath"));
}

/**
* Set path to theme.
* \param char* path. 
*/
void LFSTK_lib::LFSTK_setThemePath(char *path)
{
	this->prefs.LFSTK_setString("themepath",path);
}

/**
* Clean up string.
* \param const char *str.
* \return char* Cleaned string.
* \note uri translated to local files.
* \note strings are cleaned of white space at front and back.
* \note returned string should be freed by caller.
*/
char* LFSTK_lib::LFSTK_cleanString(const char *str)
{
	char	*localstr=strdup(str);
	char	*uristr=g_uri_parse_scheme(localstr);
	char	*cleanstr;
	if(uristr!=NULL)
		{
			cleanstr=g_filename_from_uri((const char*)g_strstrip(localstr),NULL,NULL);
			free(uristr);
		}
	else
		{
			cleanstr=strdup((const char*)g_strstrip(localstr));
		}
	free(localstr);
	return(cleanstr);
}

/**
* Find line in a text file.
* \param const std::string filepath.
* \paramconst std::string needle
* \return std::string returns line containg 'needle' or "".
*/
std::string	LFSTK_lib::LFSTK_grepInFile(const std::string filepath,const std::string needle)
{
    std::string		line;
	std::ifstream	fin(filepath);
    
	while(getline(fin,line))
		{
			if(line.find(needle) != std::string::npos)
				return(line);
		}
	return("");
}

/**
* Find Nth token in a string.
* \param const std::string haystack.
* \param int needlecnt token number to get base 1.
* \param const std::string delimiter default is " ".
* \return std::string returns Nth token ( from left ).
* \note returns "" if not found.
*/
std::string	LFSTK_lib::LFSTK_getNthNeedle(const std::string haystack,int needlecnt,const std::string delimiter)
{
	size_t		pos=0;
	std::string	token;
	std::string	tstr=haystack;
	int			itemnum=1;

	while((pos=tstr.find(delimiter)) != std::string::npos)
		{
			token=tstr.substr(0,pos);
			if(itemnum==needlecnt)
   				return(token);
			else
				itemnum++;
			tstr.erase(0,pos+delimiter.length());
		}
	return("");
}

