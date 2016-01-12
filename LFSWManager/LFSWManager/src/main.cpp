/*
 * Copyright 2010 Johan Veenhuizen
 *
 * Permission is hereby granted,free of charge,to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction,including without limitation
 * the rights to use,copy,modify,merge,publish,distribute,sublicense,
 * and/or sell copies of the Software,and to permit persons to whom the
 * Software is furnished to do so,subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS",WITHOUT WARRANTY OF ANY KIND,EXPRESS OR
 * IMPLIED,INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,DAMAGES OR
 * OTHER LIABILITY,WHETHER IN AN ACTION OF CONTRACT,TORT OR OTHERWISE,
 * ARISING FROM,OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*
	Thanks to Johan for the original code available here:
	http://sourceforge.net/projects/windwm/?source=navbar

	Changes/additions
	©keithhedger Tue 23 Jun 09:56:25 BST 2015 kdhedger68713@gmail.com

	Extra code released under GPL3

*/

#include <errno.h>
#include <locale.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/select.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xinerama.h>

#include <Imlib2.h>

#include "wind.h"
#include "x11font.h"
#include "list.h"
#include "lib.h"
#include "root.h"
#include "ewmh.h"
#include "client.h"
#include "mwm.h"
#include "prefs.h"
#include "button.h"
#include "atoms.h"
#include "appmenu.h"

#include "config.h"

#include "deleven.xbm"
#include "delodd.xbm"
#include "maxeven.xbm"
#include "maxodd.xbm"
#include "mineven.xbm"
#include "minodd.xbm"
#include "shadeeven.xbm"
#include "shadeodd.xbm"

DEFINE_BITMAP(deleven);
DEFINE_BITMAP(delodd);
DEFINE_BITMAP(maxeven);
DEFINE_BITMAP(maxodd);
DEFINE_BITMAP(mineven);
DEFINE_BITMAP(minodd);
DEFINE_BITMAP(shadeeven);
DEFINE_BITMAP(shadeodd);

enum			runlevel runlevel=RL_STARTUP;
int				exitstatus;

/*
 * If true,enable debug mode. This will enable synchronous
 * X11 transactions,and print Xlib errors to standard error.
 */
//Bool			debug=False;

// The display name used in call to XOpenDisplay
const char		*displayname=NULL;

// The last X error reported
const char		*xerror=NULL;

Window 			root;

sigset_t		sigmask;

int errhandler(Display *dpy,XErrorEvent *e)
{
#if _DEBUGLEVEL_ == DBG1
	char buf[128];

	if(xLibWarnings==true)
		{
			lastcheckpoint(">>>>>>>>>>>>>>>>>>>>>>>");
			buf[0]='\0';
			XGetErrorText(dpy,e->error_code,buf,sizeof buf);
			errorf("Xlib: %s",buf);
			xerror=buf;
			printf("<<<<<<<<<<<<<<<<<<<<<<<\n");
		}
#endif
	return 0;
}

void onsignal(int signal)
{
#if _DEBUGLEVEL_ == DBG1
	FILE*	fp=NULL;

	fp=fopen("/tmp/lfswmanager.error.log","a");

	fprintf(stderr,"Traped signal %s\n",strsignal(signal));
	fprintf(stderr,"File: %s, Function: %s, Line: %i\n",errFile,errFunc,errLine);

	if(fp!=NULL)
		{
			fprintf(fp,"Traped signal %s\n",strsignal(signal));
			fprintf(fp,"File: %s, Function: %s, Line: %i\n",errFile,errFunc,errLine);
		}

	if(fp!=NULL)
		fclose(fp);
#endif

	if(signal==SIGSEGV)
		exit(EXIT_FAILURE);
	if(signal==SIGTERM)
		exit(EXIT_SUCCESS);
}

int waitevent(void)
{
	CHECKPOINT

	if (XPending(dpy)>0)
		return 0;
	fd_set rfds;
	int nfds=0;
	FD_ZERO(&rfds);

	int conn=ConnectionNumber(dpy);
	FD_SET(conn,&rfds);
	nfds=MAX(conn+1,nfds);

	if (pselect(nfds,&rfds,NULL,NULL,NULL,&sigmask)==-1)
		{
			errorf("pselect: %s",strerror(errno));
			exitstatus=1;
			return -1;
		}
	else if (FD_ISSET(conn,&rfds))
		{
			// Normal X event
			return 0;
		}
	else
		{
			// Shouldn't happen
			errorf("BUG: unhandled pselect condition");
			exitstatus=1;
			return -1;
		}
}

void usage(FILE *f)
{
	fprintf(f,"%s Version %s\nUsage: %s [[ OPT ] ... [ OPT ]] [ DISPLAY ]\n"
	        "-n	number		Number of desktops\n"
	        "-t	font		Title font\n"
	        "-B	colour		Active fill colour\n"
	        "-b	colour		Inactive fill colour\n"
	        "-F	colour		Active frame colour\n"
	        "-f	colour		Inactive frame colour\n"
			"-X	colour		Button and title colour\n"
	        "-p	placement	New window placement (0=Smart( Screen ), 1=Under mouse ,2=Centre on monitor with mouse( default ), 3=Screen centre, 4=Smart( Monitor with mouse ) )\n"
			"-l	updates		Live update of window when resizing ( >0=Live ( default=5 - slowest=1, faster/less updates >1 ), 0=Update window on relase of mouse button - fastest\n"
			"-T	theme		Name of installed theme to use\n"
			"-x	command		Set terminal command ( default xterm -e )\n"
			"-w	outfile		After setting prefs from default, prefsfile and command line write out a prefsfile and quit ( MUST be last option ).\n"
	        ,progname,VERSION,progname);
}

const char	*themePartNames[]=
{
	"top-left-active",
	"top-left-inactive",
	"top-right-active",
	"top-right-inactive",
	"title-1-active",
	"title-1-inactive",
	"title-2-active",
	"title-2-inactive",
	"title-3-active",
	"title-3-inactive",
	"title-4-active",
	"title-4-inactive",
	"title-5-active",
	"title-5-inactive",
	"left-active",
	"left-inactive",
	"right-active",
	"right-inactive",
	"bottom-left-active",
	"bottom-left-inactive",
	"bottom-active",
	"bottom-inactive",
	"bottom-right-active",
	"bottom-right-inactive",
	"close-active",
	"close-inactive",
	"close-prelight",
	"close-pressed",
	"maximize-active",
	"maximize-inactive",
	"maximize-prelight",
	"maximize-pressed",
	"hide-active",
	"hide-inactive",
	"hide-prelight",
	"hide-pressed",
	"shade-active",
	"shade-inactive",
	"shade-prelight",
	"shade-pressed",
	
	NULL
};

void loadTheme(void)
{
	CHECKPOINT

	char		buffer[2048];
	int			partcnt=0;
	int			hite=0;
	Imlib_Image	image;

	imlib_context_set_dither(0);
	imlib_context_set_display(dpy);
	imlib_context_set_visual(visual);

	while(themePartNames[partcnt]!=NULL)
		{
			theme.gotPart[partcnt]=false;
			sprintf(buffer,"%s/xfwm4/%s.png",theme.pathToTheme,themePartNames[partcnt]);
			image=imlib_load_image(buffer);
			if(image==NULL)
				{
					sprintf(buffer,"%s/xfwm4/%s.xpm",theme.pathToTheme,themePartNames[partcnt]);
					image=imlib_load_image(buffer);
				}

			if(image!=NULL)
				{
					theme.gotPart[partcnt]=true;
					imlib_context_set_image(image);

					hite=imlib_image_get_height();
					theme.partsWidth[partcnt]=imlib_image_get_width();
					if(hite>theme.titleBarHeight)
						theme.titleBarHeight=hite;
					theme.partsHeight[partcnt]=hite;

					imlib_context_set_drawable(root);
					imlib_image_set_has_alpha(1);
					imlib_render_pixmaps_for_whole_image(&theme.pixmaps[partcnt],&theme.masks[partcnt]);
					imlib_free_image();
				}
			partcnt++;
		}

	theme.leftWidth=theme.partsWidth[LEFTACTIVE];
	theme.rightWidth=theme.partsWidth[RIGHTACTIVE];
	theme.titleBarHeight=theme.partsHeight[TITLE3ACTIVE];
	theme.bottomHeight=theme.partsHeight[BOTTOMACTIVE];

//button correction for incomplete themes
	for(int j=0;j<4;j++)
		{
			int off=j*4;
			if((theme.gotPart[CLOSEPRELIGHT+off]==false) && (theme.gotPart[CLOSEACTIVE+off]==true))
				{
					theme.pixmaps[CLOSEPRELIGHT+off]=theme.pixmaps[CLOSEACTIVE+off];
					theme.masks[CLOSEPRELIGHT+off]=theme.masks[CLOSEACTIVE+off];
					theme.partsWidth[CLOSEPRELIGHT+off]=theme.partsWidth[CLOSEACTIVE+off];
					theme.partsHeight[CLOSEPRELIGHT+off]=theme.partsHeight[CLOSEACTIVE+off];
					theme.gotPart[CLOSEPRELIGHT+off]=true;
				}
		}
}

void makeFullPathToTheme(void)
{
	char	*buffer;
//no theme set
	if(theme.pathToTheme==NULL)
		return;
//full path to theme already set
	if(theme.pathToTheme[0]=='/')
		return;

//search home themes first
	asprintf(&buffer,"%s/.themes/%s",getenv("HOME"),theme.pathToTheme);
	if(fileExists(buffer)==0)
		{
			free(theme.pathToTheme);
			theme.pathToTheme=buffer;
			return;
		}

//search global themes
	free(buffer);
	asprintf(&buffer,"/usr/share/themes/%s",theme.pathToTheme);
	if(fileExists(buffer)==0)
		{
			free(theme.pathToTheme);
			theme.pathToTheme=buffer;
			return;
		}
	free(buffer);
}

int main(int argc,char *argv[])
{
	CHECKPOINT

	int					cnt=-1;
	XineramaScreenInfo	*p=NULL;
	char				*prefsfile;
	int					opt;
	progname=argv[0];

	setlocale(LC_CTYPE,"");

	srand((unsigned)time(NULL));

	runlevel=RL_STARTUP;

	fontColours[ACTIVEFRAME]=strdup("rgb:00/00/00");
	fontColours[ACTIVEFRAMEFILL]=strdup("rgb:00/ff/ff");
	fontColours[INACTIVEFRAME]=strdup("rgb:00/00/00");
	fontColours[INACTIVEFRAMEFILL]=strdup("rgb:ff/ff/ff");
	fontColours[TEXTCOLOUR]=strdup("rgb:ff/ff/ff");

	Desk ndesk=0;
	numberOfDesktops=4;
	placement=CENTREMMONITOR;
	titleFont=strdup(DEFAULTFONT);
	liveUpdate=5;
	theme.pathToTheme=NULL;

	asprintf(&prefsfile,"%s/.config/LFS/lfswmanager.rc",getenv("HOME"));
	asprintf(&terminalCommand,"xterm -e ");

	loadVarsFromFile(prefsfile,wmPrefs," ");
	makeFullPathToTheme();

	free(prefsfile);

	ndesk=numberOfDesktops;

	XSetErrorHandler(errhandler);

	if ((dpy=XOpenDisplay(displayname))==NULL)
		{
			errorf("cannot open display \"%s\"",XDisplayName(displayname));
			exit(1);
		}
	XSynchronize(dpy,xLibWarnings);

	screen=DefaultScreen(dpy);
	displayWidth=DisplayWidth(dpy,screen);
	displayHeight=DisplayHeight(dpy,screen);

	root=DefaultRootWindow(dpy);
	visual=DefaultVisual(dpy,screen);
	blackColor=BlackPixel(dpy,screen);
	whiteColor=WhitePixel(dpy,screen);
	mainGC=XCreateGC(dpy,root,0,NULL);

	cnt=ScreenCount(dpy);
	p=XineramaQueryScreens(dpy,&cnt);
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
						}
				}
			free(p);
		}

	if((fileExists(theme.pathToTheme)==0) && (theme.pathToTheme!=NULL))
		{
			char	*themercpath;
			theme.buttonOffset=40;
			theme.useTheme=true;
			theme.titleOffset=0;
			loadTheme();
			asprintf(&themercpath,"%s/xfwm4/themerc",theme.pathToTheme);
			loadVarsFromFile(themercpath,themeRC,"=");
			free(themercpath);
		}
	else
		theme.useTheme=false;

	while ((opt=getopt(argc,argv,"?hp:B:b:F:f:X:n:t:l:T:w:x:")) != -1)
		switch (opt)
			{
			case 'B':
				free(fontColours[ACTIVEFRAMEFILL]);
				fontColours[ACTIVEFRAMEFILL]=strdup(optarg);
				break;

			case 'b':
				free(fontColours[INACTIVEFRAMEFILL]);
				fontColours[INACTIVEFRAMEFILL]=strdup(optarg);
				break;
			case 'F':
				free(fontColours[ACTIVEFRAME]);
				fontColours[ACTIVEFRAME]=strdup(optarg);
				break;
			case 'f':
				free(fontColours[INACTIVEFRAME]);
				fontColours[INACTIVEFRAME]=strdup(optarg);
				break;
			case 'X':
				free(fontColours[TEXTCOLOUR]);
				fontColours[TEXTCOLOUR]=strdup(optarg);
				break;

			case 'n':
				{
					errno=0;
					char *p;
					long n=strtol(optarg,&p,10);
					if ((n<0) || (errno != 0) || (*optarg=='\0') || (*p != '\0'))
						{
							errorf("%s: invalid desktop count",optarg);
							exit(1);
						}
					ndesk=n;
				}
				break;
			case 't':
				free(titleFont);
				titleFont=strdup(optarg);
				break;
			case 'p':
				placement=atoi(optarg);
				break;

			case 'l':
				liveUpdate=atoi(optarg);
				break;
			case 'T':
				break;
			case 'w':
				saveVarsToFile(optarg,wmPrefs," ");
				exit(0);
				break;
			case 'x':
				if(terminalCommand!=NULL)
					free(terminalCommand);
				terminalCommand=strdup(optarg);
				break;

			default:
				usage(stderr);
				exit(1);
			}

	if (optind<argc)
		displayname=argv[optind++];

	if (optind<argc)
		{
			errorf("unexpected argument -- %s",argv[optind]);
			usage(stderr);
			exit(1);
		}

#if _DEBUGLEVEL_ == DBG1
	xLibWarnings=true;
	fprintf(stderr,"%s\n",PACKAGE_STRING);
	fprintf(stderr,"Synchronous DEBUG mode enabled. Printing Xlib errors on standard error.\n");
	fprintf(stderr,"Report bugs to <kdhedger68713@gmail.com>.\n");
#else
	xLibWarnings=false;
#endif

	font=ftload(titleFont);
	if (font==NULL)
		{
			errorf("cannot load font");
			exit(1);
		}

	if(theme.useTheme==false)
		{
			frameTop=20;
			frameBottom=8;
			frameLeft=2+1;
			frameRight=2+1;
		}
	else
		{
			frameTop=theme.titleBarHeight;
			frameBottom=theme.bottomHeight;
			frameLeft=theme.leftWidth;
			frameRight=theme.rightWidth;
		}

	fnormal=ftloadcolor(fontColours[INACTIVEFRAME],"#808080");
	fhighlight=ftloadcolor(fontColours[TEXTCOLOUR],"#ffffff");

	if (fnormal==NULL || fhighlight==NULL)
		{
			errorf("cannot load font colors");
			exit(1);
		}

	if (frameTop % 2==0)
		deletebitmap=&deleven;
	else
		deletebitmap=&delodd;

	if (frameTop % 2==0)
		maximizeBitmap=&maxeven;
	else
		maximizeBitmap=&maxodd;

	if (frameTop % 2==0)
		minimizeBitmap=&mineven;
	else
		minimizeBitmap=&minodd;

	if (frameTop % 2==0)
		shadeBitmap=&shadeeven;
	else
		shadeBitmap=&shadeodd;

	activeFrame=getpixel(fontColours[ACTIVEFRAME]);
	activeFrameFill=getpixel(fontColours[ACTIVEFRAMEFILL]);
	inactiveFrame=getpixel(fontColours[INACTIVEFRAME]);
	inactiveFrameFill=getpixel(fontColours[INACTIVEFRAMEFILL]);
	widgetColour=getpixel(fontColours[TEXTCOLOUR]);

	XGCValues	gcv;
	gcv.foreground=whiteColor;
	gcv.background=inactiveFrameFill;
	activeGC=XCreateGC(dpy,root,GCForeground | GCBackground,&gcv);

	listeners=XUniqueContext();

	struct sigaction sa;
	sigfillset(&sa.sa_mask);
	sa.sa_flags=SA_RESTART;
	sa.sa_handler=onsignal;

	sigaction(SIGHUP,&sa,NULL);
	sigaction(SIGINT,&sa,NULL);
	sigaction(SIGTERM,&sa,NULL);
	sigaction(SIGSEGV,&sa,NULL);

	WM_CHANGE_STATE=XInternAtom(dpy,"WM_CHANGE_STATE",False);
	WM_DELETE_WINDOW=XInternAtom(dpy,"WM_DELETE_WINDOW",False);
	WM_PROTOCOLS=XInternAtom(dpy,"WM_PROTOCOLS",False);
	WM_STATE=XInternAtom(dpy,"WM_STATE",False);

	initroot();
	ewmh_startwm();
	mwm_startwm();

	if (ndesk != 0)
		setndesk(ndesk);

	Cursor curs=XCreateFontCursor(dpy,XC_left_ptr);
	XDefineCursor(dpy,root,curs);

	XSetInputFocus(dpy,PointerRoot,RevertToPointerRoot,CurrentTime);
	manageall();

	refocus(CurrentTime);
	runlevel=RL_NORMAL;

	CHECKPOINT
	Window			root_return;
	Window			child_return;
	int				root_x_return;
	int				root_y_return;
	int				win_x_return;
	int				win_y_return;
	unsigned int	mask_return;
#define MAXBUFFER 512
	char			buffer[MAXBUFFER];

	while (waitevent() != -1)
		{
			XEvent e;
			XNextEvent(dpy,&e);
			if(XQueryPointer(dpy,DefaultRootWindow(dpy),&root_return,&child_return,&root_x_return,&root_y_return,&win_x_return,&win_y_return, &mask_return)==true)
					{
					
						if(mask_return & Button3Mask)
							{
								if(child_return==0)
									{
#if 0
									if (redirect(&e,e.xany.window)==-1)
													{
														if (e.type==ClientMessage)
															redirect(&e,root);
													}
									bool popupLoop=true;
									XEvent	evente;

									printf(">>>>here\n");
										//appWindow->LFSTK_moveWindow(root_x_return,root_y_return,true);
										appWindow->LFSTK_moveWindow(100,100,true);
										appWindow->LFSTK_resizeWindow(800,600,true);
										appWindow->LFSTK_clearWindow();
										appWindow->LFSTK_showWindow(true);
										appWindow->LFSTK_hideWindow();
										appWindow->LFSTK_showWindow(true);
										refreshMenu();
										while(popupLoop==true)
											{
											

												XNextEvent(dpy,&evente);
												printf("----------\n");
			//	{
					/*
					 * EWMH specifies some root window client
					 * messages with a non-root event window,
					 * so we need to redirect those manually.
					 */

			//		
			//	}
//												listener *l=appWindow->LFSTK_getListener(evente.xany.window);
//
//												if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
//													l->function(l->pointer,&evente,l->type);

			if((evente.xany.window==appWindow->window) ||(evente.xany.window==catButtons[0]->LFSTK_getWindow()))
			//if(redirect(&evente,evente.xany.window)==-1)
		//	{
		//												if (evente.type==ClientMessage)
		//													redirect(&evente,root);
		//	}
		//	else
			{
printf(">>winid=%p<<\n",evente.xany.window);

												listener *l=appWindow->LFSTK_getListener(evente.xany.window);

												if((l!=NULL) && (l->pointer!=NULL) && (l->function!=NULL) )
													l->function(l->pointer,&evente,l->type);
												switch(evente.type)
													{
														case ClientMessage:
														appWindow->LFSTK_clearWindow();
															//appWindow->LFSTK_showWindow(true);
															refreshMenu();
														//	if (redirect(&evente,evente.xany.window)==-1)
														//		redirect(&evente,root);
															break;
														case LeaveNotify:
															//if(evente.xany.window==appWindow->window)
															//	popupLoop=false;
															break;
														case Expose:
														printf("expose\n");
															appWindow->LFSTK_clearWindow();
															//appWindow->LFSTK_showWindow(true);
															refreshMenu();
															break;
														case ConfigureNotify:
														printf("config\n");
															appWindow->LFSTK_resizeWindow(evente.xconfigurerequest.width,evente.xconfigurerequest.height);
															appWindow->LFSTK_clearWindow();
															//appWindow->LFSTK_showWindow(true);
															refreshMenu();
															break;
													}
													}
													else
													{
												if (redirect(&evente,evente.xany.window)==-1)
													{
														if (evente.type==ClientMessage)
															redirect(&evente,evente.xany.window);
													}
}
											}
										appWindow->LFSTK_hideWindow();
#endif
										sprintf(buffer,"%s \"%s\"",MAINMENUAPP,terminalCommand);
										system(buffer);
									}
							}
					}
			if (redirect(&e,e.xany.window)==-1)
				{
					/*
					 * EWMH specifies some root window client
					 * messages with a non-root event window,
					 * so we need to redirect those manually.
					 */

					if (e.type==ClientMessage)
						redirect(&e,root);
				}

		if(needrestack==true)
			{
				shuffle();
				restack();
			}
		}

	runlevel=RL_SHUTDOWN;

	// We make sure the focused window stays on top
	// when we map windows from other desktops,and
	// to warp the pointer so that focus is not lost.
	Window w=None;
	struct geometry g;
	struct client *c=getfocus();
	if (c != NULL)
		{
			cpopapp(c);
			restack();
			w=c->window;
			g=cgetgeom(c);
		}

	unmanageall();

	if (w != None)
		XWarpPointer(dpy,None,w,0,0,0,0,g.width / 2,g.height / 2);
	XSetInputFocus(dpy,PointerRoot,RevertToPointerRoot,CurrentTime);

	ewmh_stopwm();

	ftfreecolor(fnormal);
	ftfreecolor(fhighlight);
	ftfree(font);

	XFreeGC(dpy,activeGC);
	XCloseDisplay(dpy);

	free(terminalCommand);
	for(int j=0;j<TEXTCOLOUR+1;j++)
		free(fontColours[j]);

	return exitstatus;
}
