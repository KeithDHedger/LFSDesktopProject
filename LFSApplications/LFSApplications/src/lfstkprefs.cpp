/*
 *
 * ©K. D. Hedger. Sun 13 Aug 18:33:35 BST 2017 kdhedger68713@gmail.com

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

#include <getopt.h>
#include "lfstk/LFSTKGlobals.h"

#define EDITBOXWIDTH	GADGETWIDTH*2

LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass		*label=NULL;
LFSTK_labelClass		*personal=NULL;
LFSTK_labelClass		*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_buttonClass		*apply=NULL;
LFSTK_buttonClass		*test=NULL;

//preview buttons
LFSTK_buttonClass		*previewButtons[4]={NULL,};
LFSTK_lineEditClass		*previeBackColourEdit[4]={NULL,};
LFSTK_lineEditClass		*previeFontColourEdit[4]={NULL,};
const char				*previewButtonLabels[4]={"Button Normal","Button Prelight","Button Active","Button Inactive"};

//menu buttons
LFSTK_buttonClass		*previewMenus[4]={NULL,};
LFSTK_lineEditClass		*previeMenuBackColourEdit[4]={NULL,};
LFSTK_lineEditClass		*previeMenuFontColourEdit[4]={NULL,};
const char				*previewMenuLabels[4]={"Menu Normal","Menu Prelight","Menu Active","Menu Inactive"};

//buton font
LFSTK_fontDialogClass	*buttonFontDialogButton=NULL;
LFSTK_lineEditClass		*buttonFontEdit=NULL;

//menu font
LFSTK_fontDialogClass	*menuFontDialogButton=NULL;
LFSTK_lineEditClass		*menuFontEdit=NULL;

//window stuff
LFSTK_lineEditClass		*windowColourEdit=NULL;
LFSTK_toggleButtonClass	*autoColourCheck=NULL;

//themes
LFSTK_toggleButtonClass	*useTheme=NULL;
LFSTK_buttonClass		*windowTile=NULL;
LFSTK_buttonClass		*buttonTile=NULL;
LFSTK_buttonClass		*menuTile=NULL;
LFSTK_lineEditClass		*windowTileEdit=NULL;
LFSTK_lineEditClass		*buttonTileEdit=NULL;
LFSTK_lineEditClass		*menuTileEdit=NULL;
LFSTK_fileDialogClass	*tileDialog=NULL;

bool					mainLoop=true;
Display					*display;
char					*wd;
int						parentWindow=-1;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

void setVars(void)
{
	for(int j=0;j<4;j++)
		{
//buttons
			wc->globalLib->LFSTK_setGlobalString(j,TYPEBUTTON,previeBackColourEdit[j]->LFSTK_getBuffer()->c_str());
			wc->globalLib->LFSTK_setGlobalString(j,TYPEFONTCOLOUR,previeFontColourEdit[j]->LFSTK_getBuffer()->c_str());
//menus
			wc->globalLib->LFSTK_setGlobalString(j,TYPEMENUITEM,previeMenuBackColourEdit[j]->LFSTK_getBuffer()->c_str());
			wc->globalLib->LFSTK_setGlobalString(j,TYPEMENUITEMFONTCOLOUR,previeMenuFontColourEdit[j]->LFSTK_getBuffer()->c_str());
		}

	wc->globalLib->LFSTK_setGlobalString(0,TYPEWINDOW,windowColourEdit->LFSTK_getBuffer()->c_str());
	wc->globalLib->LFSTK_setAutoLabelColour(autoColourCheck->LFSTK_getValue());
	wc->globalLib->LFSTK_setUseTheme(useTheme->LFSTK_getValue());
//tiles
	wc->globalLib->LFSTK_setGlobalString(0,TYPEWINDOWTILE,windowTileEdit->LFSTK_getBuffer()->c_str());
	wc->globalLib->LFSTK_setGlobalString(0,TYPEBUTTONTILE,buttonTileEdit->LFSTK_getBuffer()->c_str());
	wc->globalLib->LFSTK_setGlobalString(0,TYPEMENUITEMTILE,menuTileEdit->LFSTK_getBuffer()->c_str());
//fonts
	wc->globalLib->LFSTK_setGlobalString(0,TYPEFONT,buttonFontEdit->LFSTK_getBuffer()->c_str());
	wc->globalLib->LFSTK_setGlobalString(0,TYPEMENUITEMFONT,menuFontEdit->LFSTK_getBuffer()->c_str());

}

void setPreviewData(void)
{
//	if(useTheme->LFSTK_getValue()==true)
//		wc->LFSTK_setTile(windowTileEdit->LFSTK_getBuffer()->c_str(),0);
//	else
//		wc->LFSTK_setTile(NULL,0);
//	wc->LFSTK_clearWindow();
//	XSync(wc->display,false);
	
	for(int j=0;j<4;j++)
		{
			previewButtons[j]->LFSTK_setLabelAutoColour(autoColourCheck->LFSTK_getValue());
			if(useTheme->LFSTK_getValue()==true)
				previewButtons[j]->LFSTK_setTile(buttonTileEdit->LFSTK_getBuffer()->c_str(),0);
			else
				previewButtons[j]->LFSTK_setTile(NULL,0);
			previewButtons[j]->LFSTK_setFontString(buttonFontEdit->LFSTK_getBuffer()->c_str());
			previewButtons[j]->LFSTK_setCairoFontData();
			previewButtons[j]->LFSTK_setColourName(NORMALCOLOUR,previeBackColourEdit[j]->LFSTK_getBuffer()->c_str());
			previewButtons[j]->LFSTK_setFontColourName(NORMALCOLOUR,previeFontColourEdit[j]->LFSTK_getBuffer()->c_str(),false);
			previewButtons[j]->LFSTK_clearWindow();

			previewMenus[j]->LFSTK_setLabelAutoColour(autoColourCheck->LFSTK_getValue());
			if(useTheme->LFSTK_getValue()==true)
				previewMenus[j]->LFSTK_setTile(menuTileEdit->LFSTK_getBuffer()->c_str(),0);
			else
				previewMenus[j]->LFSTK_setTile(NULL,0);
			previewMenus[j]->LFSTK_setFontString(menuFontEdit->LFSTK_getBuffer()->c_str());
			previewMenus[j]->LFSTK_setCairoFontData();
			previewMenus[j]->LFSTK_setColourName(NORMALCOLOUR,previeMenuBackColourEdit[j]->LFSTK_getBuffer()->c_str());
			previewMenus[j]->LFSTK_setFontColourName(NORMALCOLOUR,previeMenuFontColourEdit[j]->LFSTK_getBuffer()->c_str(),false);
			previewMenus[j]->LFSTK_clearWindow();
		}
}

bool selectfile(void *object,void* ud)
{
	char	*buffer;

	LFSTK_lineEditClass	*fd=static_cast<LFSTK_lineEditClass*>(ud);

	tileDialog->LFSTK_showFileDialog(wd,"Select A File");
	if(tileDialog->LFSTK_isValid()==true)
		{
			asprintf(&buffer,"%s/%s",tileDialog->LFSTK_getCurrentDir(),tileDialog->LFSTK_getCurrentFile());
			fd->LFSTK_setBuffer(buffer);
			free(buffer);
			free(wd);
			wd=strdup(tileDialog->LFSTK_getCurrentDir());			
		}
	return(true);
}

bool buttonCB(void *p,void* ud)
{
	char					*prefsfile;
	const fontDataStruct	*fd;
	if(ud!=NULL)
		{
			if(strcmp((char*)ud,"SELECTBUTTONFONT")==0)
				{
					buttonFontDialogButton->LFSTK_showDialog("");
					fd=buttonFontDialogButton->LFSTK_getFontData(false);
					if(fd->isValid==true)
						buttonFontEdit->LFSTK_setBuffer(fd->fontString);
					return(true);
				}

			if(strcmp((char*)ud,"SELECTMENUFONT")==0)
				{
					menuFontDialogButton->LFSTK_showDialog("");
					fd=menuFontDialogButton->LFSTK_getFontData(false);
					if(fd->isValid==true)
						menuFontEdit->LFSTK_setBuffer(fd->fontString);
					return(true);
				}

			if(strcmp((char*)ud,"TEST")==0)
				setPreviewData();

			if(strcmp((char*)ud,"APPLY")==0)
				{
					setPreviewData();
					setVars();
					asprintf(&prefsfile,"%s/.config/LFS/lfstoolkit.rc",getenv("HOME"));
					wc->globalLib->LFSTK_saveVarsToFile(prefsfile,wc->globalLib->LFSTK_getTKArgs());
					free(prefsfile);
				}
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent		event;
	int			sy=0;
	int			sx=BORDER;
	int			c=0;
	int			option_index=0;
	const char	*shortOpts="h?w:";
	option		longOptions[]=
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
	
	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH-BORDER-BORDER-BORDER,DIALOGHITE,"LFS Toolkit Prefs",false);
	display=wc->display;
	//wc->LFSTK_initDnD();

	wd=strdup(wc->configDir);
	tileDialog=new LFSTK_fileDialogClass(wc,"Select File",wd,false);

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//normal buutons
	for(int j=0;j<4;j++)
		{
			previewButtons[j]=new LFSTK_buttonClass(wc,previewButtonLabels[j],sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
			previewButtons[j]->LFSTK_setIgnoreCB(true);
			sx+=GADGETWIDTH+BORDER;
			previeBackColourEdit[j]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(j,TYPEBUTTON),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
			sx+=EDITBOXWIDTH+BORDER;
			previeFontColourEdit[j]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(j,TYPEFONTCOLOUR),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
			sy+=YSPACING;
			sx=BORDER;
		}

	sx=BORDER;
//button font
	buttonFontDialogButton=new LFSTK_fontDialogClass(wc,"Select Font",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	buttonFontDialogButton->LFSTK_setCallBack(NULL,buttonCB,(void*)"SELECTBUTTONFONT");
	sx+=GADGETWIDTH+BORDER;
	buttonFontEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(0,TYPEFONT),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	sx=BORDER;
//menu buttons
	for(int j=0;j<4;j++)
		{
			previewMenus[j]=new LFSTK_buttonClass(wc,previewMenuLabels[j],sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
			previewMenus[j]->LFSTK_setIgnoreCB(true);
			sx+=GADGETWIDTH+BORDER;
			previeMenuBackColourEdit[j]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEM),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
			sx+=EDITBOXWIDTH+BORDER;
			previeMenuFontColourEdit[j]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(j,TYPEMENUITEMFONTCOLOUR),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
			sy+=YSPACING;
			sx=BORDER;
		}
	sx=BORDER;
//menu font
	menuFontDialogButton=new LFSTK_fontDialogClass(wc,"Select Font",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	menuFontDialogButton->LFSTK_setCallBack(NULL,buttonCB,(void*)"SELECTMENUFONT");
	sx+=GADGETWIDTH+BORDER;
	menuFontEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(0,TYPEMENUITEMFONT),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;
	sx=BORDER;

//window
	label=new LFSTK_labelClass(wc,"Window Colour",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=BORDER+GADGETWIDTH;
	windowColourEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(0,TYPEWINDOW),sx,sy,EDITBOXWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=BORDER+EDITBOXWIDTH;
	autoColourCheck=new LFSTK_toggleButtonClass(wc,"Auto Colour",sx,sy,GADGETWIDTH,GADGETHITE,NorthWestGravity);
	sy+=YSPACING;

//themes
	useTheme=new LFSTK_toggleButtonClass(wc,"Use Theme",BORDER,sy,GADGETWIDTH,GADGETHITE,NorthWestGravity);
	sy+=YSPACING;
	sx=BORDER;
//window tile
	windowTile=new LFSTK_buttonClass(wc,"Window Tile",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=GADGETWIDTH+BORDER;
	windowTileEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(0,TYPEWINDOWTILE),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	windowTile->LFSTK_setCallBack(NULL,selectfile,(void*)windowTileEdit);
	sy+=YSPACING;
	sx=BORDER;
//button tile
	buttonTile=new LFSTK_buttonClass(wc,"Button Tile",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=GADGETWIDTH+BORDER;
	buttonTileEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(0,TYPEBUTTONTILE),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	buttonTile->LFSTK_setCallBack(NULL,selectfile,(void*)buttonTileEdit);
	sy+=YSPACING;
	sx=BORDER;
//menu tile
	menuTile=new LFSTK_buttonClass(wc,"Menu Tile",sx,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sx+=GADGETWIDTH+BORDER;
	menuTileEdit=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(0,TYPEMENUITEMTILE),sx,sy,EDITBOXWIDTH*2+BORDER,GADGETHITE,BUTTONGRAV);
	menuTile->LFSTK_setCallBack(NULL,selectfile,(void*)menuTileEdit);
	sy+=YSPACING;

	autoColourCheck->LFSTK_setValue(wc->autoLabelColour);
	useTheme->LFSTK_setValue(wc->globalLib->LFSTK_getUseTheme());

	setPreviewData();
	sx=BORDER;

	sx=EDITBOXWIDTH*2+(BORDER*4)+GADGETWIDTH;
//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);

//test
	test=new LFSTK_buttonClass(wc,"Test",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	test->LFSTK_setCallBack(NULL,buttonCB,(void*)"TEST");

//apply
	apply=new LFSTK_buttonClass(wc,"Apply",sx-GADGETWIDTH-BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	apply->LFSTK_setCallBack(NULL,buttonCB,(void*)"APPLY");
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(sx,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		wc->LFSTK_setTransientFor(parentWindow);

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				{
					ml->function(ml->gadget,&event,ml->type);
				}

			switch(event.type)
				{
					case ButtonRelease:
						break;
					case LeaveNotify:
						break;
					case Expose:
						wc->LFSTK_clearWindow();
						break;

					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
						wc->globalLib->LFSTK_setCairoSurface(wc->display,wc->window,wc->visual,&wc->sfc,&wc->cr,event.xconfigurerequest.width,event.xconfigurerequest.height);
						wc->LFSTK_clearWindow();
						break;

					case ClientMessage:
					case SelectionNotify:
						{
							if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
								{
									wc->LFSTK_hideWindow();
									mainLoop=false;
								}
							if(wc->acceptDnd==true)
								wc->LFSTK_handleDnD(&event);
						}
						break;
				}
		}

	delete wc;
	XCloseDisplay(display);
//	cairo_debug_reset_static_data();
	return 0;
}