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

#define UNKNOWNARG -100
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#include <X11/Xlib.h>

#include "config.h"
#include "lfswm2Class.h"
#include "eventsClass.h"
#include "windowsClass.h"

LFSWM2_Class			*mainLFSWM2Class;
LFSWM2_eventsClass	*mainEventClass;
LFSWM2_windowClass	*mainWindowClass;

struct option long_options[] =
	{
		{"long1",1,0,'l'},
		{"version",0,0,'v'},
		{"help",0,0,'?'},
		{0, 0, 0, 0}
	};

void printhelp(void)
{
printf("Usage: lfswm2 [OPTION]\n"
	"A CLI application\n"
	" -l, --long1	Do somthing good\n"
	" -v, --version	output version information and exit\n"
	" -h, -?, --help	print this help\n\n"
	"Report bugs to kdhedger@yahoo.co.uk\n"
	);
}

int main(int argc, char **argv)
{
	int c;
	while (1)
		{
		int option_index = 0;
		c = getopt_long (argc, argv, "v?h:l:",long_options, &option_index);
		if (c == -1)
			break;

		switch (c)
			{
			case 'l':
				printf("Arg=%s\n",optarg);
				break;
		
			case 'v':
				printf("lfswm2 %s\n",VERSION);
				return 0;
				break;

			case '?':
			case 'h':
				printhelp();
				return 0;
				break;

			default:
				fprintf(stderr,"?? Unknown argument ??\n");
				return UNKNOWNARG;
			break;
			}
		}
	
	if (optind < argc)
		{
			printf("non-option ARGV-elements: ");
			while (optind < argc)
				printf("%s ", argv[optind++]);
			printf("\n");
		}

	mainLFSWM2Class=new LFSWM2_Class();
	mainLFSWM2Class->mainEventClass->LFSWM2_mainEventLoop();

	return(0);
}