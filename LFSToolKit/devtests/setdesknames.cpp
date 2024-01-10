#if 0

# (c)keithhedger Tue 9 Jan 13:48:05 GMT 2024 kdhedger68713@gmail.com

#USEVALGRIND="valgrind --leak-check=full"

APPNAME=$(basename $0 .cpp)

g++ "$0" -O0 -ggdb $(pkg-config --cflags --libs x11 xft cairo lfstk ) -llfstoolkit -lImlib2 -o $APPNAME ||exit 1
$USEVALGRIND ./$APPNAME "$@"

retval=$?
echo "Exit code $retval"
rm $APPNAME
exit $retval
#endif

#include "lfstk/LFSTKGlobals.h"
#include <memory>

int main(int argc, char **argv)
{
	std::vector<std::string>	tokenstrings;
	const char				*x1;
	std::string				x="";
	int						totallen=0;
	LFSTK_applicationClass	*apc=new LFSTK_applicationClass();
	std::string				names="w1,w2,w3,w4,w5,w6";

	tokenstrings=LFSTK_UtilityClass::LFSTK_strTok(names,",");
	for(int j=0;j<tokenstrings.size();j++)
		{
			totallen+=tokenstrings.at(j).length()+1;
			x+=tokenstrings.at(j)+'\0';
		}

	x1=x.c_str();
	XChangeProperty(apc->display,apc->rootWindow,XInternAtom(apc->display,"_NET_DESKTOP_NAMES",false),
	XInternAtom(apc->display,"UTF8_STRING",false),
	8,
	PropModeReplace,
	(const unsigned char*)*&x1
	,totallen
	);

	XSync(apc->display,false);
	//system("xprop -root _NET_DESKTOP_NAMES");
	delete apc;
	cairo_debug_reset_static_data();
	return(0);
}