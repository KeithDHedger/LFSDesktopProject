/*
 *
 * ©K. D. Hedger. Sat 28 Nov 15:08:39 GMT 2020 keithdhedger@gmail.com

 * This file (LFSTKFontDialog.cpp) is part of LFSToolKit.

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

#include "lfstk/LFSTKGlobals.h"

const char	*fontTest=
	"0123456789\nThe quick brown fox jumped over the lazy dog\nTHE QUICK BROWN FOX JUMPED OVER THE LAZY DOG\n";

LFSTK_fontDialogClass::~LFSTK_fontDialogClass()
{
	delete this->dialog;
}

LFSTK_fontDialogClass::LFSTK_fontDialogClass()
{
}

/**
* Build font string from parts.
*/
void LFSTK_fontDialogClass::buildFontString(void)
{
	const char	*boldstr="";
	const char	*italicstr="";
	const char	*dobold="b";
	const char	*doitalic="i";
	char			*formatstring=NULL;

	this->fontData.bold=false;
	this->fontData.italic=false;

	if(this->boldcheck->LFSTK_getValue()==true)
		{
			boldstr=":bold";
			dobold="B";
			this->fontData.bold=true;
		}
	if(this->italiccheck->LFSTK_getValue()==true)
		{
			italicstr=":italic";
			doitalic="I";
			this->fontData.italic=true;
		}

	this->fontData.fontSize=atoi(this->fontsize->LFSTK_getCStr());
	this->selectedFontNumber=fontlist->LFSTK_getCurrentListItem();

	this->fontData.fontString=this->fontsAZV.at(this->selectedFontNumber)+":size="+this->fontsize->LFSTK_getCStr()+boldstr+italicstr;

	asprintf(&formatstring,"ns%s%s",dobold,doitalic);
	this->preview->LFSTK_setCairoFontDataParts(formatstring,this->fontsAZV.at(selectedFontNumber).c_str(),atoi(this->fontsize->LFSTK_getCStr()));
	this->preview->LFSTK_upDateText();

	this->fontData.fontName=this->fontsAZV.at(selectedFontNumber).c_str();
	this->fontData.isValid=true;
	freeAndNull(&formatstring);
	this->preview->LFSTK_clearWindow();
}

bool select(void *object,void* userdata)
{
	LFSTK_fontDialogClass	*fd=static_cast<LFSTK_fontDialogClass*>(userdata);
	fd->LFSTK_getFontData(true);
	fd->wc->LFSTK_clearWindow(true);
	XSync(fd->wc->app->display,false);
	return(true);
}

bool checkCB(void *p,void* ud)
{
	LFSTK_fontDialogClass	*fdata=static_cast<LFSTK_fontDialogClass*>(ud);
	fdata->LFSTK_getFontData(true);
	fdata->wc->LFSTK_clearWindow();
	XSync(fdata->wc->app->display,false);
	return(true);
}

/**
* Get pointer to font data.
* \param bool rebuild
*\return const pointer to structure.
* \note private data do NOT alter/free
*/
const fontDataStruct* LFSTK_fontDialogClass::LFSTK_getFontData(bool rebuild)
{
	if(rebuild==true)
		this->buildFontString();
	return(&this->fontData);
}

/*
* Load font names from disk.
*/
void LFSTK_fontDialogClass::loadFontStrings(void)
{
	char		line[1024];
	FILE		*fp;

	fp=popen("fc-list : family|awk -F, '{print $1}'|sort -u","r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					line[strlen(line)-1]=0;
					this->fontsAZV.push_back(std::string(line));
				}
			pclose(fp);
		}
	this->maxFonts=this->fontsAZV.size();
}

/*
* Private parse fontstring.
*/
void LFSTK_fontDialogClass::parseFontString(std::string fontstr)//TODO//
{
	std::vector<std::string>	tokenstrings;

	tokenstrings=LFSTK_UtilityClass::LFSTK_strTok(fontstr,":");

	if(tokenstrings.size()>3)
		{
			this->fontlist->LFSTK_findByLabel(tokenstrings.at(0).c_str());
			this->boldcheck->LFSTK_setValue(tokenstrings.at(2).c_str());
			this->italiccheck->LFSTK_setValue(tokenstrings.at(3).c_str());
			this->fontsize->LFSTK_setBuffer(tokenstrings.at(1).substr(5,(tokenstrings.at(1).length()-5)).c_str());
			this->LFSTK_getFontData(true);
		}
	return;
}

/*
* Show the dialog.
* \param const char* fontstring
* \note Set font options, fontstring should be of the form:Helvetica:size=18:bold:italic
*/
bool LFSTK_fontDialogClass::LFSTK_showDialog(const char* fontstring)
{
	bool				mainLoop=true;
	XEvent			event;
	geometryStruct	geomfont;
	pointStruct		pt;

	this->parseFontString(fontstring);
	this->dialog->LFSTK_showWindow(true);
	this->dialog->LFSTK_setKeepAbove(true);
	this->dialog->LFSTK_setTransientFor(this->wc->window);
	this->mainLoop=true;

	while(this->mainLoop==true)
		{
			XNextEvent(this->dialog->app->display,&event);
			mappedListener *ml=this->dialog->LFSTK_getMappedListener(event.xany.window);

			if(ml==NULL)
				continue;

			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			switch(event.type)
				{
					case KeyRelease:
						this->preview->LFSTK_upDateText();
						break;

					case ButtonRelease:
						if(ml!=NULL)
							{
								if(ml->gadget==this->apply)
									{
										this->LFSTK_getFontData(true);
										this->fontData.isValid=true;
										this->mainLoop=false;
									}
								if(ml->gadget==this->cancel)
									{
										this->mainLoop=false;
										this->fontData.isValid=false;
									}
							}
						break;
				}

			if(this->dialog->LFSTK_handleWindowEvents(&event)<0)
				mainLoop=false;
		}
	this->dialog->LFSTK_hideWindow();

	return(true);
}

void LFSTK_fontDialogClass::buildDialog(void)
{
	int				sy=BORDER;
	char				*sizestr[4]={0,};
	infoDataStruct	ls;

	windowInitStruct	*win;//TODO//
	win=this->wc->app->LFSTK_getDefaultWInit();
	win->w=DIALOGWIDTH;
	win->h=DIALOGHITE;
	win->wc=this->wc;
	win->windowName="Font Selector";
	win->windowType=win->app->appAtomsHashed.at(LFSTK_UtilityClass::LFSTK_hashFromKey("_NET_WM_WINDOW_TYPE_DIALOG"));
	win->level=ABOVEALL;
	this->dialog=new LFSTK_windowClass(win,this->wc->app);
	delete win;

	this->dialog->autoLabelColour=false;
	this->dialog->LFSTK_reloadGlobals();	
	this->dialog->LFSTK_clearWindow();

//list
	this->fontlist=new LFSTK_listGadgetClass(this->dialog,"",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE*5);
	this->loadFontStrings();
	
	for(int j=0;j<this->maxFonts;j++)
		{
			ls.label=this->fontsAZV.at(j);
			ls.imageType=NOTHUMB;
			fontlist->LFSTK_appendToList(ls);
		}

	this->fontlist->LFSTK_updateList();
	this->fontlist->LFSTK_setMouseCallBack(NULL,select,this);
	sy+=GADGETHITE*5;

//bold
	sy+=GADGETHITE;
	this->boldcheck=new LFSTK_toggleButtonClass(this->dialog,"Bold",BORDER,sy,GADGETWIDTH,CHECKBOXSIZE,NorthGravity);
	this->boldcheck->LFSTK_setMouseCallBack(NULL,select,(void*)this);

//italic
	this->italiccheck=new LFSTK_toggleButtonClass(this->dialog,"Italic",BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,CHECKBOXSIZE,NorthGravity);
	this->italiccheck->LFSTK_setMouseCallBack(NULL,select,(void*)this);

//font size
	snprintf((char*)&sizestr,3,"%i",size);
	this->fontsize=new LFSTK_lineEditClass(this->dialog,(const char*)sizestr,BORDER*2+GADGETWIDTH+BORDER*2+GADGETWIDTH,sy-CHECKBOXSIZE/2,GADGETHITE,GADGETHITE,NorthGravity);
	this->fontsize->LFSTK_setKeyCallBack(NULL,select,(void*)this);
	this->fontsize->LFSTK_setCallbackOnReturn(false);
	sy+=YSPACING;

//preview
	this->preview=new LFSTK_multiLineEditClass(this->dialog,fontTest,BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE*6,NorthGravity);
	sy+=GADGETHITE*6;

//line
	seperator=new LFSTK_buttonClass(this->dialog,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&this->dialog->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//apply
	apply=new LFSTK_buttonClass(this->dialog,"Apply",BORDER,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);

//cancel
	cancel=new LFSTK_buttonClass(this->dialog,"Cancel",DIALOGWIDTH-BORDER-GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	sy+=YSPACING;

	this->dialog->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);

	XSizeHints sh;
	sh.flags=PMinSize|PMaxSize|PSize|PResizeInc;
	sh.min_width=DIALOGWIDTH;
	sh.min_height=sy;
	sh.max_width=DIALOGWIDTH;
	sh.max_height=sy;
	sh.width_inc=0;
	sh.height_inc=0;
	sh.base_width=DIALOGWIDTH;
	sh.base_width=sy;

	XSetWMNormalHints(this->wc->app->display,dialog->window,&sh);
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
LFSTK_fontDialogClass::LFSTK_fontDialogClass(LFSTK_windowClass* parentwc,const char* label,int x,int y,unsigned w,unsigned h,int gravity)
{
	XSetWindowAttributes	wa;
	windowInitStruct		*win;

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;
	this->window=XCreateWindow(this->wc->app->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	this->gc=XCreateGC(this->wc->app->display,this->window,0,NULL);
	this->wc->globalLib->LFSTK_setCairoSurface(this->wc->app->display,this->window,this->wc->app->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->wc->app->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=BUTTONGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
	else
		this->useTile=false;

	fontData.fontString="";
	fontData.fontName="";
	fontData.bold=false;
	fontData.italic=false;
	fontData.isValid=false;

	this->buildDialog();
	gadgetDetails={&this->newGadgetBGColours.at(NORMALCOLOUR),BEVELOUT,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false,false,false};
}
