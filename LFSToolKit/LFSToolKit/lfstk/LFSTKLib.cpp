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
const char	*defaultThemePath="/usr/share/themes/Crux/xfwm4";
const char	*defaultFrameStrings[]={"black","#00ffff","black","white","white"};
const char	*defaultWindowTile="";
const char	*defaultButtonTile="";
const char	*defaultMenuItemTile="";

char		filterBuffer[256];
char		retBuffer[512];

LFSTK_lib::~LFSTK_lib()
{
	free(lfsToolKitGlobals);
	for(int j=0; j<MAXCOLOURS; j++)
		{
			if(this->globalWindowColours[j]!=NULL)
				free(this->globalWindowColours[j]);

			if(this->globalButtonColours[j]!=NULL)
				free(this->globalButtonColours[j]);

			if(this->globalMenuItemColours[j]!=NULL)
				free(this->globalMenuItemColours[j]);

			if(this->globalMenuItemFontColourNames[j]!=NULL)
				free(this->globalMenuItemFontColourNames[j]);

			if(this->globalFontColourNames[j]!=NULL)
				free(this->globalFontColourNames[j]);
		}

	if(this->globalWindowTile!=NULL)
		free(this->globalWindowTile);

	if(this->globalButtonTile!=NULL)
		free(this->globalButtonTile);

	if(this->globalMenuItemTile!=NULL)
		free(this->globalMenuItemTile);

	if(this->globalMenuItemFontString!=NULL)
		free(this->globalMenuItemFontString);

	if(this->globalFontString!=NULL)
		free(this->globalFontString);

	if(this->globalMonoFontString!=NULL)
		free(this->globalMonoFontString);
}

/**
* Sets a global string or the default.
*
* \param state Gadget state.
* \param type Type of string to set.
* \param str New string.
*/
void LFSTK_lib::LFSTK_setGlobalString(int state,int type,const char *str)
{
	const char	*ptr=NULL;
	
	switch(type)
		{
			case TYPEWINDOW:
				ptr=this->globalWindowColours[state];
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalWindowColours[state]=strdup(str);
				break;
			case TYPEWINDOWTILE:
				ptr=this->globalWindowTile;
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalWindowTile=strdup(str);
				break;
			case TYPEBUTTON:
				ptr=this->globalButtonColours[state];
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalButtonColours[state]=strdup(str);
				break;
			case TYPEBUTTONTILE:
				ptr=this->globalButtonTile;
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalButtonTile=strdup(str);
				break;
			case TYPEMENUITEM:
				ptr=this->globalMenuItemColours[state];
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalMenuItemColours[state]=strdup(str);
				break;
			case TYPEMENUITEMTILE:
				ptr=this->globalMenuItemTile;
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalMenuItemTile=strdup(str);
				break;
			case TYPEFONTCOLOUR:
				ptr=this->globalFontColourNames[state];
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalFontColourNames[state]=strdup(str);
				break;
			case TYPEMENUITEMFONTCOLOUR:
				ptr=this->globalMenuItemFontColourNames[state];
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalMenuItemFontColourNames[state]=strdup(str);
				break;
			case TYPEMENUITEMFONT:
				ptr=this->globalMenuItemFontString;
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalMenuItemFontString=strdup(str);
				break;
			case TYPEFONT:
				ptr=this->globalFontString;
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalFontString=strdup(str);
				break;
			case TYPEMONOFONT:
				ptr=this->globalMonoFontString;
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalMonoFontString=strdup(str);
				break;
			case TYPECURSORCOLOUR:
				ptr=this->globalCursorColour;
				if(ptr!=NULL)
					free((void*)ptr);
				this->globalCursorColour=strdup(str);
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
	
	switch(type)
		{
			case TYPEWINDOW:
			//printf("--->%s<---\n",this->globalWindowColours[state]);
			ptr=this->globalWindowColours[state];
				if(ptr==NULL)
					ptr=defaultColourStrings[state];
				break;
			case TYPEWINDOWTILE:
				ptr=this->globalWindowTile;
				if(ptr==NULL)
					ptr=defaultWindowTile;
				break;
			case TYPEBUTTON:
				ptr=this->globalButtonColours[state];
				if(ptr==NULL)
					ptr=defaultColourStrings[state];
				break;
			case TYPEBUTTONTILE:
				ptr=this->globalButtonTile;
				if(ptr==NULL)
					ptr=defaultButtonTile;
				break;
			case TYPEMENUITEM:
				ptr=this->globalMenuItemColours[state];
				if(ptr==NULL)
					ptr=defaultColourStrings[state];
				break;
			case TYPEMENUITEMTILE:
				ptr=this->globalMenuItemTile;
				if(ptr==NULL)
					ptr=defaultMenuItemTile;
				break;
			case TYPEFONTCOLOUR:
				ptr=this->globalFontColourNames[state];
				if(ptr==NULL)
					ptr=defaultFontColourStrings[state];
				break;
			case TYPEMENUITEMFONTCOLOUR:
				ptr=this->globalMenuItemFontColourNames[state];
				if(ptr==NULL)
					ptr=defaultFontColourStrings[state];
				break;
			case TYPEMENUITEMFONT:
				ptr=this->globalMenuItemFontString;
				if(ptr==NULL)
					ptr=defaultFontString;			
				break;
			case TYPEFONT:
				ptr=this->globalFontString;
				if(ptr==NULL)
					ptr=defaultFontString;
				break;
			case TYPEMONOFONT:
				ptr=this->globalMonoFontString;
				if(ptr==NULL)
					ptr=defaultMonoFontString;
				break;
			case TYPECURSORCOLOUR:
				ptr=this->globalCursorColour;
				if(ptr==NULL)
					ptr=defaultCursorColour;
				break;
		}
	return(ptr);
}

LFSTK_lib::LFSTK_lib(bool loadvars)
{
	char *env=NULL;

	args myargs[]=
	{
//window
		{"window_normal",TYPESTRING,&(this->globalWindowColours[NORMALCOLOUR])},
		{"window_prelight",TYPESTRING,&(this->globalWindowColours[PRELIGHTCOLOUR])},
		{"window_active",TYPESTRING,&(this->globalWindowColours[ACTIVECOLOUR])},
		{"window_inactive",TYPESTRING,&(this->globalWindowColours[INACTIVECOLOUR])},
		{"windowtile",TYPESTRING,&(this->globalWindowTile)},
//button
		{"button_normal",TYPESTRING,&(this->globalButtonColours[NORMALCOLOUR])},
		{"button_prelight",TYPESTRING,&(this->globalButtonColours[PRELIGHTCOLOUR])},
		{"button_active",TYPESTRING,&(this->globalButtonColours[ACTIVECOLOUR])},
		{"button_inactive",TYPESTRING,&(this->globalButtonColours[INACTIVECOLOUR])},
		{"buttontile",TYPESTRING,&(this->globalButtonTile)},
//menu button
		{"menuitem_normal",TYPESTRING,&(this->globalMenuItemColours[NORMALCOLOUR])},
		{"menuitem_prelight",TYPESTRING,&(this->globalMenuItemColours[PRELIGHTCOLOUR])},
		{"menuitem_active",TYPESTRING,&(this->globalMenuItemColours[ACTIVECOLOUR])},
		{"menuitem_inactive",TYPESTRING,&(this->globalMenuItemColours[INACTIVECOLOUR])},

		{"menuitem_font",TYPESTRING,&(this->globalMenuItemFontString)},
		{"menuitem_font_normal",TYPESTRING,&(this->globalMenuItemFontColourNames[NORMALCOLOUR])},
		{"menuitem_font_prelight",TYPESTRING,&(this->globalMenuItemFontColourNames[PRELIGHTCOLOUR])},
		{"menuitem_font_active",TYPESTRING,&(this->globalMenuItemFontColourNames[ACTIVECOLOUR])},
		{"menuitem_font_inactive",TYPESTRING,&(this->globalMenuItemFontColourNames[INACTIVECOLOUR])},
		{"menuitemtile",TYPESTRING,&(this->globalMenuItemTile)},

//font
		{"font",TYPESTRING,&(this->globalFontString)},
		{"font_normal",TYPESTRING,&(this->globalFontColourNames[NORMALCOLOUR])},
		{"font_prelight",TYPESTRING,&(this->globalFontColourNames[PRELIGHTCOLOUR])},
		{"font_active",TYPESTRING,&(this->globalFontColourNames[ACTIVECOLOUR])},
		{"font_inactive",TYPESTRING,&(this->globalFontColourNames[INACTIVECOLOUR])},
//other
		{"autotextcolour",TYPEBOOL,&(this->autoLabelColour)},
		{"usetheme",TYPEBOOL,&(this->useTheme)},
//monofont
		{"monofont",TYPESTRING,&(this->globalMonoFontString)},
//cursor colour
		{"cursorcolour",TYPESTRING,&(this->globalCursorColour)},
		{NULL,0,NULL},
	};

	this->lfsToolKitGlobals=(args*)calloc(1,sizeof(myargs));
	memcpy(this->lfsToolKitGlobals,myargs,sizeof(myargs));

	for(int j=0;j<MAXCOLOURS;j++)
		{
			this->globalWindowColours[j]=NULL;
			this->globalButtonColours[j]=NULL;
			this->globalMenuItemColours[j]=NULL;
			this->globalMenuItemFontColourNames[j]=NULL;
		}

	for(int j=0;j<MAXCOLOURS;j++)
		this->globalFontColourNames[j]=NULL;

	this->globalFontString=NULL;
	this->globalMonoFontString=NULL;
	this->globalMenuItemFontString=NULL;
	this->globalWindowTile=NULL;
	this->globalButtonTile=NULL;
	this->globalMenuItemTile=NULL;
	this->autoLabelColour=false;
	this->useTheme=false;

	if(loadvars==true)
		{
			asprintf(&env,"%s/.config/LFS/lfstoolkit.rc",getenv("HOME"));
			this->LFSTK_loadVarsFromFile(env,this->lfsToolKitGlobals);
			free(env);
		}
}

/**
* Load variables from prefs file.
*
* \param filepath Path to prefs file.
* \param args Argument list..
* \return bool Success.
* \note args is an array of the form:
* \note PREFNAME,PREFTYPE,VARIABLEADDRESS.
* \note PREFTYPE=TYPEINT|TYPESTRING|TYPEBOOL.
* \note eg:
* \note {...,"font_inactive",TYPESTRING,&(this->globalFontColourNames[INACTIVECOLOUR],...,NULL,0,NULL};
*/
bool LFSTK_lib::LFSTK_loadVarsFromFile(const char* filepath,const args* dataptr)
{
	FILE*	fd=NULL;
	char	buffer[2048];
	int		cnt;
	char*	argname=NULL;
	char*	strarg=NULL;

	fd=fopen(filepath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
					argname=NULL;
					strarg=NULL;
					sscanf(buffer,"%ms %ms",&argname,&strarg);
					cnt=0;
					while(dataptr[cnt].name!=NULL)
						{
							if((strarg!=NULL) && (argname!=NULL) && (strcmp(argname,dataptr[cnt].name)==0))
								{
									switch(dataptr[cnt].type)
										{
										case TYPEINT:
											*(int*)dataptr[cnt].data=atoi(strarg);
											break;
										case TYPESTRING:
											if(*(char**)(dataptr[cnt].data)!=NULL)
												free(*(char**)(dataptr[cnt].data));
											sscanf(buffer,"%*s %m[^\n]s",(char**)dataptr[cnt].data);
											break;
										case TYPEBOOL:
											*(bool*)dataptr[cnt].data=(bool)atoi(strarg);
											break;
										}
								}
							cnt++;
						}
					if(argname!=NULL)
						free(argname);
					if(strarg!=NULL)
						free(strarg);
				}
			fclose(fd);
			return(true);
		}
	return(false);
}

/**
* Save variables to prefs file.
*
* \param filepath Path to prefs file.
* \param args Argument list..
* \note args is an array of the form:
* \note PREFNAME,PREFTYPE,VARIABLEADDRESS.
* \note PREFTYPE=TYPEINT|TYPESTRING|TYPEBOOL.
* \note eg:
* \note {...,"font_inactive",TYPESTRING,&(this->globalFontColourNames[INACTIVECOLOUR],...,NULL,0,NULL};
*/
void LFSTK_lib::LFSTK_saveVarsToFile(const char* filepath,const args* dataptr)
{
	FILE*	fd=NULL;
	int		cnt=0;

	if(filepath[0]=='-')
		fd=stdout;
	else
		fd=fopen(filepath,"w");

	if(fd!=NULL)
		{
			while(dataptr[cnt].name!=NULL)
				{
					switch(dataptr[cnt].type)
						{
						case TYPEINT:
							fprintf(fd,"%s %i\n",dataptr[cnt].name,*(int*)dataptr[cnt].data);
							break;
						case TYPESTRING:
							if(*(char**)(dataptr[cnt].data)!=NULL)
								fprintf(fd,"%s %s\n",dataptr[cnt].name,*(char**)(dataptr[cnt].data));
							break;
						case TYPEBOOL:
							fprintf(fd,"%s %i\n",dataptr[cnt].name,(int)*(bool*)dataptr[cnt].data);
							break;
						}
					cnt++;
				}
			if(fd!=stdout)
				fclose(fd);
		}

}

/**
* Get tool kit args array.
* \return args.
* \note args is an array of the form:
* \note PREFNAME,PREFTYPE,VARIABLEADDRESS.
* \note PREFTYPE=TYPEINT|TYPESTRING|TYPEBOOL.
* \note eg:
* \note {...,"font_inactive",TYPESTRING,&(this->globalFontColourNames[INACTIVECOLOUR],...,NULL,0,NULL};
* \note Don't free returned array.
*/
const args *LFSTK_lib::LFSTK_getTKArgs(void)
{
	return(this->lfsToolKitGlobals);
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

//printf("---%i---\n",type);
//printf("===type=%i====\n",e->type);
	gadget=static_cast<LFSTK_gadgetClass*>(self);

	switch (e->type)
		{
			case EnterNotify:
				retval=gadget->mouseEnter(&e->xbutton);
				break;
			case LeaveNotify:
				retval=gadget->mouseExit(&e->xbutton);
				break;
			case ButtonRelease:
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
							gadget->isDoubleClick=true;
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
				//DEBUGFUNC("gadget=%p",gadget);
				gadget->currentButton=e->xbutton.button;
				if((gadget->LFSTK_getContextWindow()!=NULL) && (gadget->currentButton==Button3))
					{
						gadget->wc->popupFromGadget=gadget;
						gadget->LFSTK_getGlobalGeom(&geom);
						gadget->LFSTK_doPopUp(geom.x+(geom.w/2),geom.y+(geom.h/4));
						retval=true;
						break;
					}
				else
					gadget->wc->popupFromGadget=NULL;

				if(type==LINEEDITGADGET)
					XSetInputFocus(gadget->wc->display,e->xbutton.window,RevertToNone,CurrentTime);
				retval=gadget->mouseDown(&e->xbutton);
				break;

			case MotionNotify:
				if(gadget->currentButton==Button1)
					retval=gadget->mouseDrag(&e->xmotion);
				///printf("MotionNotify\n");
				//gadget->LFSTK_clearWindow();
				break;
			case Expose:
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
	
			case ConfigureNotify:
//				printf("conf>>>>>>>>>\n");
				break;
			case GravityNotify:
//				printf("grav>>>>>>>>>>>\n");
//				retval=true;
				break;
			case ResizeRequest:
				//XCheckTypedWindowEvent
//				printf("resize\n");
				break;
			case ClientMessage:
//				//printf("ClientMessage from lib\n");
//				retval=gadget->clientMessage(e);
				break;
			case SelectionRequest:
				//fprintf(stderr,"from SelectionRequest lib\n");
				//retval=gadget->selectionRequest((XSelectionRequestEvent*)e);
				break;				
		}
	if(retval==false)
		XSendEvent(gadget->wc->display,gadget->wc->window,False,0L,e);
	return(retval);
}

/**
* Get global auto text colour.
* \return bool Use auto text colour in gadgets.
*/
bool LFSTK_lib::LFSTK_getAutoLabelColour(void)
{
	return(this->autoLabelColour);
}

/**
* Set global auto text colour.
* \param bool Use auto text colour in gadgets.
*/
void LFSTK_lib::LFSTK_setAutoLabelColour(bool toset)
{
	this->autoLabelColour=toset;
}

/**
* Get whether to use theme.
* \return bool Use theme pixmaps in gadgets.
*/
bool LFSTK_lib::LFSTK_getUseTheme(void)
{
	return(this->useTheme);
}

/**
* Set whether to use theme.
*/
void LFSTK_lib::LFSTK_setUseTheme(bool use)
{
	this->useTheme=use;
}

int LFSTK_lib::callback(const char *fpath,const struct stat *sb,int typeflag)
{
	if(typeflag!=FTW_F)
		return(0);

//	retBuffer[0]=0;
	if (fnmatch(filterBuffer,fpath,FNM_CASEFOLD)==0)
		{
			snprintf(retBuffer,511,"%s",fpath);
			return 1;
		}
	return 0;
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
	char		*holdicon=strdup(icon);
	
	if(icon[0]=='/')
		return(strdup(icon));

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
			printf("no icon\n");
		}
breakReturn:
	free(holdicon);
	return(iconpath);

#if 0
	char	dirbuffer[2048];

	sprintf(dirbuffer,"/usr/share/icons/%s",theme);
	sprintf(filterBuffer,"*%s*%s.png",catagory,icon);
	retBuffer[0]=0;
	if(ftw(dirbuffer,this->callback,16)==1)
		return(retBuffer);

	sprintf(dirbuffer,"%s/.icons/%s",getenv("HOME"),theme);
	if(ftw(dirbuffer,this->callback,16)==1)
		return(retBuffer);

	sprintf(dirbuffer,"/usr/share/icons/hicolor");
	if(ftw(dirbuffer,this->callback,16)==1)
		return(retBuffer);

	sprintf(dirbuffer,"/usr/share/icons/gnome");
	if(ftw(dirbuffer,this->callback,16)==1)
		return(retBuffer);

	sprintf(dirbuffer,"/usr/share/pixmaps");
	sprintf(filterBuffer,"*%s.png",icon);
	if(ftw(dirbuffer,this->callback,16)==1)
		return(retBuffer);

	return(NULL);
#endif
}

//synchronous only
/**
* Execute and return stout from string.
* \return char* Allocated string caller should free.
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

//debugfunc("%s",buffer);
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
		free(colptr->name);
	colptr->name=strdup(name);
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
*/
char* LFSTK_lib::LFSTK_getMimeType(const char* path)
{
	return(this->LFSTK_oneLiner("file -L -b --mime-type \"%s\"",path));
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

