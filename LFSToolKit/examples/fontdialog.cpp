#if 0

#(c)keithhedger Sat 5 Aug 16:52:45 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o fontdialogexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./fontdialogexample "$@"
retval=$?
echo "Exit code $retval"
rm fontdialogexample
exit $retval
#endif

#include "../config.h"
#include "lfstk/LFSTKGlobals.h"

LFSTK_applicationClass		*apc=NULL;
LFSTK_windowClass			*wc=NULL;
LFSTK_fontDialogClass		*fontdialog=NULL;

int main(int argc, char **argv)
{
	const fontDataStruct	*fd;
	const char				*bools[]={"false","true"};
	
	apc=new LFSTK_applicationClass();
	apc->LFSTK_addWindow(NULL,NULL);
	wc=apc->mainWindow;

	fontdialog=new LFSTK_fontDialogClass(wc,"Select Font",0,0,1,1,BUTTONGRAV);

	fontdialog->LFSTK_showDialog("Helvetica:size=18:bold:italic");
	fd=fontdialog->LFSTK_getFontData(false);
	if(fd->isValid==true)
		printf("Font String:%s\nFont:%s\nSize:%i\nBold:%s\nItalic:%s\n",fd->fontString,fd->fontName,fd->fontSize,bools[fd->bold],bools[fd->italic]);
		
	delete apc;
	cairo_debug_reset_static_data();
	return(0);
}