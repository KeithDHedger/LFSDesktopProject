/*
 *
 * ©K. D. Hedger. Fri 21 Aug 12:28:57 BST 2015 kdhedger68713@gmail.com

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

#include "LFSTKGlobals.h"

enum {DEFNORMAL,DEFPRELIE,DEFACTIVE,DEFINACTIVE,DEFFONTNORMAL,DEFFONTPRELIE,DEFFONTACTIVE,DEFFONTINACTIVE};

const char	*defaultColourStrings[]={"grey50","grey80","grey40","grey90"};
const char	*defaultFontColourStrings[]={"white","black","white","grey80"};
const char	*defaultFontString="sans-serif:size=10";
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
					argname=NULL;
					strarg=NULL;
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
	int r,g,b;
	r=(pixel & 0xff0000)>>16;
	g=(pixel & 0x00ff00)>>8;
	b=(pixel & 0x0000ff);

	if((r+r+r+b+g+g+g+g)>>3>128)
		return("black");
	else
		return("white");
}

/**
* Get width of string.
* \param disp* Dsiplay.
* \param font* Font to use.
* \param str* String.
* \return int Width in pixels.
*/
int LFSTK_lib::LFSTK_getTextwidth(Display *disp,XftFont *font,const char *str)
{
	XGlyphInfo info;
	XftTextExtentsUtf8(disp,font,(XftChar8 *)str,strlen(str),&info);
	return info.width-info.x;
}

/**
* Load a font.
* \param disp* Display.
* \param scr Screen.
* \param name* Font name.
* \return fontStruct* Font structure.
* \note Return structure shopuld be freed.
*/
fontStruct* LFSTK_lib::LFSTK_loadFont(Display *disp,int scr,const char *name)
{
	XftFont	*font=NULL;

	if (name!=NULL)
		{
			font=XftFontOpenXlfd(disp,scr,name);

			if (font==NULL)
				font=XftFontOpenName(disp,scr,name);

			if (font==NULL)
				fprintf(stderr,"cannot not load font %s",name);
		}

	if (font==NULL)
		font=XftFontOpenName(disp,scr,DEFAULTFONT);

	if (font==NULL)
		return NULL;

	fontStruct *f=(fontStruct*)malloc(sizeof(fontStruct));
	f->size=font->ascent+font->descent;
	f->ascent=font->ascent;
	f->descent=font->descent;
	f->data=font;

	return f;
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
				retval=gadget->mouseUp(&e->xbutton);
				break;
			case ButtonPress:
				if(type==LINEEDITGADGET)
					XSetInputFocus(gadget->wc->display,e->xbutton.window,RevertToNone,CurrentTime);
				retval=gadget->mouseDown(&e->xbutton);
				break;

			case MotionNotify:
				break;
			case Expose:
			 //		printf("xpose from lib type=%i\n",type);
				//XSetInputFocus(gadget->wc->display,None,RevertToNone,CurrentTime);
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
//			case ClientMessage:
//				//printf("ClientMessage from lib\n");
//				retval=gadget->clientMessage(e);
//				break;
				
		}
//	printf("%i\n",ud);
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


/**
* Set Pixmaps From Path.
* \param display Xlib display.
* \param visual Xlib visual.
* \param cm Xlib colormap.
* \param w Xlib window.
* \param file Path to image file.
* \param image Return address for image pixmap.
* \param mask Return address for image mask.
* \param size Destination size.
* \return true on success or false on fail.
* \note If size=-1 then the size is set from the image file.
*/
bool LFSTK_lib::LFSTK_setPixmapsFromPath(Display *display,Visual *visual,Colormap cm,Window w,const char *file,Pixmap *image,Pixmap *mask,int size)
{
	Imlib_Image	data=NULL;
	int			imagesizew;
	int			imagesizeh;

	if(file==NULL)
		return(false);

	data=imlib_load_image(file);
	if(data!=NULL)
		{
			imlib_context_set_display(display);
			imlib_context_set_visual(visual);
			imlib_context_set_colormap(cm);
			imlib_context_set_drawable(w);
			imlib_context_set_image(data);
			if(size==-1)
				{
					imagesizew=imlib_image_get_width();
					imagesizeh=imlib_image_get_height();
				}
			else
				{
					imagesizew=size;
					imagesizeh=size;
				}
			imlib_render_pixmaps_for_whole_image_at_size(image,mask,imagesizew,imagesizeh);
			imlib_free_image();
			return(true);
		}
	return(false);
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
* \return char* Static string caller should NOT free.
* \note returned string is set to NULL on error.
*/
const char* LFSTK_lib::LFSTK_findThemedIcon(const char *theme,const char *icon,const char *catagory)
{

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






