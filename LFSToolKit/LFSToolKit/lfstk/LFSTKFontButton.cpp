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

#include "lfstk/LFSTKGlobals.h"

LFSTK_fontButtonClass::~LFSTK_fontButtonClass()
{
	for(int j=0;j<this->maxFonts;j++)
		free(this->fontsAZ[j]);
	delete this->fontsAZ;

	if(this->fontDesc!=NULL)
		free(this->fontDesc);
	if(this->fontSize!=NULL)
		free(this->fontSize);
	if(this->fontName!=NULL)
		free(this->fontName);

	delete this->dialog;
	delete this->previewData;
	delete this->buttonData;
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

	this->loadFontStrings();
	this->dialog=NULL;
	this->fontOffset=0;
	this->fontSize=strdup("10");
	this->labelIsFont=true;
	this->previewData=new dialogData[MAXPREVIEW];
	this->buttonData=new dialogData[DMAXBUTTONS];

	this->fontDesc=NULL;
	this->fontSize=NULL;
	this->fontName=NULL;
	this->bold=false;
	this->italic=false;
	this->fontNumber=0;
	this->fontOffset=0;
	this->fontDesc=NULL;
}

void LFSTK_fontButtonClass::loadFontStrings(void)
{
	unsigned	fontcnt=0;
	FILE		*fp;
	char		*command;
	char		*out=NULL;
	char		line[1024];

	line[0]=0;

	asprintf(&command,"%s","fc-list : family|awk -F, '{print $1}'|sort -u|wc -l");
	out=this->wc->globalLib->LFSTK_oneLiner("%s",command);
	if(out==NULL)
		fontcnt=0;
	else
		{
			fontcnt=atoi(out);
			free(out);
		}

	this->fontsAZ=new char*[fontcnt];
	this->maxFonts=fontcnt;

	asprintf(&command,"%s","fc-list : family|awk -F, '{print $1}'|sort -ur");
	fp=popen(command, "r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					fontcnt--;
					line[strlen(line)-1]=0;
					this->fontsAZ[fontcnt]=strdup(line);
				}
			pclose(fp);
		}
	free(command);
}

/**
* Get current font name.
* \return const char * name of font.
* \note  String is owned by object and must not be freed.
*/
const char* LFSTK_fontButtonClass::LFSTK_getCurrentFontName(void)
{
	return(this->fontsAZ[this->fontNumber]);
}

void LFSTK_fontButtonClass::buildFontString(bool usedata)
{
	const char	*boldstr="";
	const char	*italicstr="";

	if(this->toggleBold->LFSTK_getValue()==true)
		boldstr=":bold";
	if(this->toggleItalic->LFSTK_getValue()==true)
		italicstr=":italic";

	if(this->fontDesc!=NULL)
		free(this->fontDesc);

	if(this->fontSize!=NULL)
		free(this->fontSize);
	this->fontSize=strdup(this->fontSizeEdit->LFSTK_getBuffer()->c_str());
	asprintf(&this->fontDesc,"%s:size=%s%s%s",this->fontsAZ[this->fontNumber],this->fontSize,boldstr,italicstr);
}

bool LFSTK_fontButtonClass::dialogCB(void *object,void* userdata)
{
	LFSTK_fontButtonClass	*button;
	dialogData				*d=(dialogData*)userdata;
	unsigned				data;

	button=static_cast<LFSTK_fontButtonClass*>(d->mainObject);
	data=d->userData;

	switch(data)
		{
			case DAPPLY:
				button->buildFontString(true);
				if(button->labelIsFont==true)
					button->LFSTK_setLabel(button->fontsAZ[button->fontNumber]);
				break;
			case DCANCEL:
				button->parseFontString(button->fontDesc);
				button->updateDialog(false);
				break;
		}
	button->mainLoop=false;
	return(true);
}

bool LFSTK_fontButtonClass::styleCB(void *object,void* userdata)
{
	LFSTK_fontButtonClass	*button;
	LFSTK_toggleButtonClass	*toggle;

	dialogData				*d=(dialogData*)userdata;

	button=static_cast<LFSTK_fontButtonClass*>(d->mainObject);
	toggle=static_cast<LFSTK_toggleButtonClass*>(object);

	if(d->userData==DBOLD)
		button->bold=toggle->LFSTK_getValue();
	else
		button->italic=toggle->LFSTK_getValue();
	button->updateDialog(false);
	return(true);
}

void LFSTK_fontButtonClass::setNavSensitive(void)
{
	this->buttonDown->LFSTK_setActive(true);
	this->buttonUp->LFSTK_setActive(true);
	this->buttonHome->LFSTK_setActive(true);
	this->buttonEnd->LFSTK_setActive(true);

	if(this->fontOffset<=0)
		{
			this->buttonUp->LFSTK_setActive(false);
			this->buttonHome->LFSTK_setActive(false);
		}
	if(this->fontOffset>=this->maxFonts-MAXPREVIEW)
		{
			this->buttonDown->LFSTK_setActive(false);
			this->buttonEnd->LFSTK_setActive(false);
		}

	this->buttonDown->LFSTK_clearWindow();
	this->buttonUp->LFSTK_clearWindow();
	this->buttonHome->LFSTK_clearWindow();
	this->buttonEnd->LFSTK_clearWindow();
}

bool LFSTK_fontButtonClass::scrollCB(void *object,void* userdata)
{
	LFSTK_fontButtonClass	*button;
	dialogData				*d=(dialogData*)userdata;
	unsigned				data;

	button=static_cast<LFSTK_fontButtonClass*>(d->mainObject);
	data=d->userData;

	switch(data)
		{
			case DUP:
				button->fontOffset-=MAXPREVIEW;
				break;
			case DDOWN:
				button->fontOffset+=MAXPREVIEW;
				break;
			case DHOME:
				button->fontOffset=0;
				break;
			case DEND:
				button->fontOffset=button->maxFonts-MAXPREVIEW;
				break;
		}

	if(button->fontOffset<=0)
		button->fontOffset=0;

	if(button->fontOffset>=(button->maxFonts-MAXPREVIEW))
		button->fontOffset=button->maxFonts-MAXPREVIEW;

	button->setNavSensitive();

//font select buttons
	for(int j=0;j<MAXPREVIEW;j++)
		{
			button->previews[j]->LFSTK_setFontString(button->fontsAZ[j+button->fontOffset]);
			button->previews[j]->LFSTK_setLabel(button->fontsAZ[j+button->fontOffset]);
			button->previews[j]->LFSTK_clearWindow();
		}
	return(true);
}

void LFSTK_fontButtonClass::updateDialog(bool fonts)
{	
	const char	*boldstr="";
	const char	*italicstr="";
	char		*fs;

//	for(int j=0;j<MAXPREVIEW;j++)
//		{
//			this->previews[j]->LFSTK_setFontString(this->fontsAZ[j+this->tFontOffset]);
//			this->previews[j]->LFSTK_setLabel(this->fontsAZ[j+this->tFontOffset]);
//		}

	if(this->toggleBold->LFSTK_getValue()==true)
		boldstr=":bold";
	if(this->toggleItalic->LFSTK_getValue()==true)
		italicstr=":italic";

	asprintf(&fs,"%s:size=%s%s%s",this->fontsAZ[this->fontNumber],this->fontSizeEdit->LFSTK_getBuffer()->c_str(),boldstr,italicstr);
	previewEdit->LFSTK_setFontString(fs);
	previewEdit->LFSTK_clearWindow();

	free(fs);
}

bool LFSTK_fontButtonClass::selectFontCB(void *object,void* userdata)
{
	LFSTK_fontButtonClass	*fb;
	unsigned				data;
	dialogData				*d=(dialogData*)userdata;

	data=d->userData;
	fb=static_cast<LFSTK_fontButtonClass*>(d->mainObject);

	fb->fontNumber=data+fb->fontOffset;
	fb->updateDialog(false);

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

	this->bold=false;
	this->italic=false;
	if(this->fontSize!=NULL)
		free(this->fontSize);
	this->fontSize=strdup("10");

	str=strtok(string,":");
	while(1)
		{
			bool	found=false;
			if(str==NULL)
				break;
			if(strcasecmp(str,"bold")==0)
				{
					this->bold=true;
					found=true;
				}
			if(strcasecmp(str,"italic")==0)
				{
					this->italic=true;
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
					if(this->fontName!=NULL)
						free(this->fontName);
					this->fontName=strdup(str);
					for(int j=0;j<this->maxFonts;j++)
						{
							if(strcasecmp(this->fontsAZ[j],this->fontName)==0)
								{
									this->fontNumber=j;
									break;
								}
						}
				}
			str=strtok(NULL,":");
		}
	free(string);
}

/**
* Get current boldness
* \return boldness
*/
bool LFSTK_fontButtonClass::LFSTK_getBold(void)
{
	return(this->bold);
}

/**
* Get current italicness
* \return italicness
*/
bool LFSTK_fontButtonClass::LFSTK_getItalic(void)
{
	return(this->italic);
}

/**
* Get font string
* \return const char* font desription string
* \note string is owned and should not be freed
*/
const char* LFSTK_fontButtonClass::LFSTK_getFontString(void)
{
	return(this->fontDesc);
}

/**
* Get current fontsize
* \return const char* size
* \note string is owned by object.
*/
const char* LFSTK_fontButtonClass::LFSTK_getFontSize(void)
{
	return(static_cast<const char*>(this->fontSize));
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

	if(this->fontDesc!=NULL)
		free(this->fontDesc);
	this->fontDesc=strdup(fs);

	this->parseFontString(this->fontDesc);
	this->fontOffset=this->fontNumber;

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
					this->previewData[j].mainObject=this;
					this->previewData[j].userData=j;
					this->previews[j]->LFSTK_setCallBack(NULL,selectFontCB,(void*)(&this->previewData[j]));
					buttony+=DBUTTONHITE+DGAP;
				}
//style buttons
			gadgetwidth=this->wc->globalLib->LFSTK_getTextwidth(this->display,lfstkfontstruct->data,"Bold")+DBUTTONHITE;
			this->toggleBold=new LFSTK_toggleButtonClass(this->dialog,"Bold",DBORDER,buttony,gadgetwidth,DBUTTONHITE,NorthWestGravity);
			this->buttonData[DBOLD].mainObject=this;
			this->buttonData[DBOLD].userData=DBOLD;
			this->toggleBold->LFSTK_setCallBack(NULL,styleCB,DIALOGDATA(buttonData[DBOLD]));
			gadgetwidth=this->wc->globalLib->LFSTK_getTextwidth(this->display,lfstkfontstruct->data,"Italic")+DBUTTONHITE;
			this->toggleItalic=new LFSTK_toggleButtonClass(this->dialog,"Italic",DBORDER+DBUTTONWIDTH+DGAP,buttony,gadgetwidth,DBUTTONHITE,NorthWestGravity);
			this->buttonData[DITALIC].mainObject=this;
			this->buttonData[DITALIC].userData=DITALIC;
			this->toggleItalic->LFSTK_setCallBack(NULL,styleCB,DIALOGDATA(buttonData[DITALIC]));

//navigate
//line up/down
			this->buttonUp=new LFSTK_buttonClass(this->dialog,"↑",DWIDTH-DBORDER-(DBUTTONWIDTH/4),buttony,DBUTTONWIDTH/4,DBUTTONHITE,NorthEastGravity);
			this->buttonData[DUP].mainObject=this;
			this->buttonData[DUP].userData=DUP;
			buttonUp->LFSTK_setCallBack(NULL,scrollCB,DIALOGDATA(buttonData[DUP]));
			buttonDown=new LFSTK_buttonClass(this->dialog,"↓",DWIDTH-DBORDER-((DBUTTONWIDTH/4)*2)-DGAP,buttony,DBUTTONWIDTH/4,DBUTTONHITE,NorthEastGravity);
			this->buttonData[DDOWN].mainObject=this;
			this->buttonData[DDOWN].userData=DDOWN;
			buttonDown->LFSTK_setCallBack(NULL,scrollCB,DIALOGDATA(buttonData[DDOWN]));
//page home/end
			buttonHome=new LFSTK_buttonClass(this->dialog,"⇤",DWIDTH-DBORDER-((DBUTTONWIDTH/4)*3)-DGAP*2,buttony,DBUTTONWIDTH/4,DBUTTONHITE,NorthEastGravity);
			this->buttonData[DHOME].mainObject=this;
			this->buttonData[DHOME].userData=DHOME;
			buttonHome->LFSTK_setCallBack(NULL,scrollCB,DIALOGDATA(buttonData[DHOME]));
			buttonEnd=new LFSTK_buttonClass(this->dialog,"⇥",DWIDTH-DBORDER-((DBUTTONWIDTH/4)*4)-DGAP*3,buttony,DBUTTONWIDTH/4,DBUTTONHITE,NorthEastGravity);
			this->buttonData[DEND].mainObject=this;
			this->buttonData[DEND].userData=DEND;
			buttonEnd->LFSTK_setCallBack(NULL,scrollCB,DIALOGDATA(buttonData[DEND]));
			buttony+=DBUTTONHITE+DGAP;

//size
			label=new LFSTK_labelClass(this->dialog,"Font Size",DBORDER,buttony,DBUTTONWIDTH,DBUTTONHITE,NorthWestGravity);
			label->LFSTK_setLabelGravity(0);

			this->fontSizeEdit=new LFSTK_lineEditClass(this->dialog,this->fontSize,DBORDER+DBUTTONWIDTH+DGAP,buttony,DBUTTONWIDTH,DBUTTONHITE,NorthWestGravity);
			buttony+=DBUTTONHITE+DGAP;

//font preview
			previewEdit=new LFSTK_lineEditClass(this->dialog,DPREVIEWTEXT,DBORDER,buttony,DWIDTH-(DBORDER*2),72,NorthWestGravity);

//dialog buttons
			button=new LFSTK_buttonClass(this->dialog,"Cancel",DBORDER,DHITE-DBORDER-DBUTTONHITE,DBUTTONWIDTH,DBUTTONHITE,SouthWestGravity);
			this->buttonData[DCANCEL].mainObject=this;
			this->buttonData[DCANCEL].userData=DCANCEL;
			button->LFSTK_setCallBack(NULL,dialogCB,(void*)(&this->buttonData[DCANCEL]));
			button=new LFSTK_buttonClass(this->dialog,"Apply",DWIDTH-DBORDER-DBUTTONWIDTH,DHITE-DBORDER-DBUTTONHITE,DBUTTONWIDTH,DBUTTONHITE,SouthEastGravity);
			this->buttonData[DAPPLY].mainObject=this;
			this->buttonData[DAPPLY].userData=DAPPLY;
			button->LFSTK_setCallBack(NULL,dialogCB,(void*)(&this->buttonData[DAPPLY]));
		}

	for(int j=0;j<MAXPREVIEW;j++)
		{
			this->previews[j]->LFSTK_setFontString(this->fontsAZ[j+this->fontOffset]);
			this->previews[j]->LFSTK_setLabel(this->fontsAZ[j+this->fontOffset]);
		}

	this->toggleBold->LFSTK_setValue(this->bold);
	this->toggleItalic->LFSTK_setValue(this->italic);
	this->fontSizeEdit->LFSTK_setBuffer(this->fontSize);
	previewEdit->LFSTK_setFontString(fs);
	this->updateDialog(false);

	this->setNavSensitive();
	this->dialog->LFSTK_showWindow();
	this->dialog->LFSTK_setKeepAbove(true);
	this->dialog->LFSTK_setTransientFor(this->wc->window);

	this->mainLoop=true;
	while(this->mainLoop==true)
		{
			XNextEvent(this->dialog->display,&event);
			mappedListener *ml=this->dialog->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			switch(event.type)
				{
				case KeyRelease:
					if(ml->gadget==this->fontSizeEdit)
						this->updateDialog(true);
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
							this->mainLoop=false;
						}
						break;
				}
		}

	this->dialog->LFSTK_hideWindow();

}



