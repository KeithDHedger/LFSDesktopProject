#if 0

#(c)keithhedger Sat 5 Aug 17:26:40 BST 2017 kdhedger68713@gmail.com

echo "Compiling, please wait ..."

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo  glib-2.0) -llfstoolkit -lImlib2 -o dialogsexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./dialogsexample "$@"
retval=$?
echo "Exit code $retval"
rm dialogsexample
exit $retval
#endif

#include "../config.h"
#include <lfstk/LFSTKGlobals.h>

#define BOXLABEL			"File/Folder Dialogs"

LFSTK_applicationClass		*apc=NULL;
LFSTK_windowClass			*wc=NULL;
LFSTK_labelClass			*label=NULL;
LFSTK_labelClass			*personal=NULL;
LFSTK_labelClass			*copyrite=NULL;
LFSTK_buttonClass			*seperator=NULL;
LFSTK_buttonClass			*quit=NULL;
LFSTK_buttonClass			*filebutton;
LFSTK_buttonClass			*folderbutton;
LFSTK_buttonClass			*colourbutton;
LFSTK_fileDialogClass		*filedialogfile;
LFSTK_fileDialogClass		*filedialogdir;

char						*wd;

bool doQuit(void *p,void* ud)
{
	apc->exitValue=0;
	apc->mainLoop=false;
	return(false);
}

bool selectcol(void *object,void* ud)
{
	system("./colourchooser.cpp;echo");
	return(true);
}

//use wd =~
bool selectfile(void *object,void* ud)
{
	std::string	mimetype;
	filedialogfile->LFSTK_showFileDialog(NULL,"Select A File");

	if(filedialogfile->LFSTK_isValid()==true)
		{
			printf("Selected File Path=%s\n",filedialogfile->LFSTK_getCurrentPath().c_str());
			printf("Selected Dir Path=%s\n",filedialogfile->LFSTK_getCurrentDir().c_str());
			printf("Selected File Name=%s\n",filedialogfile->LFSTK_getCurrentFile().c_str());
			mimetype=wc->globalLib->LFSTK_getMimeType(filedialogfile->LFSTK_getCurrentPath());
			printf("File Mime-Type=%s\n",mimetype.c_str());
			free(wd);
			wd=strdup(filedialogfile->LFSTK_getCurrentDir().c_str());
		}
	return(true);
}

bool fbup(void *object,void* ud)
{
	LFSTK_fileDialogClass *fd=static_cast<LFSTK_fileDialogClass*>(ud);
	LFSTK_listGadgetClass *list=static_cast<LFSTK_listGadgetClass*>(object);

	
	fprintf(stderr,"fbup=%p\n",ud);
	return(true);
}

//get last used folder
bool selectdir(void *object,void* ud)
{
	filedialogdir->LFSTK_showFileDialog(NULL,"Select A Folder");
	if(filedialogdir->LFSTK_isValid()==true)
		{
			printf("Selected Folder=%s\n",filedialogdir->LFSTK_getCurrentDir().c_str());
			free(wd);
			wd=strdup(filedialogdir->LFSTK_getCurrentDir().c_str());			
		}
	return(true);
}

int main(int argc, char **argv)
{
	int	sy=BORDER;

	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,BOXLABEL,"LFSTKExample");
	wc=apc->mainWindow;

	label=new LFSTK_labelClass(wc,BOXLABEL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	label->LFSTK_setCairoFontDataParts("sB",20);
	sy+=YSPACING;

	copyrite=new LFSTK_labelClass(wc,COPYRITE,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	sy+=HALFYSPACING;
	personal=new LFSTK_labelClass(wc,PERSONAL,BORDER,sy,DIALOGWIDTH-BORDER-BORDER,GADGETHITE);
	personal->LFSTK_setCairoFontDataParts("B");
	sy+=YSPACING;

//files and folders
	asprintf(&wd,"%s",apc->userHome.c_str());
	filedialogfile=new LFSTK_fileDialogClass(wc,"Select File",wd,FILEDIALOG);
	filedialogfile->LFSTK_setNameFilter("*");
	//filedialogfile->useThumbs=true;
	//filedialogfile=new LFSTK_fileDialogClass(wc,"Select File",NULL,FILEDIALOG);
	filedialogdir=new LFSTK_fileDialogClass(wc,"Select Folder",NULL,FOLDERDIALOG,"dialogscpp");
	
//	filedialogdir->LFSTK_setNameFilter("xfwm4");
//TODO
//	filedialogfile->LFSTK_setShowPreview(false);

//file button
	filebutton=new LFSTK_buttonClass(wc,"Select File",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	filebutton->LFSTK_setMouseCallBack(NULL,selectfile,NULL);
	sy+=YSPACING;

//folder button
	folderbutton=new LFSTK_buttonClass(wc,"Select Folder",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	folderbutton->LFSTK_setMouseCallBack(NULL,selectdir,NULL);
	sy+=YSPACING;

//colour button
	colourbutton=new LFSTK_buttonClass(wc,"Select Colour",DIALOGMIDDLE-GADGETWIDTH,sy,GADGETWIDTH*2,GADGETHITE,BUTTONGRAV);
	colourbutton->LFSTK_setMouseCallBack(NULL,selectcol,NULL);
	sy+=YSPACING;

//line
	seperator=new LFSTK_buttonClass(wc,"--",0,sy,DIALOGWIDTH,GADGETHITE,BUTTONGRAV);
	seperator->LFSTK_setStyle(BEVELNONE);
	seperator->gadgetDetails.buttonTile=false;
	seperator->gadgetDetails.colour=&wc->windowColourNames[NORMALCOLOUR];
	sy+=YSPACING;

//quit
	quit=new LFSTK_buttonClass(wc,"Quit",DIALOGMIDDLE-HALFGADGETWIDTH,sy,GADGETWIDTH,GADGETHITE,BUTTONGRAV);
	quit->LFSTK_setMouseCallBack(NULL,doQuit,NULL);
	sy+=YSPACING;

	wc->LFSTK_resizeWindow(DIALOGWIDTH,sy,true);
	wc->LFSTK_showWindow();

	printf("Number of gadgets in window=%i\n",wc->LFSTK_gadgetCount());
	int retval=apc->LFSTK_runApp();
	delete filedialogfile;
	delete filedialogdir;
	delete apc;
	free(wd);
	cairo_debug_reset_static_data();
	return(retval);
}