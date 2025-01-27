#if 0

pushd ../
	make
popd

if [ "X$USEVALGRIND" != "X" ];then
	VALGRIND="valgrind --leak-check=full"
fi

APPNAME=$(basename $0 .cpp)
# Run as for instance ./PrefsTest.cpp --colour="green" --gravity=3 -v0 --theme="mytheme"
g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo glib-2.0 imlib2) -llfstoolkit -o $APPNAME||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $VALGRIND ./$APPNAME "$@"

retval=$?
echo "Exit code $retval"
rm $APPNAME
exit $retval
#endif

#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"Basic LFSTK App"

LFSTK_applicationClass	*apc=NULL;
LFSTK_windowClass		*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass		*seperator=NULL;
LFSTK_buttonClass		*quit=NULL;
LFSTK_buttonClass		*test=NULL;
LFSTK_prefsClass			prefs;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool buttonCB(void *p,void* ud)
{
	LFSTK_buttonClass	*button=NULL;

	fprintf(stderr,"button=%p ud=%p\n",p,ud);
	return(true);
}

int cnt=1;
bool timerCB(LFSTK_applicationClass *p,void* ud)
{
	printf("From window %s\n",ud);
	printf("Timer callback number %i of 10\n",cnt);
	cnt++;
	if(cnt<11)
		return(true);
	else
		return(false);
}

void setPrefs(int argc, char **argv)
{
	prefs.prefsMap=
		{
			{LFSTK_UtilityClass::LFSTK_hashFromKey("theme"),{TYPESTRING,"theme","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("colour"),{TYPESTRING,"colour","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("gravity"),{TYPEINT,"gravity","",false,0}},
			{LFSTK_UtilityClass::LFSTK_hashFromKey("vertical"),{TYPEBOOL,"vertical","",false,0}},
		};

	prefs.LFSTK_loadVarsFromFile("./testprefs.rc");
}

void printPrefs(void)
{
	prefs.LFSTK_saveVarsToFile("-");
}

int main(int argc, char **argv)
{
	int					sy=BORDER;
	LFSTK_buttonClass	*button=NULL;

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKExample");
	wc=apc->mainWindow;
	wc->userData=USERDATA("Main Window");

	label=new LFSTK_labelClass(apc->windows->back().window,BOXLABEL,0,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	label->LFSTK_setTile(NULL,0);

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//test
	test=new LFSTK_buttonClass(wc,"Somthing",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	test->LFSTK_setMouseCallBack(NULL,buttonCB,(void*)(apc->windows->size()-2));
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());

	//apc->LFSTK_setTimer(1);
	//apc->LFSTK_setTimerCallBack(timerCB,NULL);
	fprintf(stderr,"Loading prefs from file ...\n");
	setPrefs(argc,argv);
	printPrefs();

	option long_options[]=
		{
			{"theme",required_argument,NULL,'t'},
			{"colour",required_argument,NULL,'c'},
			{"gravity",required_argument,NULL,'g'},
			{"vertical",optional_argument,NULL,'v'},
			{"help",no_argument,0,'?'},
			{0,0,0,0}
		};

	fprintf(stderr,"\nOverriding prefs from commandline ...\n");
	prefs.LFSTK_argsToPrefs(argc,argv,long_options);
	printPrefs();

	int retval=apc->LFSTK_runApp();

	delete apc;
	cairo_debug_reset_static_data();
	return retval;
}