#if 0

# (c)keithhedger Tue 9 Jan 13:48:05 GMT 2024 kdhedger68713@gmail.com

USEVALGRIND="valgrind --leak-check=full"

APPNAME=$(basename $0 .cpp)

g++ "$0" -O0 -ggdb $(pkg-config --cflags --libs x11 xft cairo lfstk ) -llfstoolkit -lImlib2 -o $APPNAME ||exit 1
$USEVALGRIND ./$APPNAME "$@"

retval=$?
echo "Exit code $retval"
rm $APPNAME
exit $retval
#endif

#include "lfstk/LFSTKGlobals.h"

int main(int argc, char **argv)
{
	LFSTK_applicationClass	*apc=new LFSTK_applicationClass();
	propReturn				pr1;

	pr1=apc->globalLib->LFSTK_getSingleProp(apc->display,apc->rootWindow,XInternAtom(apc->display,"_NET_DESKTOP_NAMES",false),XInternAtom(apc->display,"UTF8_STRING",false));

	std::cout<<"number of items returned="<<pr1.strlist.size()<<std::endl;
	for(int j=0;j<pr1.strlist.size();j++)
		std::cout<<pr1.strlist.at(j)<<std::endl;

	delete apc;
	cairo_debug_reset_static_data();
	return(0);
}