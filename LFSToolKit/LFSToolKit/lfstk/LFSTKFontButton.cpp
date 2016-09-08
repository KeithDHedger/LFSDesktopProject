/*
 *
 * ©K. D. Hedger. Tue  6 Sep 20:04:47 BST 2016 kdhedger68713@gmail.com

 * This file (LFSTKFontButton.cpp) is part of LFSToolKit.

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

#include "LFSTKGlobals.h"

static LFSTK_fontButtonClass	*thisFB;
static bool						mainLoop;
static LFSTK_lineEditClass		*previewEdit;

LFSTK_fontButtonClass::~LFSTK_fontButtonClass()
{
	for(int j=0;j<this->maxFonts;j++)
		free(this->fontsAZ[j]);
	free(this->fontsAZ);
	if(this->currentFontName!=NULL)
		free(this->currentFontName);
	if(this->holdFont!=NULL)
		free(this->holdFont);
	delete this->dialog;
}

/**
* Main button constructor.
*
* \param parentwc Main parent window class.
* \param label Displayed name.
* \param x X pos.
* \param y Y pos.
* \param w Width.
* \param h Height.
* \param gravity Button gravity.
*/
LFSTK_fontButtonClass::LFSTK_fontButtonClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,int w,int h,int gravity)
{
	XSetWindowAttributes	wa;
	mappedListener			*ml=new mappedListener;
	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	ml->gadget=this;
	ml->type=BUTTONGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
	else
		this->useTile=false;

	this->maxFonts=0;
	this->fontsAZ=NULL;
	this->currentFontName=strdup(label);
	this->isBold=false;
	this->isItalic=false;
	this->currentFont=0;
	this->holdFont=NULL;

	this->loadFontStrings();
	this->dialog=NULL;
	this->fontOffset=0;
	this->fontSize=strdup("10");
	this->finalFont=NULL;
	this->parseFontString(this->label);
	this->LFSTK_setLabel(currentFontName);
	this->labelIsFont=true;
	thisFB=this;
}

void LFSTK_fontButtonClass::loadFontStrings(void)
{
	unsigned	fontcnt=0;
	FILE		*fp;
	char		*command;
	char		*out=NULL;
	char		line[1024];

	line[0]=0;
	currentFont=0;

	asprintf(&command,"%s","fc-list : family|awk -F, '{print $1}'|sort -u|wc -l");
	command=this->wc->globalLib->LFSTK_oneLiner("%s",command);
	if(command==NULL)
		fontcnt=0;
	else
		fontcnt=atoi(command);

	this->fontsAZ=(char**)calloc(fontcnt,sizeof(char*));
	this->maxFonts=fontcnt;
	this->currentFont=0;

	asprintf(&command,"%s","fc-list : family|awk -F, '{print $1}'|sort -ur");
	fp=popen(command, "r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					fontcnt--;
					line[strlen(line)-1]=0;
					this->fontsAZ[fontcnt]=strdup(line);
					if((this->currentFontName!=NULL) && (strcasecmp(this->fontsAZ[fontcnt],this->currentFontName)==0))
						this->currentFont=fontcnt;
				}
			pclose(fp);
		}
	free(command);
}

/**
* Get current font name.
*
* \return const char * name of font.
*
* \note  String is owned by object and must not be freed.
*/

const char* LFSTK_fontButtonClass::LFSTK_getCurrentFontName(void)
{
	return(this->fontsAZ[currentFont]);
}

void buildFontString(bool usedata)
{
	const char	*boldstr="";
	const char	*italicstr="";
	char		*fs;

	if(usedata==false)
		{
			thisFB->parseFontString(thisFB->holdFont);
			buildFontString(true);
			return;
		}

	if(thisFB->LFSTK_getBold()==true)
		boldstr=":bold";
	if(thisFB->LFSTK_getItalic()==true)
		italicstr=":italic";

	if(thisFB->fontSize!=NULL)
		free(thisFB->fontSize);
	asprintf(&thisFB->fontSize,"%i",thisFB->LFSTK_getFontSize());

	asprintf(&fs,"%s:size=%i%s%s",thisFB->LFSTK_getCurrentFontName(),thisFB->LFSTK_getFontSize(),boldstr,italicstr);

	previewEdit->LFSTK_setFontString(fs);
	previewEdit->LFSTK_clearWindow();
	thisFB->LFSTK_setFontDesription(fs);
	free(fs);
}

bool LFSTK_fontButtonClass::dialogCB(void *object,void* userdata)
{
	switch(GETUSERDATA(userdata))
		{
			case DAPPLY:
				buildFontString(true);
				mainLoop=false;
				if(thisFB->labelIsFont==true)
					thisFB->LFSTK_setLabel(thisFB->fontsAZ[thisFB->currentFont]);
				break;
			case DCANCEL:
				buildFontString(false);
				mainLoop=false;
				break;
		}
	return(true);
}

bool LFSTK_fontButtonClass::styleCB(void *object,void* userdata)
{

	if(GETUSERDATA(userdata)==DBOLD)
		thisFB->LFSTK_setBold(!thisFB->LFSTK_getBold());
	else
		thisFB->LFSTK_setItalic(!thisFB->LFSTK_getItalic());

	buildFontString(true);
	return(true);
}

bool LFSTK_fontButtonClass::scrollCB(void *object,void* userdata)
{
	if(GETUSERDATA(userdata)==0)
		thisFB->fontOffset-=MAXPREVIEW;
	else
		thisFB->fontOffset+=MAXPREVIEW;

	if(thisFB->fontOffset<0)
		thisFB->fontOffset=0;
	if(thisFB->fontOffset>(thisFB->maxFonts-MAXPREVIEW))
		thisFB->fontOffset=thisFB->maxFonts-MAXPREVIEW;

//font select buttons
	for(int j=0;j<MAXPREVIEW;j++)
		{
			thisFB->previews[j]->LFSTK_setFontString(thisFB->fontsAZ[j+thisFB->fontOffset]);
			thisFB->previews[j]->LFSTK_setLabel(thisFB->fontsAZ[j+thisFB->fontOffset]);
			thisFB->previews[j]->LFSTK_clearWindow();
		}
	return(true);
}

bool LFSTK_fontButtonClass::selectFontCB(void *object,void* userdata)
{
	LFSTK_gadgetClass	*gadg=LFSTKGADGET(object);
	thisFB->currentFont=GETUSERDATA(userdata)+thisFB->fontOffset;
	buildFontString(true);
	return(true);
}

/**
* Parse fontdesc to items.
* \note Only for internal use
*/
void LFSTK_fontButtonClass::parseFontString(const char *fontstr)
{
	char	*string=strdup(fontstr);
	char	*str;

	this->LFSTK_setBold(false);
	this->LFSTK_setItalic(false);

	str=strtok(string,":");
	while(1)
		{
			bool	found=false;
			if(str==NULL)
				break;
			if(strcasecmp(str,"bold")==0)
				{
					this->LFSTK_setBold(true);
					found=true;
				}
			if(strcasecmp(str,"italic")==0)
				{
					this->LFSTK_setItalic(true);
					found=true;
				}
			if(strcasestr(str,"size=")!=NULL)
				{
					if(this->fontSize!=NULL)
						free(this->fontSize);
					this->fontSize=strndup(&str[5],strlen(str)-5);
					found=true;
				}
			if(found==false)
				{
					if(this->currentFontName!=NULL)
						free(this->currentFontName);
					this->currentFontName=strdup(str);
					for(int j=0;j<this->maxFonts;j++)
						{
							if(strcasecmp(this->fontsAZ[j],this->currentFontName)==0)
								{
									this->currentFont=j;
									break;
								}
						}
				}
			str=strtok(NULL,":");
		}
	free(string);
}

/**
* Set current font bold
* \param bool bold
*/
void LFSTK_fontButtonClass::LFSTK_setBold(bool bold)
{
	this->isBold=bold;
}

/**
* Set current font italic
* \param bool italic
*/
void LFSTK_fontButtonClass::LFSTK_setItalic(bool italic)
{
	this->isItalic=italic;
}

/**
* Get current boldness
* \return boldness
*/
bool LFSTK_fontButtonClass::LFSTK_getBold(void)
{
	return(this->isBold);
}

/**
* Get current italicness
* \return italicness
*/
bool LFSTK_fontButtonClass::LFSTK_getItalic(void)
{
	return(this->isItalic);
}

/**
* Get font string
* \return const char* font desription string
* \note string is owned and should not be freed
*/
const char* LFSTK_fontButtonClass::LFSTK_getFontString(void)
{
	return(this->finalFont);
}

/**
* Set font string
* \param const char* fs font description string
*/
void LFSTK_fontButtonClass::LFSTK_setFontDesription(const char* fs)
{
	if(this->finalFont!=NULL)
		free(this->finalFont);
	this->finalFont=strdup(fs);
	this->parseFontString(fs);
}

/**
* Get current fontsize
* \return int size
*/
int LFSTK_fontButtonClass::LFSTK_getFontSize(void)
{
	return(atoi(static_cast<const char*>(this->fontSizeEdit->LFSTK_getBuffer()->c_str())));
}

/**
* Set show button label as font
* \param bool val
*/
void LFSTK_fontButtonClass::LFSTK_setLabelIsFont(bool val)
{
	this->labelIsFont=val;
}


/**
* show font select dialog.
*/
void LFSTK_fontButtonClass::LFSTK_showDialog(const char *fs)
{
	XEvent					event;
	unsigned				buttony=DBORDER;
	unsigned				gadgetwidth;
	LFSTK_buttonClass		*button;
	LFSTK_labelClass		*label;
	fontStruct				*lfstkfontstruct;
	const char				*lfstkfontstr;

	lfstkfontstr=this->wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEFONT);
	lfstkfontstruct=this->wc->globalLib->LFSTK_loadFont(this->display,this->screen,lfstkfontstr);

	this->parseFontString(fs);

	this->fontOffset=this->currentFont;
	if(this->fontOffset>(this->maxFonts-MAXPREVIEW))
		this->fontOffset=this->maxFonts-MAXPREVIEW;

	if(this->dialog==NULL)
		{
			this->dialog=new LFSTK_windowClass(0,0,DWIDTH,DHITE,"Font Selector",false);

//font select buttons
			for(int j=0;j<MAXPREVIEW;j++)
				{
					this->previews[j]=new LFSTK_buttonClass(this->dialog,this->fontsAZ[j+this->fontOffset],DBORDER,buttony,DWIDTH-(DBORDER*2),DBUTTONHITE,NorthWestGravity);
					this->previews[j]->LFSTK_setColourName(INACTIVECOLOUR,"white");
					this->previews[j]->LFSTK_setFontColourName(INACTIVECOLOUR,"black");
					this->previews[j]->LFSTK_setCallBack(NULL,selectFontCB,USERDATA((j)));
					buttony+=DBUTTONHITE+DGAP;
				}

//style buttons
			gadgetwidth=this->wc->globalLib->LFSTK_getTextwidth(this->display,lfstkfontstruct->data,"Bold")+DBUTTONHITE;
			this->toggleBold=new LFSTK_toggleButtonClass(this->dialog,"Bold",DBORDER,buttony,gadgetwidth,DBUTTONHITE,NorthWestGravity);
			this->toggleBold->LFSTK_setCallBack(NULL,styleCB,USERDATA(DBOLD));
			gadgetwidth=this->wc->globalLib->LFSTK_getTextwidth(this->display,lfstkfontstruct->data,"Italic")+DBUTTONHITE;
			this->toggleItalic=new LFSTK_toggleButtonClass(this->dialog,"Italic",DBORDER+DBUTTONWIDTH+DGAP,buttony,gadgetwidth,DBUTTONHITE,NorthWestGravity);
			this->toggleItalic->LFSTK_setCallBack(NULL,styleCB,USERDATA(DITALIC));

//navigate
			button=new LFSTK_buttonClass(this->dialog,"↑",DWIDTH-DBORDER-(DBUTTONWIDTH/4),buttony,DBUTTONWIDTH/4,DBUTTONHITE,NorthEastGravity);
			button->LFSTK_setCallBack(NULL,scrollCB,USERDATA(0));
			button=new LFSTK_buttonClass(this->dialog,"↓",DWIDTH-DBORDER-((DBUTTONWIDTH/4)*2)-DGAP,buttony,DBUTTONWIDTH/4,DBUTTONHITE,NorthEastGravity);
			button->LFSTK_setCallBack(NULL,scrollCB,USERDATA(1));
			buttony+=DBUTTONHITE+DGAP;

//size
			label=new LFSTK_labelClass(this->dialog,"Font Size",DBORDER,buttony,DBUTTONWIDTH,DBUTTONHITE,NorthWestGravity);
			label->LFSTK_setLabelOriention(0);

			this->fontSizeEdit=new LFSTK_lineEditClass(this->dialog,this->fontSize,DBORDER+DBUTTONWIDTH+DGAP,buttony,DBUTTONWIDTH,DBUTTONHITE,NorthWestGravity);
			buttony+=DBUTTONHITE+DGAP;

//font preview
			previewEdit=new LFSTK_lineEditClass(this->dialog,DPREVIEWTEXT,DBORDER,buttony,DWIDTH-(DBORDER*2),72,NorthWestGravity);

//dialog buttons
			button=new LFSTK_buttonClass(this->dialog,"Cancel",DBORDER,DHITE-DBORDER-DBUTTONHITE,DBUTTONWIDTH,DBUTTONHITE,SouthWestGravity);
			button->LFSTK_setCallBack(NULL,dialogCB,USERDATA(DCANCEL));
			button=new LFSTK_buttonClass(this->dialog,"Apply",DWIDTH-DBORDER-DBUTTONWIDTH,DHITE-DBORDER-DBUTTONHITE,DBUTTONWIDTH,DBUTTONHITE,SouthEastGravity);
			button->LFSTK_setCallBack(NULL,dialogCB,USERDATA(DAPPLY));
		}

	for(int j=0;j<MAXPREVIEW;j++)
		{
			this->previews[j]->LFSTK_setFontString(this->fontsAZ[j+this->fontOffset]);
			this->previews[j]->LFSTK_setLabel(this->fontsAZ[j+this->fontOffset]);
		}
	this->toggleBold->LFSTK_setValue(this->isBold);
	this->toggleItalic->LFSTK_setValue(this->isItalic);
	previewEdit->LFSTK_setFontString(fs);
	if(this->holdFont!=NULL)
		free(this->holdFont);
	this->holdFont=strdup(fs);

	this->dialog->LFSTK_showWindow();
	this->dialog->LFSTK_setKeepAbove(true);
	this->dialog->LFSTK_setTransientFor(this->wc->window);

	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(this->dialog->display,&event);
			mappedListener *ml=this->dialog->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			switch(event.type)
				{
				case KeyRelease:
					if(ml->gadget==this->fontSizeEdit)
						buildFontString(true);
					break;
					case LeaveNotify:
						break;
					case Expose:
						this->dialog->LFSTK_clearWindow();
						break;

					case ConfigureNotify:
						this->dialog->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						break;

					case ClientMessage:
					case SelectionNotify:
						{
							if(event.xclient.message_type==XInternAtom(this->dialog->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(this->dialog->display, "WM_DELETE_WINDOW", 1))
							mainLoop=false;
						}
						break;
				}
		}

	this->dialog->LFSTK_hideWindow();

}



