#if 0

#©keithhedger Sat 5 Aug 17:26:40 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o dialogsexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./dialogsexample "$@"
retval=$?
echo "Exit code $retval"
rm dialogsexample
exit $retval
#endif

#include "lfstk/LFSTKGlobals.h"

#define BOXLABEL			"File/Folder Dialogs"

LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_buttonClass			*filebutton;
LFSTK_buttonClass			*folderbutton;
LFSTK_fileDialogClass		*filedialogfile;
LFSTK_fileDialogClass		*filedialogdir;

bool						mainLoop=true;
Display						*display;
char						*wd;

bool doQuit(void *p,void* ud)
{
	mainLoop=false;
	XFlush(wc->display);
	XSync(wc->display,true);
	return(false);
}

//use wd =~
bool selectfile(void *object,void* ud)
{
	filedialogfile->LFSTK_showFileDialog(wd,"Select A File");
	if(filedialogfile->LFSTK_isValid()==true)
		{
			printf("Selected File Path=%s\n",filedialogfile->LFSTK_getCurrentPath());
			printf("Selected Dir Path=%s\n",filedialogfile->LFSTK_getCurrentDir());
			printf("Selected File Name=%s\n",filedialogfile->LFSTK_getCurrentFile());
			printf("File Mime-Type=%s\n",wc->globalLib->LFSTK_getMimeType(filedialogfile->LFSTK_getCurrentPath()));

			free(wd);
			wd=strdup(filedialogfile->LFSTK_getCurrentDir());			
		}
	return(true);
}

//get last used folder
bool selectdir(void *object,void* ud)
{
	filedialogdir->LFSTK_showFileDialog(NULL,"Select A Folder");
	if(filedialogdir->LFSTK_isValid()==true)
		{
			printf("Selected Folder=%s\n",filedialogdir->LFSTK_getCurrentDir());
			free(wd);
			wd=strdup(filedialogdir->LFSTK_getCurrentDir());			
		}
	return(true);
}

int main(int argc, char **argv)
{
	XEvent	event;
	int		sy=BORDER;
		
	wc=new LFSTK_windowClass(0,0,DIALOGWIDTH,DIALOGHITE,"File/Folder Select",false);
	display=wc->display;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//files and folders
	asprintf(&wd,"%s",wc->userHome);
	filedialogfile=new LFSTK_fileDialogClass(wc,"Select File",wd,FILEDIALOG);
	filedialogdir=new LFSTK_fileDialogClass(wc,"Select Folder",NULL,FOLDERDIALOG,"dialogscpp");

//	filedialogdir->LFSTK_setNameFilter("xfwm4");
//TODO
//	filedialogfile->LFSTK_setShowPreview(false);

//file button
	filebutton=new LFSTK_buttonClass(wc,"Select File",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	filebutton->LFSTK_setCallBack(NULL,selectfile,NULL);
	sy+=YSPACING;

//folder button
	folderbutton=new LFSTK_buttonClass(wc,"Select Folder",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	folderbutton->LFSTK_setCallBack(NULL,selectdir,NULL);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setCallBack(NULL,doQuit,NULL);
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	mainLoop=true;
	while(mainLoop==true)
		{
			XNextEvent(wc->display,&event);
			mappedListener *ml=wc->LFSTK_getMappedListener(event.xany.window);

			if(ml!=NULL)
				ml->function(ml->gadget,&event,ml->type);

			if(wc->LFSTK_handleWindowEvents(&event)<0)
				mainLoop=false;
		}

	delete wc;
	XCloseDisplay(display);
	cairo_debug_reset_static_data();
	return 0;
}