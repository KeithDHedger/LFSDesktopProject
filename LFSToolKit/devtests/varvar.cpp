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

#include <iostream>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
//#include <cstdarg>
#include <stdarg.h>

std::string LFSTK_oneLinerStd(const std::string fmt,...)
{
	va_list ap;
	va_start(ap, fmt);
 	std::string	str="";
	int fmtcnt=0;
	while(fmtcnt<fmt.length())
		{
			if(fmt.at(fmtcnt)=='%')
				{
					fmtcnt++;
					switch(fmt.at(fmtcnt))
						{
							case 's':
								str+=va_arg(ap,char*);
								break;
							case 'S':
								str+=va_arg(ap,std::string);
								break;
							case 'i':
								str+=std::to_string(va_arg(ap,int));
								break;
							case '%':
								str+="%";
								break;
							default:
								str+=fmt.at(fmtcnt);
								break;
						}
				}
			else
				{
					str+=fmt.at(fmtcnt);
				}
			fmtcnt++;
		}
    va_end(ap);
    return(str);
}


void simple_printf(const char* fmt...)      // C-style "const char* fmt, ..." is also valid
{
    va_list args;
    va_start(args, fmt);

    while (*fmt != '\0') {
        if (*fmt == 'd') {
            int i = va_arg(args, int);
            std::cout << i << '\n';
        } else if (*fmt == 'c') {
            // note automatic conversion to integral type
            int c = va_arg(args, int);
            std::cout << static_cast<char>(c) << '\n';
        } else if (*fmt == 'f') {
            double d = va_arg(args, double);
            std::cout << d << '\n';
        } else if (*fmt == 's') {
            std::string st = va_arg(args, std::string);
            std::cout << st << '\n';
        }
        ++fmt;
    }
 
    va_end(args);
}

template <typename... Ts>
void foo_print(Ts... args) 
{
fprintf(stderr,"arg=%p\n",((args),...));
    ((std::cout << args << ' '), ...);
 
}

template <typename... Ts1>
std::string foo_print2(Ts1... args) 
{
//std::cout << ((args[1]),...);
	std::string s;//=((args),...);
	std::stringstream ss;
	 ((ss << args ), ...);
	return(ss.str());
   // ((std::cout << args << ' '), ...);
}

int main(int argc, char **argv)
{
    std::cout << std::boolalpha;
    std::cout << std::noskipws;
    std::string str=argv[1];
    foo_print(1, 3.14f, "\n"); // 1 3.14
    foo_print("Foo", 'b', true, nullptr,"\n",str, "\n"); // Foo b true nullptr
    str=str+"foobar\n";
    foo_print("\n\n",str);
   
   std::string arg1="/etc/fstab";
   std::string x=foo_print2("cp ",arg1," ","/tmp");
   std::cerr<<">>"<<x<<"<<"<<std::endl;


simple_printf("dcffs", 3, 'a', 1.999, 42.5,x);

x=LFSTK_oneLinerStd("cp %s /tmp","/etc/init.d/http");
   std::cerr<<"===>>"<<x<<"<<==="<<std::endl;
x=LFSTK_oneLinerStd("cp %S /tmp",arg1);
   std::cerr<<"=>>"<<x<<"<<="<<std::endl;

	LFSTK_lib lib(false);

	std::cout<<lib.LFSTK_oneLiner("cp %S %s",arg1,"/tmp")<<std::endl;
	return 0;
}
