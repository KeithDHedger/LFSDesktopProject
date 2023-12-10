#if 0

#(c)keithhedger Thu 17 Aug 16:30:38 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o ./lfsruncommandexample ||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./lfsruncommandexample "$@"
retval=$?
echo "Exit code $retval"
rm lfsruncommandexample
exit $retval
#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

#undef DIALOGWIDTH
#define DIALOGWIDTH		800
#define LISTHITE		GADGETHITE * 8
#define MAXHISTORY		40
#define BOXLABEL		"Run Command"

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_listGadgetClass	*list=NULL;
LFSTK_lineEditClass		*le=NULL;

const char				*commandToRun=NULL;
char					*commandfile;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool doExecute(void *object,void* ud)
{
	char	*data;

	asprintf(&data,"%s",le->LFSTK_getCStr());
	if(strlen(data)>2)
		{
			apc->exitValue=system(data);
			free(data);

			asprintf(&data,"echo %s >> %s",le->LFSTK_getCStr(),commandfile);
			system(data);
			free(data);
			asprintf(&data,"tail -n %i %s|sort -u -o %s.tmp; mv %s.tmp %s",MAXHISTORY,commandfile,commandfile,commandfile,commandfile);
			system(data);
		}
	free(data);

	apc->mainLoop=false;
	return(true);
}

bool select(void *object,void* ud)
{
	LFSTK_listGadgetClass	*list=static_cast<LFSTK_listGadgetClass*>(object);

	commandToRun=list->LFSTK_getSelectedLabel();
	le->LFSTK_setBuffer(commandToRun);
	return(true);
}

int main(int argc, char **argv)
{
	int 				sy=0;
	LFSTK_windowClass	*wc;
	LFSTK_buttonClass	*quit;
	LFSTK_buttonClass	*run;
	LFSTK_labelClass	*label;

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKExample");
	wc=apc->mainWindow;

	label=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	label=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

	list=list=new LFSTK_listGadgetClass(wc,"",BORDER,sy,DIALOGWIDTH-(BORDER*2)-LGAP,LISTHITE);

	asprintf(&commandfile,"%s/%s",apc->configDir.c_str(),"command.hist");
	list->LFSTK_setListFromFile(commandfile,false);
	list->LFSTK_setMouseCallBack(NULL,select,NULL);
	sy+=LISTHITE+8;

//command
	le=new LFSTK_lineEditClass(wc,"",BORDER,sy,DIALOGWIDTH-(BORDER*2),GADGETHITE);
	sy+=GADGETHITE+8;

	quit=new LFSTK_buttonClass(wc,"Quit",BORDER,sy,GADGETWIDTH,GADGETHITE,NorthWestGravity);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);

	run=new LFSTK_buttonClass(wc,"Execute",DIALOGWIDTH-BORDER-GADGETWIDTH,sy,GADGETWIDTH,GADGETHITE);
	run->LFSTK_setMouseCallBack(NULL,doExecute,NULL);
	sy+=GADGETHITE+BORDER;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();
	wc->LFSTK_setKeepAbove(true);

	int retval=apc->LFSTK_runApp();
	free(commandfile);
	delete apc;
	printf("Command returned %i\n",retval);
	cairo_debug_reset_static_data();

	return(retval);
}
