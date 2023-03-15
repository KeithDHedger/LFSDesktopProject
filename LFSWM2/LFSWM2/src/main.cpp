/*
 *
 * ©K. D. Hedger. Thu 28 Jul 20:31:03 BST 2022 keithdhedger@gmail.com

 * This file (main.cpp) is part of LFSWM2.

 * LFSWM2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * LFSWM2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with LFSWM2.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include <X11/Xlib.h>

#include "lfswm2Class.h"

LFSWM2_Class			*mainLFSWM2Class;

void printHelp(void)
{
	printf(	"Usage: lfswm2 [OPTION]\n"
			"LFS Window Manager\n"
			"-k, --key\n\tSet message queue key\n"
			"-b, --backcolour\n\tSet frame back colour\n"
			"-f, --forecolour\n\tSet frame fore colour\n"
			"-t, --textcolour\n\tSet frame text colour\n"
			"-F, --font\n\tSet font string ( default: 'sans:size=14:bold' )\n"
			"-p, --titleposition\n\tSet window name position ( 1=left. 2=center( default ), 3=right )\n"
			"-w, --windowposition\n\tSet new window position ( NOPLACE=0, UNDERMOUSE=1, CENTREMMONITOR=2 ( default ), CENTRESCREEN=3 )\n"
			" -v, --version\n\tOutput version information and exit\n"
			" -h, -?, --help\n\tPrint this help\n\n"
			"Report bugs to kdhedger@gmail.com\n"
		);
}

int main(int argc, char **argv)
{
	bool					loop=false;
	LFSWM2_clientClass	*cc;
//	int					c;
//
//	struct option long_options[]=
//		{
//			{"key",1,0,'k'},
//			{"backcolour",1,0,'b'},
//			{"forecolour",1,0,'f'},
//			{"textcolour",1,0,'t'},
//			{"font",1,0,'F'},
//			{"titleposition",1,0,'p'},
//			{"windowposition",1,0,'w'},
//			{"version",0,0,'v'},
//			{"help",0,0,'?'},
//			{0, 0, 0, 0}
//		};
//
//int ac=argc;
//char **av=argv;
//
//	while(true)
//		{
//			int option_index=0;
//			c=getopt_long (argc,argv,"v?hk:b:f:t:F:p:w:",long_options,&option_index);
//			if(c==-1)
//				break;
//
//			switch(c)
//				{
//					case 'v':
//						printf("LFSWM2 %s\n",VERSION);
//						exit(0);
//						break;
//					case '?':
//					case 'h':
//						printHelp();
//						exit(0);
//						break;
//				}
//		}
//
//
//argc=ac;
//argv=av;
	mainLFSWM2Class=new LFSWM2_Class(argc,argv);
	mainLFSWM2Class->mainEventClass->LFSWM2_mainEventLoop();

	do
		{
			loop=false;
			for(auto& x:mainLFSWM2Class->mainWindowClass->clientList)
				{
    					cc=mainLFSWM2Class->mainWindowClass->LFSWM2_getClientClass(x.first);
					if((cc!=NULL))
						{
    							if(cc->transientFor!=None)
   								cc->LFSWM2_sendCloseWindow();

    							delete cc;
    							loop=true;
    							XSync(mainLFSWM2Class->display,false);
    							break;
						}
 	 			}
		}
	while(loop==true);
	delete mainLFSWM2Class;
	return(0);
}