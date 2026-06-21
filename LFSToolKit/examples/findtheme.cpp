#if 0

# keithhedger Fri 28 Jul 20:19:00 BST 2017 kdhedger68713@gmail.com

if [ "X$1" != "X" ];then
	USEVALGRIND="valgrind --leak-check=full"
fi

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo glib-2.0 ) -llfstoolkit -lImlib2 -o findexample||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $USEVALGRIND ./findexample "$@"

retval=$?
echo "Exit code $retval"
rm findexample
exit $retval
#endif

#include <stdio.h>
#include <stdlib.h>

#include "../config.h"
#include "../LFSToolKit/lfstk/LFSTKGlobals.h"

char	*themeDir=NULL;

int main(int argc, char **argv)
{

	LFSTK_findClass	*fc=new LFSTK_findClass;

	//asprintf(&themeDir,"%s","/usr/share/themes");
	asprintf(&themeDir,"%s","/tmp/Gold");
	//fc->LFSTK_setDepth(100,1000);
	//fc->LFSTK_setFindType(ANYTYPE);
	//fc->LFSTK_setFollowLinks(false);
	//fc->LFSTK_setIncludeHidden(false);
	//fc->LFSTK_setIgnoreNavLinks(true);
	//fc->LFSTK_setFileTypes("");
	fc->LFSTK_setIgnoreFolders(false);
	fc->LFSTK_findFiles(themeDir,true);
	fc->LFSTK_sortByName();
	for(int j=0;j<fc->data.size();j++)
		{
			fprintf(stderr,"j=%i name=%s path=%s type=%i\n",j,fc->data.at(j).name.c_str(),fc->data.at(j).path.c_str(),fc->data.at(j).fileType);
		}
	fprintf(stderr,"Number of items=%i\n",fc->LFSTK_getDataCount());

	delete fc;
	free(themeDir);
	return 0;
}

