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

#include "config.h"

#define UNKNOWNARG -100

enum imageMode {STRETCH=0,TILE,CENTRE,SCALED,ZOOMED};
enum {TYPEINT=1,TYPESTRING,TYPEBOOL};

struct args
{
	const char	*name;
	int			type;
	void*		data;
};

struct	monitors
{
	int		monW;
	int		monH;
	int		monX;
	int		monY;
	int		monMode;
	char	*monitorPath;
};

char		*wallpaperPath=NULL;

Imlib_Image	buffer;

Display*	display;
Window		rootWin;
int			displayWidth;
int			displayHeight;
GC			gc;

Visual*		visual=NULL;
int			depth=0;
int			screen;
Pixmap		backDropPixmap;

int			mainColour=0x808080;

int			backdropMode=STRETCH;
char		*prefsPath;
char*		monitorRCPath;
int			numberOfMonitors=0;
int			currentMonitor;
bool		multiMode=false;

args		*lfsmonitors_rc;
monitors	*monitorData;

args		lfsdesktop_rc[]=
{
	{"backdrop",TYPESTRING,&wallpaperPath},
	{"mainmode",TYPEINT,&backdropMode},
	{"colour",TYPEINT,&mainColour},
	{"multimode",TYPEBOOL,&multiMode},
	{NULL,0,NULL}
};

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

void loadVarsFromFile(char* filepath,args* dataptr)
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
		}
}

void loadMonitorData(void)
{
	FILE*	fd=NULL;
	char	buffer[2048];
	int		monnum=0;

	fd=fopen(monitorRCPath,"r");
	if(fd!=NULL)
		{
			while(feof(fd)==0)
				{
					buffer[0]=0;
					fgets(buffer,2048,fd);
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

void blitToMonitorPos(int mode,char* path,int x,int y,int w,int h)
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
		case STRETCH:
			imlib_context_set_image(buffer);
			imlib_context_set_cliprect(x,y,w,h);
			imlib_blend_image_onto_image(image,0,0,0,imagew,imageh,x,y,w,h);
			break;

		case TILE:
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

		case CENTRE:
			xoffset=(w/2)-(imagew/2);
			yoffset=(h/2)-(imageh/2);

			imlib_context_set_image(buffer);
			imlib_context_set_cliprect(x,y,w,h);
			imlib_blend_image_onto_image(image,0,0,0,imagew,imageh,xoffset+x,yoffset+y,imagew,imageh);
			break;

		case SCALED:
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

		case ZOOMED:
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

	imlib_context_set_image(buffer);
	imlib_context_set_drawable(rootWin);
	imlib_image_set_has_alpha(0);
	imlib_render_pixmaps_for_whole_image(&backDropPixmap,NULL);
	imlib_free_image();
}

int main(int argc,char **argv)
{
	Window				parentWindow;
	XEvent				ev;
	int					cnt=-1;
	XineramaScreenInfo	*p=NULL;
	int c;
	char				*command;

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
					lfsmonitors_rc=(args*)malloc(sizeof(args)*cnt*2);
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
	
	loadVarsFromFile(prefsPath,lfsdesktop_rc);
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
						wallpaperPath=optarg;
					else
						monitorData[currentMonitor].monitorPath=strdup(optarg);
					break;

				case 'c':
					mainColour=strtol(optarg,NULL,0);
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
	saveVarsToFile(prefsPath,lfsdesktop_rc);

	char	*buf;
	int		mn=0;
	asprintf(&buf,":>%s",monitorRCPath);
	system(buf);
	free(buf);
	fflush(NULL);
	for(int j=0;j<numberOfMonitors;j++)
		{
			asprintf(&buf,"echo -en \"%i\n\"%s\"\n\" >>%s",monitorData[j].monMode,monitorData[j].monitorPath,monitorRCPath);
			system(buf);
			free(buf);
			fflush(NULL);
		}
	free(monitorRCPath);
	free(prefsPath);
	fflush(NULL);

	return(0);
}

