#if 0

#©keithhedger Sat 5 Aug 16:52:45 BST 2017 kdhedger68713@gmail.com

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

#include "lfstk/LFSTKGlobals.h"

LFSTK_windowClass			*wc=NULL;
LFSTK_fontDialogClass		*fontdialog=NULL;
Display						*display;

int main(int argc, char **argv)
{
	const fontDataStruct	*fd;
	const char				*bools[]={"false","true"};
	
	wc=new LFSTK_windowClass(0,0,1,1,"Gadgets",false);
	display=wc->display;

	fontdialog=new LFSTK_fontDialogClass(wc,"Select Font",0,0,1,1,BUTTONGRAV);

	wc->LFSTK_setWindowType("_NET_WM_WINDOW_TYPE_DOCK");
	wc->LFSTK_showWindow();

	fontdialog->LFSTK_showDialog("Helvetica:size=18:bold:italic");
	fd=fontdialog->LFSTK_getFontData(false);
	if(fd->isValid==true)
		printf("Font String:%s\nFont:%s\nSize:%i\nBold:%s\nItalic:%s\n",fd->fontString,fd->fontName,fd->fontSize,bools[fd->bold],bools[fd->italic]);
		
	delete wc;
	XCloseDisplay(display);
	cairo_debug_reset_static_data();
	return 0;
}