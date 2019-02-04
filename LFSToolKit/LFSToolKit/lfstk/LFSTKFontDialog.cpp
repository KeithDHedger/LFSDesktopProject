

#include "lfstk/LFSTKGlobals.h"

LFSTK_fontDialogClass::~LFSTK_fontDialogClass()
{
	for(int j=0;j<this->maxFonts;j++)
		{
			free(this->fontsAZ[j]);
		}
	delete[] this->fontsAZ;
	free(this->fontData.fontString);
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
	char		*formatstring=NULL;
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
	this->selectedFontNumber=fontlist->LFSTK_getCurrentListItem();

	if(this->fontData.fontString!=NULL)
		free(this->fontData.fontString);
	asprintf(&this->fontData.fontString,"%s:size=%s%s%s",this->fontsAZ[this->selectedFontNumber],this->fontsize->LFSTK_getCStr(),boldstr,italicstr);
	asprintf(&formatstring,"ns%s%s",dobold,doitalic);
	this->preview->LFSTK_setCairoFontDataParts(formatstring,this->fontsAZ[selectedFontNumber],atoi(this->fontsize->LFSTK_getCStr()));
	this->preview->LFSTK_clearWindow();

	this->fontData.fontString;
	this->fontData.fontName=this->fontsAZ[selectedFontNumber];
	this->fontData.isValid=true;
	free(formatstring);
	this->preview->LFSTK_clearWindow();
}

bool select(void *object,void* userdata)
{
	LFSTK_fontDialogClass	*fd=static_cast<LFSTK_fontDialogClass*>(userdata);
	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);

	fd->LFSTK_getFontData(true);
	return(true);
}

bool checkCB(void *p,void* ud)
{
	LFSTK_fontDialogClass	*fdata=static_cast<LFSTK_fontDialogClass*>(ud);

	fdata->LFSTK_getFontData(true);
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
	unsigned	fontcnt=0;
	FILE		*fp;
	char		*command;
	char		*out=NULL;
	char		line[1024];

	line[0]=0;

	asprintf(&command,"%s","fc-list : family|awk -F, '{print $1}'|sort -u|wc -l");
	out=wc->globalLib->LFSTK_oneLiner("%s",command);
	if(out==NULL)
		fontcnt=0;
	else
		{
			fontcnt=atoi(out);
			free(out);
		}
	free(command);

	fontsAZ=new char*[fontcnt];
	maxFonts=fontcnt;

	asprintf(&command,"%s","fc-list : family|awk -F, '{print $1}'|sort -ur");
	fp=popen(command, "r");
	if(fp!=NULL)
		{
			while(fgets(line,1024,fp))
				{
					fontcnt--;
					line[strlen(line)-1]=0;
					fontsAZ[fontcnt]=strdup(line);
				}
			pclose(fp);
		}
	free(command);
}

bool LFSTK_fontDialogClass::LFSTK_showDialog(const char* font)
{
	bool	mainLoop=true;
	XEvent	event;
	geometryStruct	geomfont;
	pointStruct		pt;

	this->dialog->LFSTK_showWindow(true);
	this->dialog->LFSTK_setKeepAbove(true);
	this->dialog->LFSTK_setTransientFor(this->wc->window);
	this->mainLoop=true;

//TODO//
	//this->fontsize->LFSTK_setFocus();
	while(this->mainLoop==true)
		{
			XNextEvent(this->dialog->display,&event);
			mappedListener *ml=this->dialog->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);


			if(this->dialog->LFSTK_handleWindowEvents(&event)<0)
				mainLoop=false;

			if(ml==NULL)
				continue;

			switch(event.type)
				{
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
		}
	this->dialog->LFSTK_hideWindow();
	return(true);
}

void LFSTK_fontDialogClass::buildDialog(void)
{
	int				sy=BORDER;
	char			*sizestr[4]={0,};

	dialog=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"Font Selector",false);
	dialog->closeDisplayOnExit=true;
	this->dialog->autoLabelColour=false;
	this->dialog->LFSTK_reloadGlobals();	
	this->dialog->LFSTK_clearWindow();
//list
	this->fontlist=new LFSTK_listGadgetClass(this->dialog,"",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE*5,BUTTONGRAV,NULL,0);
	this->loadFontStrings();
	labelLst=labelLst=new listLabelStruct*[this->maxFonts];
	for(int j=0;j<this->maxFonts;j++)
		{
			labelLst[j]=new listLabelStruct;
			labelLst[j]->label=strdup(this->fontsAZ[j]);
			labelLst[j]->imageType=NOTHUMB;
		}

	this->fontlist->LFSTK_setList(labelLst,this->maxFonts);
	this->fontlist->LFSTK_setCallBack(NULL,select,this);
	sy+=GADGETHITE*5;

//bold
	sy+=GADGETHITE;
	this->boldcheck=new LFSTK_toggleButtonClass(this->dialog,"Bold",BORDER,sy,GADGETWIDTH,CHECKBOXSIZE,NorthWestGravity);
	this->boldcheck->LFSTK_setCallBack(NULL,select,(void*)this);

//italic
	this->italiccheck=new LFSTK_toggleButtonClass(this->dialog,"Italic",BORDER*2+GADGETWIDTH,sy,GADGETWIDTH,CHECKBOXSIZE,NorthWestGravity);
	this->italiccheck->LFSTK_setCallBack(NULL,select,(void*)this);
	//sy-=CHECKBOXSIZE;

//font size
	snprintf((char*)&sizestr,3,"%i",size);
	fontsize=new LFSTK_lineEditClass(this->dialog,(const char*)sizestr,BORDER*2+GADGETWIDTH+BORDER*2+GADGETWIDTH,sy-CHECKBOXSIZE/2,GADGETHITE,GADGETHITE,BUTTONGRAV);
	this->fontsize->LFSTK_setCallBack(select,NULL,(void*)this);
	sy+=YSPACING;

//preview
	this->preview=new LFSTK_lineEditClass(this->dialog,"ABCDEFGHIJKabcdefjhijk1234567890",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE*3,BUTTONGRAV);
	sy+=YSPACING;
	sy+=GADGETHITE*2;

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

	this->LFSTK_setCommon(parentwc,label,x,y,w,h,gravity);

	wa.win_gravity=gravity;
	wa.save_under=true;
	this->window=XCreateWindow(this->display,this->parent,x,y,w,h,0,CopyFromParent,InputOutput,CopyFromParent,CWWinGravity,&wa);
	this->gc=XCreateGC(this->display,this->window,0,NULL);
	this->wc->globalLib->LFSTK_setCairoSurface(this->display,this->window,this->visual,&this->sfc,&this->cr,w,h);
	this->LFSTK_setCairoFontData();
	XSelectInput(this->display,this->window,ButtonReleaseMask | ButtonPressMask | ExposureMask | EnterWindowMask | LeaveWindowMask);

	this->ml->function=&LFSTK_lib::LFSTK_gadgetEvent;
	this->ml->gadget=this;
	this->ml->type=BUTTONGADGET;
	this->wc->LFSTK_addMappedListener(this->window,ml);

	if(this->wc->globalLib->LFSTK_getUseTheme()==true)
		this->LFSTK_setTile(this->wc->globalLib->LFSTK_getGlobalString(-1,TYPEBUTTONTILE),-1);
	else
		this->useTile=false;

	fontData.fontString=NULL;
	fontData.fontName=NULL;
	fontData.bold=false;
	fontData.italic=false;
	fontData.isValid=false;

	this->buildDialog();
	gadgetDetails={&this->colourNames[NORMALCOLOUR],BEVELOUT,NOINDICATOR,NORMALCOLOUR,0,true,{0,0,w,h},{0,0,0,0},false};
}
