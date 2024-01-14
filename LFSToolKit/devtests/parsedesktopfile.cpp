#if 0

# keithhedger Wed 20 Dec 16:18:20 GMT 2023 kdhedger68713@gmail.com


if [[ $USEVALGRIND -eq 1 ]];then
	VALGRIND="valgrind --leak-check=full"
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


std::string getentry(std::string keyname,std::vector<std::string> lines)
{
	std::string retval;
	std::vector<std::string>	linestok;
	for(unsigned j=0;j<lines.size();j++)
		{
			if(LFSTK_UtilityClass::LFSTK_strStr(lines.at(j),keyname).empty()==false)
				{
					linestok=LFSTK_UtilityClass::LFSTK_strTok(lines.at(j),"=");
					return(LFSTK_UtilityClass::LFSTK_strStrip(linestok.at(1)));
				}
		}
	return("");
}

int main(int argc, char **argv)
{
	std::ifstream			myfile ;
	std::vector<std::string>	linestok;
	std::vector<std::string>	lines;

	myfile.open(argv[1],std::fstream::in);
	
	if(myfile.is_open())
        {
			std::string data;
			while(std::getline(myfile,data))
				{
					if(data.empty()==false)
						lines.push_back(data);
				}
			myfile.close();
		}

	for(int j=0;j<lines.size();j++)
		std::cerr<<lines.at(j)<<std::endl;

	std::cerr<<"\n"<<std::endl;

//get name
	std::cerr<<"key=name=>"<<getentry("Name",lines)<<"<="<<std::endl;
	std::cerr<<"key=GenericName[ru]=>"<<getentry("GenericName[ru]",lines)<<"<="<<std::endl;
	std::cerr<<"key=exec=>"<<getentry("Exec",lines)<<"<="<<std::endl;
	std::cerr<<"key=Comment[uk]=>"<<getentry("Comment[uk]",lines)<<"<="<<std::endl;
	std::cerr<<"\n"<<std::endl;


	lines=LFSTK_UtilityClass::LFSTK_readDesktopFile(argv[1]);
	std::cerr<<"key=name=>"<<LFSTK_UtilityClass::LFSTK_getEntry("Name",lines)<<"<="<<std::endl;
	std::cerr<<"key=GenericName[ru]=>"<<LFSTK_UtilityClass::LFSTK_getEntry("GenericName[ru]",lines)<<"<="<<std::endl;
	std::cerr<<"key=exec=>"<<LFSTK_UtilityClass::LFSTK_getEntry("Exec",lines)<<"<="<<std::endl;
	std::cerr<<"key=Comment[uk]=>"<<LFSTK_UtilityClass::LFSTK_getEntry("Comment[uk]",lines)<<"<="<<std::endl;

	return(0);
}