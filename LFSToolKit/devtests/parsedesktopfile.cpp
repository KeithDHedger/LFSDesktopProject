#if 0

# keithhedger Wed 20 Dec 16:18:20 GMT 2023 kdhedger68713@gmail.com

pushd ..
	make -j3
popd

if [[ $USEVALGRIND -eq 1 ]];then
	VALGRIND="valgrind --leak-check=full"
fi

if [[ $USEVALGRIND -eq 2 ]];then
	VALGRIND="valgrind --leak-check=full --show-leak-kinds=all"
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
	std::map<unsigned long,std::vector<std::string>>	maplines;
	std::string							mapentry;
	unsigned long						currentkey=0;

	myfile.open(argv[1],std::fstream::in);
	
	if(myfile.is_open())
        {
			std::string data;
			while(std::getline(myfile,data))
				{
					if(data.empty()==false)
						{
							data=LFSTK_UtilityClass::LFSTK_strStrip(data);
							if(data.at(0)=='[')
								{
									std::string t=data;
									mapentry=LFSTK_UtilityClass::LFSTK_strReplaceAllChar(t,"[]","",true);
									currentkey=LFSTK_UtilityClass::LFSTK_hashFromKey(mapentry);
								}
							else
								maplines[currentkey].push_back(data);
							
							lines.push_back(data);
								
						}
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

{
//argv[1]=/usr/share/applications/google-chrome.desktop for instance
	std::cerr<<"\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>-\n"<<std::endl;
	std::map<unsigned long,std::vector<std::string>>	maplines3;
	std::string entry="Desktop Action new-private-window";
	//entry="Desktop Entry";

	maplines3=LFSTK_UtilityClass::LFSTK_readFullDesktopFile(argv[1]);
	//std::cerr<<maplines3.begin()->first<<std::endl;
	//std::cerr<<maplines3.begin()->second.at(0)<<std::endl;
	const char *nd[]={"Name","GenericName","Comment","Exec","Categories","Type","Icon","Terminal","gdfghdfk",NULL};
	int cnt=0;
	std::cerr<<"Without fallback (default)"<<std::endl;
	while(nd[cnt]!=NULL)
		std::cerr<<"entry name="<<entry<<"=>"<<nd[cnt]<<"=>"<<LFSTK_UtilityClass::LFSTK_getFullEntry(entry,nd[cnt++],maplines3)<<"<="<<std::endl;

	std::cerr<<"\nWith fallback"<<std::endl;
	cnt=0;
	while(nd[cnt]!=NULL)
		//std::cerr<<"entry name="<<entry<<"=>"<<nd[cnt]<<"=>"<<LFSTK_UtilityClass::LFSTK_getFullEntry(entry,nd[cnt++],maplines3,true,"Desktop Entry")<<"<="<<std::endl;
		std::cerr<<"entry name="<<entry<<"=>"<<nd[cnt]<<"=>"<<LFSTK_UtilityClass::LFSTK_getFullEntry(entry,nd[cnt++],maplines3,true)<<"<="<<std::endl;
}

	return(0);
}