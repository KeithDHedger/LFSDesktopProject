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

int main(int argc, char **argv)
{
	bool					loop=false;
	LFSWM2_clientClass	*cc;

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