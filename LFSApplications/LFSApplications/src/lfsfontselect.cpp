/*
 *
 * ©K. D. Hedger. Sun  4 Sep 14:01:13 BST 2016 kdhedger68713@gmail.com

 * This file (lfsfontselect.cpp) is part of LFSApplications.

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

#define WIDTH 600
#define HITE 400
#define BORDER 8
#define GAP 4
#define MAXPREVIEW 6
#define BUTTONHITE 24
#define BUTTONWIDTH 72

#define USERDATA(x) (void*)(long)x
#define LFSTKGADGET(x) static_cast<LFSTK_gadgetClass*>(x)
#define GETUSERDATA(x) (long)x

#define PREVIEWTEXT "ABCDEFGHIJK abcdefghijk 0123456789"

enum {APPLY=1,CANCEL};
enum {BOLD=0,ITALIC=1};

char				**fontsAZ;
unsigned			maxFonts=0;
unsigned			currentFont;
char				*startUpFont;
int					fontOffset=0;
bool				mainLoop;
bool				isBold=false;
bool				isItalic=false;
char				*finalFont=NULL;
char				*fontSize;
bool				useDetail=false;
int					parentWindow=-1;

LFSTK_windowClass	*mainWindow;
LFSTK_buttonClass	*previews[MAXPREVIEW];
LFSTK_lineEditClass	*previewEdit;
LFSTK_lineEditClass	*fontSizeEdit;

void loadFontStrings(void)
{
	unsigned	fontcnt=0;
	FILE		*fp;
	char		*command;
	char		*out=NULL;
	char		line[1024];

	line[0]=0;
	currentFont=0;

	asprintf(&command,"%s","fc-list : family|awk -F, '{print $1}'|sort -u|wc -l");
	command=mainWindow->globalLib->LFSTK_oneLiner("%s",command);
	if(command==NULL)
		fontcnt=0;
	else
		fontcnt=atoi(command);

	fontsAZ=(char**)calloc(fontcnt,sizeof(char*));
	maxFonts=fontcnt;
	currentFont=0;

	asprintf(&command,"%s","fc-list : family|awk -F, '{print $1}'|sort -ur");
	fp=popen(command, "r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					fontcnt--;
					line[strlen(line)-1]=0;
					fontsAZ[fontcnt]=strdup(line);
					if((startUpFont!=NULL) && (strcasecmp(fontsAZ[fontcnt],startUpFont)==0))
						currentFont=fontcnt;
				}
			pclose(fp);
		}
	free(command);
}

void buildFontString(void)
{
	const char	*boldstr="";
	const char	*italicstr="";

	if(isBold==true)
		boldstr=":bold";
	if(isItalic==true)
		italicstr=":italic";

	fontSize=strdup(static_cast<const char*>(fontSizeEdit->LFSTK_getBuffer()->c_str()));

	if(finalFont!=NULL)
		free(finalFont);
	asprintf(&finalFont,"%s:size=%s%s%s",fontsAZ[currentFont],fontSize,boldstr,italicstr);

	previewEdit->LFSTK_setFontString(finalFont);
	previewEdit->LFSTK_clearWindow();
}

bool selectFontCB(void *object,void* userdata)
{
	LFSTK_gadgetClass	*gadg=LFSTKGADGET(object);
	currentFont=GETUSERDATA(userdata)+fontOffset;
	buildFontString();
	return(true);
}

bool dialogCB(void *object,void* userdata)
{
	switch(GETUSERDATA(userdata))
		{
			case APPLY:
				buildFontString();
				if(useDetail==false)
					printf("%s\n",finalFont);
				else
					printf("%s\n%s\n%i\n%i\n%s\n",fontsAZ[currentFont],fontSize,isBold,isItalic,finalFont);
			case CANCEL:
				mainLoop=false;
		}
	return(true);
}

bool styleCB(void *object,void* userdata)
{
	if(GETUSERDATA(userdata)==BOLD)
		isBold=!isBold;
	else
		isItalic=!isItalic;

	buildFontString();
	return(true);
}

bool scrollCB(void *object,void* userdata)
{
	if(GETUSERDATA(userdata)==0)
		fontOffset-=MAXPREVIEW;
	else
		fontOffset+=MAXPREVIEW;

	if(fontOffset<0)
		fontOffset=0;
	if(fontOffset>(maxFonts-MAXPREVIEW))
		fontOffset=maxFonts-MAXPREVIEW;

//font select buttons
	for(int j=0;j<MAXPREVIEW;j++)
		{
			previews[j]->LFSTK_setFontString(fontsAZ[j+fontOffset]);
			previews[j]->LFSTK_setLabel(fontsAZ[j+fontOffset]);
			previews[j]->LFSTK_clearWindow();
		}
	return(true);
}

void printHelp(void)
{
	printf("-?,-h,--help\t\tPrint this help\n");
	printf("-w,--window\t\tSet transient for window\n");
	printf("-d,--detail\t\tOutput details on seperate line like so:\n");
	printf("Fontname\n");
	printf("Size\n");
	printf("Bold\n");
	printf("Italic\n");
	printf("Font String\n");
}

void parseFontString(const char *fontstr)
{
	char	*string=strdup(fontstr);
	char	*str;

	str=strtok(string,":");
	while(1)
		{
			bool	found=false;
			if(str==NULL)
				break;
			if(strcasecmp(str,"bold")==0)
				{
					isBold=true;
					found=true;
				}
			if(strcasecmp(str,"italic")==0)
				{
					isItalic=true;
					found=true;
				}
			if(strcasestr(str,"size=")!=NULL)
				{
					if(fontSize!=NULL)
						free(fontSize);
					fontSize=strndup(&str[5],strlen(str)-5);
					found=true;
				}
			if(found==false)
				{
					if(startUpFont!=NULL)
						free(startUpFont);
					startUpFont=strdup(str);
				}
			str=strtok(NULL,":");
		}
	free(string);
}

int main(int argc, char **argv)
{
	XEvent					event;
	LFSTK_buttonClass		*button;
	LFSTK_toggleButtonClass	*toggle;
	LFSTK_labelClass		*label;
	unsigned				buttony=BORDER;
	const char				*lfstkfontstr;
	fontStruct				*lfstkfontstruct;
	unsigned				gadgetwidth;
	int						c=0;
	int						option_index=0;
	const char				*shortOpts="h?bis:w:";
	option 					longOptions[]=
		{
			{"window",1,0,'w'},
			{"detail",0,0,'d'},
			{"help",0,0,'h'},
			{0, 0, 0, 0}
		};

	startUpFont=strdup("sans");
	fontSize=strdup("10");

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
						printHelp();
						exit(0);
					case 'd':
						useDetail=true;
						break;
					case 'w':
						parentWindow=atoi(optarg);
						break;
				}
		}

	if(argv[optind]==NULL)
		parseFontString("");
	else
		parseFontString(argv[optind]);

	mainWindow=new LFSTK_windowClass(0,0,WIDTH,HITE,"Font Selector",false);
	lfstkfontstr=mainWindow->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEFONT);
	lfstkfontstruct=mainWindow->globalLib->LFSTK_loadFont(mainWindow->display,mainWindow->screen,lfstkfontstr);
	loadFontStrings();

	fontOffset=currentFont;
	if(fontOffset>(maxFonts-MAXPREVIEW))
		fontOffset=maxFonts-MAXPREVIEW;

//font select buttons
	for(int j=0;j<MAXPREVIEW;j++)
		{
			previews[j]=new LFSTK_buttonClass(mainWindow,fontsAZ[j+fontOffset],BORDER,buttony,WIDTH-(BORDER*2),BUTTONHITE,NorthWestGravity);
			previews[j]->LFSTK_setColourName(INACTIVECOLOUR,"white");
			previews[j]->LFSTK_setFontColourName(INACTIVECOLOUR,"black");
			previews[j]->LFSTK_setFontString(fontsAZ[j+fontOffset]);
			previews[j]->LFSTK_setCallBack(NULL,selectFontCB,USERDATA((j)));
			buttony+=BUTTONHITE+GAP;
		}
//style buttons
	gadgetwidth=mainWindow->globalLib->LFSTK_getTextwidth(mainWindow->display,lfstkfontstruct->data,"Bold")+BUTTONHITE;
	toggle=new LFSTK_toggleButtonClass(mainWindow,"Bold",BORDER,buttony,gadgetwidth,BUTTONHITE,NorthWestGravity);
	toggle->LFSTK_setValue(isBold);
	toggle->LFSTK_setCallBack(NULL,styleCB,USERDATA(BOLD));
	gadgetwidth=mainWindow->globalLib->LFSTK_getTextwidth(mainWindow->display,lfstkfontstruct->data,"Italic")+BUTTONHITE;
	toggle=new LFSTK_toggleButtonClass(mainWindow,"Italic",BORDER+BUTTONWIDTH+GAP,buttony,gadgetwidth,BUTTONHITE,NorthWestGravity);
	toggle->LFSTK_setValue(isItalic);
	toggle->LFSTK_setCallBack(NULL,styleCB,USERDATA(ITALIC));

//navigate
	button=new LFSTK_buttonClass(mainWindow,"↑",WIDTH-BORDER-(BUTTONWIDTH/4),buttony,BUTTONWIDTH/4,BUTTONHITE,NorthEastGravity);
	button->LFSTK_setCallBack(NULL,scrollCB,USERDATA(0));
	button=new LFSTK_buttonClass(mainWindow,"↓",WIDTH-BORDER-((BUTTONWIDTH/4)*2)-GAP,buttony,BUTTONWIDTH/4,BUTTONHITE,NorthEastGravity);
	button->LFSTK_setCallBack(NULL,scrollCB,USERDATA(1));
	buttony+=BUTTONHITE+GAP;

//size
	label=new LFSTK_labelClass(mainWindow,"Font Size",BORDER,buttony,BUTTONWIDTH,BUTTONHITE,NorthWestGravity);
	label->LFSTK_setLabelOriention(0);

	fontSizeEdit=new LFSTK_lineEditClass(mainWindow,fontSize,BORDER+BUTTONWIDTH+GAP,buttony,BUTTONWIDTH,BUTTONHITE,NorthWestGravity);
	buttony+=BUTTONHITE+GAP;

//font preview
	previewEdit=new LFSTK_lineEditClass(mainWindow,PREVIEWTEXT,BORDER,buttony,WIDTH-(BORDER*2),72,NorthWestGravity);
	previewEdit->LFSTK_setFontString(fontsAZ[currentFont]);

//dialog buttons
	button=new LFSTK_buttonClass(mainWindow,"Cancel",BORDER,HITE-BORDER-BUTTONHITE,BUTTONWIDTH,BUTTONHITE,SouthWestGravity);
	button->LFSTK_setCallBack(NULL,dialogCB,USERDATA(CANCEL));
	button=new LFSTK_buttonClass(mainWindow,"Apply",WIDTH-BORDER-BUTTONWIDTH,HITE-BORDER-BUTTONHITE,BUTTONWIDTH,BUTTONHITE,SouthEastGravity);
	button->LFSTK_setCallBack(NULL,dialogCB,USERDATA(APPLY));

	buildFontString();
	mainWindow->LFSTK_showWindow();
	mainWindow->LFSTK_setKeepAbove(true);
	if(parentWindow!=-1)
		mainWindow->LFSTK_setTransientFor(parentWindow);

	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(mainWindow->display,&event);
			mappedListener *ml=mainWindow->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			switch(event.type)
				{
				case KeyRelease:
					if(ml->gadget==fontSizeEdit)
						buildFontString();
					break;
					case LeaveNotify:
						break;
					case Expose:
						mainWindow->LFSTK_clearWindow();
						break;

					case ConfigureNotify:
						mainWindow->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						break;

					case ClientMessage:
					case SelectionNotify:
						{
							if(event.xclient.message_type==XInternAtom(mainWindow->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(mainWindow->display, "WM_DELETE_WINDOW", 1))
							mainLoop=false;
						}
						break;
				}
		}

	for(int j=0;j<maxFonts;j++)
		free(fontsAZ[j]);
	free(fontsAZ);
	free(fontSize);
	free(startUpFont);
	delete mainWindow;
	return 0;
}
