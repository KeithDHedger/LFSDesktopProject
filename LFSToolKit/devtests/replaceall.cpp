#if 0

# (c)keithhedger Tue 9 Jan 13:48:05 GMT 2024 kdhedger68713@gmail.com

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

#include "lfstk/LFSTKGlobals.h"
#include <memory>

std::string replaceAllStr(std::string haystack,std::string needle,std::string newneedle,bool erase=true)
{
	std::string::size_type	found;
	std::string				localhaystack=haystack;
	bool						flag=false;

	do
		{
			flag=false;
			found=localhaystack.find(needle);
			if(found!=std::string::npos)
				{
					if(erase==true)
						localhaystack.erase(found,needle.length());
					else
						localhaystack.replace(found,needle.length(),newneedle);
					flag=true;
				}
		}while(flag==true);
	return(localhaystack);
}

std::string replaceAllChar(std::string haystack,std::string needle,std::string newneedle,bool erase=true)
{
	std::string::size_type	found;
	std::string				localhaystack=haystack;
	bool						flag=false;

	do
		{
			flag=false;
			found=localhaystack.find_first_of(needle);
			if(found!=std::string::npos)
				{
					if(erase==true)
						localhaystack.erase(found,1);
					else
						localhaystack.replace(found,1,newneedle);
					flag=true;
				}
		}while(flag==true);
	return(localhaystack);
}

int main(int argc, char **argv)
{
	std::string				x="";
	LFSTK_applicationClass	*apc=new LFSTK_applicationClass();
	std::string				haystack="w1,w2,wXXXX3,.w4,XXXX,w.5,wX6";
	std::string				needle="XXXX";
	std::string				newneedle="++";
//
////replace ,>0
//	x=replaceAll(haystack,needle,std::string("")+='\0',false);
//	std::cout<<x<<std::endl;
//	fprintf(stderr,"x=%s\n",x.c_str());
//		for(int j=0;j<x.length();j++)
//			fprintf(stderr,"0x%x\n",x.at(j));
//
//replace str
	x=replaceAllStr(haystack,needle,newneedle,false);
	std::cout<<"Replace "<<x<<std::endl;
	fprintf(stderr,"x=%s\n",x.c_str());
		for(int j=0;j<x.length();j++)
			fprintf(stderr,"0x%x\n",x.at(j));

//replace char
	needle=".,";
	x=replaceAllChar(haystack,needle,newneedle,false);
	std::cout<<"Replace Char "<<x<<std::endl;
	fprintf(stderr,"x=%s\n",x.c_str());
		for(int j=0;j<x.length();j++)
			fprintf(stderr,"0x%x\n",x.at(j));
			
//
//erase
	x=replaceAllChar(haystack,needle,newneedle,true);
	std::cout<<x<<std::endl;
	fprintf(stderr,"x=%s\n",x.c_str());
		for(int j=0;j<x.length();j++)
			fprintf(stderr,"0x%x\n",x.at(j));

//libtest
	std::cout<<"Library test"<<std::endl;
	x=LFSTK_UtilityClass::LFSTK_strReplaceAllChar(haystack,needle,"=!=");
	std::cout<<x<<std::endl;
	fprintf(stderr,"x=%s\n",x.c_str());
		for(int j=0;j<x.length();j++)
			fprintf(stderr,"0x%x\n",x.at(j));

	delete apc;
	cairo_debug_reset_static_data();
	return(0);
}