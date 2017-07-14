#if 0

#©keithhedger Mon 20 Jul 14:09:10 BST 2015 kdhedger68713@gmail.com
g++ -Wall "$0" -I../LFSToolKit -L../LFSToolKit/app/.libs -llfstoolkit -lXm $(pkg-config --cflags --libs xt xext ice sm x11 xft) -lXm -lXaw3d -lXt -lXext -lICE -lSM -lX11||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs ./a.out "$@"
retval=$?
rm ./a.out
exit $retval
#endif

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "lfstk/LFSTKGlobals.h"

enum {EXIT=0,APPLY,SAVE,FONTLABEL,FONTBOX,MENUFONTLABEL,MENUFONTBOX,THEMELABEL,THEMEBOX,WNORMAL,EWNORMAL,

BNORMAL,ENORMAL,BPRELIGHT,EPRELIGHT,BACTIVE,EACTIVE,BINACTIVE,EINACTIVE,MENUNORM,EMENUNORM,MENUPRELITE,EMENUPRELITE,MENUACTIVE,EMENUACTIVE,MENUINACTIVE,EMENUINACTIVE,

FONTNORMCOL,EFONTNORMCOL,FONTPRELIRECOL,EFONTPRELITECOL,FONTACTIVECOL,EFONTACTIVECOL,FONTINACTIVECOL,EFONTINACTIVECOL,
ITEMFONTNORMCOL,EITEMFONTNORMCOL,ITEMFONTPRELIRECOL,EITEMFONTPRELITECOL,ITEMFONTACTIVECOL,EITEMFONTACTIVECOL,ITEMFONTINACTIVECOL,EITEMFONTINACTIVECOL,
NOMORE};

const char	*buttonnames[]={"Button Normal","Button Prelight","Button Active","Button Inactive"};
const char	*menunames[]={"Menu Normal","Menu Prelight","Menu Active","Menu Inactive"};
const char	*fontnames[]={"Font Normal","Font Prelight","Font Active","Font Inactive"};
const char	*itemfontnames[]={"Menu Font Norm","Menu Font Prel","Menu Font Activ","Menu Font Inact"};

bool					mainloop=false;
LFSTK_windowClass		*wc;
LFSTK_gadgetClass		*bc[NOMORE]={NULL,};

bool callback(void *p,void* ud)
{
	char *env;

	if((long)ud==EXIT)
		{
			mainloop=false;
			return(false);
		}

	switch((long)ud)
		{
			case SAVE:
				asprintf(&env,"%s/.config/LFS/lfstoolkit.rc",getenv("HOME"));
				wc->globalLib->LFSTK_saveVarsToFile(env,wc->globalLib->LFSTK_getTKArgs());
				free(env);
				break;

			case APPLY:
				wc->LFSTK_setWindowColourName(NORMALCOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EWNORMAL])->LFSTK_getBuffer()->c_str()));
				wc->globalLib->LFSTK_setGlobalString(NORMALCOLOUR,TYPEWINDOW,wc->windowColourNames[NORMALCOLOUR].name);
				wc->LFSTK_clearWindow();

				int butonnum=ENORMAL;
				for(int j=NORMALCOLOUR;j<MAXCOLOURS;j++)
					{
						wc->globalLib->LFSTK_setGlobalString(j,TYPEBUTTON,(char*)(static_cast<LFSTK_lineEditClass*>(bc[butonnum])->LFSTK_getBuffer()->c_str()));
						wc->globalLib->LFSTK_setGlobalString(j,TYPEMENUITEM,(char*)(static_cast<LFSTK_lineEditClass*>(bc[butonnum+MENUNORM-BNORMAL])->LFSTK_getBuffer()->c_str()));
						wc->globalLib->LFSTK_setGlobalString(j,TYPEFONTCOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[butonnum+FONTNORMCOL-BNORMAL])->LFSTK_getBuffer()->c_str()));
						wc->globalLib->LFSTK_setGlobalString(j,TYPEMENUITEMFONTCOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[butonnum+ITEMFONTNORMCOL-BNORMAL])->LFSTK_getBuffer()->c_str()));
						butonnum+=2;
					}
				wc->globalLib->LFSTK_setGlobalString(-1,TYPEFONT,(char*)(static_cast<LFSTK_lineEditClass*>(bc[FONTBOX])->LFSTK_getBuffer()->c_str()));
				wc->globalLib->LFSTK_setGlobalString(-1,TYPEMENUITEMFONT,(char*)(static_cast<LFSTK_lineEditClass*>(bc[MENUFONTBOX])->LFSTK_getBuffer()->c_str()));
				//wc->globalLib->LFSTK_setGlobalString(-1,TYPETHEME,(char*)(static_cast<LFSTK_lineEditClass*>(bc[THEMEBOX])->LFSTK_getBuffer()->c_str()));

				
				for(int j=FONTLABEL;j<BNORMAL;j+=2)
					{
						bc[j]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EWNORMAL])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_clearWindow();
					}

				for(int j=BNORMAL;j<MENUNORM;j+=2)
					{
						bc[j]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[j+1])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_setFontString((char*)(static_cast<LFSTK_lineEditClass*>(bc[FONTBOX])->LFSTK_getBuffer()->c_str()));
					}

				for(int j=MENUNORM;j<FONTNORMCOL;j+=2)
					{
						bc[j]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[j+1])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_setFontString((char*)(static_cast<LFSTK_lineEditClass*>(bc[MENUFONTBOX])->LFSTK_getBuffer()->c_str()));
					
					}

				int x=ENORMAL-2;
				int state=0;
				for(int j=FONTNORMCOL;j<ITEMFONTNORMCOL;j+=2)
					{
						bc[j]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[x+=2])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_setFontColourName(NORMALCOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[j+1])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_gadgetClass::LFSTK_setFontColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(state,TYPEFONTCOLOUR));
						state++;
					}

				x=EMENUNORM-2;
				state=0;
				for(int j=ITEMFONTNORMCOL;j<NOMORE;j+=2)
					{
						bc[j]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[x+=2])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_setFontColourName(NORMALCOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[j+1])->LFSTK_getBuffer()->c_str()));
						bc[j]->LFSTK_gadgetClass::LFSTK_setFontColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(state,TYPEMENUITEMFONTCOLOUR));
						state++;
					}


				for(int j=FONTNORMCOL;j<ITEMFONTNORMCOL;j+=2)
					if(bc[j]!=NULL)
						bc[j]->LFSTK_setFontString((char*)(static_cast<LFSTK_lineEditClass*>(bc[FONTBOX])->LFSTK_getBuffer()->c_str()));

				for(int j=ITEMFONTNORMCOL;j<NOMORE;j+=2)
					if(bc[j]!=NULL)
						bc[j]->LFSTK_setFontString((char*)(static_cast<LFSTK_lineEditClass*>(bc[MENUFONTBOX])->LFSTK_getBuffer()->c_str()));

				for(int j=BNORMAL;j<NOMORE;j++)
					if(bc[j]!=NULL)
							bc[j]->LFSTK_clearWindow();
				wc->globalLib->LFSTK_saveVarsToFile("-",wc->globalLib->LFSTK_getTKArgs());
				printf("\n");
				break;
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent			event;
	int				sx=0;
	int				sy=0;
	const geometryStruct	*geom;
	int				bwidth=96;
	int				bhite=24;
	int				spacing=bwidth+10;
	int				vspacing=bhite+10;
	int				col1=10,col2=col1+bwidth+spacing+20,col3=col2+bwidth+spacing+20,col4;

	wc=new LFSTK_windowClass(sx,sy,800,600,"LFS Tool Kit Prefs",false);
	wc->LFSTK_setDecorated(true);
	geom=wc->LFSTK_getWindowGeom();
	bc[EXIT]=new LFSTK_buttonClass(wc,"Exit",10,geom->h-32,64,24,SouthWestGravity);
	bc[EXIT]->LFSTK_setCallBack(NULL,callback,(void*)EXIT);

	bc[APPLY]=new LFSTK_buttonClass(wc,"Apply",geom->w-74,geom->h-32,64,24,SouthEastGravity);
	bc[APPLY]->LFSTK_setCallBack(NULL,callback,(void*)APPLY);

	bc[SAVE]=new LFSTK_buttonClass(wc,"Save",(geom->w/2)-(bwidth/2),geom->h-32,64,24,SouthGravity);
	bc[SAVE]->LFSTK_setCallBack(NULL,callback,(void*)SAVE);

//buttons
	sx=col1;
	sy=10;
	int state=0;	
	for (int j=BNORMAL;j<EINACTIVE+1;j+=2)
		{
			bc[j]=new LFSTK_buttonClass(wc,buttonnames[state],sx,sy,bwidth,24,NorthWestGravity);
			bc[j]->LFSTK_setColourName(state,wc->globalLib->LFSTK_getGlobalString(state,TYPEBUTTON));
			bc[j]->LFSTK_setLabelAutoColour(true);
			sx+=spacing;
			bc[j+1]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(state,TYPEBUTTON),sx,sy-1,bwidth,24,NorthWestGravity);
			sy+=vspacing;
			sx=col1;
			state++;
		}

//menu items
	sx=col2;
	sy=10;
	state=0;	
	for (int j=MENUNORM;j<EMENUINACTIVE+1;j+=2)
		{
			bc[j]=new LFSTK_buttonClass(wc,menunames[state],sx,sy,bwidth,24,NorthWestGravity);
			bc[j]->LFSTK_setColourName(state,wc->globalLib->LFSTK_getGlobalString(state,TYPEBUTTON));
			bc[j]->LFSTK_setLabelAutoColour(true);

			sx+=spacing;
			bc[j+1]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(state,TYPEMENUITEM),sx,sy-1,bwidth,24,NorthWestGravity);
			sy+=vspacing;
			sx=col2;
			state++;
		}

//button font
	sx=col1;
	bc[FONTLABEL]=new LFSTK_buttonClass(wc,"Font",sx,sy,bwidth,24,NorthWestGravity);
	static_cast<LFSTK_buttonClass*>(bc[FONTLABEL])->LFSTK_setStyle(BEVELNONE);
	bc[FONTLABEL]->LFSTK_setLabelAutoColour(true);

	sx+=spacing;
	bc[FONTBOX]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(-1,TYPEFONT),sx,sy-1,(bwidth*2)+spacing+20,24,NorthWestGravity);

//menbu item font
	sy+=vspacing;
	sx=col1;
	bc[MENUFONTLABEL]=new LFSTK_buttonClass(wc,"Menu Item Font",sx,sy,bwidth,24,NorthWestGravity);
	bc[MENUFONTLABEL]->LFSTK_setLabelAutoColour(true);
	static_cast<LFSTK_buttonClass*>(bc[MENUFONTLABEL])->LFSTK_setStyle(BEVELNONE);
	sx+=spacing;
	bc[MENUFONTBOX]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(-1,TYPEMENUITEMFONT),sx,sy-1,(bwidth*2)+spacing+20,24,NorthWestGravity);
	sy+=vspacing;

//font colours
	sx=col1;
	state=0;
	int vhold=sy;
	for (int j=FONTNORMCOL;j<ITEMFONTNORMCOL;j+=2)
		{
			bc[j]=new LFSTK_buttonClass(wc,fontnames[state],sx,sy,bwidth,24,NorthWestGravity);
			bc[j]->LFSTK_setLabelAutoColour(false);

			sx+=spacing;
			bc[j+1]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(state,TYPEFONTCOLOUR),sx,sy-1,bwidth,24,NorthWestGravity);

			bc[j]->LFSTK_gadgetClass::LFSTK_setFontColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(state,TYPEFONTCOLOUR));
			sy+=vspacing;
			sx=col1;
			state++;
		}

//menu item font colours
	sx=col2;
	state=0;
	sy=vhold;
	for (int j=ITEMFONTNORMCOL;j<NOMORE;j+=2)
		{
			bc[j]=new LFSTK_buttonClass(wc,itemfontnames[state],sx,sy,bwidth,24,NorthWestGravity);
			sx+=spacing;
			bc[j+1]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(state,TYPEMENUITEMFONTCOLOUR),sx,sy-1,bwidth,24,NorthWestGravity);
			bc[j]->LFSTK_gadgetClass::LFSTK_setFontColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(state,TYPEMENUITEMFONTCOLOUR));
			sy+=vspacing;
			sx=col2;
			state++;
		}

//window
	sx=col1;
	bc[WNORMAL]=new LFSTK_buttonClass(wc,"Window Colour",sx,sy,bwidth,24,NorthWestGravity);
	bc[WNORMAL]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEWINDOW));
	bc[WNORMAL]->LFSTK_setLabelAutoColour(true);
	static_cast<LFSTK_buttonClass*>(bc[WNORMAL])->LFSTK_setStyle(BEVELNONE);

	sx+=spacing;
	bc[EWNORMAL]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEWINDOW),sx,sy-1,bwidth,24,NorthWestGravity);
	bc[EWNORMAL]->LFSTK_setColourName(INACTIVECOLOUR,(char*)(static_cast<LFSTK_lineEditClass*>(bc[EWNORMAL])->LFSTK_getBuffer()->c_str()));

	sx=col1;
	sy+=vspacing;
	bc[THEMELABEL]=new LFSTK_buttonClass(wc,"Theme",sx,sy,bwidth,24,NorthWestGravity);
	bc[THEMELABEL]->LFSTK_setColourName(INACTIVECOLOUR,wc->globalLib->LFSTK_getGlobalString(NORMALCOLOUR,TYPEWINDOW));
	bc[THEMELABEL]->LFSTK_setLabelAutoColour(true);
	static_cast<LFSTK_buttonClass*>(bc[THEMELABEL])->LFSTK_setStyle(BEVELNONE);

	sx+=spacing;
	//bc[THEMEBOX]=new LFSTK_lineEditClass(wc,wc->globalLib->LFSTK_getGlobalString(-1,TYPETHEME),sx,sy-1,(bwidth*2)+spacing+20,24,NorthWestGravity);

	printf("Current Settings:\n\n");
	callback(NULL,(void*)APPLY);
	printf("\n\n");

	XMapSubwindows(wc->display,wc->window);
	XMapRaised(wc->display,wc->window);
	wc->LFSTK_setKeepAbove(true);
	sy+=(vspacing*2);
	wc->LFSTK_resizeWindow(col3-10,sy);
	
	for(int j=FONTLABEL;j<NOMORE;j+=2)
		bc[j]->LFSTK_setActive(false);

	mainloop=true;
	while(mainloop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			switch(event.type)
				{
					case LeaveNotify:
						//mainloop=inWindow();
						break;
					case Expose:
						wc->LFSTK_setActive(true);
						wc->LFSTK_clearWindow();
						break;
					case ConfigureNotify:
					//printf("configureWindow\n");
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height,false);
						break;
					case ClientMessage:
					case SelectionNotify:
						{
						if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
							wc=false;
						}
						if(wc->acceptDnd==true)
							{
								wc->LFSTK_handleDnD(&event);
							}
						break;
					default:
						//XNextEvent(wc->display,&event);
						break;
				}
		}

	delete wc;
	return 0;
}