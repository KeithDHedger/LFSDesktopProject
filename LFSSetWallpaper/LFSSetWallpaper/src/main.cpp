/*
 *
 * ©K. D. Hedger. Tue 15 Sep 20:20:50 BST 2015 keithdhedger@gmail.com

 * This file (main.cpp) is part of LFSSetWallpaper.

 * LFSSetWallpaper is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.

 * LFSSetWallpaper is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSSetWallpaper.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <X11/Xlib.h>
#include <Imlib2.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xinerama.h>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <ostream>

#include "config.h"

#include <lfstk/LFSTKGlobals.h>
#include <lfstk/LFSTKPrefsClass.h>

#define UNKNOWNARG -100

enum imageMode {WALLSTRETCH=0,WALLTILE,WALLCENTRE,WALLSCALED,WALLZOOMED};

struct	monitors
{
	int		monW=0;
	int		monH=0;
	int		monX=0;
	int		monY=0;
	int		monMode;
	char		*monitorPath=NULL;
};

char			*wallpaperPath=NULL;

Imlib_Image	buffer;

Display		*display;
Window		rootWin;
int			displayWidth;
int			displayHeight;
GC			gc;

Visual*		visual=NULL;
int			depth=0;
int			screen;
Pixmap		backDropPixmap;

int			mainColour=0x808080;
char			*mainColourStr=NULL;

int			backdropMode=STRETCH;
char			*prefsPath;
char			*monitorPrefs;

char*		monitorRCPath;
int			numberOfMonitors=0;
int			currentMonitor;
bool			multiMode=false;

LFSTK_lib 	*lib;
monitors		*monitorData;

struct option long_options[] =
{
	{"wallpaper",1,0,'w'},
	{"colour",1,0,'c'},
	{"mainmode",1,0,'a'},
	{"monitormode",1,0,'o'},
	{"monitor",1,0,'m'},
	{"multimode",1,0,'y'},
	{"version",0,0,'v'},
	{"help",0,0,'?'},
	{0,0,0,0}
};

void printhelp(void)
{
	printf("Usage: lfssetwallpaper [OPTION]\n"
	       "Set the desktop wallpaper for LFSDesktop\n"
	       " -w,--wallpaper	/path/to/wallpaper ARG\n"
	       " -c,--colour		Background colour N (decimal or hex)\n"
	       " -a,--mainmode		Main Mode (Stretch=0,Tile=1,Centre=2,Scaled=3,Zoomed=4) N\n"
	       " -o,--monitormode	Mode for individual monitors (as above) N\n"
	       " -m,--monitor		Monitor number N\n"
	       " -y,--multimode		Multi monitor image mode 0=Single image,1=Multi image N\n"
	       " -v,--version		output version information and exit\n"
	       " -h,-?,--help		print this help\n\n"
	       "Report bugs to keithdhedger@gmail.com\n"
	      );
}

void loadMonitorData(void)
{
	FILE		*fd=NULL;
	char		buffer[2048];
	int		monnum=0;

	fd=fopen(monitorRCPath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2047,fd);
					if(buffer[0]!=0)
						{
							buffer[strlen(buffer)-1]=0;
							monitorData[monnum].monMode=atoi(buffer);
							buffer[0]=0;
							fgets(buffer,2048,fd);
							buffer[strlen(buffer)-1]=0;
							monitorData[monnum].monitorPath=strdup(buffer);
							monnum++;
						}
				}
			fclose(fd);
		}
}

void blitToMonitorPos(int mode,const char* path,int x,int y,int w,int h)
{
	Imlib_Image	image;
	int			imagew,imageh;
	int			xoffset,yoffset;
	int			numx,numy;
	float		ratio=1.0;
	int			finalw,finalh;
	int			maxdim;
	float		wratio,hratio;

	imlib_context_set_blend(1);

	image=imlib_load_image_immediately_without_cache(path);
	if(image==NULL)
		return;
	imlib_context_set_image(image);
	imagew=imlib_image_get_width();
	imageh=imlib_image_get_height();

	switch(mode)
		{
		case WALLSTRETCH:
			imlib_context_set_image(buffer);
			imlib_context_set_cliprect(x,y,w,h);
			imlib_blend_image_onto_image(image,0,0,0,imagew,imageh,x,y,w,h);
			break;

		case WALLTILE:
			numx=(w/imagew)+1;
			numy=(h/imageh)+1;
			imlib_context_set_image(buffer);
			imlib_context_set_cliprect(x,y,w,h);
			for(int yy=0; yy<numy; yy++)
				{
					for(int xx=0; xx<numx; xx++)
						imlib_blend_image_onto_image(image,0,0,0,imagew,imageh,(xx*imagew)+x,(yy*imageh)+y,imagew,imageh);
				}
			break;

		case WALLCENTRE:
			xoffset=(w/2)-(imagew/2);
			yoffset=(h/2)-(imageh/2);

			imlib_context_set_image(buffer);
			imlib_context_set_cliprect(x,y,w,h);
			imlib_blend_image_onto_image(image,0,0,0,imagew,imageh,xoffset+x,yoffset+y,imagew,imageh);
			break;

		case WALLSCALED:
			if(imagew>imageh)
				{
					maxdim=w;
					ratio=((float)maxdim/imagew);
					finalw=w;
					finalh=imageh*ratio;
					xoffset=0;
					yoffset=(h/2)-(finalh/2);
				}
			else
				{
					maxdim=h;
					ratio=((float)maxdim/imageh);
					finalh=h;
					finalw=imagew*ratio;
					xoffset=(w/2)-(finalw/2);
					yoffset=0;
				}
			imlib_context_set_image(buffer);
			imlib_context_set_cliprect(x,y,w,h);
			imlib_blend_image_onto_image(image,0,0,0,imagew,imageh,x+xoffset,y+yoffset,finalw,finalh);
			break;

		case WALLZOOMED:
			wratio=((float)w/imagew);
			hratio=((float)h/imageh);
			if(wratio>hratio)
				ratio=wratio;
			else
				ratio=hratio;
			finalh=imageh*ratio;
			finalw=imagew*ratio;

			xoffset=(w/2)-(finalw/2);
			yoffset=(h/2)-(finalh/2);
			imlib_context_set_image(buffer);
			imlib_context_set_cliprect(x,y,w,h);
			imlib_blend_image_onto_image(image,0,0,0,imagew,imageh,x+xoffset,y+yoffset,finalw,finalh);
			break;
		}
	imlib_context_set_image(image);
	imlib_free_image();
}

void initBackdrop(void)
{
	Imlib_Image		image;
	int				imagew,imageh;

	imlib_context_set_display(display);
	imlib_context_set_visual(visual);
	imlib_context_set_blend(1);
	buffer=imlib_create_image(displayWidth,displayHeight);
	imlib_context_set_image(buffer);
	imlib_context_set_color((mainColour/65536),(mainColour/256) & 0xff,mainColour & 0xff,0xff);
	imlib_image_fill_rectangle(0,0,displayWidth,displayHeight);

	if((wallpaperPath!=NULL) && (strcmp(wallpaperPath,"\"\"")!=0))
		{
			if(multiMode==false)
				{
					blitToMonitorPos(backdropMode,wallpaperPath,0,0,displayWidth,displayHeight);
				}
			else
				{
					for(int j=0;j<numberOfMonitors;j++)
						{
							if((monitorData[j].monW!=-1) && (monitorData[j].monitorPath!=NULL))
								{
									blitToMonitorPos(monitorData[j].monMode,monitorData[j].monitorPath,monitorData[j].monX,monitorData[j].monY,monitorData[j].monW,monitorData[j].monH);
								}	
						}
				}
		}

	imlib_context_set_image(buffer);
	imlib_context_set_drawable(rootWin);
	imlib_image_set_has_alpha(0);
	imlib_render_pixmaps_for_whole_image(&backDropPixmap,NULL);
	imlib_free_image();
}

void savePrefs(void)
{
	FILE				*fd=NULL;
	LFSTK_prefsClass	prefs;

	prefs.prefsMap=
		{
			{LFSTK_UtilityClass::LFSTK_hashFromKey("backdrop"),{TYPESTRING,"backdrop","",wallpaperPath,false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("colour"),{TYPESTRING,"colour","",mainColourStr,false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("mainmode"),{TYPEINT,"mainmode","","",false,backdropMode}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("multimode"),{TYPEBOOL,"multimode","","",multiMode,0}},
		};
	prefs.LFSTK_saveVarsToFile(prefsPath);

	fd=fopen(monitorRCPath,"w");
	if(fd!=NULL)
		{
			for(long j=0;j<numberOfMonitors;j++)
				fprintf(fd,"%i\n%s\n",monitorData[j].monMode,monitorData[j].monitorPath);
			fclose(fd);
		}
}

void loadPrefs(void)
{
	LFSTK_prefsClass	prefs;

	prefs.prefsMap=
		{
			{LFSTK_UtilityClass::LFSTK_hashFromKey("backdrop"),{TYPESTRING,"backdrop","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("mainmode"),{TYPEINT,"mainmode","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("colour"),{TYPESTRING,"colour","","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("multimode"),{TYPEBOOL,"multimode","","",false,0}}
		};

	prefs.LFSTK_loadVarsFromFile(prefsPath);
	wallpaperPath=strdup(prefs.LFSTK_getCString("backdrop"));
	backdropMode=prefs.LFSTK_getInt("mainmode");
	mainColourStr=strdup(prefs.LFSTK_getCString("colour"));
	mainColour=lib->LFSTK_getColourFromName(display,DefaultColormap(display,DefaultScreen(display)),prefs.LFSTK_getCString("colour"));
	multiMode=prefs.LFSTK_getBool("multimode");
}

int main(int argc,char **argv)
{
	Window				parentWindow;
	XEvent				ev;
	int					cnt=-1;
	XineramaScreenInfo	*p=NULL;
	int c;
	char				*command;

	lib=new LFSTK_lib(true);

	asprintf(&prefsPath,"%s/.config/LFS/lfssetwallpaper.rc",getenv("HOME"));
	asprintf(&command,"mkdir -vp %s/.config/LFS/ 2>&1 >/dev/null",getenv("HOME"));
	system(command);
	free(command);
	asprintf(&monitorRCPath,"%s/.config/LFS/lfsmonitors.rc",getenv("HOME"));

	backdropMode=STRETCH;
	multiMode=false;

	display=XOpenDisplay(NULL);
	if(display==NULL)
		exit(1);

	screen=DefaultScreen(display);
	displayWidth=DisplayWidth(display,screen);
	displayHeight=DisplayHeight(display,screen);
	screen=DefaultScreen(display);
	rootWin=DefaultRootWindow(display);
	visual=DefaultVisual(display,screen);
	gc=XCreateGC(display,rootWin,0,NULL);

	XSetFillStyle(display,gc,FillSolid);
	XSelectInput(display,rootWin,ExposureMask | SubstructureNotifyMask);

	cnt=ScreenCount(display);
	p=XineramaQueryScreens(display,&cnt);
	if(p!=NULL)
		{
			if(cnt>0)
				{
					monitorData=(monitors*)malloc(sizeof(monitors)*cnt);
					numberOfMonitors=cnt;

					for (int x=0; x<cnt; x++)
						{
							monitorData[x].monW=p[x].width;
							monitorData[x].monH=p[x].height;
							monitorData[x].monX=p[x].x_org;
							monitorData[x].monY=p[x].y_org;
							monitorData[x].monitorPath=NULL;
						}
				}
			free(p);
		}

	loadPrefs();	
	currentMonitor=-1;
	loadMonitorData();

	while (1)
		{
			int option_index = 0;
			c = getopt_long (argc,argv,"w:o:m:y:a:c:v?h",long_options,&option_index);
			if (c == -1)
				break;

			switch (c)
				{
				case 'w':
					if(currentMonitor==-1)
						{
							if(wallpaperPath!=NULL)
								free(wallpaperPath);
							wallpaperPath=strdup(optarg);
						}
					else
						{
							if(monitorData[currentMonitor].monitorPath!=NULL)
								free(monitorData[currentMonitor].monitorPath);
							monitorData[currentMonitor].monitorPath=strdup(optarg);
						}
					break;

				case 'c':
					if(mainColourStr!=NULL)
						free(mainColourStr);
					if(optarg[0]=='0' && optarg[1]=='x')
						{
							
							mainColour=strtol(&optarg[2],NULL,16);
							asprintf(&mainColourStr,"#%s",&optarg[2]);
							break;
						}
					if(optarg[0]=='#')
						{
							mainColour=strtol(&optarg[1],NULL,16);
							asprintf(&mainColourStr,"%s",optarg);
							break;
						}
					mainColour=strtol(optarg,NULL,0);
					mainColourStr=strdup(optarg);
					break;

				case 'o':
					monitorData[currentMonitor].monMode=atoi(optarg);
				break;

				case 'a':
					backdropMode=atoi(optarg);
					break;

				case 'm':
					currentMonitor=atoi(optarg);
					break;

				case 'y':
					multiMode=(bool)atoi(optarg);
					break;

				case 'v':
					printf("setlfswalpaper %s\n",VERSION);
					return 0;
					break;

				case '?':
				case 'h':
					printhelp();
					return 0;
					break;

				default:
					fprintf(stderr,"?? Unknown argument ??\n");
					return UNKNOWNARG;
					break;
				}
		}

	initBackdrop();

	XChangeProperty(display,rootWin,XInternAtom(display,"_XROOTPMAP_ID",false),XA_PIXMAP,32,PropModeReplace,(const unsigned char*)&backDropPixmap,1);
	XChangeProperty(display,rootWin,XInternAtom(display,"ESETROOT_PMAP_ID",false),XA_PIXMAP,32,PropModeReplace,(const unsigned char*)&backDropPixmap,1);

	XSetCloseDownMode(display,RetainPermanent);
	XSetWindowBackgroundPixmap(display,rootWin,backDropPixmap);
	XClearWindow(display,rootWin);
	XFlush(display);

	savePrefs();

	for(int j=0;j<numberOfMonitors;j++)
		{
			if(monitorData[j].monitorPath!=NULL)
				free(monitorData[j].monitorPath);
		}

	free(monitorRCPath);
	free(prefsPath);
	free(wallpaperPath);
	freeAndNull(&mainColourStr);
	fflush(NULL);
	delete lib;

	return(0);
}

