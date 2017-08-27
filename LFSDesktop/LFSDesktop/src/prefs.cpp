/*
 *
 * ©K. D. Hedger. Thu 13 Aug 16:54:44 BST 2015 kdhedger68713@gmail.com

 * This file (prefs.cpp) is part of LFSDesktop.

 * LFSDesktop is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSDesktop is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSDesktop.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "prefs.h"
#include "files.h"

char			*iconTheme;

//desktop prefs
int				iconSize=40;
int				gridBorder=32;
int				gridSize=iconSize+gridBorder;
int				refreshRate=2;
char			*terminalCommand=NULL;
bool			showSuffix=false;
char			*fontFace=NULL;
char			*foreCol;
char			*backCol;
char			*backAlpha;
char			*ignores=NULL;

Display			*display;
Window			rootWin;
int				displayWidth;
int				displayHeight;
GC				gc;
Region			rg;
XdbeBackBuffer	buffer;
XdbeSwapInfo	swapInfo;
Drawable		drawOnThis;
Colormap		cm;
Visual			*visual=NULL;
int				depth=0;
int				screen;
int				blackColor;
int				whiteColor;

GC				labelGC;
//XFontStruct		*labelFont;

unsigned long	labelBackground;
unsigned long	labelForeground;

int				**xySlot;

args			desktopPrefs[]=
{
	{"icontheme",TYPESTRING,&iconTheme},
	{"iconsize",TYPEINT,&iconSize},
	{"gridsize",TYPEINT,&gridSize},
	{"gridborder",TYPEINT,&gridBorder},
	{"refreshrate",TYPEINT,&refreshRate},
	{"termcommand",TYPESTRING,&terminalCommand},
	{"showextension",TYPEBOOL,&showSuffix},
	{"fontface",TYPESTRING,&fontFace},
	{"labelforeground",TYPESTRING,&foreCol},
	{"labelbackground",TYPESTRING,&backCol},
	{"labelalpha",TYPESTRING,&backAlpha},
	{"noshow",TYPESTRING,&ignores},
	{NULL,0,NULL}
};

void saveVarsToFile(const char* filepath,args* dataptr)
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
							fprintf(fd,"%s	%i\n",dataptr[cnt].name,*(int*)dataptr[cnt].data);
							break;
						case TYPESTRING:
							if(*(char**)(dataptr[cnt].data)!=NULL)
								fprintf(fd,"%s	%s\n",dataptr[cnt].name,*(char**)(dataptr[cnt].data));
							break;
						case TYPEBOOL:
							fprintf(fd,"%s	%i\n",dataptr[cnt].name,(int)*(bool*)dataptr[cnt].data);
							break;
						}
					cnt++;
				}
			fclose(fd);
		}
}

bool loadVarsFromFile(char* filepath,args* dataptr)
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
