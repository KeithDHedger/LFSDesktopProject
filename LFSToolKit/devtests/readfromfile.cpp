#if 0

# keithhedger Wed 20 Dec 16:18:20 GMT 2023 kdhedger68713@gmail.com


if [[ $USEVALGRIND -eq 1 ]];then
	VALGRIND="valgrind --leak-check=full "
fi

APPNAME=$(basename $0 .cpp)

g++ "$0" -O0 -ggdb -I../LFSToolKit -L../LFSToolKit/app/.libs $(pkg-config --cflags --libs x11 xft cairo ) -llfstoolkit -lImlib2 -o $APPNAME||exit 1
LD_LIBRARY_PATH=../LFSToolKit/app/.libs $VALGRIND ./$APPNAME "$@"

retval=$?
echo "Exit code $retval"
rm $APPNAME
exit $retval

#endif

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#include "lfstk/LFSTKGlobals.h"

int main(int argc, char **argv)
{
	std::ifstream	myfile ;
	std::ofstream	myoutfile;
	std::streambuf	*buf;
	bool				tostderr=true;

	myfile.open(argv[1], std::fstream::in);
	
	if(myfile.is_open())
        {
			if(tostderr==true)
				{
					myoutfile.open("/tmp/test.txt",std::ofstream::out);
				    buf=myoutfile.rdbuf();
				}
			else
				{
					buf=std::cout.rdbuf();
				}
			std::ostream out(buf);
			std::string keyname;
			std::string data;
			while(myfile>>keyname)
				{
					std::getline(myfile, data);
					//std::cout << "keyname="<<keyname<<" data="<<LFSTK_UtilityClass::LFSTK_strStrip(data)<<std::endl;
					out  << "keyname="<<keyname<<" data="<<LFSTK_UtilityClass::LFSTK_strStrip(data)<<std::endl;
				}
			myfile.close();
			myoutfile.close();
		}

	return(0);
}