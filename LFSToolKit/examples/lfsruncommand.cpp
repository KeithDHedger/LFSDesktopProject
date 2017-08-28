#if 0

#©keithhedger Thu 17 Aug 16:30:38 BST 2017 kdhedger68713@gmail.com

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o ./runcommandexample ||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs ./runcommandexample "$@"
retval=$?
echo "Exit code $retval"
exit $retval
#endif

#include "lfstk/LFSTKGlobals.h"

#undef DIALOGWIDTH
#define DIALOGWIDTH		800
#define LISTHITE		GADGETHITE * 8
#define MAXHISTORY		40

LFSTK_windowClass		*wc=NULL;
LFSTK_listGadgetClass	*list=NULL;
LFSTK_lineEditClass		*le=NULL;

const char				*commandToRun=NULL;
char					*commandfile;

bool					mainLoop=true;
Display					*display;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	return(false);
}

bool doExecute(void *object,void* ud)
{
	char	*data;

	asprintf(&data,"%s &",le->LFSTK_getBuffer()->c_str());
	if(strlen(data)>2)
		{
			system(data);
			free(data);

			asprintf(&data,"echo %s >> %s",le->LFSTK_getBuffer()->c_str(),commandfile);
			system(data);
			free(data);
			asprintf(&data,"tail -n %i %s|sort -u -o %s.tmp; mv %s.tmp %s",MAXHISTORY,commandfile,commandfile,commandfile,commandfile);
			system(data);
		}
	free(data);
	mainLoop=false;
	return(true);
}

bool select(void *object,void* ud)
{
	commandToRun=static_cast<LFSTK_listGadgetClass*>(object)->LFSTK_getListString(static_cast<LFSTK_listGadgetClass*>(object)->LFSTK_getCurrentListItem());
	le->LFSTK_setBuffer(commandToRun);
	return(true);
}

int main(int argc, char **argv)
{
	XEvent				event;
	int 				sy=0;
	LFSTK_windowClass	*wc;
	LFSTK_buttonClass	*quit;
	LFSTK_buttonClass	*run;
	LFSTK_labelClass	*label;

	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"Run Command",false);
	display=wc->display;

	label=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	sy+=HALFYSPACING;
	label=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE,BUTTONGRAV);
	label->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

	list=list=new LFSTK_listGadgetClass(wc,"",BORDER,sy,DIALOGWIDTH-(BORDER*2)-(NAVBUTTONSIZE)-LGAP,LISTHITE,NorthWestGravity,NULL,0);

	asprintf(&commandfile,"%s/%s",wc->configDir,"command.hist");
	list->LFSTK_setListFromFile(commandfile,false);
	list->LFSTK_setCallBack(NULL,select,NULL);
	sy+=LISTHITE+8;

//command
	le=new LFSTK_lineEditClass(wc,"",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE,NorthWestGravity);
	sy+=GADGETHITE+8;

	quit=new LFSTK_buttonClass(wc,"Quit",BORDER,sy,GADGETWIDTH,GADGETHITE,NorthWestGravity);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);

	run=new LFSTK_buttonClass(wc,"Execute",DIALOGWIDTH-BORDER-GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,NorthEastGravity);
	run->LFSTK_setCallBack(NULL,doExecute,NULL);
	sy+=GADGETHITE+BORDER;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);
			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);
			switch(event.type)
				{
					case LeaveNotify:
						break;
					case Expose:
						wc->LFSTK_clearWindow();
						list->LFSTK_clearWindow();
						break;

					case ConfigureNotify:
						wc->LFSTK_resizeWindow(event.xconfigurerequest.width,event.xconfigurerequest.height);
						list->LFSTK_clearWindow();
						break;

					case ClientMessage:
						if (event.xclient.message_type == XInternAtom(wc->display, "WM_PROTOCOLS", 1) && (Atom)event.xclient.data.l[0] == XInternAtom(wc->display, "WM_DELETE_WINDOW", 1))
							mainLoop=false;
				}
		}

	free(commandfile);
	delete wc;
	XCloseDisplay(display);
	
	return 0;
}
